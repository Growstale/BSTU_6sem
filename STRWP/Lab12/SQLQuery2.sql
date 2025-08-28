-- ��� 1: �������� ����� ����� (Login) �� ������ �������
-- ����������� � ��������� ���� ������ master
USE master;
GO

-- �������� 'NodeAppUser' �� �������� ��� ������������
-- �������� 'your_strong_password_here' �� �������� ������
DECLARE @LoginName NVARCHAR(128) = 'NodeAppUser';
DECLARE @Password NVARCHAR(128) = 'password'; -- <-- �������� ���� ������!
DECLARE @SQL NVARCHAR(MAX);

-- ���������, ���������� �� ��� ����� ��� �����
IF NOT EXISTS (SELECT 1 FROM sys.server_principals WHERE name = @LoginName)
BEGIN
    PRINT 'Creating Login [' + @LoginName + ']...';
    -- ������� ��� ����� SQL Server Authentication
    SET @SQL = 'CREATE LOGIN [' + @LoginName + '] WITH PASSWORD = N''' + REPLACE(@Password, '''', '''''') + ''', DEFAULT_DATABASE=[VAV], CHECK_EXPIRATION=OFF, CHECK_POLICY=OFF;';
    EXEC (@SQL);
    PRINT 'Login [' + @LoginName + '] created.';
END
ELSE
BEGIN
    PRINT 'Login [' + @LoginName + '] already exists.';
END
GO

-- ��� 2: �������� ������������ ���� ������ (User) � ����� ���� ������ (VAV)
-- � �������������� ��� ����
USE VAV; -- ������������� �� ���� ���� ������
GO

DECLARE @LoginNameStep2 NVARCHAR(128) = 'NodeAppUser'; -- ���������� ������ ��� ����������, ����� �������� ��������� ������� ���������
DECLARE @UserNameStep2 NVARCHAR(128) = 'NodeAppUser'; -- ��� ������������ � ��

-- ���������, ���������� �� ��� ����� ������������ � ���� ���� ������
IF NOT EXISTS (SELECT 1 FROM sys.database_principals WHERE name = @UserNameStep2)
BEGIN
    PRINT 'Creating User [' + @UserNameStep2 + '] for Login [' + @LoginNameStep2 + '] in database [VAV]...';
    -- ������� ������������ � ���� ������ VAV � ��������� ��� � ��������� ������ �����
    DECLARE @SQLUser NVARCHAR(MAX) = 'CREATE USER [' + @UserNameStep2 + '] FOR LOGIN [' + @LoginNameStep2 + '];';
    EXEC (@SQLUser);
    PRINT 'User [' + @UserNameStep2 + '] created in database [VAV].';
END
ELSE
BEGIN
    PRINT 'User [' + @UserNameStep2 + '] already exists in database [VAV].';
END
GO

-- ��� 3: �������������� ���� ������������ � ���� ������ VAV
-- ���������� ������������ SQL ��� ���������� � ����

DECLARE @UserNameStep3 NVARCHAR(128) = 'NodeAppUser'; -- ��� ������������ ��� ���������� �����
DECLARE @SQLGrant NVARCHAR(MAX);

-- ������� 1: ����� �� ������ � ������ ������ (������������� ��� API)
PRINT 'Granting db_datareader and db_datawriter roles to User [' + @UserNameStep3 + ']...';

-- ��������� ������� �����������, ���������� ��� ������������
SET @SQLGrant = 'ALTER ROLE db_datareader ADD MEMBER [' + @UserNameStep3 + '];';
EXEC (@SQLGrant);

SET @SQLGrant = 'ALTER ROLE db_datawriter ADD MEMBER [' + @UserNameStep3 + '];';
EXEC (@SQLGrant);

PRINT 'Roles db_datareader and db_datawriter granted.';

/* -- ���������������� ���� ����, ���� ������ ���� ������ ����� (�� ������������� ��� �������������)
-- ������� 2: ������ ����� ��������� ���� ������
PRINT 'Granting db_owner role to User [' + @UserNameStep3 + ']...';
SET @SQLGrant = 'ALTER ROLE db_owner ADD MEMBER [' + @UserNameStep3 + '];';
EXEC (@SQLGrant);
PRINT 'Role db_owner granted.';
*/
GO

-- ��������� ���������
-- ������� ���������� ����� �����, ��� ��� ��� ����� ����� GO
DECLARE @UserNameFinal NVARCHAR(128) = 'NodeAppUser';
PRINT 'User setup complete for [' + @UserNameFinal + '].';
GO