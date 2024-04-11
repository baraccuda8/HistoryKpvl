-- Table: public.alloy
DROP TABLE IF EXISTS public.alloy;
CREATE TABLE IF NOT EXISTS public.alloy
(
    create_at timestamp(0) with time zone NOT NULL DEFAULT now(),
    id SERIAL NOT NULL PRIMARY KEY,
    id_alloy integer,
    id_thick integer,
    alloy text COLLATE pg_catalog."default",
    thick text COLLATE pg_catalog."default"
)

TABLESPACE "pgKPVL";
ALTER TABLE IF EXISTS public.alloy OWNER to "user";


-- Table: public.cassette
DROP TABLE IF EXISTS public.cassette;
CREATE TABLE IF NOT EXISTS public.cassette
(
    create_at timestamp without time zone NOT NULL DEFAULT now(),
    id SERIAL NOT NULL PRIMARY KEY,
    event integer DEFAULT 0,
    day integer DEFAULT 0,
    month integer DEFAULT 0,
    year integer DEFAULT 0,
    cassetteno integer DEFAULT 0,
    sheetincassette integer DEFAULT 0,
    close_at timestamp without time zone,
    peth integer DEFAULT 0,
    run_at timestamp without time zone,
    error_at timestamp without time zone,
    end_at timestamp without time zone,
    delete_at timestamp without time zone,
    tempref numeric(4,1) DEFAULT 0,
    pointtime_1 numeric(4,1) DEFAULT 0,
    pointref_1 numeric(4,1) DEFAULT 0,
    timeprocset numeric(4,1) DEFAULT 0,
    pointdtime_2 numeric(4,1) DEFAULT 0,
    facttemper numeric(5,1) DEFAULT 0
)

TABLESPACE pg_default;
ALTER TABLE IF EXISTS public.cassette OWNER to "user";


-- Table: public.sheet
DROP TABLE IF EXISTS public.sheet;
CREATE TABLE IF NOT EXISTS public.sheet
(
    create_at timestamp without time zone NOT NULL DEFAULT now(),
    zone integer DEFAULT 0,
    pos integer DEFAULT 0,
    id SERIAL NOT NULL PRIMARY KEY,
    datatime_end timestamp with time zone,
    datatime_all numeric(3,1) DEFAULT 0,
    alloy text COLLATE pg_catalog."default",
    thikness text COLLATE pg_catalog."default",
    melt integer DEFAULT 0,
    partno integer DEFAULT 0,
    pack integer DEFAULT 0,
    sheet integer DEFAULT 0,
    temper integer DEFAULT 0,
    speed integer DEFAULT 0,
    za_pt3 numeric(3,1) DEFAULT 0,
    za_te3 numeric(3,1) DEFAULT 0,
    lampresstop numeric(3,1) DEFAULT 0,
    lampressbot numeric(3,1) DEFAULT 0,
    posclapantop numeric(3,1) DEFAULT 0,
    posclapanbot numeric(3,1) DEFAULT 0,
    mask text COLLATE pg_catalog."default" DEFAULT ''::text,
    lam1posclapantop numeric(3,1) DEFAULT 0,
    lam1posclapanbot numeric(3,1) DEFAULT 0,
    lam2posclapantop numeric(3,1) DEFAULT 0,
    lam2posclapanbot numeric(3,1) DEFAULT 0,
    lam_te1 numeric(3,1) DEFAULT 0,
    news integer DEFAULT 0,
    top1 numeric(3,1) DEFAULT 0,
    top2 numeric(3,1) DEFAULT 0,
    top3 numeric(3,1) DEFAULT 0,
    top4 numeric(3,1) DEFAULT 0,
    top5 numeric(3,1) DEFAULT 0,
    top6 numeric(3,1) DEFAULT 0,
    top7 numeric(3,1) DEFAULT 0,
    top8 numeric(3,1) DEFAULT 0,
    bot1 numeric(3,1) DEFAULT 0,
    bot2 numeric(3,1) DEFAULT 0,
    bot3 numeric(3,1) DEFAULT 0,
    bot4 numeric(3,1) DEFAULT 0,
    bot5 numeric(3,1) DEFAULT 0,
    bot6 numeric(3,1) DEFAULT 0,
    bot7 numeric(3,1) DEFAULT 0,
    bot8 numeric(3,1) DEFAULT 0,
    day text COLLATE pg_catalog."default" DEFAULT ''::text,
    month text COLLATE pg_catalog."default" DEFAULT ''::text,
    year text COLLATE pg_catalog."default" DEFAULT ''::text,
    cassetteno integer DEFAULT 0,
    sheetincassette integer DEFAULT 0,
    start_at timestamp without time zone,
    timeforplateheat numeric(3,1) DEFAULT 0,
    prestostartcomp numeric(3,1) DEFAULT 0
)

