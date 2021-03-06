// Bai10.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Bai10.h"
#include "Winuser.h"

// Define Constant Variables 
#pragma region DEFINE
#define MAX_LOADSTRING 100
#define COLOR_RED RGB(255, 0, 0)
#define COLOR_ORANGE RGB(255, 128, 64)
#define COLOR_YELLOW RGB(255, 255, 0)
#define COLOR_GREEN RGB(0, 255, 0)
#define COLOR_BLUE RGB(0, 0, 255)
#define COLOR_BLACK RGB(0, 0, 0)
#define COLOR_GRAY RGB(128, 128, 128)
#define COLOR_WHITE RGB(255, 255, 255)
#pragma endregion

// Default Variables and functions
#pragma region DefaultVariablesAndFunctions
// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
												// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
#pragma endregion

// khai bao bien toan cuc
#pragma region KhaiBaoBien

int left, right, top, bottom;
PAINTSTRUCT ps;
HDC hdc;
POINT point[55000];
HPEN hPen1, hPen2; // bút vẽ dùng trong tool: pencil và eraser và shape.
HBRUSH hbrushMauNen; // dùng để đổ màu và tính năng fill của shape. 
// Lưu 2 giá trị màu sắc: màu vẽ và màu nền. 
COLORREF colorDraw = COLOR_BLACK;
COLORREF colorSec = COLOR_WHITE;
// open and save

//cursor
HCURSOR hCursor = LoadCursor(hInst, MAKEINTRESOURCE(CUR_PENCIL));

int n = 0;
// lấy các giá trị hiện tại. 
int firstTools = -1;
int firstTool = -1;

int cursorTools = TOOLS_TOOL;        //lay id cua loai cong cu hien tai. (Pencil or Eraser or Brush)
int cursorTool = TOOL_PENCIL;        //lay id cua cong cu hien tai. (Pencil - Fill color - Eraser - Color picker)
int cursorShape = -1;                // lay id cua hinh (Line - Rectangle - Elipse ...vv)
// Size
int Size1, Size2, Size3, Size4;
int cursorSize = 1;                  // lay SIZE hiện tại
// flag
bool isDraw = false;
bool isFill = false;  // 
/* -----------------   Edit color  ----------------------*/
COLORREF acrCustClr[16];
COLORREF color = RGB(0, 0, 0);

// file
OPENFILENAME ofn;       // common dialog box structure
char szFile[260];       // buffer for file name
HANDLE hf;              // file handle
#pragma endregion

// ham tu dinh nghia
#pragma region CacHamTuDinhNghia
// cac phuong thuc về công cụ - thiet lap con tro chuot
void processToolShape(HWND hWnd, int wmId);
void processToolPencil(HWND hWnd);
void processToolEraser(HWND hWnd);
void processToolFillColor(HWND hWnd);
void processToolColorPicker(HWND hWnd);
// cac phuong thuc tao but va choi
void setPen(HDC hdc, COLORREF colorDraw);
void setBrush(HDC hdc, COLORREF colorSec);
// cac phuong thuc ve mau sac. 
void EditColor(HWND hWnd);
// cac phuong thuc ve tool 
void drawPencil(HWND hWnd, HDC hdc);
// cac phuong thuc ve hinh
void drawShape(HWND hWnd, HDC hdc, int cursorShape);
void drawLine(HWND hWnd, HDC hdc);
void drawTriangle(HWND hWnd, HDC hdc);
void drawStarIV(HWND hWnd, HDC hdc);
void drawStarV(HWND hWnd, HDC hdc);
// cac phuong thuc mo va luu file
void OpenFile(HWND hWnd);
void SaveFile(HWND hWnd);
// set kich thuoc cua line and point
void setSize(int cursorTools, int cursorTool);
void setCurrentSize(int cursorSize);
#pragma endregion

// các hàm khởi tạo mặc định của hệ thống. 
#pragma region CacHamKhoiTaoMacDinhCuaHeThong
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_BAI10, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BAI10));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ICON_APPPAINT));
	wcex.hCursor = LoadCursor(nullptr, MAKEINTRESOURCE(CUR_PENCIL));
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_BAI10);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(ICON_APPPAINT));

	return RegisterClassExW(&wcex);
}
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}
#pragma endregion

