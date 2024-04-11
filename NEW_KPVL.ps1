# Change these values
$SourceFolder = "D:\YandexDisk\Project\HistoryKpvl\Build\tag.txt"
$DestinationFolder = "\\192.168.9.65\private\sqlflag\"
$Logfile = "D:\YandexDisk\Project\HistoryKpvl\Build\Robocopy.log"

# Copy Folder with Robocopy
echo "" >> $Logfile
echo "-----------------------------------------------------------------------------" >> $Logfile
echo "Start " >> $Logfile
Get-Date -Format "dd MM yyyy HH:mm:ss" >> $Logfile
echo "-----------------------------------------------------------------------------" >> $Logfile
echo "ping 192.168.9.65" >> $Logfile
ping 192.168.9.65 >> $Logfile
Robocopy $SourceFolder $DestinationFolder /E /R:1 /W:1 /PURGE /UNILOG+:$Logfile /NP