TABLESPACE "pgKPVL";
ALTER TABLE IF EXISTS public.sheet OWNER to "user";



-- Table: public.tag
DROP TABLE IF EXISTS public.tag;
CREATE TABLE IF NOT EXISTS public.tag
(
    create_at timestamp(0) with time zone NOT NULL DEFAULT now(),
    id SERIAL NOT NULL PRIMARY KEY,
    id_name integer,
    name text COLLATE pg_catalog."default",
    type integer,
    arhive boolean NOT NULL DEFAULT true,
    comment text COLLATE pg_catalog."default",
    content text COLLATE pg_catalog."default" DEFAULT ''::text,
    content_at time with time zone
)

TABLESPACE "pgKPVL";
ALTER TABLE IF EXISTS public.tag OWNER to "user";


-- Table: public.todos
DROP TABLE IF EXISTS public.todos;
CREATE TABLE IF NOT EXISTS public.todos
(
    create_at timestamp(0) with time zone NOT NULL DEFAULT now(),
    id SERIAL NOT NULL PRIMARY KEY,
    id_name integer NOT NULL DEFAULT 0,
    content text COLLATE pg_catalog."default"
)

TABLESPACE "pgKPVL";
ALTER TABLE IF EXISTS public.todos OWNER to postgres;

COMMENT ON COLUMN public.sheet.create_at IS 'Дата, время загрузки листа в закалочную печь';
COMMENT ON COLUMN public.sheet.zone IS 'Зона обнаружения листа';
COMMENT ON COLUMN public.sheet.pos IS 'Текущая позиция листа';
COMMENT ON COLUMN public.sheet.id IS 'Уникальный номер';
COMMENT ON COLUMN public.sheet.datatime_end IS 'Дата, время выдачи листа из закалочной печи';
COMMENT ON COLUMN public.sheet.datatime_all IS 'Продолжительность закалки, мин';
COMMENT ON COLUMN public.sheet.alloy IS 'Марка стали';
COMMENT ON COLUMN public.sheet.thikness IS 'Толщина листа, мм';
COMMENT ON COLUMN public.sheet.melt IS 'Плавка';
COMMENT ON COLUMN public.sheet.partno IS 'Партия';
COMMENT ON COLUMN public.sheet.pack IS 'Пачка';
COMMENT ON COLUMN public.sheet.sheet IS 'Номер листа';
COMMENT ON COLUMN public.sheet.temper IS 'Уставка температуры';
COMMENT ON COLUMN public.sheet.speed IS 'Скорость выгрузки';
COMMENT ON COLUMN public.sheet.za_pt3 IS 'Давление воды в баке';
COMMENT ON COLUMN public.sheet.za_te3 IS 'Температура воды в баке';
COMMENT ON COLUMN public.sheet.lampresstop IS 'Давление в верхнем коллекторе ';
COMMENT ON COLUMN public.sheet.lampressbot IS 'Давление нижнем коллекторе';
COMMENT ON COLUMN public.sheet.posclapantop IS 'Клапан. Скоростная секция. Верх';
COMMENT ON COLUMN public.sheet.posclapanbot IS 'Клапан. Скоростная секция. Низ';
COMMENT ON COLUMN public.sheet.mask IS 'Режим работы клапана';
COMMENT ON COLUMN public.sheet.lam1posclapantop IS 'Клапан. Ламинарная секция 1. Верх';
COMMENT ON COLUMN public.sheet.lam1posclapanbot IS 'Клапан. Ламинарная секция 1. Низ';
COMMENT ON COLUMN public.sheet.lam2posclapantop IS 'Клапан. Ламинарная секция 2. Верх';
COMMENT ON COLUMN public.sheet.lam2posclapanbot IS 'Клапан. Ламинарная секция 2. Низ';
COMMENT ON COLUMN public.sheet.lam_te1 IS 'Температура воды в поддоне';
COMMENT ON COLUMN public.sheet.news IS 'Признак кантовки';
COMMENT ON COLUMN public.sheet.top1 IS 'Отклонения от плоскостности листа До кантовки 1';
COMMENT ON COLUMN public.sheet.top2 IS 'Отклонения от плоскостности листа До кантовки 2';
COMMENT ON COLUMN public.sheet.top3 IS 'Отклонения от плоскостности листа До кантовки 3';
COMMENT ON COLUMN public.sheet.top4 IS 'Отклонения от плоскостности листа До кантовки 4';
COMMENT ON COLUMN public.sheet.top5 IS 'Отклонения от плоскостности листа До кантовки 5';
COMMENT ON COLUMN public.sheet.top6 IS 'Отклонения от плоскостности листа До кантовки 6';
COMMENT ON COLUMN public.sheet.top7 IS 'Отклонения от плоскостности листа До кантовки 7';
COMMENT ON COLUMN public.sheet.top8 IS 'Отклонения от плоскостности листа До кантовки 8';
COMMENT ON COLUMN public.sheet.bot1 IS 'Отклонения от плоскостности листа После кантовки 1';
COMMENT ON COLUMN public.sheet.bot2 IS 'Отклонения от плоскостности листа После кантовки 2';
COMMENT ON COLUMN public.sheet.bot3 IS 'Отклонения от плоскостности листа После кантовки 3';
COMMENT ON COLUMN public.sheet.bot4 IS 'Отклонения от плоскостности листа После кантовки 4';
COMMENT ON COLUMN public.sheet.bot5 IS 'Отклонения от плоскостности листа После кантовки 5';
COMMENT ON COLUMN public.sheet.bot6 IS 'Отклонения от плоскостности листа После кантовки 6';
COMMENT ON COLUMN public.sheet.bot7 IS 'Отклонения от плоскостности листа После кантовки 7';
COMMENT ON COLUMN public.sheet.bot8 IS 'Отклонения от плоскостности листа После кантовки 8';
COMMENT ON COLUMN public.sheet.day IS 'ID Листа День';
COMMENT ON COLUMN public.sheet.month IS 'ID Листа Месяц';
COMMENT ON COLUMN public.sheet.year IS 'ID Листа Год';
COMMENT ON COLUMN public.sheet.cassetteno IS 'ID Листа Касета';
COMMENT ON COLUMN public.sheet.sheetincassette IS 'ID Листа Лист';