// hàm xử lý các thông điệp của chương trình. 
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE: {
		setSize(cursorTools, cursorTool);
		processToolPencil(hWnd);
		break; }
	case WM_LBUTTONDOWN: {
		isDraw = true;
		if (cursorTools == TOOLS_TOOL && (cursorTool == TOOL_PENCIL || cursorTool == TOOL_ERASER)) {
			setPen(hdc, colorDraw);
			setBrush(hdc, colorSec);
			left = LOWORD(lParam);
			top = HIWORD(lParam);
			point[0].x = left;
			point[0].y = top;

		}
		else if (cursorTools == TOOLS_SHAPE) {
			left = LOWORD(lParam);
			top = HIWORD(lParam);
			point[0].x = left;
			point[0].y = top;
			/*point[n].x = LOWORD(lParam);
			point[n].y = HIWORD(lParam)*/;
		}
		break;
	}
	case WM_LBUTTONUP: {
		hdc = GetDC(hWnd);
		if (cursorTools == TOOLS_TOOL && (cursorTool == TOOL_PENCIL || cursorTool == TOOL_ERASER)) isDraw = false;
		else if (cursorTools == TOOLS_TOOL && cursorTool == TOOL_COLORPICKER) {
			colorDraw = GetPixel(hdc, LOWORD(lParam), HIWORD(lParam));
			cursorTools = firstTools;
			if (cursorTools != TOOLS_SHAPE) {
				if (firstTool == TOOL_ERASER)	processToolEraser(hWnd);
				else if (firstTool == TOOL_PENCIL) processToolPencil(hWnd);
				else if (firstTool == TOOL_ERASER) processToolEraser(hWnd);
			}
			else if (firstTools == TOOLS_SHAPE) processToolShape(hWnd, cursorShape);
			break;
		}
		if (cursorTools == TOOLS_SHAPE) {
			right = LOWORD(lParam);
			bottom = HIWORD(lParam);
			point[n].x = LOWORD(lParam);
			point[n].y = HIWORD(lParam);
			setPen(hdc, colorDraw);
			setBrush(hdc, colorSec);
			if (isDraw) { drawShape(hWnd, hdc, cursorShape); isDraw = false; }
			break;
		}
		break;
	}
	case WM_MOUSEMOVE: {
		n++;
		point[n].x = LOWORD(lParam);
		point[n].y = HIWORD(lParam);

		hdc = GetDC(hWnd);
		SelectObject(hdc, hPen1);
		if (isDraw) {
			if (cursorTools == TOOLS_TOOL && cursorTool == TOOL_PENCIL) {
				point[1].x = LOWORD(lParam);
				point[1].y = HIWORD(lParam);
				Polygon(hdc, point, 2);
				ReleaseDC(hWnd, hdc);
				point[0].x = point[1].x;
				point[0].y = point[1].y;
			}
			else if (cursorTools == TOOLS_TOOL && cursorTool == TOOL_ERASER) {
				point[1].x = LOWORD(lParam);
				point[1].y = HIWORD(lParam);
				hPen2 = CreatePen(PS_SOLID, cursorSize, COLOR_WHITE);
				SelectObject(hdc, hPen2);
				Polygon(hdc, point, 2);
				ReleaseDC(hWnd, hdc);
				point[0].x = point[1].x;
				point[0].y = point[1].y;
			}
			else if (cursorTools == TOOLS_BRUSH) {
				//drawPencil(hWnd);
			}
			else if (cursorTools == TOOLS_SHAPE) {
				//drawShape(hWnd, cursorShape);
			}
		}

		break;
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		int wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
			// menu  file
#pragma region MenuFile
		case IDM_NEW:   InvalidateRect(hWnd, NULL, TRUE); break;
		case IDM_OPEN: 	OpenFile(hWnd);	break;
		case IDM_SAVE:	SaveFile(hWnd);	break;
		case ID_EDITCOLOR:	EditColor(hWnd);	break;
		case IDM_ABOUT:   DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);   break;
		case IDM_EXIT:    DestroyWindow(hWnd);    break;
#pragma endregion
			// phan tool
#pragma region Tool
		case ID_TOOLS_PENCIL:	processToolPencil(hWnd);	break;
		case ID_TOOLS_ERASER:	processToolEraser(hWnd);	break;
		case ID_TOOLS_FILLCOLOR:	processToolFillColor(hWnd);	break;
		case ID_TOOLS_COLORPICKER:	processToolColorPicker(hWnd);	break;
#pragma endregion
			// Chon hinh
#pragma region phan shape. 
		case ID_HINH_LINE:
		case ID_HINH_TAMGIAC:
		case ID_HINH_ELIPSE:
		case ID_HINH_HINHCHUNHAT:
		case ID_HINH_SAO4CANH:
		case ID_HINH_SAO5CANH:	processToolShape(hWnd, wmId);  break;
