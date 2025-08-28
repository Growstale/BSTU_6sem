CREATE TABLE PersonTripTable(
    EmployeeID NUMBER,
    BusinessTripID NUMBER,
    FOREIGN KEY (EmployeeID) REFERENCES Employee(EmployeeID),
    FOREIGN KEY (BusinessTripID) REFERENCES BusinessTrip(BusinessTripID)
);

CREATE OR REPLACE TYPE TripType AS OBJECT (
    id NUMBER,
    bonus NUMBER,
    startDate DATE,
    endDate DATE
);

CREATE OR REPLACE TYPE TripTableType IS TABLE OF TripType;

CREATE OR REPLACE TYPE PersonType AS OBJECT (
    id NUMBER,
    name VARCHAR2(100),
    address NVARCHAR2(150),
    trips TripTableType  -- Вложенная коллекция командировок для каждого человека
);

CREATE OR REPLACE TYPE PersonTableType IS TABLE OF PersonType;


DECLARE
    K1 PersonTableType := PersonTableType();
    is_Member INTEGER := 0;
BEGIN
    -- a. Создать коллекцию на основе t1, далее K1, для нее как атрибут – вложенную коллекцию на основе t2, далее К2;
    FOR person_rec IN (SELECT EmployeeID, Name, Address FROM Employee) LOOP
        DECLARE
            K2 TripTableType := TripTableType(); -- Вложенная коллекция
        BEGIN
            FOR trip_rec IN (
                SELECT B.BusinessTripID, B.Bonus, B.StartDate, B.EndDate
                    FROM BusinessTrip B
                    JOIN PersonTripTable P 
                    ON B.BusinessTripID = P.BusinessTripID
                    WHERE P.EmployeeID = person_rec.EmployeeID
            ) LOOP
                K2.EXTEND;
                K2(K2.LAST) := TripType(
                    trip_rec.BusinessTripID,
                    trip_rec.Bonus,
                    trip_rec.StartDate,
                    trip_rec.EndDate
                );
            END LOOP;

            K1.EXTEND;
            K1(K1.LAST) := PersonType(
                person_rec.EmployeeID,
                person_rec.Name,
                person_rec.Address,
                K2 
            );
        END;
    END LOOP;

    DBMS_OUTPUT.PUT_LINE('Данные загружены в коллекции K1 и K2');

    -- Проверка загруженных данных
    FOR i IN 1 .. K1.COUNT LOOP
        DBMS_OUTPUT.PUT_LINE('Сотрудник: ' || K1(i).id || ' - ' || K1(i).name || ' - ' || K1(i).address);

        FOR j IN 1 .. K1(i).trips.COUNT LOOP
            DBMS_OUTPUT.PUT_LINE('  Командировка: ' || 
                'ID=' || K1(i).trips(j).id || 
                ', Бонус=' || K1(i).trips(j).bonus ||
                ', Даты=' || TO_CHAR(K1(i).trips(j).startDate, 'YYYY-MM-DD') || ' - ' ||
                TO_CHAR(K1(i).trips(j).endDate, 'YYYY-MM-DD')
            );
        END LOOP;
    END LOOP;


    DBMS_OUTPUT.PUT_LINE(CHR(10) || 'Проверяем пересечение вложенных коллекций K2 (Trips)' || CHR(10));


    -- b. Выяснить для каких коллекций К1 коллекции К2 пересекаются
    FOR i IN 1 .. K1.COUNT LOOP
        FOR j IN 1 .. K1(i).trips.COUNT LOOP
            FOR k IN i+1 .. K1.COUNT LOOP
                FOR m IN 1 .. K1(k).trips.COUNT LOOP
                    IF K1(i).trips(j).id = K1(k).trips(m).id THEN
                        DBMS_OUTPUT.PUT_LINE('Пересечение. Сотрудник ' || K1(i).id || ' - ' || K1(i).name ||
                        ' и ' || K1(k).id || ' - ' || K1(m).name || ' в командировке ' || K1(i).trips(j).id);
                    END IF;
                END LOOP;
            END LOOP;
        END LOOP;
    END LOOP;

    DBMS_OUTPUT.PUT_LINE(CHR(10) || 'Проверяем, является ли 1 членом коллекции K1' || CHR(10));

    -- c. Выяснить, является ли членом коллекции К1 какой-то произвольный элемент

    FOR i IN 1 .. K1.COUNT LOOP
        IF K1(i).id = 1 THEN 
            is_Member := 1;
            EXIT;
        END IF;
    END LOOP;
    IF is_Member = 1 THEN
        DBMS_OUTPUT.PUT_LINE('Person с ID = 1 находится в коллекции K1');
    ELSE
        DBMS_OUTPUT.PUT_LINE('Person с ID = 1 не находится в коллекции K1');
    END IF;

    DBMS_OUTPUT.PUT_LINE(CHR(10) || 'Ищем пустые коллекции' || CHR(10));

    -- d. Найти пустые коллекции К1

    FOR i IN 1 .. K1.COUNT LOOP
        IF K1(i).trips IS EMPTY THEN
            DBMS_OUTPUT.PUT_LINE('Пустая коллекция K1: Person_ID = ' || K1(i).id);
        END IF;
    END LOOP;

    DBMS_OUTPUT.PUT_LINE(CHR(10) || 'Обмен данными между элементами K1' || CHR(10));

    -- e. Для двух элементов коллекции К1 обменять их атрибуты К2

    IF K1.COUNT > 1 THEN 
        DECLARE
            temp_Trips TripTableType;
        BEGIN
            temp_Trips := K1(1).trips;
            K1(1).trips := K1(2).trips;
            K1(2).trips := temp_Trips;

            DBMS_OUTPUT.PUT_LINE('Обмен атрибутами K2 между двумя элементами K1 сделан');
        END;
        FOR i IN 1 .. K1.COUNT LOOP
            DBMS_OUTPUT.PUT_LINE('Сотрудник: ' || K1(i).id || ' - ' || K1(i).name || ' - ' || K1(i).address);

            FOR j IN 1 .. K1(i).trips.COUNT LOOP
                DBMS_OUTPUT.PUT_LINE('  Командировка: ' || 
                    'ID=' || K1(i).trips(j).id || 
                    ', Бонус=' || K1(i).trips(j).bonus ||
                    ', Даты=' || TO_CHAR(K1(i).trips(j).startDate, 'YYYY-MM-DD') || ' - ' ||
                    TO_CHAR(K1(i).trips(j).endDate, 'YYYY-MM-DD')
                );
            END LOOP;
        END LOOP;
    END IF;
    COMMIT;

END;



-- 3.Преобразовать коллекцию к другому виду (к реляционным данным).

DECLARE
    K2 TripTableType := TripTableType();
BEGIN
    SELECT TripType(BusinessTripID, Bonus, StartDate, EndDate)
    BULK COLLECT INTO K2
    FROM BusinessTrip;
    
    FOR rec IN (SELECT * FROM TABLE(K2)) LOOP
        DBMS_OUTPUT.PUT_LINE('BusinessTripID: ' || rec.id || ', Bonus: ' || rec.bonus);
    END LOOP;
END;



-- 4.	Продемонстрировать применение BULK операций на примере своих коллекций

DECLARE
    K2 TripTableType := TripTableType();
BEGIN
    SELECT TripType(BusinessTripID, Bonus, StartDate, EndDate)
    BULK COLLECT INTO K2
    FROM BusinessTrip;

    DBMS_OUTPUT.PUT_LINE('Загружено туров: ' || K2.COUNT);
END;
