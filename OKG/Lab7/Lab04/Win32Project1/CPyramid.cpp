#include "CPyramid.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath> 
#include <afxwin.h> 

#define PI 3.14159265358979323846
using namespace std;

CPyramid::CPyramid()
{
    // Координаты вершин в мировой системе
    //      A    A1   B    B1   C    C1
    // X:   3    1    0    0    0    0
    // Y:   0    0    0    0    3    1
    // Z:   0    3    0    3    0    3
    // W:   1    1    1    1    1    1
    this->Vertices.RedimMatrix(4, 6);
    this->Vertices(0, 0) = 3; this->Vertices(1, 0) = 0; this->Vertices(2, 0) = 0; // A (индекс 0)
    this->Vertices(0, 1) = 1; this->Vertices(1, 1) = 0; this->Vertices(2, 1) = 3; // A1 (индекс 1)
    this->Vertices(0, 2) = 0; this->Vertices(1, 2) = 0; this->Vertices(2, 2) = 0; // B (индекс 2)
    this->Vertices(0, 3) = 0; this->Vertices(1, 3) = 0; this->Vertices(2, 3) = 3; // B1 (индекс 3)
    this->Vertices(0, 4) = 0; this->Vertices(1, 4) = 3; this->Vertices(2, 4) = 0; // C (индекс 4)
    this->Vertices(0, 5) = 0; this->Vertices(1, 5) = 1; this->Vertices(2, 5) = 3; // C1 (индекс 5)

    for (int i = 0; i < 6; i++)
    {
        this->Vertices(3, i) = 1;
    }
}

// Функция создания матрицы преобразования из Мировой Системы Координат (МСК) в Видовую (ВСК)
// r, fi, q - сферические координаты точки наблюдения (камеры) в МСК
// fi - азимут (угол в плоскости XY от оси X), q - угол места (угол с плоскостью XY)

CMatrix CreateViewCoord(double r, double fi, double q)
{
    // Переводим углы в радианы
    double fi_rad = (fi / 180.0) * PI;
    double q_rad = (q / 180.0) * PI;


    // 1. Расчет декартовых координат камеры в МСК
    double camX = r * cos(q_rad) * cos(fi_rad);
    double camY = r * cos(q_rad) * sin(fi_rad);
    double camZ = r * sin(q_rad);

    CMatrix T_cam(3); T_cam(0) = camX; T_cam(1) = camY; T_cam(2) = camZ;

    // 2. Построение матрицы преобразования МСК -> ВСК (View Matrix)
    CMatrix Zview_world(3);
    Zview_world(0) = camX;
    Zview_world(1) = camY;
    Zview_world(2) = camZ;
    double lenZ = sqrt(ScalarMult(Zview_world, Zview_world));
    if (lenZ > 1e-9) { 
        Zview_world(0) /= lenZ; Zview_world(1) /= lenZ; Zview_world(2) /= lenZ;
    }
    else {
        Zview_world(0) = 0; Zview_world(1) = 0; Zview_world(2) = 1; // Смотрит вдоль +Z мир
    }

    CMatrix Up_world(3); Up_world(0) = 0; Up_world(1) = 0; Up_world(2) = 1.0;

    CMatrix Xview_world = VectorMult(Up_world, Zview_world);
    double lenX = sqrt(ScalarMult(Xview_world, Xview_world));
    if (lenX < 1e-9) {
        Xview_world(0) = 1.0; Xview_world(1) = 0.0; Xview_world(2) = 0.0;
    }
    else {
        Xview_world(0) /= lenX; Xview_world(1) /= lenX; Xview_world(2) /= lenX;
    }

    CMatrix Yview_world = VectorMult(Zview_world, Xview_world);

    /*
    
            | -sin(φ_v)            cos(φ_v)              0           0 |
        M = | -cos(θ_v)cos(φ_v)   -cos(θ_v)sin(φ_v)    sin(θ_v)      0 |
            | -sin(θ_v)cos(φ_v)   -sin(θ_v)sin(φ_v)   -cos(θ_v)     r_v|  
            |      0                    0                   0        1 |

    */

    CMatrix K(4, 4);
    K(0, 0) = Xview_world(0); K(0, 1) = Xview_world(1); K(0, 2) = Xview_world(2);
    K(1, 0) = Yview_world(0); K(1, 1) = Yview_world(1); K(1, 2) = Yview_world(2);
    K(2, 0) = Zview_world(0); K(2, 1) = Zview_world(1); K(2, 2) = Zview_world(2);
    K(0, 3) = -ScalarMult(Xview_world, T_cam);
    K(1, 3) = -ScalarMult(Yview_world, T_cam);
    K(2, 3) = -ScalarMult(Zview_world, T_cam);
    K(3, 0) = 0; K(3, 1) = 0; K(3, 2) = 0; K(3, 3) = 1.0;

    return K;
}

