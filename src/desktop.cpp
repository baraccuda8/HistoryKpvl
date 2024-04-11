#include "pch.h"

#include <windows.h>
#include <wtsapi32.h>
#include <Tchar.h>
#include <userenv.h>
#include <string>

#include <exception>

#define CATCH2(_s) catch(std::exception& exc){\
    addLogMessage(std::string(_s) + std::string(" ") + exc.what());\
}catch(...){\
    addLogMessage(std::string(_s) + " Unknown error");\
};;


#define DESKTOP_ALL (DESKTOP_READOBJECTS | DESKTOP_CREATEWINDOW | DESKTOP_CREATEMENU | DESKTOP_HOOKCONTROL | DESKTOP_JOURNALRECORD | DESKTOP_JOURNALPLAYBACK | DESKTOP_ENUMERATE | DESKTOP_WRITEOBJECTS | DESKTOP_SWITCHDESKTOP | STANDARD_RIGHTS_REQUIRED)
#define WINSTA_ALL (WINSTA_ENUMDESKTOPS | WINSTA_READATTRIBUTES | WINSTA_ACCESSCLIPBOARD | WINSTA_CREATEDESKTOP | WINSTA_WRITEATTRIBUTES | WINSTA_ACCESSGLOBALATOMS | WINSTA_EXITWINDOWS | WINSTA_ENUMERATE | WINSTA_READSCREEN | STANDARD_RIGHTS_REQUIRED)
#define GENERIC_ACCESS (GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE | GENERIC_ALL)

DWORD ProcessId = 0;
HANDLE hProcess = NULL;

//void addLogMessage(const char* ss);
void addLogMessage(std::string ss);

BOOL AddAceToWindowStation(HWINSTA hwinsta, PSID psid);

BOOL AddAceToDesktop(HDESK hdesk, PSID psid);


VOID FreeLogonSID (PSID* ppsid)
{
    HeapFree(GetProcessHeap(), 0, (LPVOID)*ppsid);
}

BOOL GetLogonSID (HANDLE hToken, PSID* ppsid)
{
    BOOL bSuccess = FALSE;
    DWORD dwIndex;
    DWORD dwLength = 0;
    PTOKEN_GROUPS ptg = NULL;

    try
    {

        // Verify the parameter passed in is not NULL.
        if(NULL == ppsid)
            throw std::exception("!ppsid ");

        // Get required buffer size and allocate the TOKEN_GROUPS buffer.
        if(!GetTokenInformation(hToken, TokenGroups, (LPVOID)ptg, 0, &dwLength))
        {

            if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
                throw std::exception("GetLastError() != ERROR_INSUFFICIENT_BUFFER ");

            if((ptg = (PTOKEN_GROUPS)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwLength)) == NULL)
                throw std::exception("ptg == NULL ");
        }

        // Get the token group information from the access token.
        if(!GetTokenInformation(hToken, TokenGroups, (LPVOID)ptg, dwLength, &dwLength))
            throw std::exception("!GetTokenInformation ");

        // Loop through the groups to find the logon SID.
        for(dwIndex = 0; dwIndex < ptg->GroupCount; dwIndex++)
        {
            if((ptg->Groups[dwIndex].Attributes & SE_GROUP_LOGON_ID) == SE_GROUP_LOGON_ID)
            {
                // Found the logon SID; make a copy of it.
                dwLength = GetLengthSid(ptg->Groups[dwIndex].Sid);
                *ppsid = (PSID)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwLength);
                if(*ppsid == NULL)
                    throw std::exception("*ppsid == NULL ");

                if(!CopySid(dwLength, *ppsid, ptg->Groups[dwIndex].Sid))
                {
                    HeapFree(GetProcessHeap(), 0, (LPVOID)*ppsid);
                    throw std::exception("!CopySid ");
                }
                break;
            }
        }
        bSuccess = TRUE;
    }
    CATCH2("GetLogonSID: ");

    // Free the buffer for the token groups.
    if(ptg != NULL)
        HeapFree(GetProcessHeap(), 0, (LPVOID)ptg);

    return bSuccess;
}

