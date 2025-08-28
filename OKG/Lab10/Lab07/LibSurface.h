using namespace std;


typedef vector<CMatrix> CVecMatrix;
typedef vector<CVecMatrix> CMasMatrix;

typedef vector<CPoint> CVecPoint;
typedef vector<CVecPoint> CMatrPoint;


typedef double(*pfunc2)(double, double);// Указатель на функцию


class CPlot3D
{
private:
	pfunc2 pFunc;          // Указатель на функцию f(x,y), описывающая поверхность 
	CRectD SpaceRect;      // Прямоугольник области, на которую опирается поверхность
	CMasMatrix MatrF;      // Матрица для хранения координат точек (x,y,z,1) поверхности
	CMasMatrix MatrView;   // Матрица для хранения координат точек (x,y,1) проекции XY видовой СК
	CRectD ViewRect;       // Прямоугольная область, охватывающая проекцию поверхности видовой СК
	CRect WinRect;         // Прямоугольная область в оконной системе координат для
	CMatrix ViewPoint;     // Вектор (3x1) – координаты точки наблюдения в мировой сферической СК (r,fi,q)
	CMatrPoint MatrWindow; // Матрица для хранения оконных координат

public:
	CPlot3D();
	~CPlot3D() { pFunc = NULL; }
	void SetFunction(pfunc2 pF, CRectD RS, double dx, double dy);
	void SetViewPoint(double r, double fi, double q);

	CMatrix GetViewPoint();                 // Возвращает вектор ViewPoint
	void SetWinRect(CRect Rect);            // Устанавливает область в окне для рисования
	void CreateMatrF(double dx, double dy); // Заполняет матрицу MatrF координатами
	void SetMatrF(CMasMatrix &Matr);        // Задает значение матрицы MatrF извне
	void CreateMatrView();                  // Заполняет матрицу MatrView координатами точек проекции XY видовой СК
	void CreateMatrWindow();                // Заполняет матрицу MatrWindow
	int GetNumberRegion();                  // Определяет номер области для рисования
	void Draw(CDC& dc);
};


double Function1(double x, double y)
{
	return x*x + y*y;
}
double Function2(double x, double y)
{
	return x*x - y*y;
}
double Function3(double x, double y)
{
	//return sqrt(1 - x*x - y*y);
	double r = sqrt(x*x + y*y);
	double z;
	if (r < 0.001)
		z = 10;
	else
		z = 10 * sin(r) / r;
	return z;
}