// Функция Draw1 - РИСУЕТ ТОЛЬКО ВИДИМЫЕ ГРАНИ (с заливкой и удалением нелицевых граней)
// dc - контекст устройства для рисования
// PView - вектор (3x1) с координатами точки наблюдения [r, fi, q] в МСК
// RW - прямоугольник в окне для отображения (оконные координаты)
void CPyramid::Draw1(CDC& dc, CMatrix& PView, CRect& RW)
{
    // 1. Матрица преобразования: Мир -> Вид
    CMatrix XV = CreateViewCoord(PView(0), PView(1), PView(2));

    // 2. Преобразуем все вершины пирамиды в видовые координаты
    CMatrix ViewVert = XV * this->Vertices;

    // 3. Преобразование: Вид -> Окно 
    // Определяем охватывающий прямоугольник RectView в ВИДОВЫХ координатах (Xv, Yv).
    CRectD RectView;
    RectView.left = ViewVert.GetRow(0, 0, 5).MinElement();   // Min X_view
    RectView.right = ViewVert.GetRow(0, 0, 5).MaxElement();  // Max X_view
    RectView.bottom = ViewVert.GetRow(1, 0, 5).MinElement(); // Min Y_view (Внимание: в CRectD bottom обычно < top)
    RectView.top = ViewVert.GetRow(1, 0, 5).MaxElement();    // Max Y_view

    const double min_size = 1e-6;
    if (fabs(RectView.right - RectView.left) < min_size) {
        RectView.left -= 0.5; RectView.right += 0.5;
    }
    if (fabs(RectView.top - RectView.bottom) < min_size) {
        RectView.bottom -= 0.5; RectView.top += 0.5;
    }

    // Получаем матрицу преобразования из видовых координат (XY) в оконные
    CMatrix MW = SpaceToWindow(RectView, RW);

    // 4. Определение граней пирамиды
    // Порядок вершин задаем против часовой стрелки (CCW), если смотреть СНАРУЖИ объекта.
    const int faces[][4] = {
        { 0, 2, 4, -1}, // Нижняя грань ABC (A->B->C). Нормаль в мировой СК: (0,0,-9) -> вниз.
        { 1, 5, 3, -1}, // Верхняя грань A1C1B1 (A1->C1->B1). Нормаль в мировой СК: (0,0,1) -> вверх.
        { 0, 4, 5, 1 }, // Боковая грань ACC1A1 (A->C->C1->A1). Нормаль в мировой СК: (9,9,6).
        { 2, 0, 1, 3 }, // Боковая грань BAA1B1 (B->A->A1->B1). Нормаль в мировой СК: (0,-9,0).
        { 4, 2, 3, 5 }  // Боковая грань CBB1C1 (C->B->B1->C1). Нормаль в мировой СК: (-9,0,0).
    };
    const int face_counts[] = { 3, 3, 4, 4, 4 }; // Количество вершин в каждой грани
    
    // 5. Настройка инструментов рисования GDI
    CPen Pen(PS_SOLID, 1, RGB(0, 0, 0)); // Черное перо толщиной 1 для контуров
    CPen* pOldPen = dc.SelectObject(&Pen);

    // Кисти для заливки граней разными цветами
    CBrush BrushBottom(RGB(255, 100, 100)); // Красная для нижней
    CBrush BrushTop(RGB(100, 255, 100));   // Зеленая для верхней
    CBrush BrushSide1(RGB(100, 100, 255)); // Синяя для AC1A1
    CBrush BrushSide2(RGB(255, 255, 100)); // Желтая для BA1B1
    CBrush BrushSide3(RGB(100, 255, 255)); // Голубая для CB1C1
    CBrush* brushes[] = { &BrushBottom, &BrushTop, &BrushSide1, &BrushSide2, &BrushSide3 };
    CBrush* pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH); // Начальная кисть

    // 6. Цикл обработки и рисования каждой грани
    for (int i = 0; i < 5; ++i) // Проходим по всем 5 граням
    {
        // Получаем 3D видовые координаты первых трех вершин грани
        CMatrix P1_view = ViewVert.GetCol(faces[i][0], 0, 2); 
        CMatrix P2_view = ViewVert.GetCol(faces[i][1], 0, 2);
        CMatrix P3_view = ViewVert.GetCol(faces[i][2], 0, 2);

        // Вычисляем два вектора, лежащих на грани, в видовых координатах
        CMatrix V1_view = P2_view - P1_view; // Вектор от P1 к P2
        CMatrix V2_view = P3_view - P1_view; // Вектор от P1 к P3

        // Вычисляем нормаль грани N = V1 x V2 в видовых координатах
        CMatrix N_view = VectorMult(V1_view, V2_view);
        CMatrix ViewVec = -P1_view;

        // Вычисляем скалярное произведение нормали и вектора взгляда
        double dot = ScalarMult(N_view, ViewVec);

        // --- Проверка видимости грани ---
        // Если dot > 0, угол между нормалью и вектором взгляда < 90 градусов, грань видна.
        if (dot > 0)
        {
            // Преобразуем видовые координаты вершин (Xv, Yv) в оконные (Xs, Ys)
            CPoint screen_points[4]; // Массив для экранных координат (максимум 4 вершины)
            int nVerts = face_counts[i]; // Количество вершин в текущей грани

            for (int j = 0; j < nVerts; ++j)
            {
                int vertex_index = faces[i][j]; // Индекс текущей вершины в ViewVert

                // Создаем вектор (Xv, Yv, 1) для 2D аффинного преобразования матрицей MW
                CMatrix V_project(3);
                V_project(0) = ViewVert(0, vertex_index); // X_view
                V_project(1) = ViewVert(1, vertex_index); // Y_view
                V_project(2) = 1.0;                     // W=1 для преобразования MW

                // Применяем матрицу MW (View -> Window)
                CMatrix V_screen = MW * V_project;

                // Записываем экранные координаты, округляя до целых пикселей
                screen_points[j].x = (int)round(V_screen(0));
                screen_points[j].y = (int)round(V_screen(1));
            }

            // Выбираем соответствующую кисть для заливки
            pOldBrush = dc.SelectObject(brushes[i]);

            // Рисуем полигон (залитую грань с контуром)
            dc.Polygon(screen_points, nVerts);

            // Возвращаем старую кисть 
            dc.SelectObject(pOldBrush);
        }
        // else { // Грань невидима (dot <= epsilon) - пропускаем ее, ничего не рисуем }
    }

    // 7. Восстановление старых объектов GDI
    dc.SelectObject(pOldPen);
    dc.SelectObject(pOldBrush); // Восстанавливаем исходную кисть

    // 8. Вывод текущих углов камеры (для отладки)
    CString strFi, strQ;
    strFi.Format(_T("Fi (azimuth): %d"), (int)round(PView(1)));
    strQ.Format(_T("Q (elevation): %d"), (int)round(PView(2)));
    dc.TextOut(10, 10, strFi); 
    dc.TextOut(10, 30, strQ);
}