DWORD StartInteractiveClientProcess (
    const char* lpszUsername,    // client to log on
    const char* lpszDomain,      // domain of client's account
    const char* lpszPassword,    // client's password
    const char* lpCommandLine,    // command line to execute
    const char* lpCurrentDirectory = NULL
)
{
    HANDLE      hToken = NULL;
    HANDLE      hToken1 = NULL;
    HDESK       hdesk = NULL;
    HWINSTA     hwinsta = NULL, hwinstaSave = NULL;
    PROCESS_INFORMATION pi;
    PSID pSid = NULL;
    STARTUPINFO si;
    BOOL bResult = FALSE;
    void* environment = NULL;

    try
    {
        if(!LogonUser(lpszUsername, lpszDomain, lpszPassword, LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_WINNT50, &hToken1))
            throw std::exception("!LogonUser ");

        if(!DuplicateTokenEx(hToken1, MAXIMUM_ALLOWED, nullptr, SecurityImpersonation, TokenPrimary, &hToken))
            throw std::exception("!DuplicateTokenEx ");


        //if(!WTSQueryUserToken (1, &hToken))
        //    throw std::exception("!WTSQueryUserToken ");

        if(!CreateEnvironmentBlock (&environment, hToken, TRUE))
            throw std::exception("!CreateEnvironmentBlock ");


        if((hwinstaSave = GetProcessWindowStation()) == NULL)
            throw std::exception(__FUN("!GetProcessWindowStation "));

        if((hwinsta = OpenWindowStation(_T("winsta0"), TRUE, READ_CONTROL | WRITE_DAC)) == NULL)
            throw std::exception(__FUN("!OpenWindowStation "));

        if(!SetProcessWindowStation(hwinsta))
            throw std::exception(__FUN("!SetProcessWindowStation 1 "));

        // Get a handle to the interactive desktop.
        hdesk = OpenDesktop(_T("default"), 0, TRUE, READ_CONTROL | WRITE_DAC | DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS);

        if(!SetProcessWindowStation(hwinstaSave))
            throw std::exception(__FUN("!SetProcessWindowStation 2 "));

        if(hdesk == NULL)
            throw std::exception(__FUN("!OpenDesktop "));

        // Get the SID for the client's logon session.
        if(!GetLogonSID(hToken, &pSid))
            throw std::exception(__FUN("!GetLogonSID "));

        // Allow logon SID full access to interactive window station.
        if(!AddAceToWindowStation(hwinsta, pSid))
            throw std::exception(__FUN("!AddAceToWindowStation "));

        // Allow logon SID full access to interactive desktop.
        if(!AddAceToDesktop(hdesk, pSid))
            throw std::exception(__FUN("!AddAceToDesktop "));

        // Impersonate client to ensure access to executable file.
        if(!ImpersonateLoggedOnUser(hToken))
            throw std::exception(__FUN("!ImpersonateLoggedOnUser "));

        // Initialize the STARTUPINFO structure.
        // Specify that the process runs in the interactive desktop.
        ZeroMemory(&si, sizeof(STARTUPINFO));
        si.cb= sizeof(STARTUPINFO);
        si.lpDesktop = (LPSTR)TEXT("winsta0\\default");

        // Launch the process in the client's logon session.
        bResult = CreateProcessAsUser(hToken,
                                      NULL,
                                      (LPSTR)lpCommandLine,
                                      NULL, NULL,
                                      FALSE,
                                      NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT,
                                      environment,
                                      lpCurrentDirectory,
                                      &si,
                                      &pi);
        if(!bResult)
            throw std::exception(__FUN("!CreateProcessAsUser "));

        ProcessId = pi.dwProcessId;
        hProcess = pi.hProcess;

        addLogMessage("CreateProcessAsUser == " + std::to_string(ProcessId));

        // End impersonation of client.
        RevertToSelf();

        if(bResult && pi.hProcess != INVALID_HANDLE_VALUE)
        {
            WaitForSingleObject(pi.hProcess, INFINITE);
            CloseHandle(pi.hProcess);
        }

        if(pi.hThread != INVALID_HANDLE_VALUE)
            CloseHandle(pi.hThread);

    }
    CATCH2("Error: ");


    if(hwinstaSave != NULL)
        SetProcessWindowStation (hwinstaSave);

    // Free the buffer for the logon SID.
    if(pSid)
        FreeLogonSID(&pSid);

    // Close the handles to the interactive window station and desktop.
    if(hwinsta)
        CloseWindowStation(hwinsta);

    if(hdesk)
        CloseDesktop(hdesk);

    // Close the handle to the client's access token.
    if(hToken != INVALID_HANDLE_VALUE)
        CloseHandle(hToken);

    if(hToken1 != INVALID_HANDLE_VALUE)
        CloseHandle(hToken1);

    if(environment)
        DestroyEnvironmentBlock (environment);

    return 0;
}