CPlot3D::CPlot3D()
{
	pFunc = NULL;
	ViewPoint.RedimMatrix(3);
	WinRect.SetRect(0, 0, 200, 200);
	ViewPoint(0) = 10, ViewPoint(1) = 30, ViewPoint(2) = 45;
}
void CPlot3D::SetFunction(pfunc2 pF, CRectD RS, double dx, double dy)
// Устанавливает указатель на фукцию f(x,y)
// pFunc – указатель на функцию f(x,y) – поверхность
// RS – область  МСК (xmin, ymax, xmax, ymin) для расчета
// dx, dy – шаги для расчета значений f(x) по x и y
// VP – координаты точки наблюдения (r,fi,q) в сферической СК
// RW – область в окне для рисования
{
	pFunc = pF;
	SpaceRect.SetRectD(RS.left, RS.top, RS.right, RS.bottom);
	MatrF.clear();
	MatrView.clear();
	MatrWindow.clear();

	CreateMatrF(dx, dy);
	CreateMatrView();
	CreateMatrWindow();

}
void CPlot3D::SetViewPoint(double r, double fi, double q)
// Устанавливает положение точки наблюдения в МИРОВОЙ СК
// –заполняет матрицу ViewPoint
// r – модуль радиус-вектора
// fi – азимут (от оси OX против часовой стрелки)
// q – угол (от оси Z по часовой стрелке, 0-180)
{
	ViewPoint(0) = r, ViewPoint(1) = fi, ViewPoint(2) = q;
	MatrView.clear();
	CreateMatrView();
	MatrWindow.clear();
	CreateMatrWindow();
}
CMatrix CPlot3D::GetViewPoint()
{
	CMatrix P = ViewPoint;
	return P;
}
void CPlot3D::SetWinRect(CRect Rect)
{
	WinRect = Rect;
	MatrWindow.clear();
	CreateMatrWindow();
}
void CPlot3D::CreateMatrF(double dx, double dy)
// Заполняет матрицу MatrF координатами точек поверхности
// dx – шаг для расчета значений функции f(x,y) оси
// dy – шаг для расчета значений функции f(x,y) оси
{
	double xL = SpaceRect.left;
	double xH = SpaceRect.right;
	double yL = SpaceRect.bottom;
	double yH = SpaceRect.top;
	CVecMatrix VecMatrix;
	CMatrix V(4);
	V(3) = 1;

	for (double x = xL; x <= xH; x += dx)
	{
		VecMatrix.clear();
		for (double y = yL; y <= yH; y += dy)
		{
			V(0) = x; V(1) = y; V(2) = pFunc(x, y);
			VecMatrix.push_back(V);
		}
		MatrF.push_back(VecMatrix);
	}
}
void CPlot3D::SetMatrF(CMasMatrix &Matr)
// Задает значение матрицы MatrF извне
// Matr – новове значение для MatrF
{
	CVecMatrix VecMatrix;
	CMatrix V(4);
	double xmin, xmax, ymin, ymax;
	pFunc = NULL;
	MatrF.clear();
	MatrView.clear();
	MatrWindow.clear();
	V = Matr[0][0];
	xmin = xmax = V(0);
	ymin = ymax = V(1);
	for (int i = 0; i < Matr.size(); i++)
	{
		VecMatrix.clear();
		for (int j = 0; j < Matr[i].size(); j++)
		{
			V = Matr[i][j];
			VecMatrix.push_back(V);
			double x = V(0);
			double y = V(1);
			if (x < xmin) xmin = x;
			if (x < xmax) xmax = x;
			if (y < ymin) ymin = y;
			if (y < ymax) ymax = y;
		}
		MatrF.push_back(VecMatrix);
	}
	SpaceRect.SetRectD(xmin, ymax, xmax, ymin);
	CreateMatrView();
	CreateMatrWindow();
}
int CPlot3D::GetNumberRegion()
// Определяет номер области для рисования
{
	CMatrix CartPoint = SphereToCart(ViewPoint);
	double xView = CartPoint(0);
	double yView = CartPoint(1);
	double zView = CartPoint(2);

	double xL = SpaceRect.left;
	double xH = SpaceRect.right;
	double yL = SpaceRect.bottom;
	double yH = SpaceRect.top;

	// Определяем где нахоится точка наблюдения относительно
	// получаем уравнение диагонали y1=y1(x) [точки (x)]
	double y1 = yL + (yH - yL)*(xView - xL) / (xH - xL);
	// получаем уравнение диагонали y2=y2(x) [точки (x)]
	double y2 = yH + (yH - yL)*(xView - xL) / (xH - xL);
	if ((yView <= y1) && (yView <= y2)) 
		return 1;
	if ((yView > y2) && (yView < y1)) 
		return 2;
	if ((yView >= y1) && (yView >= y2)) 
		return 3;
	if ((yView > y1) && (yView < y2)) 
		return 4;
}
void CPlot3D::CreateMatrView()
// Заполняет матрицу MatrView координатами точек проекции поверхности f(x,y)
// на плоскость XY видовой СК
// И
// Определяет прямоугольная область ViewRect, охватывающая проекцию
// поверхности на плоскость XY видовой СК
{
	CMatrix MV = CreateViewCoord(ViewPoint(0), ViewPoint(1), ViewPoint(2));
	CVecMatrix VecMatrix;
	CMatrix VX(4), V(3);
	V(2) = 1;
	double xmin = DBL_MAX;
	double xmax = DBL_MIN;
	double ymin = DBL_MAX;
	double ymax = DBL_MIN;

	for (int i = 0; i < MatrF.size(); i++)
	{
		VecMatrix.clear();
		for (int j = 0; j < MatrF[i].size(); j++)
		{
			VX = MatrF[i][j];
			VX = MV*VX;
			V(0) = VX(0); // x – координата проекции
			V(1) = VX(1); // y – координата проекции
			VecMatrix.push_back(V);

			// Для определения области ViewRect
			double x = V(0);
			double y = V(1);
			if (x < xmin) xmin = x;
			if (x > xmax) xmax = x;
			if (y < ymin) ymin = y;
			if (y < ymax) xmin = y;
		}
		MatrView.push_back(VecMatrix);
	}
	ViewRect.SetRectD(xmin, ymax, xmax, ymin);
}
void CPlot3D::CreateMatrWindow()
// Заполняет матрицу MatrWindow оконными координатами точки изображения
{
	CMatrix MW = SpaceToWindow(ViewRect, WinRect);

	CVecPoint VecPoint;
	CMatrix V(3);
	for (int i = 0; i < MatrView.size(); i++)
	{
		VecPoint.clear();
		for (int j = 0; j < MatrView[i].size(); j++)
		{
			V = MatrView[i][j];
			V = MW*V;
			CPoint P((int)V(0), (int)V(1));
			VecPoint.push_back(P);
		}
		MatrWindow.push_back(VecPoint);
	}
}
void CPlot3D::Draw(CDC& dc)
{
	if (MatrWindow.empty())
	{
		TCHAR* error = TEXT("Массив данных для рисования пуст");
		MessageBox(NULL, error, TEXT("Ошибка"), MB_ICONSTOP);
		return;
	}
	CPoint pt[4];
	int kRegion = GetNumberRegion();
	int nRows = MatrWindow.size();
	int nCols = MatrWindow[0].size();

	switch (kRegion)
	{
	case 1:
	{
		for (int j = nCols - 1; j > 0; j--)
			for (int i = 0; i < nRows - 1; i++)
			{
				pt[0] = MatrWindow[i][j];
				pt[1] = MatrWindow[i][j - 1];
				pt[2] = MatrWindow[i + 1][j - 1];
				pt[3] = MatrWindow[i + 1][j];
				dc.Polygon(pt, 4);
			}
	} break;
	case 2:
	{
		for (int i = 0; i < nRows - 1; i++)
			for (int j = 0; i < nCols - 1; j++)
			{
				pt[0] = MatrWindow[i][j];
				pt[1] = MatrWindow[i][j + 1];
				pt[2] = MatrWindow[i + 1][j + 1];
				pt[3] = MatrWindow[i + 1][j];
				dc.Polygon(pt, 4);
			}
	} break;
	case 3:
	{
		for (int j = 0; j < nCols; j++)
			for (int i = 0; i < nRows - 1; i++)
			{
				pt[0] = MatrWindow[i][j];
				pt[1] = MatrWindow[i][j + 1];
				pt[2] = MatrWindow[i + 1][j + 1];
				pt[3] = MatrWindow[i + 1][j];
				dc.Polygon(pt, 4);
			}
	} break;
	case 4:
	{
		for (int i = nRows - 1; i > 0; i--)
			for (int j = 0; i < nCols - 1; j++)
			{
				pt[0] = MatrWindow[i][j];
				pt[1] = MatrWindow[i][j + 1];
				pt[2] = MatrWindow[i - 1][j + 1];
				pt[3] = MatrWindow[i - 1][j];
				dc.Polygon(pt, 4);
			}
	} break;
	}
}