COMMENT ON COLUMN public.cassette.create_at IS 'Время создания кассеты';
COMMENT ON COLUMN public.cassette.id IS 'Уникальный номер';
COMMENT ON COLUMN public.cassette.event IS 'Собитие кассеты';
COMMENT ON COLUMN public.cassette.day IS 'ID День кассеты';
COMMENT ON COLUMN public.cassette.month IS 'ID Месяц кассеты';
COMMENT ON COLUMN public.cassette.year IS 'ID Год кассеты';
COMMENT ON COLUMN public.cassette.cassetteno IS 'ID Номер кассеты';
COMMENT ON COLUMN public.cassette.sheetincassette IS 'Количество листов';
COMMENT ON COLUMN public.cassette.close_at IS 'Время набора кассеты';
COMMENT ON COLUMN public.cassette.peth IS 'Номер печи';
COMMENT ON COLUMN public.cassette.run_at IS 'Начало процесса';
COMMENT ON COLUMN public.cassette.error_at IS 'Ошибка процесса';
COMMENT ON COLUMN public.cassette.end_at IS 'Конец процесса';
COMMENT ON COLUMN public.cassette.tempref IS 'Заданное значение температуры';
COMMENT ON COLUMN public.cassette.pointtime_1 IS 'Время разгона';
COMMENT ON COLUMN public.cassette.pointref_1 IS 'Уставка температуры';
COMMENT ON COLUMN public.cassette.timeprocset IS 'Полное время процесса (уставка), мин';
COMMENT ON COLUMN public.cassette.pointdtime_2 IS 'Время выдержки';






INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (1, '|var|PLC210 OPC-UA.Application.AI_Hmi_210.Hmi.LAM_TE1.AI_eu', 10, true, 'Температура воды в поддоне');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (2, '|var|PLC210 OPC-UA.Application.AI_Hmi_210.Hmi.LaminPressBot.AI_eu', 10, true, 'Давление воды закалка коллектор низ');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (3, '|var|PLC210 OPC-UA.Application.AI_Hmi_210.Hmi.LaminPressTop.AI_eu', 10, true, 'Давление воды закалка коллектор верх');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (4, '|var|PLC210 OPC-UA.Application.AI_Hmi_210.Hmi.Za_PT3.AI_eu', 10, true, 'Давление воды в баке');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (5, '|var|PLC210 OPC-UA.Application.AI_Hmi_210.Hmi.Za_TE3.AI_eu', 10, true, 'Температура воды в баке');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (6, '|var|PLC210 OPC-UA.Application.AI_Hmi_210.Hmi.Za_TE4.AI_eu', 10, true, 'Перометр');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (7, '|var|PLC210 OPC-UA.Application.GenSeqfalseromHmi.Data.PlateData.AlloyCode', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (8, '|var|PLC210 OPC-UA.Application.GenSeqfalseromHmi.Data.PlateData.Melt', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (9, '|var|PLC210 OPC-UA.Application.GenSeqfalseromHmi.Data.PlateData.Pack', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (10, '|var|PLC210 OPC-UA.Application.GenSeqfalseromHmi.Data.PlateData.PartNo', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (11, '|var|PLC210 OPC-UA.Application.GenSeqfalseromHmi.Data.PlateData.Sheet', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (12, '|var|PLC210 OPC-UA.Application.GenSeqfalseromHmi.Data.PlateData.Thikness', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (13, '|var|PLC210 OPC-UA.Application.GenSeqfalseromHmi.Data.TempSet1', 10, true, 'Уставка температуры');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (14, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.CoolTimeAct', 10, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (15, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.HeatTime_Z1', 10, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (16, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.HeatTime_Z2', 10, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (17, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z1.AlloyCode', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (18, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z1.Melt', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (19, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z1.Pack', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (20, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z1.PartNo', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (21, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z1.Sheet', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (22, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z1.Thikness', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (23, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z2.AlloyCode', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (24, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z2.Melt', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (25, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z2.Pack', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (26, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z2.PartNo', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (27, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z2.Sheet', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (28, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z2.Thikness', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (29, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z3.AlloyCode', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (30, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z3.Melt', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (31, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z3.Pack', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (32, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z3.PartNo', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (33, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z3.Sheet', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (34, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z3.Thikness', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (35, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z4.AlloyCode', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (36, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z4.Melt', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (37, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z4.Pack', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (38, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z4.PartNo', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (39, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z4.Sheet', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (40, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z4.Thikness', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (41, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z5.AlloyCode', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (42, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z5.Melt', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (43, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z5.Pack', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (44, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z5.PartNo', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (45, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z5.Sheet', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (46, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.PlateData_Z5.Thikness', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (47, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.Seq_1_StateNo', 4, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (48, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.Seq_2_StateNo', 4, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (49, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.Seq_3_StateNo', 4, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (50, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.Zone_1_Occup', 1, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (51, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.Zone_2_Occup', 1, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (52, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.Zone_3_Occup', 1, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (53, '|var|PLC210 OPC-UA.Application.GenSeqToHmi.Data.Zone_4_Occup', 1, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (54, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.Bottom_Side.h1', 10, true, 'Откланение после кантовки');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (55, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.Bottom_Side.h2', 10, true, 'Откланение после кантовки');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (56, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.Bottom_Side.h3', 10, true, 'Откланение после кантовки');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (57, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.Bottom_Side.h4', 10, true, 'Откланение после кантовки');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (58, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.Bottom_Side.h5', 10, true, 'Откланение после кантовки');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (59, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.Bottom_Side.h6', 10, true, 'Откланение после кантовки');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (60, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.Bottom_Side.h7', 10, true, 'Откланение после кантовки');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (61, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.Bottom_Side.h8', 10, true, 'Откланение после кантовки');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (62, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.CasseteIsfalseill', 1, true, 'Кассета наполяентся');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (63, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.Cassette.CassetteNo', 6, true, 'Вывод Номер кассеты за день');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (64, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.Cassette.Day', 6, true, 'Вывод День ID листа');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (65, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.Cassette.Month', 6, true, 'Вывод Месяц ID листа');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (66, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.Cassette.SheetInCassette', 4, true, 'Вывод Номер листа в касете');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (67, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.Cassette.Year', 6, true, 'Вывод Год ID листа');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (68, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.CassetteIsComplete', 1, true, 'Кассета готова');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (69, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.LaminarSection1.Bottom', 10, true, 'Режим работы клапана');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (70, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.LaminarSection1.Top', 10, true, 'Режим работы клапана');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (71, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.LaminarSection2.Bottom', 10, true, 'Режим работы клапана');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (72, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.LaminarSection2.Top', 10, true, 'Режим работы клапана');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (73, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.NewData', 1, true, 'Новые лист в касету');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (74, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.SaveDone', 1, true, 'Новые лист в касету');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (75, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.SheetDataIN.AlloyCode', 6, true, 'вывод Код марки 0- "7", 1 - "A3", 2 - "СМ1"');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (76, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.SheetDataIN.Melt', 6, true, 'вывод Номер плавки');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (77, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.SheetDataIN.Pack', 6, true, 'вывод Пачка');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (78, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.SheetDataIN.PartNo', 6, true, 'вывод Номер партии');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (79, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.SheetDataIN.Sheet', 6, true, 'вывод Номер листа');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (80, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.SheetDataIN.Thikness', 6, true, 'вывод Код толщины. Зависит от марки стали.');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (81, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.SpeedSection.Bottom', 10, true, 'Режим работы клапана');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (82, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.SpeedSection.Top', 10, true, 'Режим работы клапана');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (83, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.StartNewCassette', 1, true, 'Начать новую кассету');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (84, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.Top_Side.h1', 10, true, 'Откланение до кантовки');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (85, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.Top_Side.h2', 10, true, 'Откланение до кантовки');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (86, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.Top_Side.h3', 10, true, 'Откланение до кантовки');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (87, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.Top_Side.h4', 10, true, 'Откланение до кантовки');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (88, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.Top_Side.h5', 10, true, 'Откланение до кантовки');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (89, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.Top_Side.h6', 10, true, 'Откланение до кантовки');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (90, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.Top_Side.h7', 10, true, 'Откланение до кантовки');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (91, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.Top_Side.h8', 10, true, 'Откланение до кантовки');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (92, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.Valve_1x', 5, true, 'Режим работы клапана');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (93, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.Valve_2x', 5, true, 'Режим работы клапана');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (94, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.WDG', 6, false, 'вачдог WDG для обратной связи');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (95, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.WDG_falseromBase', 1, false, 'вачдог WDG для обратной связи');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (96, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.WDG_toBase', 1, false, 'Битовый вачтог');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (97, '|var|PLC210 OPC-UA.Application.HMISheetData.Sheet.udiDataTime', 7, false, 'вывод Дата-Время');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (98, '|var|PLC210 OPC-UA.Application.Hmi210_1.Htr2_1.ToHmi.TAct', 10, true, 'Температура в зоне 2.1');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (99, '|var|PLC210 OPC-UA.Application.Hmi210_1.Htr2_2.ToHmi.TAct', 10, true, 'Температура в зоне 2.2');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (100, '|var|PLC210 OPC-UA.Application.Hmi210_1.Htr2_3.ToHmi.TAct', 10, true, 'Температура в зоне 2.3');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (101, '|var|PLC210 OPC-UA.Application.Hmi210_1.Htr2_4.ToHmi.TAct', 10, true, 'Температура в зоне 2.4');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (102, '|var|PLC210 OPC-UA.Application.Hmi210_1.Htr_1.ToHmi.TAct', 10, true, 'Температура в зоне 1.1');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (103, '|var|PLC210 OPC-UA.Application.Hmi210_1.Htr_2.ToHmi.TAct', 10, true, 'Температура в зоне 1.2');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (104, '|var|PLC210 OPC-UA.Application.Hmi210_1.Htr_3.ToHmi.TAct', 10, true, 'Температура в зоне 1.3');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (105, '|var|PLC210 OPC-UA.Application.Hmi210_1.Htr_4.ToHmi.TAct', 10, true, 'Температура в зоне 1.4');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (106, '|var|PLC210 OPC-UA.Application.Par_Gen.Par.UnloadSpeed', 10, true, 'Скорость выгрузки');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (107, '|var|PLC210 OPC-UA.Application.Par_Gen.Par.TimefalseorPlateHeat', 10, true, 'Время сигнализации окончания нагрева, мин');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (108, '|var|PLC210 OPC-UA.Application.Par_Gen.Par.PresToStartComp', 10, true, 'Уставка давления для запуска комперссора');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (109, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_1.Data.Cassette.CaasetteNo', 6, true, 'Номер кассеты');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (110, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_1.Data.Cassette.Day', 6, true, 'День кассеты');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (111, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_1.Data.Cassette.Month', 6, true, 'Месяц кассеты');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (112, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_1.Data.Cassette.Year', 6, true, 'Год кассеты');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (113, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_1.Data.PointDTime_2', 10, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (114, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_1.Data.PointRefalse_1', 10, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (115, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_1.Data.PointTime_1', 10, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (116, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_1.Data.ProcEnd', 1, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (117, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_1.Data.Procfalseault', 1, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (118, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_1.Data.ProcRun', 1, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (119, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_1.Data.ProcTimeMin', 10, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (120, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_1.Data.T1', 10, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (121, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_1.Data.T2', 10, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (122, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_1.Data.TempAct', 10, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (123, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_1.Data.TempRefalse', 10, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (124, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_1.Data.TimeProcSet', 10, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (125, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_1.Data.TimeToProcEnd', 10, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (126, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_1.Data.WDG_falseromBase', 1, false, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (127, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_1.Data.WDG_toBase', 1, false, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (128, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_2.Data.Cassette.CaasetteNo', 6, true, 'Номер кассеты');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (129, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_2.Data.Cassette.Day', 6, true, 'День кассеты');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (130, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_2.Data.Cassette.Month', 6, true, 'Месяц кассеты');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (131, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_2.Data.Cassette.Year', 6, true, 'Год кассеты');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (132, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_2.Data.PointDTime_2', 10, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (133, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_2.Data.PointRefalse_1', 10, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (134, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_2.Data.PointTime_1', 10, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (135, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_2.Data.ProcEnd', 1, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (136, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_2.Data.Procfalseault', 1, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (137, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_2.Data.ProcRun', 1, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (138, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_2.Data.ProcTimeMin', 10, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (139, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_2.Data.T1', 10, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (140, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_2.Data.T2', 10, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (141, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_2.Data.TempAct', 10, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (142, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_2.Data.TempRefalse', 10, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (143, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_2.Data.TimeProcSet', 10, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (144, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_2.Data.TimeToProcEnd', 10, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (145, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_2.Data.WDG_falseromBase', 1, false, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (146, '|var|SPK107 (M01).Application.falseorBase_Relfalseurn_2.Data.WDG_toBase', 1, false, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (147, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[1].CaasetteNo', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (148, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[1].Day', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (149, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[1].Month', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (150, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[1].Year', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (151, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[2].CaasetteNo', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (152, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[2].Day', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (153, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[2].Month', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (154, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[2].Year', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (155, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[3].CaasetteNo', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (156, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[3].Day', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (157, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[3].Month', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (158, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[3].Year', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (159, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[4].CaasetteNo', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (160, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[4].Day', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (161, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[4].Month', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (162, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[4].Year', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (163, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[5].CaasetteNo', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (164, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[5].Day', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (165, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[5].Month', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (166, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[5].Year', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (167, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[6].CaasetteNo', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (168, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[6].Day', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (169, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[6].Month', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (170, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[6].Year', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (171, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[7].CaasetteNo', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (172, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[7].Day', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (173, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[7].Month', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (174, '|var|SPK107 (M01).Application.cassetteArray.data.cassette[7].Year', 6, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (175, '|var|SPK107 (M01).Application.cassetteArray.data.selected_casset', 4, true, '');
INSERT INTO tag (id_name, name, type, arhive, comment) VALUES (176, '|var|SPK107 (M01).Application.cassetteArray.data.selected_cassetfalseurn1', 4, true, '');