// Функция Draw - РИСУЕТ ВСЕ РЕБРА (КАРКАС, wireframe)
// dc - контекст устройства для рисования
// PView - вектор (3x1) с координатами точки наблюдения [r, fi, q] в МСК
// RW - прямоугольник в окне для отображения (оконные координаты)
void CPyramid::Draw(CDC& dc, CMatrix& PView, CRect& RW)
{
    // 1. Матрица преобразования: Мир -> Вид (World -> View)
    CMatrix XV = CreateViewCoord(PView(0), PView(1), PView(2));

    // 2. Преобразуем все вершины пирамиды в видовые координаты
    CMatrix ViewVert = XV * this->Vertices;

    // 3. Настройка преобразования: Вид -> Окно (View -> Window)
    // Определяем охватывающий прямоугольник RectView в ВИДОВЫХ координатах (Xv, Yv)
    CRectD RectView;
    RectView.left = ViewVert.GetRow(0, 0, 5).MinElement();
    RectView.right = ViewVert.GetRow(0, 0, 5).MaxElement();
    RectView.bottom = ViewVert.GetRow(1, 0, 5).MinElement();
    RectView.top = ViewVert.GetRow(1, 0, 5).MaxElement();
    // Обработка вырожденных случаев
    const double min_size = 1e-6;
    if (fabs(RectView.right - RectView.left) < min_size) {
        RectView.left -= 0.5; RectView.right += 0.5;
    }
    if (fabs(RectView.top - RectView.bottom) < min_size) {
        RectView.bottom -= 0.5; RectView.top += 0.5;
    }

    // Получаем матрицу преобразования из видовых координат (XY) в оконные
    CMatrix MW = SpaceToWindow(RectView, RW);

    // 4. Преобразуем видовые координаты ВСЕХ вершин в оконные
    CPoint MasVert[6]; // Массив для хранения экранных координат вершин
    CMatrix V_project(3);
    V_project(2) = 1.0; // W=1 для преобразования MW
    for (int i = 0; i < 6; i++)
    {
        V_project(0) = ViewVert(0, i); // X_view
        V_project(1) = ViewVert(1, i); // Y_view
        CMatrix V_screen = MW * V_project;
        MasVert[i].x = (int)round(V_screen(0));
        MasVert[i].y = (int)round(V_screen(1));
    }

    // 5. Настройка пера для рисования линий
    CPen Pen(PS_SOLID, 1, RGB(0, 0, 0)); // Тонкое черное перо
    CPen* pOldPen = dc.SelectObject(&Pen);

    // 6. Рисование ребер (линий между вершинами)

    // Ребра нижнего основания (ABC)
    dc.MoveTo(MasVert[0]); dc.LineTo(MasVert[2]); // A-B
    dc.MoveTo(MasVert[2]); dc.LineTo(MasVert[4]); // B-C
    dc.MoveTo(MasVert[4]); dc.LineTo(MasVert[0]); // C-A

    // Ребра верхнего основания (A1B1C1)
    dc.MoveTo(MasVert[1]); dc.LineTo(MasVert[3]); // A1-B1
    dc.MoveTo(MasVert[3]); dc.LineTo(MasVert[5]); // B1-C1
    dc.MoveTo(MasVert[5]); dc.LineTo(MasVert[1]); // C1-A1

    // Боковые ребра (соединяющие основания)
    dc.MoveTo(MasVert[0]); dc.LineTo(MasVert[1]); // A-A1
    dc.MoveTo(MasVert[2]); dc.LineTo(MasVert[3]); // B-B1
    dc.MoveTo(MasVert[4]); dc.LineTo(MasVert[5]); // C-C1

    // 7. Восстановление старого пера
    dc.SelectObject(pOldPen);

    // 8. Вывод текущих углов камеры 
    CString strFi, strQ;
    strFi.Format(_T("Fi (azimuth): %d"), (int)round(PView(1))); 
    strQ.Format(_T("Q (elevation): %d"), (int)round(PView(2)));
    dc.TextOut(10, 10, strFi); 
    dc.TextOut(10, 30, strQ);
}

