USE VAV;
GO

-- Отключаем вывод количества обработанных строк для чистоты вывода
SET NOCOUNT ON;
GO

-- === Создание таблиц без внешних ключей ===

-- Таблица Факультетов (FACULTY)
PRINT 'Creating table FACULTY...';
CREATE TABLE FACULTY (
    FACULTY      NVARCHAR(10) NOT NULL, -- Первичный ключ (код факультета)
    FACULTY_NAME NVARCHAR(100) NULL,  -- Название факультета

    CONSTRAINT PK_FACULTY PRIMARY KEY (FACULTY)
);
GO

-- Таблица Типов Аудиторий (AUDITORIUM_TYPE)
PRINT 'Creating table AUDITORIUM_TYPE...';
CREATE TABLE AUDITORIUM_TYPE (
    AUDITORIUM_TYPE     NVARCHAR(10) NOT NULL, -- Первичный ключ (код типа аудитории)
    AUDITORIUM_TYPENAME NVARCHAR(100) NOT NULL, -- Название типа аудитории

    CONSTRAINT PK_AUDITORIUM_TYPE PRIMARY KEY (AUDITORIUM_TYPE)
);
GO


-- === Создание таблиц с внешними ключами ===

-- Таблица Кафедр (PULPIT)
PRINT 'Creating table PULPIT...';
CREATE TABLE PULPIT (
    PULPIT      NVARCHAR(10) NOT NULL, -- Первичный ключ (код кафедры)
    PULPIT_NAME NVARCHAR(100) NOT NULL, -- Название кафедры
    FACULTY     NVARCHAR(10) NOT NULL, -- Внешний ключ на FACULTY

    CONSTRAINT PK_PULPIT PRIMARY KEY (PULPIT),
    CONSTRAINT FK_PULPIT_FACULTY FOREIGN KEY (FACULTY) REFERENCES FACULTY(FACULTY)
);
GO

-- Таблица Аудиторий (AUDITORIUM)
PRINT 'Creating table AUDITORIUM...';
CREATE TABLE AUDITORIUM (
    AUDITORIUM          NVARCHAR(10) NOT NULL, -- Первичный ключ (номер/код аудитории)
    AUDITORIUM_NAME     NVARCHAR(100) NULL,  -- Название аудитории (опционально)
    AUDITORIUM_CAPACITY INT          NOT NULL, -- Вместимость
    AUDITORIUM_TYPE     NVARCHAR(10) NOT NULL, -- Внешний ключ на AUDITORIUM_TYPE

    CONSTRAINT PK_AUDITORIUM PRIMARY KEY (AUDITORIUM),
    CONSTRAINT FK_AUDITORIUM_AUDITORIUM_TYPE FOREIGN KEY (AUDITORIUM_TYPE) REFERENCES AUDITORIUM_TYPE(AUDITORIUM_TYPE),
    -- Дополнительное ограничение: вместимость должна быть положительной
    CONSTRAINT CK_AUDITORIUM_CAPACITY CHECK (AUDITORIUM_CAPACITY > 0)
);
GO

-- Таблица Преподавателей (TEACHER)
PRINT 'Creating table TEACHER...';
CREATE TABLE TEACHER (
    TEACHER      NVARCHAR(10) NOT NULL, -- Первичный ключ (код/табельный номер преподавателя)
    TEACHER_NAME NVARCHAR(100) NOT NULL, -- ФИО преподавателя
    PULPIT       NVARCHAR(10) NOT NULL, -- Внешний ключ на PULPIT

    CONSTRAINT PK_TEACHER PRIMARY KEY (TEACHER),
    CONSTRAINT FK_TEACHER_PULPIT FOREIGN KEY (PULPIT) REFERENCES PULPIT(PULPIT)
);
GO

-- Таблица Предметов/Дисциплин (SUBJECT)
PRINT 'Creating table SUBJECT...';
CREATE TABLE SUBJECT (
    SUBJECT      NVARCHAR(10) NOT NULL, -- Первичный ключ (код предмета)
    SUBJECT_NAME NVARCHAR(100) NOT NULL, -- Название предмета
    PULPIT       NVARCHAR(10) NOT NULL, -- Внешний ключ на PULPIT (какая кафедра ведет предмет)

    CONSTRAINT PK_SUBJECT PRIMARY KEY (SUBJECT),
    CONSTRAINT FK_SUBJECT_PULPIT FOREIGN KEY (PULPIT) REFERENCES PULPIT(PULPIT)
);
GO

PRINT 'Database schema created successfully.';
GO