BOOL AddAceToWindowStation(HWINSTA hwinsta, PSID psid)
{
    ACCESS_ALLOWED_ACE* pace = NULL;
    ACL_SIZE_INFORMATION aclSizeInfo;
    BOOL                 bDaclExist;
    BOOL                 bDaclPresent;
    BOOL                 bSuccess = FALSE;
    DWORD                dwNewAclSize;
    DWORD                dwSidSize = 0;
    DWORD                dwSdSizeNeeded;
    PACL                 pacl;
    PACL                 pNewAcl = NULL;
    PSECURITY_DESCRIPTOR psd = NULL;
    PSECURITY_DESCRIPTOR psdNew = NULL;
    PVOID                pTempAce;
    SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;
    unsigned int         i;

    try
    {
        // Obtain the DACL for the window station.
        if(!GetUserObjectSecurity(hwinsta, &si, psd, dwSidSize, &dwSdSizeNeeded))
            if(GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                psd = (PSECURITY_DESCRIPTOR)HeapAlloc(
                    GetProcessHeap(),
                    HEAP_ZERO_MEMORY,
                    dwSdSizeNeeded);

                if(psd == NULL)
                    throw std::exception(__FUN("psd == NULL "));
                    //__leave;

                psdNew = (PSECURITY_DESCRIPTOR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSdSizeNeeded);

                if(psdNew == NULL)
                    throw std::exception(__FUN("psdNew == NULL "));
                    //__leave;

                dwSidSize = dwSdSizeNeeded;

                if(!GetUserObjectSecurity(hwinsta, &si, psd, dwSidSize, &dwSdSizeNeeded))
                    throw std::exception(__FUN("!GetUserObjectSecurity "));
                    //__leave;
            }
            else
                throw std::exception(__FUN("GetLastError() != ERROR_INSUFFICIENT_BUFFER "));
                //__leave;

             // Create a new DACL.

        if(!InitializeSecurityDescriptor(psdNew, SECURITY_DESCRIPTOR_REVISION))
            throw std::exception(__FUN("!InitializeSecurityDescriptor "));
            //__leave;

         // Get the DACL from the security descriptor.
        if(!GetSecurityDescriptorDacl(psd, &bDaclPresent, &pacl, &bDaclExist))
            throw std::exception(__FUN("!GetSecurityDescriptorDacl "));
            //__leave;

         // Initialize the ACL.
        ZeroMemory(&aclSizeInfo, sizeof(ACL_SIZE_INFORMATION));
        aclSizeInfo.AclBytesInUse = sizeof(ACL);

        // Call only if the DACL is not NULL.
        if(pacl != NULL)
        {
            if(!GetAclInformation(pacl, (LPVOID)&aclSizeInfo, sizeof(ACL_SIZE_INFORMATION), AclSizeInformation))
                throw std::exception(__FUN("!GetAclInformation "));
                //__leave;
        }

        // Compute the size of the new ACL.
        dwNewAclSize = aclSizeInfo.AclBytesInUse + (2 * sizeof(ACCESS_ALLOWED_ACE)) + (2 * GetLengthSid(psid)) - (2 * sizeof(DWORD));

        // Allocate memory for the new ACL.
        pNewAcl = (PACL)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwNewAclSize);

        if(pNewAcl == NULL)
            throw std::exception(__FUN("pNewAcl == NULL "));
            //__leave;

         // Initialize the new DACL.
        if(!InitializeAcl(pNewAcl, dwNewAclSize, ACL_REVISION))
            throw std::exception(__FUN("!InitializeAcl "));
            //__leave;

         // If DACL is present, copy it to a new DACL.
        if(bDaclPresent)
        {
           // Copy the ACEs to the new ACL.
            if(aclSizeInfo.AceCount)
            {
                for(i=0; i < aclSizeInfo.AceCount; i++)
                {
                   // Get an ACE.
                    if(!GetAce(pacl, i, &pTempAce))
                        throw std::exception(__FUN("!GetAce "));
                        //__leave;

                     // Add the ACE to the new ACL.
                    if(!AddAce(pNewAcl, ACL_REVISION, MAXDWORD, pTempAce, ((PACE_HEADER)pTempAce)->AceSize))
                        throw std::exception(__FUN("!AddAce "));
                        //__leave;
                }
            }
        }

        // Add the first ACE to the window station.

        pace = (ACCESS_ALLOWED_ACE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(psid) - sizeof(DWORD));

        if(pace == NULL)
            throw std::exception(__FUN("pace == NULL "));
            //__leave;

        pace->Header.AceType  = ACCESS_ALLOWED_ACE_TYPE;
        pace->Header.AceFlags = CONTAINER_INHERIT_ACE | INHERIT_ONLY_ACE | OBJECT_INHERIT_ACE;
        pace->Header.AceSize  = LOWORD(sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(psid) - sizeof(DWORD));
        pace->Mask            = GENERIC_ACCESS;

        if(!CopySid(GetLengthSid(psid), &pace->SidStart, psid))
            throw std::exception(__FUN("!CopySid "));
            //__leave;

        if(!AddAce(pNewAcl, ACL_REVISION, MAXDWORD, (LPVOID)pace, pace->Header.AceSize))
            throw std::exception(__FUN("!AddAce "));
            //__leave;

        // Add the second ACE to the window station.
        pace->Header.AceFlags = NO_PROPAGATE_INHERIT_ACE;
        pace->Mask            = WINSTA_ALL;

        if(!AddAce(pNewAcl, ACL_REVISION, MAXDWORD, (LPVOID)pace, pace->Header.AceSize))
            throw std::exception(__FUN("!AddAce "));
            //__leave;

        // Set a new DACL for the security descriptor.
        if(!SetSecurityDescriptorDacl(psdNew, TRUE, pNewAcl, FALSE))
            throw std::exception(__FUN("!SetSecurityDescriptorDacl "));
            //__leave;

        // Set the new security descriptor for the window station.
        if(!SetUserObjectSecurity(hwinsta, &si, psdNew))
            throw std::exception(__FUN("!SetUserObjectSecurity "));
            //__leave;

        // Indicate success.
        bSuccess = TRUE;
    }
    //__finally
    CATCH2("AddAceToWindowStation: ");

            // Free the allocated buffers.
    if(pace != NULL)
        HeapFree(GetProcessHeap(), 0, (LPVOID)pace);

    if(pNewAcl != NULL)
        HeapFree(GetProcessHeap(), 0, (LPVOID)pNewAcl);

    if(psd != NULL)
        HeapFree(GetProcessHeap(), 0, (LPVOID)psd);

    if(psdNew != NULL)
        HeapFree(GetProcessHeap(), 0, (LPVOID)psdNew);

    return bSuccess;
}