#pragma endregion
			// chon mau vẽ
#pragma region ChonMauVe
		case ID_MAUSAC_DO: colorDraw = COLOR_RED; break;
		case ID_MAUSAC_CAM: colorDraw = COLOR_ORANGE; break;
		case ID_MAUSAC_VANG: colorDraw = COLOR_YELLOW; break;
		case ID_MAUSAC_LUC: colorDraw = COLOR_GREEN; break;
		case ID_MAUSAC_LAM: colorDraw = COLOR_BLUE; break;
		case ID_MAUSAC_DEN: colorDraw = COLOR_BLACK; break;
		case ID_MAUSAC_XAM:	colorDraw = COLOR_GRAY; break;
		case ID_MAUSAC_TRANG: colorDraw = COLOR_WHITE; break;
#pragma endregion
			// Chon mau nền
#pragma region ChonMauNen
		case ID_MAUNEN_DO: colorSec = COLOR_RED; break;
		case ID_MAUNEN_CAM: colorSec = COLOR_ORANGE; break;
		case ID_MAUNEN_VANG: colorSec = COLOR_YELLOW; break;
		case ID_MAUNEN_LUC: colorSec = COLOR_GREEN; break;
		case ID_MAUNEN_LAM: colorSec = COLOR_BLUE; break;
		case ID_MAUNEN_DEN: colorSec = COLOR_BLACK; break;
		case ID_MAUNEN_XAM: colorSec = COLOR_BLACK; break;
		case ID_MAUNEN_TRANG: colorSec = COLOR_WHITE; break;
#pragma endregion
			// thiet lap gia tri cua size
#pragma region ThietLapSize
		case ID_SIZE1: setSize(cursorTools, cursorTool); cursorSize = Size1; break;
		case ID_SIZE2: setSize(cursorTools, cursorTool); cursorSize = Size2; break;
		case ID_SIZE3: setSize(cursorTools, cursorTool); cursorSize = Size3; break;
		case ID_SIZE4: setSize(cursorTools, cursorTool); cursorSize = Size4; break;
#pragma endregion
			// flag fill
#pragma region flag fill
		case ID_FILL_FILL: isFill = true; break;
		case ID_FILL_NOFILL: isFill = false; break;
#pragma endregion

		default:   return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	}
	case WM_PAINT: {
		hdc = BeginPaint(hWnd, &ps);
		if (IsIconic(hWnd)) { FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1)); }
		//FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_ERASEBKGND:		break;
	case WM_DESTROY: {
		PostQuitMessage(0);
		break; }
	default:    return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// định nghĩa các hàm
