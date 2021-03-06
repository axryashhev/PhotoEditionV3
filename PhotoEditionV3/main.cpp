#include "resource.h"
#include "WinProc.h"


/*   variables  */
char szClassName[] = "Photo Edition";

/*  Declare  procedures  */
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

HBITMAP MakeBitmapFromFile(HMODULE hWinLib, WCHAR* Name);

HDC GetPrinterDC(HWND Hwnd);
BOOL OpenFileDialog(HWND hwnd, LPTSTR pFileName, LPTSTR pTitleName);
BOOL SaveFileDialog(HWND hwnd, LPTSTR pFileName, LPTSTR pTitleName);
void InitialiseDialog(HWND hwnd);
PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp, PBITMAPINFO pbmi);
void SaveBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC);
bool BitmapToClipboard(HBITMAP hBM, HWND hWnd);
HBITMAP CopyScreenToBitmap(HWND hwnd, int x1, int y1, int nWidth, int nHeight);
void PaintLoadBitmap(HWND hwnd, SCROLLINFO si, BITMAP bitmap, int pcxsize, int pcysize, int xMaxScroll, int xCurrentScroll, int xMinScroll, int yMaxScroll, int yCurrentScroll, int yMinScroll);
void DrawBoundingBox(HDC hdc, int StartxPos, int EndxPos, int StartyPos, int EndyPos);
void Cut(HDC hdc, HDC hdcMem, HBITMAP hBitmap, double Width, double Height, double oWidth, double oHeight, double StartxPos, double EndxPos, double StartyPos, double EndyPos, int xCurrentScroll, int yCurrentScroll);
void Paste(HWND hwnd, HBITMAP hBitmap, double Width, double Height, double oWidth, double oHeight, HDC hdc, HDC hdcMem, double StartxPos, double EndxPos, double xCurrentScroll, double StartyPos, double EndyPos, int yCurrentScroll, double zoom, double cZoom);
void ZeroScrollbars(HWND hwnd, SCROLLINFO* si, BITMAP bitmap, int cxsize, int cysize, int xCurrentScroll, int yCurrentScroll, int xMaxScroll, int yMaxScroll, int xMinScroll, int yMinScroll);
HBITMAP fadetogrey(HDC hdc, HDC hdcMem, HBITMAP Hbitmap, double Width, double Height, double Width2, double Height2, int xCurrentScroll, int yCurrentScroll);
HBITMAP fadetoyellow(HDC hdc, HDC hdcMem, HBITMAP hBitmap, double Width, double Height, double Width2, double Height2, int xCurrentScroll, int yCurrentScroll);
HBITMAP flipvertical(HDC hdc, HDC hdcMem, HBITMAP hBitmap, double Width, double Height, double Width2, double Height2, int xCurrentScroll, int yCurrentScroll);
HBITMAP fliphorizontal(HDC hdc, HDC hdcMem, HBITMAP hBitmap, double Width, double Height, double Width2, double Height2, int xCurrentScroll, int yCurrentScroll);
HBITMAP RedeyeReduction(HDC hdc, HDC hdcMem, HBITMAP hBitmap, double Width, double Height, double Width2, double Height2, int xCurrentScroll, int yCurrentScroll, double StartxPos, double EndxPos, double StartyPos, double EndyPos);
HBITMAP RotateRight(HDC hdc, HDC hdcMem, HBITMAP hBitmap, double Width, double Height, double Width2, double Height2, int xCurrentScroll, int yCurrentScroll);
HBITMAP RotateLeft(HDC hdc, HDC hdcMem, HBITMAP hBitmap, double Width, double Height, double Width2, double Height2, int xCurrentScroll, int yCurrentScroll);
HBITMAP DuoTone(HDC hdc, HDC hdcMem, HBITMAP hBitmap, double Width, double Height, double Width2, double Height2, int xCurrentScroll, int yCurrentScroll);
void SetScrollBars(HWND hwnd, RECT WinRect, SCROLLINFO* si, double bxWidth2, double bxHeight2, double cxsize, double cysize, int xMaxScroll, int xMinScroll, int* xCurrentScroll, int yMaxScroll, int yMinScroll, int* yCurrentScroll, int xDelta, int yDelta);
/*Start of Program Entry point*/

int WINAPI WinMain(HINSTANCE hThisInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpszArgument,
	int nCmdShow)
{
	HWND hwnd;               /* This is the handle for our window */
	MSG messages;            /* Here messages to the application are saved */
	WNDCLASSEX wincl;        /* Data structure for the windowclass */

	/* The Window structure */
	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
	wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
	wincl.cbSize = sizeof(WNDCLASSEX);

	/* Use default icon and mouse-pointer */
	wincl.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	wincl.hIconSm = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.lpszMenuName = MAKEINTRESOURCE(H_Menu);                 /* No menu */
	wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
	wincl.cbWndExtra = 0;                      /* structure or the window instance */
	/* Use Windows's default colour as the background of the window */
	wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

	/* Register the window class, and if it fails quit the program */
	if (!RegisterClassEx(&wincl))
		return 0;

	/* The class is registered, let's create the program*/
	/* The class is registered, let's create the program*/
	hwnd = CreateWindowEx(
		0,                   /* Extended possibilites for variation */
		szClassName,         /* Classname */
		szClassName,       /* Title Text */
		WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL, /* default window */
		CW_USEDEFAULT,       /* Windows decides the position */
		CW_USEDEFAULT,       /* where the window ends up on the screen */
		544,                 /* The programs width */
		375,                 /* and height in pixels */
		HWND_DESKTOP,        /* The window is a child-window to desktop */
		NULL,                /* menu */
		hThisInstance,       /* Program Instance handler */
		NULL                 /* No Window Creation data */
	);

	/* Make the window visible on the screen */
	ShowWindow(hwnd, SW_MAXIMIZE);

	/* Run the message loop. It will run until GetMessage() returns 0 */
	while (GetMessage(&messages, NULL, 0, 0))
	{
		/* Translate virtual-key messages into character messages */
		TranslateMessage(&messages);
		/* Send message to WindowProcedure */
		DispatchMessage(&messages);
	}

	/* The program return-value is 0 - The value that PostQuitMessage() gave */
	return messages.wParam;
}