void DrawLightSphere(CDC& dc, double Radius, CMatrix& PView_Observer, CMatrix& PSourceLight, CRect RW, COLORREF Color, int Index)
// PView_Observer - НАБЛЮДАТЕЛЬ (сферические r, phi, theta)
// PSourceLight - ИСТОЧНИК СВЕТА (сферические r, phi, theta)
{
	BYTE R_component = GetRValue(Color);
	BYTE G_component = GetGValue(Color);
	BYTE B_component = GetBValue(Color);

	CMatrix point_on_sphere_sph(3);    // (r, phi, theta) для текущей точки на сфере
	CMatrix P_world_cart(3);           // Текущая точка на сфере в декартовых мировых координатах
	CMatrix N_world_norm(3);           // Нормаль к сфере в точке P_world_cart (нормализованная)
	CMatrix L_world_norm(3);           // Вектор от точки P_world_cart к источнику света (нормализованный)
	CMatrix V_world_norm(3);           // Вектор от точки P_world_cart к наблюдателю (нормализованный)
	CMatrix P_view_homogeneous(4);     // Точка в видовых гомогенных координатах
	CMatrix P_screen_coords(3);        // Экранные координаты (x, y, 1)

	COLORREF finalPixelColor;
	double df_step = 0.7; // Шаг по азимуту (phi). Подбери значение.
	double dq_step = 0.7; // Шаг по углу места (theta). Подбери значение.
	double kLightIntensity; // Итоговый коэффициент освещенности [0,1]

	// 1. Устанавливаем радиус для точек на сфере
	point_on_sphere_sph(0) = 2;

	// 2. Преобразуем положения наблюдателя и источника света в декартовы мировые координаты
	CMatrix V_Observer_world_cart = SphereToCart(PView_Observer);
	CMatrix V_SourceLight_world_cart = SphereToCart(PSourceLight);

	// 3. Создаем матрицу вида МСК -> ВСК
	CMatrix MV_world_to_view = CreateViewCoord(PView_Observer(0), PView_Observer(1), PView_Observer(2));

	// 4. Определяем область проекции в видовой системе координат (XY плоскость)
	CRectD RV_view_projection_plane(-Radius, Radius, Radius, -Radius); // left, top, right, bottom

	// 5. Создаем матрицу преобразования из ВСК -> ОСК
	CMatrix MW_view_to_screen = SpaceToWindow(RV_view_projection_plane, RW);

	// Коэффициенты материала для освещения
	double k_d = 0.7; // Диффузный коэффициент отражения материала
	double k_s = 1.0; // Зеркальный коэффициент отражения материала
	double shininess = 1.0; // Экспонента блеска

	// Проходим по точкам сферы
	for (double fi_deg = 0; fi_deg < 360.0; fi_deg += df_step)
	{
		point_on_sphere_sph(1) = fi_deg; // phi
		for (double q_deg = 0; q_deg <= 180.0; q_deg += dq_step)
		{
			point_on_sphere_sph(2) = q_deg; // theta

			// Декартовы координаты текущей точки на сфере в мировой СК
			P_world_cart = SphereToCart(point_on_sphere_sph);

			// --- Вектор нормали N (нормализованный) ---
			N_world_norm = P_world_cart; // Для сферы из (0,0,0) вектор к точке = нормаль
			double mod_N = ModVec(N_world_norm);
			if (mod_N > 1e-9) {
				N_world_norm(0) /= mod_N; N_world_norm(1) /= mod_N; N_world_norm(2) /= mod_N;
			}
			else { 
				N_world_norm(0) = 0; N_world_norm(1) = 0; N_world_norm(2) = 1; // Запасной вариант
			}

			// --- Вектор к наблюдателю V (нормализованный) ---
			CMatrix V_to_observer_temp = V_Observer_world_cart - P_world_cart;
			V_world_norm = V_to_observer_temp;
			double mod_V = ModVec(V_world_norm);
			if (mod_V > 1e-9) {
				V_world_norm(0) /= mod_V; V_world_norm(1) /= mod_V; V_world_norm(2) /= mod_V;
			} // else V_world_norm останется (0,0,0)

			// Отсечение невидимых граней (Back-face culling)
			// Точка видна, если скалярное произведение (N . V) > 0
			if (ScalarMult(N_world_norm, V_world_norm) <= 1e-6) 
			{
				continue; // Точка не видна наблюдателю, пропускаем ее
			}

			// --- Преобразование координат точки для отрисовки на экране ---
			P_view_homogeneous(0) = P_world_cart(0);
			P_view_homogeneous(1) = P_world_cart(1);
			P_view_homogeneous(2) = P_world_cart(2);
			P_view_homogeneous(3) = 1.0;
			P_view_homogeneous = MV_world_to_view * P_view_homogeneous; // В видовую СК

			// Для аксонометрической/ортографической проекции берем Xv, Yv
			P_screen_coords(0) = P_view_homogeneous(0); // X_view
			P_screen_coords(1) = P_view_homogeneous(1); // Y_view
			P_screen_coords(2) = 1.0;                   // Для 2D преобразования

			P_screen_coords = MW_view_to_screen * P_screen_coords; // В экранные координаты
			int screen_x = static_cast<int>(round(P_screen_coords(0)));
			int screen_y = static_cast<int>(round(P_screen_coords(1)));


			// --- Расчет освещенности ---
			kLightIntensity = 0.0; // Инициализация итоговой интенсивности

			// --- Вектор к источнику света L (нормализованный) ---
			CMatrix L_to_source_temp = V_SourceLight_world_cart - P_world_cart;
			L_world_norm = L_to_source_temp;
			double mod_L = ModVec(L_world_norm);
			if (mod_L > 1e-9) {
				L_world_norm(0) /= mod_L; L_world_norm(1) /= mod_L; L_world_norm(2) /= mod_L;
			}

			// Скалярное произведение нормали и вектора к свету
			double N_dot_L = ScalarMult(N_world_norm, L_world_norm);

			if (N_dot_L > 0) // Только если свет падает на лицевую сторону поверхности
			{
				// Диффузное освещение
				if (Index == 0)
				{
					double diffuse_intensity = k_d * N_dot_L;
					kLightIntensity += diffuse_intensity;
				}

				// Зеркальное освещение
				if (Index == 1)
				{
					// Вектор отражения R = 2 * (N.L) * N - L
					CMatrix R_reflected_world(3);
					R_reflected_world(0) = 2.0 * N_dot_L * N_world_norm(0) - L_world_norm(0);
					R_reflected_world(1) = 2.0 * N_dot_L * N_world_norm(1) - L_world_norm(1);
					R_reflected_world(2) = 2.0 * N_dot_L * N_world_norm(2) - L_world_norm(2);

					double R_dot_V = ScalarMult(R_reflected_world, V_world_norm);
					if (R_dot_V > 0)
					{
						double specular_intensity = k_s * pow(R_dot_V, shininess);
						kLightIntensity += specular_intensity;
					}
				}
			}
			// Ограничиваем итоговую интенсивность диапазоном [0, 1]
			kLightIntensity = max(0.0, min(1.0, kLightIntensity));

			finalPixelColor = RGB(
				static_cast<BYTE>(kLightIntensity * R_component),
				static_cast<BYTE>(kLightIntensity * G_component),
				static_cast<BYTE>(kLightIntensity * B_component)
			);

			dc.SetPixelV(screen_x, screen_y, finalPixelColor);
		}
	}
}