#pragma region DinhNghiaCacHam
// các hàm vẽ hình. 
#pragma region CacPhuongThuVeVeHinh
void setPoint() {
	left = point[0].x;		top = point[0].y;
	right = point[n].x;		bottom = point[n].y;
}
void drawPencil(HWND hWnd, HDC hdc) {
	MoveToEx(hdc, point[n - 1].x, point[n - 1].y, NULL);
	LineTo(hdc, point[n].x, point[n].y);
	ReleaseDC(hWnd, hdc);
}
void drawShape(HWND hWnd, HDC hdc, int cursorShape) {
	switch (cursorShape)
	{
	case ID_HINH_LINE: 	drawLine(hWnd, hdc);		break;
	case ID_HINH_TAMGIAC:	drawTriangle(hWnd, hdc);	break;
	case ID_HINH_ELIPSE:	Ellipse(hdc, left, top, right, bottom);	break;
	case ID_HINH_HINHCHUNHAT:	Rectangle(hdc, left, top, right, bottom);	break;
		//RoundRect(hdc, left, top, right, bottom, 50, 50); break;
	case ID_HINH_SAO4CANH:	drawStarIV(hWnd, hdc);	break;
	case ID_HINH_SAO5CANH:	drawStarV(hWnd, hdc);	break;
	}
}
void drawLine(HWND hWnd, HDC hdc) {
	static POINT Double[2];
	setPoint();
	Double[0].x = left;
	Double[0].y = top;

	Double[1].x = right;
	Double[1].y = bottom;

	Polyline(hdc, Double, 2);
	ReleaseDC(hWnd, hdc);
}
void drawTriangle(HWND hWnd, HDC hdc) {
	static POINT trippel[3];
	//setPoint();
	trippel[0].x = left + (right - left) / 2;					trippel[0].y = top;
	trippel[1].x = left;										trippel[1].y = bottom;
	trippel[2].x = right;										trippel[2].y = bottom;

	Polygon(hdc, trippel, 3);
	ReleaseDC(hWnd, hdc);
}
void drawStarIV(HWND hWnd, HDC hdc) {
	point[0].x = left;
	point[0].y = top + (bottom - top) / 2;

	point[1].x = left + 3 * (right - left) / 8;
	point[1].y = top + 4 * (bottom - top) / 10;

	point[2].x = left + (right - left) / 2;
	point[2].y = top;

	point[3].x = left + 5 * (right - left) / 8;
	point[3].y = top + 4 * (bottom - top) / 10;

	point[4].x = right;
	point[4].y = top + (bottom - top) / 2;

	point[5].x = left + 5 * (right - left) / 8;
	point[5].y = top + 6 * (bottom - top) / 10;

	point[6].x = left + (right - left) / 2;
	point[6].y = bottom;

	point[7].x = left + 3 * (right - left) / 8;
	point[7].y = top + 6 * (bottom - top) / 10;

	Polygon(hdc, point, 8);
	ReleaseDC(hWnd, hdc);
}
void drawStarV(HWND hWnd, HDC hdc) {
	point[0].x = left;
	point[0].y = top + 2 * (bottom - top) / 5;

	point[1].x = left + 38 * (right - left) / 100 - 2;
	point[1].y = top + 2 * (bottom - top) / 5 - 3;

	point[2].x = left + (right - left) / 2;
	point[2].y = top;

	point[3].x = left + 62 * (right - left) / 100 + 3;
	point[3].y = top + 2 * (bottom - top) / 5 - 3;

	point[4].x = right;
	point[4].y = top + 2 * (bottom - top) / 5;

	point[5].x = left + 7 * (right - left) / 10 - 2;
	point[5].y = top + 3 * (bottom - top) / 5;

	point[6].x = left + 8 * (right - left) / 10;
	point[6].y = bottom;

	point[7].x = left + (right - left) / 2;
	point[7].y = top + 385 * (bottom - top) / 500;

	point[8].x = left + 2 * (right - left) / 10;
	point[8].y = bottom;

	point[9].x = left + 3 * (right - left) / 10 + 2;
	point[9].y = top + 3 * (bottom - top) / 5;

	Polygon(hdc, point, 10);
	ReleaseDC(hWnd, hdc);
}
#pragma endregion
// hàm xử lý file
#pragma region CacPhuongThucXuLyFile
void OpenFile(HWND hWnd) {
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = (LPWSTR)szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"Bitmap Files(*.bmp;*.dib)\0*.bmp;*.dib\0JPEG (*jpg;*jpeg;*jpe;*jfif)\0*jpg;*jpeg;*jpe;*jfif*\0GIF\0*.gif*\0TIFF(*.tif;*.tiff)\0*.tif;*.tiff*\0PNG(*.png)\0*.png*\0ICO (*.ico)\0*.ico*\0All Picture Files\0*.jpg;*.png;*.jpeg;*.bmp.gif\0All Files\0*.*\0";
	ofn.nFilterIndex = 7;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 

	if (GetOpenFileName(&ofn) == TRUE)
		hf = CreateFile(ofn.lpstrFile, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
}
void SaveFile(HWND hWnd) {
	static HBITMAP hBmp;
	static PBITMAPINFO pbi;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = (LPWSTR)szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"Bitmap Files(*.bmp;*.dib)\0*.bmp;*.dib\0JPEG (*jpg;*jpeg;*jpe;*jfif)\0*jpg;*jpeg;*jpe;*jfif*\0GIF\0*.gif*\0TIFF(*.tif;*.tiff)\0*.tif;*.tiff*\0PNG(*.png)\0*.png*\0ICO (*.ico)\0*.ico*\0All Picture Files\0*.jpg;*.png;*.jpeg;*.bmp.gif\0All Files\0*.*\0";
	ofn.nFilterIndex = 7;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	GetSaveFileName(&ofn);
}

#pragma endregion
// hàm lựa chọn size và color
#pragma region CacPhuongThucVeColorVaSize
void EditColor(HWND hWnd)
{
	CHOOSECOLOR chooseColor = { sizeof(CHOOSECOLOR) };
	chooseColor.Flags = CC_RGBINIT | CC_FULLOPEN | CC_ANYCOLOR;
	chooseColor.hwndOwner = hWnd;
	chooseColor.lpCustColors = acrCustClr;
	chooseColor.rgbResult = color;
	if (ChooseColor(&chooseColor))
	{
		color = chooseColor.rgbResult;
	}
}
void setSize(int cursorTools, int cursorTool) {
	switch (cursorTools)
	{
	case TOOLS_TOOL:
		if (cursorTool == TOOL_PENCIL) { Size1 = 1;	Size2 = 2;	Size3 = 3;	Size4 = 4; }
		if (cursorTool == TOOL_ERASER) { Size1 = 4;	Size2 = 6;	Size3 = 8;	Size4 = 20; }
		break;

	case TOOLS_BRUSH:
		if (cursorTool == BRUSH_BRUSH) { Size1 = 1;	Size2 = 3;	Size3 = 5;	Size4 = 8; }
		if (cursorTool == BRUSH_AIRBRUSH) { Size1 = 4;	Size2 = 8;	Size3 = 16;	Size4 = 24; }
		if (cursorTool == BRUSH_OILBRUSH) { Size1 = 8;	Size2 = 16;	Size3 = 30;	Size4 = 40; }
		break;
	}
}
void setCurrentSize(int cursorSize) {
	if (cursorSize == Size1) { setSize(cursorTools, cursorTool); cursorSize = Size1; }
	else if (cursorSize == Size2) { setSize(cursorTools, cursorTool); cursorSize = Size2; }
	else if (cursorSize == Size3) { setSize(cursorTools, cursorTool); cursorSize = Size3; }
	else if (cursorSize == Size4) { setSize(cursorTools, cursorTool); cursorSize = Size4; }
}
#pragma endregion
// cac phuong thuc tao but va choi
#pragma region CacPhuongThucLuaChonButVaChoi
void setPen(HDC hdc, COLORREF colorDraw) {
	hPen1 = CreatePen(PS_SOLID, cursorSize, colorDraw);
	SelectObject(hdc, hPen1);
}
void setBrush(HDC hdc, COLORREF colorSec) {
	if (isFill)	hbrushMauNen = CreateSolidBrush(colorSec);
	else hbrushMauNen = (HBRUSH)GetStockObject(NULL_BRUSH);
	SelectObject(hdc, hbrushMauNen);
}
#pragma endregion
// cac phuong thuc tao con tro chuot cho tool
#pragma region CacPhuongThucThietLapConTroChoTool
void processToolShape(HWND hWnd, int wmId) {
	firstTools = TOOLS_SHAPE;
	firstTool = -1;
	cursorShape = wmId;
	cursorTools = TOOLS_SHAPE;
	setCurrentSize(cursorSize);
	hCursor = LoadCursor(hInst, MAKEINTRESOURCE(CUR_SHAPE));
	SetClassLong(hWnd, GCL_HCURSOR, (LONG)(hCursor));
}
void processToolPencil(HWND hWnd) {
	firstTools = TOOLS_TOOL;
	firstTool = TOOL_PENCIL;
	cursorTools = TOOLS_TOOL;
	cursorTool = TOOL_PENCIL;
	setCurrentSize(cursorSize);
	hCursor = LoadCursor(hInst, MAKEINTRESOURCE(CUR_PENCIL));
	SetClassLong(hWnd, GCL_HCURSOR, (LONG)(hCursor));
}
void processToolEraser(HWND hWnd) {
	firstTools = TOOLS_TOOL;
	firstTool = TOOL_ERASER;
	cursorTools = TOOLS_TOOL;
	cursorTool = TOOL_ERASER;
	setCurrentSize(cursorSize);
	hCursor = LoadCursor(hInst, MAKEINTRESOURCE(CUR_ERASER));
	SetClassLong(hWnd, GCL_HCURSOR, (LONG)(hCursor));
}
void processToolFillColor(HWND hWnd) {
	cursorTools = TOOLS_TOOL;
	cursorTool = TOOL_FILLC0LOR;
	hCursor = LoadCursor(hInst, MAKEINTRESOURCE(CUR_FILLCOLOR));
	SetClassLong(hWnd, GCL_HCURSOR, (LONG)(hCursor));
}
void processToolColorPicker(HWND hWnd) {
	cursorTools = TOOLS_TOOL;
	cursorTool = TOOL_COLORPICKER;
	hCursor = LoadCursor(hInst, MAKEINTRESOURCE(CUR_COLORPICKER));
	SetClassLong(hWnd, GCL_HCURSOR, (LONG)(hCursor));
}
#pragma endregion

#pragma endregion



INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