// Функция GetRect - определяет охватывающий прямоугольник

void  CPyramid::GetRect(CMatrix& Vert, CRectD& RectView)
{
    // Определяем min/max по X и Y координатам из ПЕРЕДАННОЙ матрицы Vert
    RectView.left = Vert.GetRow(0, 0, 5).MinElement(); // Min X (строка 0)
    RectView.right = Vert.GetRow(0, 0, 5).MaxElement(); // Max X (строка 0)
    RectView.bottom = Vert.GetRow(1, 0, 5).MinElement(); // Min Y (строка 1)
    RectView.top = Vert.GetRow(1, 0, 5).MaxElement(); // Max Y (строка 1)

    // Небольшая корректировка на случай нулевой ширины/высоты
    const double min_size = 1e-6;
    if (fabs(RectView.right - RectView.left) < min_size) {
        RectView.left -= 0.5; RectView.right += 0.5;
    }
    if (fabs(RectView.top - RectView.bottom) < min_size) {
        RectView.bottom -= 0.5; RectView.top += 0.5;
    }
}

// Функция вычисления Векторного Произведения двух 3D векторов

CMatrix VectorMult(CMatrix& V1, CMatrix& V2)
{
    if (V1.rows() != 3 || V2.rows() != 3 || V1.cols() != 1 || V2.cols() != 1)
    {
        char error[] = "VectorMult: Операнды должны быть 3x1 векторами.";
        MessageBoxA(NULL, error, "Ошибка CMatrix", MB_ICONSTOP | MB_OK);
        exit(1); 
    }

    CMatrix Temp(3); // Результат - вектор 3x1
    Temp(0) = V1(1) * V2(2) - V1(2) * V2(1); // Rx = Y1*Z2 - Z1*Y2
    Temp(1) = V1(2) * V2(0) - V1(0) * V2(2); // Ry = Z1*X2 - X1*Z2
    Temp(2) = V1(0) * V2(1) - V1(1) * V2(0); // Rz = X1*Y2 - Y1*X2

    return Temp;
}

// Функция вычисления Скалярного Произведения двух 3D векторов

double ScalarMult(CMatrix& V1, CMatrix& V2)
{
    if (V1.rows() != 3 || V2.rows() != 3 || V1.cols() != 1 || V2.cols() != 1)
    {
        char error[] = "ScalarMult: Операнды должны быть 3x1 векторами.";
        MessageBoxA(NULL, error, "Ошибка CMatrix", MB_ICONSTOP | MB_OK);
        exit(1); 
    }

    return V1(0) * V2(0) + V1(1) * V2(1) + V1(2) * V2(2); // X1*X2 + Y1*Y2 + Z1*Z2
}