BOOL AddAceToDesktop(HDESK hdesk, PSID psid)
{
    ACL_SIZE_INFORMATION aclSizeInfo;
    BOOL                 bDaclExist;
    BOOL                 bDaclPresent;
    BOOL                 bSuccess = FALSE;
    DWORD                dwNewAclSize;
    DWORD                dwSidSize = 0;
    DWORD                dwSdSizeNeeded;
    PACL                 pacl;
    PACL                 pNewAcl = NULL;
    PSECURITY_DESCRIPTOR psd = NULL;
    PSECURITY_DESCRIPTOR psdNew = NULL;
    PVOID                pTempAce;
    SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;
    unsigned int         i;

    try
    {
        // Obtain the security descriptor for the desktop object.
        if(!GetUserObjectSecurity(hdesk, &si, psd, dwSidSize, &dwSdSizeNeeded))
        {
            if(GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                psd = (PSECURITY_DESCRIPTOR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSdSizeNeeded);

                if(psd == NULL)
                    throw std::exception(__FUN("psd == NULL "));
                    //__leave;

                psdNew = (PSECURITY_DESCRIPTOR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSdSizeNeeded);

                if(psdNew == NULL)
                    throw std::exception(__FUN("psdNew == NULL "));
                    //__leave;

                dwSidSize = dwSdSizeNeeded;

                if(!GetUserObjectSecurity(hdesk, &si, psd, dwSidSize, &dwSdSizeNeeded))
                    throw std::exception(__FUN("!GetUserObjectSecurity "));
                    //__leave;
            }
            else
                throw std::exception(__FUN("GetLastError() != ERROR_INSUFFICIENT_BUFFER "));
                //__leave;
        }

        // Create a new security descriptor.
        if(!InitializeSecurityDescriptor(psdNew, SECURITY_DESCRIPTOR_REVISION))
            throw std::exception(__FUN("!InitializeSecurityDescriptor "));
            //__leave;

        // Obtain the DACL from the security descriptor.
        if(!GetSecurityDescriptorDacl(psd, &bDaclPresent, &pacl, &bDaclExist))
            throw std::exception(__FUN("!GetSecurityDescriptorDacl "));
            //__leave;

        // Initialize.
        ZeroMemory(&aclSizeInfo, sizeof(ACL_SIZE_INFORMATION));
        aclSizeInfo.AclBytesInUse = sizeof(ACL);

        // Call only if NULL DACL.
        if(pacl != NULL)
        {
            // Determine the size of the ACL information.
            if(!GetAclInformation(pacl, (LPVOID)&aclSizeInfo, sizeof(ACL_SIZE_INFORMATION), AclSizeInformation))
                throw std::exception(__FUN("!GetAclInformation "));
                //__leave;
        }

        // Compute the size of the new ACL.
        dwNewAclSize = aclSizeInfo.AclBytesInUse + sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(psid) - sizeof(DWORD);

        // Allocate buffer for the new ACL.
        pNewAcl = (PACL)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwNewAclSize);

        if(pNewAcl == NULL)
            throw std::exception(__FUN("pNewAcl == NULL "));
            //__leave;

        // Initialize the new ACL.
        if(!InitializeAcl(pNewAcl, dwNewAclSize, ACL_REVISION))
            throw std::exception(__FUN("!InitializeAcl "));
            //__leave;

        // If DACL is present, copy it to a new DACL.
        if(bDaclPresent)
        {
            // Copy the ACEs to the new ACL.
            if(aclSizeInfo.AceCount)
            {
                for(i=0; i < aclSizeInfo.AceCount; i++)
                {
                    // Get an ACE.
                    if(!GetAce(pacl, i, &pTempAce))
                        throw std::exception(__FUN("!GetAce "));
                        //__leave;

                     // Add the ACE to the new ACL.
                    if(!AddAce(pNewAcl, ACL_REVISION, MAXDWORD, pTempAce, ((PACE_HEADER)pTempAce)->AceSize))
                        throw std::exception(__FUN("!AddAce "));
                        //__leave;
                }
            }
        }

        // Add ACE to the DACL.
        if(!AddAccessAllowedAce(pNewAcl, ACL_REVISION, DESKTOP_ALL, psid))
            throw std::exception(__FUN("!AddAccessAllowedAce "));
            //__leave;

        // Set new DACL to the new security descriptor.
        if(!SetSecurityDescriptorDacl(psdNew, TRUE, pNewAcl, FALSE))
            throw std::exception(__FUN("!SetSecurityDescriptorDacl "));
            //__leave;

        // Set the new security descriptor for the desktop object.
        if(!SetUserObjectSecurity(hdesk, &si, psdNew))
            throw std::exception(__FUN("!SetUserObjectSecurity "));
            //__leave;

        // Indicate success.
        bSuccess = TRUE;
    }
    CATCH2("AddAceToDesktop: ");
    //__finally

        // Free buffers.
    if(pNewAcl != NULL)
        HeapFree(GetProcessHeap(), 0, (LPVOID)pNewAcl);

    if(psd != NULL)
        HeapFree(GetProcessHeap(), 0, (LPVOID)psd);

    if(psdNew != NULL)
        HeapFree(GetProcessHeap(), 0, (LPVOID)psdNew);


    return bSuccess;
}



