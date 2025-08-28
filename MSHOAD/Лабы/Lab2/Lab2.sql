use HR

EXEC sp_configure 'clr enabled', 1;
RECONFIGURE;

ALTER DATABASE HR SET TRUSTWORTHY ON; -- ��������� �������� ���������� ������� ������� ��� ���� ������

CREATE ASSEMBLY SqlClrAssembly
FROM 'D:\6sem_BSTU\MSHOAD\����\Lab2\SqlClrAssembly\SqlClrAssembly\bin\Debug\SqlClrAssembly.dll'
WITH PERMISSION_SET = UNSAFE;

CREATE PROCEDURE SendEmailOnPermissionChange
    @Recipient NVARCHAR(255),
    @Subject NVARCHAR(255),
    @Body NVARCHAR(MAX)
AS EXTERNAL NAME SqlClrAssembly.SqlClrFunctions.SendEmailOnPermissionChange;

CREATE TYPE PhoneNumber
EXTERNAL NAME SqlClrAssembly.PhoneNumber;

CREATE FUNCTION dbo.PhoneNumberToString(@phone PhoneNumber)
RETURNS NVARCHAR(20)
AS EXTERNAL NAME SqlClrAssembly.SqlClrFunctions.PhoneNumberToString;

CREATE TABLE TestEmployees (
    EmployeeID INT PRIMARY KEY,
    Name NVARCHAR(100),
    Phone PhoneNumber
);

INSERT INTO TestEmployees (EmployeeID, Name, Phone)
VALUES (1, '���� ������', CAST('+375(29)1857432' AS PhoneNumber));

SELECT EmployeeID, Name, dbo.PhoneNumberToString(Phone) AS Phone FROM TestEmployees;


CREATE TRIGGER trg_PermissionChange
ON DATABASE
FOR GRANT_DATABASE, REVOKE_DATABASE, DENY_DATABASE
AS
BEGIN
    DECLARE @UserName NVARCHAR(255),
            @PermissionType NVARCHAR(255),
            @ObjectName NVARCHAR(255),
            @EventType NVARCHAR(50),
            @EmailBody NVARCHAR(MAX);

    SELECT 
	-- EVENTDATA() ��� ����������� ������� � SQL Server, ������� ���������� ������ � ������� � ������� XML
        @UserName = ISNULL(EVENTDATA().value('(/EVENT_INSTANCE/LoginName)[1]', 'NVARCHAR(255)'), '����������� ������������'),
        @PermissionType = ISNULL(EVENTDATA().value('(/EVENT_INSTANCE/PermissionSet)[1]', 'NVARCHAR(255)'), '����������� ��� ����'),
        @ObjectName = ISNULL(EVENTDATA().value('(/EVENT_INSTANCE/ObjectName)[1]', 'NVARCHAR(255)'), '���� ������'),
        @EventType = ISNULL(EVENTDATA().value('(/EVENT_INSTANCE/EventType)[1]', 'NVARCHAR(50)'), '����������� �������');

    SET @EmailBody = '������������: ' + @UserName + CHAR(13) + CHAR(10) +
                     '�������: ' + @EventType + CHAR(13) + CHAR(10) +
                     '������: ' + @ObjectName + CHAR(13) + CHAR(10) +
                     '��� ����: ' + @PermissionType;

    EXEC SendEmailOnPermissionChange 
         'vodchytsanastasiya@gmail.com', -- ����������
         '��������� ���� � ����',       -- ����
         @EmailBody;                     -- ���� ������
END;


CREATE LOGIN TestUser WITH PASSWORD = 'StrongPassword123!';

USE HR;
CREATE USER TestUser FOR LOGIN TestUser;

GRANT CONNECT TO TestUser;
REVOKE CONNECT FROM TestUser;


DROP TRIGGER trg_PermissionChange ON DATABASE;
DELETE FROM TestEmployees;
DROP TABLE TestEmployees;
DROP FUNCTION dbo.PhoneNumberToString;
DROP TYPE PhoneNumber;
DROP PROCEDURE SendEmailOnPermissionChange;
DROP ASSEMBLY SqlClrAssembly;
