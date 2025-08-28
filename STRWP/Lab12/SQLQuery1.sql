USE VAV;
GO

-- ��������� ����� ���������� ������������ ����� ��� ������� ������
SET NOCOUNT ON;
GO

-- === �������� ������ ��� ������� ������ ===

-- ������� ����������� (FACULTY)
PRINT 'Creating table FACULTY...';
CREATE TABLE FACULTY (
    FACULTY      NVARCHAR(10) NOT NULL, -- ��������� ���� (��� ����������)
    FACULTY_NAME NVARCHAR(100) NULL,  -- �������� ����������

    CONSTRAINT PK_FACULTY PRIMARY KEY (FACULTY)
);
GO

-- ������� ����� ��������� (AUDITORIUM_TYPE)
PRINT 'Creating table AUDITORIUM_TYPE...';
CREATE TABLE AUDITORIUM_TYPE (
    AUDITORIUM_TYPE     NVARCHAR(10) NOT NULL, -- ��������� ���� (��� ���� ���������)
    AUDITORIUM_TYPENAME NVARCHAR(100) NOT NULL, -- �������� ���� ���������

    CONSTRAINT PK_AUDITORIUM_TYPE PRIMARY KEY (AUDITORIUM_TYPE)
);
GO


-- === �������� ������ � �������� ������� ===

-- ������� ������ (PULPIT)
PRINT 'Creating table PULPIT...';
CREATE TABLE PULPIT (
    PULPIT      NVARCHAR(10) NOT NULL, -- ��������� ���� (��� �������)
    PULPIT_NAME NVARCHAR(100) NOT NULL, -- �������� �������
    FACULTY     NVARCHAR(10) NOT NULL, -- ������� ���� �� FACULTY

    CONSTRAINT PK_PULPIT PRIMARY KEY (PULPIT),
    CONSTRAINT FK_PULPIT_FACULTY FOREIGN KEY (FACULTY) REFERENCES FACULTY(FACULTY)
);
GO

-- ������� ��������� (AUDITORIUM)
PRINT 'Creating table AUDITORIUM...';
CREATE TABLE AUDITORIUM (
    AUDITORIUM          NVARCHAR(10) NOT NULL, -- ��������� ���� (�����/��� ���������)
    AUDITORIUM_NAME     NVARCHAR(100) NULL,  -- �������� ��������� (�����������)
    AUDITORIUM_CAPACITY INT          NOT NULL, -- �����������
    AUDITORIUM_TYPE     NVARCHAR(10) NOT NULL, -- ������� ���� �� AUDITORIUM_TYPE

    CONSTRAINT PK_AUDITORIUM PRIMARY KEY (AUDITORIUM),
    CONSTRAINT FK_AUDITORIUM_AUDITORIUM_TYPE FOREIGN KEY (AUDITORIUM_TYPE) REFERENCES AUDITORIUM_TYPE(AUDITORIUM_TYPE),
    -- �������������� �����������: ����������� ������ ���� �������������
    CONSTRAINT CK_AUDITORIUM_CAPACITY CHECK (AUDITORIUM_CAPACITY > 0)
);
GO

-- ������� �������������� (TEACHER)
PRINT 'Creating table TEACHER...';
CREATE TABLE TEACHER (
    TEACHER      NVARCHAR(10) NOT NULL, -- ��������� ���� (���/��������� ����� �������������)
    TEACHER_NAME NVARCHAR(100) NOT NULL, -- ��� �������������
    PULPIT       NVARCHAR(10) NOT NULL, -- ������� ���� �� PULPIT

    CONSTRAINT PK_TEACHER PRIMARY KEY (TEACHER),
    CONSTRAINT FK_TEACHER_PULPIT FOREIGN KEY (PULPIT) REFERENCES PULPIT(PULPIT)
);
GO

-- ������� ���������/��������� (SUBJECT)
PRINT 'Creating table SUBJECT...';
CREATE TABLE SUBJECT (
    SUBJECT      NVARCHAR(10) NOT NULL, -- ��������� ���� (��� ��������)
    SUBJECT_NAME NVARCHAR(100) NOT NULL, -- �������� ��������
    PULPIT       NVARCHAR(10) NOT NULL, -- ������� ���� �� PULPIT (����� ������� ����� �������)

    CONSTRAINT PK_SUBJECT PRIMARY KEY (SUBJECT),
    CONSTRAINT FK_SUBJECT_PULPIT FOREIGN KEY (PULPIT) REFERENCES PULPIT(PULPIT)
);
GO

PRINT 'Database schema created successfully.';
GO