//BOOL SetPrivilege(HANDLE hToken)
//{
//    TOKEN_PRIVILEGES tp;
//    LUID luid;
//    BOOL bEnablePrivilege = TRUE;
//    LPCTSTR lpszPrivilege = SE_DEBUG_NAME;
//
//    try
//    {
//        addLogMessage("Start SetPrivilege");
//        if(!LookupPrivilegeValue(
//            NULL,            // lookup privilege on local system
//            lpszPrivilege,   // privilege to lookup 
//            &luid))        // receives LUID of privilege
//        {
//            throw std::exception(__FUN("LookupPrivilegeValue error"));
//            //printf("LookupPrivilegeValue error: %u\n", GetLastError());
//            //return FALSE;
//        }
//
//        tp.PrivilegeCount = 1;
//        tp.Privileges[0].Luid = luid;
//        if(bEnablePrivilege)
//            tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
//        else
//            tp.Privileges[0].Attributes = 0;
//
//        // Enable the privilege or disable all privileges.
//
//        if(!AdjustTokenPrivileges(
//            hToken,
//            FALSE,
//            &tp,
//            sizeof(TOKEN_PRIVILEGES),
//            (PTOKEN_PRIVILEGES)NULL,
//            (PDWORD)NULL))
//        {
//            throw std::exception(__FUN("AdjustTokenPrivileges error"));
//
//            //printf("AdjustTokenPrivileges error: %u\n", GetLastError());
//            //return FALSE;
//        }
//
//        if(GetLastError() == ERROR_NOT_ALL_ASSIGNED)
//        {
//            throw std::exception(__FUN("The token does not have the specified privilege."));
//            //printf("The token does not have the specified privilege. \n");
//            //return FALSE;
//        }
//        addLogMessage("Stop SetPrivilege");
//        return TRUE;
//    }
//    catch(std::exception& exc)
//    {
//        addLogMessage((std::string(("SetPrivilege: ")) + exc.what()).c_str());
//    }
//    catch(...)
//    {
//        addLogMessage((std::string(("SetPrivilege: ")) + "Unknown error").c_str());
//    };
//    return FALSE;
//}