-- Шаг 1: Создание имени входа (Login) на уровне сервера
-- Выполняется в контексте базы данных master
USE master;
GO

-- Замените 'NodeAppUser' на желаемое имя пользователя
-- Замените 'your_strong_password_here' на надежный пароль
DECLARE @LoginName NVARCHAR(128) = 'NodeAppUser';
DECLARE @Password NVARCHAR(128) = 'password'; -- <-- ИЗМЕНИТЕ ЭТОТ ПАРОЛЬ!
DECLARE @SQL NVARCHAR(MAX);

-- Проверяем, существует ли уже такое имя входа
IF NOT EXISTS (SELECT 1 FROM sys.server_principals WHERE name = @LoginName)
BEGIN
    PRINT 'Creating Login [' + @LoginName + ']...';
    -- Создаем имя входа SQL Server Authentication
    SET @SQL = 'CREATE LOGIN [' + @LoginName + '] WITH PASSWORD = N''' + REPLACE(@Password, '''', '''''') + ''', DEFAULT_DATABASE=[VAV], CHECK_EXPIRATION=OFF, CHECK_POLICY=OFF;';
    EXEC (@SQL);
    PRINT 'Login [' + @LoginName + '] created.';
END
ELSE
BEGIN
    PRINT 'Login [' + @LoginName + '] already exists.';
END
GO

-- Шаг 2: Создание пользователя базы данных (User) в ВАШЕЙ базе данных (VAV)
-- и предоставление ему прав
USE VAV; -- Переключаемся на вашу базу данных
GO

DECLARE @LoginNameStep2 NVARCHAR(128) = 'NodeAppUser'; -- Используем другое имя переменной, чтобы избежать конфликта области видимости
DECLARE @UserNameStep2 NVARCHAR(128) = 'NodeAppUser'; -- Имя пользователя в БД

-- Проверяем, существует ли уже такой пользователь в ЭТОЙ базе данных
IF NOT EXISTS (SELECT 1 FROM sys.database_principals WHERE name = @UserNameStep2)
BEGIN
    PRINT 'Creating User [' + @UserNameStep2 + '] for Login [' + @LoginNameStep2 + '] in database [VAV]...';
    -- Создаем пользователя в базе данных VAV и связываем его с серверным именем входа
    DECLARE @SQLUser NVARCHAR(MAX) = 'CREATE USER [' + @UserNameStep2 + '] FOR LOGIN [' + @LoginNameStep2 + '];';
    EXEC (@SQLUser);
    PRINT 'User [' + @UserNameStep2 + '] created in database [VAV].';
END
ELSE
BEGIN
    PRINT 'User [' + @UserNameStep2 + '] already exists in database [VAV].';
END
GO

-- Шаг 3: Предоставление прав пользователю в базе данных VAV
-- Используем динамический SQL для добавления к роли

DECLARE @UserNameStep3 NVARCHAR(128) = 'NodeAppUser'; -- Имя пользователя для назначения ролей
DECLARE @SQLGrant NVARCHAR(MAX);

-- Вариант 1: Права на чтение и запись данных (Рекомендуется для API)
PRINT 'Granting db_datareader and db_datawriter roles to User [' + @UserNameStep3 + ']...';

-- Формируем команды динамически, подставляя имя пользователя
SET @SQLGrant = 'ALTER ROLE db_datareader ADD MEMBER [' + @UserNameStep3 + '];';
EXEC (@SQLGrant);

SET @SQLGrant = 'ALTER ROLE db_datawriter ADD MEMBER [' + @UserNameStep3 + '];';
EXEC (@SQLGrant);

PRINT 'Roles db_datareader and db_datawriter granted.';

/* -- Раскомментируйте этот блок, если хотите дать полные права (НЕ рекомендуется без необходимости)
-- Вариант 2: Полные права владельца базы данных
PRINT 'Granting db_owner role to User [' + @UserNameStep3 + ']...';
SET @SQLGrant = 'ALTER ROLE db_owner ADD MEMBER [' + @UserNameStep3 + '];';
EXEC (@SQLGrant);
PRINT 'Role db_owner granted.';
*/
GO

-- Финальное сообщение
-- Объявим переменную здесь снова, так как она нужна после GO
DECLARE @UserNameFinal NVARCHAR(128) = 'NodeAppUser';
PRINT 'User setup complete for [' + @UserNameFinal + '].';
GO