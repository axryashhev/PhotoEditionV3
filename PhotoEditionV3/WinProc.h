#ifndef WINPROC_H_INCLUDED
#define WINPROC_H_INCLUDED

#include "Functions.h"
#include "resource.h"
#include "Loader.h"
#include <Winspool.h>
#include <CommDlg.h>
#include <CommCtrl.h>
#include <cmath>
#include <algorithm>

HPALETTE hpal;
PBITMAPINFO pbmi;
DWORD Token;
HMENU menu;
HBITMAP hBitmap;
double bxWidth, bxHeight;
double OrigWidth, OrigHeight;
double bxWidth2, bxHeight2;
double zoom = 0;
double cZoom = 0;
static int yScroll;
static int xScroll;
bool fZoom = false;
int cxsize = 544, cxpage = 0;
int cysize = 375, cypage = 0;

RECT rect, WinRect;
SCROLLINFO si;
BITMAP bitmap;
double StartxPos;
double StartyPos;
double EndxPos;
double EndyPos;


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	PAINTSTRUCT ps;
	HMODULE hWinLib = LoadLibrary("gdiplus.dll");


	// These variables are required by BitBlt.



	static BOOL fBlt;
	static BOOL fLoad;           // TRUE if BitBlt occurred
	static BOOL fScroll;         // TRUE if scrolling occurred
	static BOOL fSize;           // TRUE if fBlt & WM_SIZE

	static BOOL mmov;
	static BOOL cut;
	static BOOL greyscale;
	static BOOL bbox;
	static BOOL redeye;
	static BOOL flipv;
	static BOOL fliph;
	static BOOL sepia;
	static BOOL rRight;
	static BOOL rLeft;
	static BOOL fDuo;
	static BOOL Undo;
	static BOOL paste;

	// These variables are required for horizontal scrolling.
	static int xMinScroll;       // minimum horizontal scroll value
	static int xCurrentScroll;   // current horizontal scroll value
	static int xMaxScroll;       // maximum horizontal scroll value
	static int xDelta;           // difference in x scrolling
	// These variables are required for vertical scrolling.
	static int yMinScroll;       // minimum vertical scroll value
	static int yCurrentScroll;   // current vertical scroll value
	static int yMaxScroll;       // maximum vertical scroll value
	static int yDelta;           // difference in y scrolling
	static HWND hwndViewer;

	switch (message)                  /* handle the messages */
	{
	case WM_CREATE:
	{


		InitialiseDialog(hwnd);
		hwndViewer = SetClipboardViewer(hwnd);

		menu = GetMenu(hwnd);


		// Initialize the flags.
		fBlt = FALSE;
		fScroll = FALSE;
		fSize = FALSE;
		yScroll = 0;
		xScroll = 0;
		// Initialize the horizontal scrolling variables.
		xMinScroll = 0;
		xCurrentScroll = 0;
		xMaxScroll = 0;

		// Initialize the vertical scrolling variables.
		yMinScroll = 0;
		yCurrentScroll = 0;
		yMaxScroll = 0;



		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		StartxPos = LOWORD(lParam);
		StartyPos = HIWORD(lParam);



		return 0;
	}

	case WM_LBUTTONUP:
	{
		EndxPos = LOWORD(lParam);
		EndyPos = HIWORD(lParam);

		mmov = true;
		InvalidateRect(hwnd, &rect, true);
		return 0;
	}



	case WM_QUERYNEWPALETTE:
	{
		if (!hpal)
			return FALSE;
		HDC hdce = GetDC(hwnd);
		SelectPalette(hdce, hpal, FALSE);
		RealizePalette(hdce);
		InvalidateRect(hwnd, NULL, TRUE);
		ReleaseDC(hwnd, hdce);
		return TRUE;
	}

	case WM_PALETTECHANGED:
	{
		HDC hdce = GetDC(hwnd);
		if (!hpal || (HWND)wParam == hwnd)
			break;

		SelectPalette(hdce, hpal, FALSE);
		RealizePalette(hdce);
		UpdateColors(hdce);
		ReleaseDC(hwnd, hdce);
		break;
	}
	case WM_DRAWCLIPBOARD:
	{

		if (hwndViewer)
		{
			SendMessage(hwndViewer, message, wParam, lParam);
		}

		EnableMenuItem(menu, IDM_PASTE, MF_ENABLED);
		InvalidateRect(hwnd, NULL, true);
		return 0;
	}

	case WM_CHANGECBCHAIN:
	{
		if ((HWND)wParam == hwndViewer)
		{
			hwndViewer = (HWND)lParam;
		}
		else if (hwndViewer)
		{
			SendMessage(hwndViewer, message, wParam, lParam);
		}

		return 0;
	}



	case WM_COMMAND:
		switch LOWORD(wParam)
		{

		case IDM_OPEN_BM:
		{




			OpenFileDialog(hwnd, szFileName, (TCHAR*)"Open an Image File.");

			if (szFileName != 0)
			{
				ZeroMemory(&hBitmap, sizeof(HBITMAP));


				HDC hdc = GetDC(hwnd);
				WCHAR* outFileName;
				outFileName = (WCHAR*)malloc(500);
				ZeroMemory(outFileName, 500);
				MultiByteToWideChar(CP_ACP, MB_COMPOSITE, szFileName, -1, outFileName, 500);

				hBitmap = MakeBitmapFromFile(hWinLib, outFileName);

				free(outFileName);



				if (hBitmap != NULL)
				{

					EnableMenuItem(menu, IDM_SAVE_BM, MF_ENABLED);
					EnableMenuItem(menu, IDM_PRINT_BM, MF_ENABLED);
					EnableMenuItem(menu, IDM_ZOOMIN, MF_ENABLED);
					EnableMenuItem(menu, IDM_ZOOMOUT, MF_ENABLED);
					cxpage = GetDeviceCaps(hdc, HORZRES);
					cypage = GetDeviceCaps(hdc, VERTRES);
					GetObject(hBitmap, sizeof(BITMAP), &bitmap);
					bxWidth = bitmap.bmWidth;
					bxHeight = bitmap.bmHeight;
					OrigWidth = bxWidth;
					OrigHeight = bxHeight;
					bxWidth2 = bxWidth;
					bxHeight2 = bxHeight;
					rect.left = 0;
					rect.top = 0;
					rect.right = (long)&cxpage;
					rect.bottom = (long)&cypage;
					GetWindowRect(hwnd, &WinRect);
					cxsize = WinRect.right;
					cysize = WinRect.bottom;
					xCurrentScroll = 0;
					yCurrentScroll = 0;
					zoom = 0;
					fZoom = false;
					fBlt = TRUE;
					fLoad = TRUE;
					fSize = FALSE;

					if (IsClipboardFormatAvailable(CF_BITMAP))
					{
						EnableMenuItem(menu, IDM_PASTE, MF_ENABLED);
					}

					ReleaseDC(hwnd, hdc);

					InvalidateRect(hwnd, NULL, TRUE);
				}
			}
			return 0;
		}

		case IDM_PRINT_BM:
		{
			DOCINFO di = { sizeof(DOCINFO), TEXT("Printing Picture...") };

			HDC prn;



			prn = GetPrinterDC(hwnd);
			cxpage = GetDeviceCaps(prn, HORZRES);
			cypage = GetDeviceCaps(prn, VERTRES);
			HDC hdcMem = CreateCompatibleDC(prn);
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

			StartDoc(prn, &di);
			StartPage(prn);
			SetMapMode(prn, MM_ISOTROPIC);
			SetWindowExtEx(prn, cxpage, cypage, NULL);
			SetViewportExtEx(prn, cxpage, cypage, NULL);

			SetViewportOrgEx(prn, 0, 0, NULL);
			StretchBlt(prn, 0, 0, cxpage, cypage, hdcMem, 0, 0, (int)bxWidth, (int)bxHeight, SRCCOPY);
			EndPage(prn);
			EndDoc(prn);
			DeleteDC(prn);
			SelectObject(hdcMem, hbmOld);
			DeleteDC(hdcMem);

			return 0;
		}

		case IDM_SAVE_BM:
		{

			char szFileName[1000];
			ZeroMemory(szFileName, 1000);

			BOOL result = SaveFileDialog(hwnd, szFileName, (TCHAR*)"Save an Image.");
			if (result != false)
			{
				PBITMAPINFO pbi = CreateBitmapInfoStruct(hwnd, hBitmap, pbmi);
				HDC hdce = GetDC(hwnd);

                // add an extension .bmp
                strcat_s(szFileName, ".bmp");

				SaveBMPFile(hwnd, szFileName, pbi, hBitmap, hdce);
				ReleaseDC(hwnd, hdce);


			}
			return 0;
		}

		case IDM_EXIT:
		{
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			return 0;

		}

		case IDM_COPY:
		{

			cZoom = zoom;
			int cutwidth = (int)abs(StartxPos - EndxPos);
			int cutheight = (int)abs(StartyPos - EndyPos);

			HBITMAP hbmp;

			int oldxpos = (int)StartxPos;
			int oldendpos = (int)EndxPos;
			int oldypos = (int)StartyPos;
			int oldendypos = (int)EndyPos;

			if (StartxPos > EndxPos)
			{
				StartxPos = oldendpos;
				EndxPos = oldxpos;
				StartyPos = oldendypos;
				EndyPos = oldypos;
			}

			ZeroMemory(&hbmp, sizeof(HBITMAP));


			hbmp = CopyScreenToBitmap(hwnd, (int)StartxPos, (int)StartyPos, cutwidth, cutheight);
			if (hbmp == NULL)
			{
				MessageBox(hwnd, "Copy not created properly", "Error", MB_OK);
			}


			BitmapToClipboard(hbmp, hwnd);
			DeleteBitmap(hbmp);


			return 0;
		}

		case IDM_CUT:
		{

			cZoom = zoom;
			int oldxpos = (int)StartxPos;
			int oldendpos = (int)EndxPos;
			int oldypos = (int)StartyPos;
			int oldendypos = (int)EndyPos;

			if (StartxPos > EndxPos)
			{
				StartxPos = oldendpos;
				EndxPos = oldxpos;
				StartyPos = oldendypos;
				EndyPos = oldypos;
			}

			int cutwidth = (int)abs(EndxPos - StartxPos);
			int cutheight = (int)abs(EndyPos - StartyPos);
			HBITMAP hbmp;
			ZeroMemory(&hbmp, sizeof(HBITMAP));
			hbmp = CopyScreenToBitmap(hwnd, (int)StartxPos, (int)StartyPos, cutwidth, cutheight);


			if (hbmp == NULL)
			{
				MessageBox(hwnd, "Copy not created properly", "Error", MB_OK);
			}
			OpenClipboard(hwnd);
			EmptyClipboard();
			CloseClipboard();

			BitmapToClipboard(hbmp, hwnd);
			DeleteBitmap(hbmp);


			cut = true;
			InvalidateRect(hwnd, NULL, true);
			return 0;
		}
		case IDM_PASTE:
		{
			paste = true;

			InvalidateRect(hwnd, NULL, true);
			return 0;
		}

		case IDM_UNDO:
		{
			Undo = TRUE;
			GetWindowRect(hwnd, &WinRect);

			SetScrollBars(hwnd, WinRect, &si, bxWidth2, bxHeight2, cxsize, cysize, xMaxScroll, xMinScroll, &xCurrentScroll, yMaxScroll, yMinScroll, &yCurrentScroll, xDelta, yDelta);
			InvalidateRect(hwnd, NULL, TRUE);

			return 0;
		}

		case IDM_ZOOMOUT:
		{
			zoom = zoom - 25;
			if (zoom < -75)
			{
				zoom = -75;
			}

			bxWidth2 = bxWidth + (bxWidth * (zoom / 100));
			bxHeight2 = bxHeight + (bxHeight * (zoom / 100));

			fZoom = TRUE;




			SetScrollBars(hwnd, WinRect, &si, bxWidth2, bxHeight2, cxsize, cysize, xMaxScroll, xMinScroll, &xCurrentScroll, yMaxScroll, yMinScroll, &yCurrentScroll, xDelta, yDelta);


			InvalidateRect(hwnd, NULL, TRUE);
			return 0;
		}

		case IDM_ZOOMIN:
		{


			zoom = zoom + 25;
			if (zoom > 75)
			{
				zoom = 75;
			}
			GetWindowRect(hwnd, &WinRect);
			bxWidth2 = bxWidth + (bxWidth * (zoom / 100));
			bxHeight2 = bxHeight + (bxHeight * (zoom / 100));

			fZoom = TRUE;


			SetScrollBars(hwnd, WinRect, &si, bxWidth2, bxHeight2, cxsize, cysize, xMaxScroll, xMinScroll, &xCurrentScroll, yMaxScroll, yMinScroll, &yCurrentScroll, xDelta, yDelta);

			InvalidateRect(hwnd, NULL, TRUE);

			return 0;
		}
		case IDM_GRAY:
		{
			greyscale = TRUE;

			InvalidateRect(hwnd, NULL, TRUE);
			return 0;
		}

		case IDM_REDEYE:
		{
			redeye = TRUE;

			InvalidateRect(hwnd, NULL, TRUE);
			return 0;
		}

		case IDM_FLIPV:
		{
			flipv = TRUE;

			InvalidateRect(hwnd, NULL, true);
			return 0;
		}
		case IDM_FLIPH:
		{
			fliph = TRUE;

			InvalidateRect(hwnd, NULL, true);
			return 0;
		}
		case IDM_SEPIA:
		{
			sepia = TRUE;

			InvalidateRect(hwnd, NULL, true);
			return 0;
		}

		case IDM_ROTATERIGHT:
		{
			rRight = TRUE;
			GetWindowRect(hwnd, &WinRect);


			SetScrollBars(hwnd, WinRect, &si, bxWidth2, bxHeight2, cxsize, cysize, xMaxScroll, xMinScroll, &xCurrentScroll, yMaxScroll, yMinScroll, &yCurrentScroll, xDelta, yDelta);

			InvalidateRect(hwnd, NULL, TRUE);


			return 0;
		}

		case IDM_ROTATELEFT:
		{
			rLeft = TRUE;

			GetWindowRect(hwnd, &WinRect);

			SetScrollBars(hwnd, WinRect, &si, bxWidth2, bxHeight2, cxsize, cysize, xMaxScroll, xMinScroll, &xCurrentScroll, yMaxScroll, yMinScroll, &yCurrentScroll, xDelta, yDelta);

			InvalidateRect(hwnd, NULL, TRUE);

			return 0;
		}
		case IDM_DUOTONE:
		{
			fDuo = TRUE;
			InvalidateRect(hwnd, NULL, true);
			return 0;
		}
		break;
		}

	case WM_PAINT:
	{


		HDC hdc = BeginPaint(hwnd, &ps);


		HDC hdcMem = CreateCompatibleDC(hdc);


		if (fLoad)
		{
			bxWidth2 = bxWidth;
			bxHeight2 = bxHeight;
			SetGraphicsMode(hdc, GM_ADVANCED);
			PaintLoadBitmap(hwnd, &si, bitmap, (int)bxWidth, (int)bxHeight, xMaxScroll, xCurrentScroll, 0, yMaxScroll, yCurrentScroll, 0);
			ZeroScrollbars(hwnd, si, bitmap, cxsize, cysize, xCurrentScroll, yCurrentScroll, xMaxScroll, yMaxScroll, xMinScroll, yMinScroll);
			SelectObject(hdcMem, hBitmap);

			PBITMAPINFO info = CreateBitmapInfoStruct(hwnd, hBitmap, pbmi);
			SaveBMPFile(hwnd, (char*)"temporarypicture.bmp", info, hBitmap, hdc);

			BitBlt(hdc, 0, 0, (int)bxWidth, (int)bxHeight, hdcMem, 0, 0, SRCCOPY);
			fLoad = FALSE;


		}
		if (fBlt)
		{
			//hdc = GetDC(hwnd);
			RECT Rect;
			GetWindowRect(hwnd, &Rect);
			FillRect(hdc, &Rect, (HBRUSH)COLOR_BACKGROUND);
			SelectObject(hdcMem, hBitmap);
			SetStretchBltMode(hdc, HALFTONE);

			StretchBlt(hdc, 0, 0, (int)bxWidth2, (int)bxHeight2, hdcMem, xCurrentScroll, yCurrentScroll, (int)bxWidth, (int)bxHeight, SRCCOPY);

		}

		// If the window has been resized and the user has
		// captured the screen, use the following call to
		// BitBlt to paint the window's client area.

		if (fSize)
		{


			SelectObject(hdcMem, hBitmap);
			SetStretchBltMode(hdc, HALFTONE);

			StretchBlt(hdc, 0, 0, (int)bxWidth2, (int)bxHeight2, hdcMem, xCurrentScroll, yCurrentScroll, (int)bxWidth, (int)bxHeight, SRCCOPY);

			fSize = FALSE;

		}

		// If scrolling has occurred, use the following call to
		// StretchBlt to paint the invalid rectangle.
		//
		// The coordinates of this rectangle are specified in the
		// RECT structure to which prect points.
		//
		// Note that it is necessary to increment the seventh
		// argument (prect->left) by xCurrentScroll and the
		// eighth argument (prect->top) by yCurrentScroll in
		// order to map the correct pixels from the source bitmap.
		if (fScroll)
		{
			SelectObject(hdcMem, hBitmap);
			SetStretchBltMode(hdc, HALFTONE);

			StretchBlt(hdc, 0, 0, (int)bxWidth2, (int)bxHeight2, hdcMem, xCurrentScroll, yCurrentScroll, (int)bxWidth, (int)bxHeight, SRCCOPY);
			fScroll = FALSE;


		}

		if (fZoom == TRUE)
		{
			hdc = GetDC(hwnd);
			RECT Rect;
			GetWindowRect(hwnd, &Rect);
			FillRect(hdc, &Rect, (HBRUSH)COLOR_BACKGROUND);
			SelectObject(hdcMem, hBitmap);
			SetStretchBltMode(hdc, HALFTONE);
			StretchBlt(hdc, 0, 0, (int)bxWidth2, (int)bxHeight2, hdcMem, xCurrentScroll, yCurrentScroll, (int)bxWidth, (int)bxHeight, SRCCOPY);

			fZoom = FALSE;
			InvalidateRect(hwnd, NULL, TRUE);

		}



		if (bbox == TRUE && redeye == TRUE)
		{

			hBitmap = RedeyeReduction(hdc, hdcMem, hBitmap, bxWidth, bxHeight, bxWidth2, bxHeight2, xCurrentScroll, yCurrentScroll, StartxPos, EndxPos, StartyPos, EndyPos);
			redeye = false;
		}

		if (mmov)
		{
			DrawBoundingBox(hdc, (int)StartxPos, (int)EndxPos, (int)StartyPos, (int)EndyPos);
			mmov = false;
			bbox = true;
			EnableMenuItem(menu, IDM_COPY, MF_ENABLED);
			EnableMenuItem(menu, IDM_CUT, MF_ENABLED);

		}

		if (cut)
		{


			Cut(hdc, hdcMem, hBitmap, (int)bxWidth2, (int)bxHeight2, (int)bxWidth, (int)bxHeight, StartxPos, EndxPos, StartyPos, EndyPos, xCurrentScroll, yCurrentScroll);

			cut = false;


		}

		if (paste)
		{

			int oldxpos = (int)StartxPos;
			int oldendpos = (int)EndxPos;
			int oldypos = (int)StartyPos;
			int oldendypos = (int)EndyPos;

			if (StartxPos > EndxPos)
			{
				StartxPos = oldendpos;
				EndxPos = oldxpos;
				StartyPos = oldendypos;
				EndyPos = oldypos;
			}



			Paste(hwnd, hBitmap, bxWidth2, bxHeight2, bxWidth, bxHeight, hdc, hdcMem, StartxPos, EndxPos, xCurrentScroll, StartyPos, EndyPos, yCurrentScroll, zoom, cZoom);


			//Reset Scrollbars to zero
			ZeroScrollbars(hwnd, si, bitmap, cxsize, cysize, xCurrentScroll, yCurrentScroll, xMaxScroll, yMaxScroll, xMinScroll, yMinScroll);


			paste = false;

		}

		if (Undo)
		{

			RECT Rect;
			GetWindowRect(hwnd, &Rect);
			FillRect(hdc, &Rect, (HBRUSH)COLOR_BACKGROUND);
			//DeleteObject(hBitmap);

			hBitmap = (HBITMAP)LoadImage(NULL, "temporarypicture.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE | LR_VGACOLOR);

			bxWidth = bxWidth2 = OrigWidth;
			bxHeight = bxHeight2 = OrigHeight;



			SelectObject(hdcMem, hBitmap);


			SetStretchBltMode(hdc, HALFTONE);
			StretchBlt(hdc, 0, 0, (int)bxWidth, (int)bxHeight, hdcMem, xCurrentScroll, yCurrentScroll, (int)bxWidth, (int)bxHeight, SRCCOPY);




			Undo = FALSE;

		}
		if (greyscale == TRUE)
		{

			hBitmap = fadetogrey(hdc, hdcMem, hBitmap, bxWidth, bxHeight, bxWidth2, bxHeight2, xCurrentScroll, yCurrentScroll);
			greyscale = false;

		}

		if (sepia == TRUE)
		{

			hBitmap = fadetoyellow(hdc, hdcMem, hBitmap, bxWidth, bxHeight, bxWidth2, bxHeight2, xCurrentScroll, yCurrentScroll);
			sepia = false;

		}

		if (flipv)
		{
			hBitmap = flipvertical(hdc, hdcMem, hBitmap, bxWidth, bxHeight, bxWidth2, bxHeight2, xCurrentScroll, yCurrentScroll);
			flipv = false;

		}

		if (fliph == TRUE)
		{
			hBitmap = fliphorizontal(hdc, hdcMem, hBitmap, bxWidth, bxHeight, bxWidth2, bxHeight2, xCurrentScroll, yCurrentScroll);
			fliph = false;

		}

		if (rRight == TRUE)
		{
			RECT Rect;

			GetWindowRect(hwnd, &Rect);
			FillRect(hdc, &Rect, (HBRUSH)COLOR_BACKGROUND);

			double TWidth = bxWidth, THeight = bxHeight, TWidth2 = bxWidth2, THeight2 = bxHeight2;
			hBitmap = RotateRight(hdc, hdcMem, hBitmap, bxWidth, bxHeight, bxWidth2, bxHeight2, xCurrentScroll, yCurrentScroll);
			bxWidth = THeight;
			bxHeight = TWidth;
			bxWidth2 = THeight2;
			bxHeight2 = TWidth2;

			rRight = FALSE;
			InvalidateRect(hwnd, NULL, TRUE);
		}

		if (rLeft == TRUE)
		{
			RECT Rect;

			GetWindowRect(hwnd, &Rect);

			FillRect(hdc, &Rect, (HBRUSH)COLOR_BACKGROUND);

			double TWidth = bxWidth, THeight = bxHeight, TWidth2 = bxWidth2, THeight2 = bxHeight2;
			hBitmap = RotateLeft(hdc, hdcMem, hBitmap, bxWidth, bxHeight, bxWidth2, bxHeight2, xCurrentScroll, yCurrentScroll);
			bxWidth = THeight;
			bxHeight = TWidth;
			bxWidth2 = THeight2;
			bxHeight2 = TWidth2;
			rLeft = FALSE;
			InvalidateRect(hwnd, NULL, TRUE);

		}

		if (fDuo == TRUE)
		{
			hBitmap = DuoTone(hdc, hdcMem, hBitmap, bxWidth, bxHeight, bxWidth2, bxHeight2, xCurrentScroll, yCurrentScroll);
			fDuo = FALSE;
		}

		DeleteDC(hdcMem);
		EndPaint(hwnd, &ps);


		return 0;
	}



	case  WM_SIZE:
	{

		GetWindowRect(hwnd, &WinRect);
		cxsize = (int)LOWORD(lParam);
		cysize = (int)HIWORD(lParam);
		if ((bxWidth2 - cxsize) > 0)
		{
			ShowScrollBar(hwnd, SB_HORZ, TRUE);
		}
		else if ((bxWidth2 - cxsize) < 0)
		{
			ShowScrollBar(hwnd, SB_HORZ, FALSE);
		}

		if ((bxHeight2 - cysize) > 0)
		{
			ShowScrollBar(hwnd, SB_VERT, TRUE);
		}
		else if ((bxHeight2 - cysize) < 0)
		{
			ShowScrollBar(hwnd, SB_VERT, FALSE);
		}

		SetScrollBars(hwnd, WinRect, &si, bxWidth2, bxHeight2, cxsize, cysize, xMaxScroll, xMinScroll, &xCurrentScroll, yMaxScroll, yMinScroll, &yCurrentScroll, xDelta, yDelta);


		fSize = TRUE;

		InvalidateRect(hwnd, NULL, TRUE);

		return 0;
	}

	case WM_HSCROLL:
	{
		int xDelta = 0;     // xDelta = new_pos - current_pos
		int xNewPos = 0;    // new position
		int yDelta = 0;

		switch (LOWORD(wParam))
		{
			// User clicked the scroll bar shaft left of the scroll box.
		case SB_PAGELEFT:
			xNewPos = xCurrentScroll - 50;
			xDelta = -50;
			break;

			// User clicked the scroll bar shaft right of the scroll box.
		case SB_PAGERIGHT:
			xNewPos = xCurrentScroll + 50;
			xDelta = 50;
			break;

			// User clicked the left arrow.
		case SB_LINELEFT:
			xNewPos = xCurrentScroll - 15;
			xDelta = -15;
			break;

			// User clicked the right arrow.
		case SB_LINERIGHT:
			xNewPos = xCurrentScroll + 15;
			xDelta = 15;
			break;

			// User dragged the scroll box.
		case SB_THUMBPOSITION:
			xNewPos = HIWORD(wParam);
			xDelta = xNewPos;
			break;

		default:
			xNewPos = xCurrentScroll;
		}

		// New position must be between 0 and the screen width.
		xMaxScroll = (int)bxWidth2;
		xNewPos = max(0, xNewPos);
		xNewPos = min(xMaxScroll, xNewPos);
		xCurrentScroll = min(xCurrentScroll, xMaxScroll);
		// If the current position does not change, do not scroll.
		if (xNewPos == xCurrentScroll)
			break;

		// Set the scroll flag to TRUE.
		fScroll = TRUE;



		// Reset the current scroll position.
		xCurrentScroll = xNewPos;

		// Scroll the window. (The system repaints most of the
		// client area when ScrollWindowEx is called; however, it is
		// necessary to call UpdateWindow in order to repaint the
		// rectangle of pixels that were invalidated.)


		// Reset the scroll bar.
		SetScrollBars(hwnd, WinRect, &si, bxWidth2, bxHeight2, cxsize, cysize, xMaxScroll, xMinScroll, &xCurrentScroll, yMaxScroll, yMinScroll, &yCurrentScroll, xDelta, yDelta);

		ScrollWindowEx(hwnd, -xDelta, -yDelta, (CONST RECT*) NULL,
			(CONST RECT*) NULL, (HRGN)NULL, (PRECT)NULL,
			SW_ERASE);

		InvalidateRect(hwnd, NULL, TRUE);
		break;
	}


	case WM_VSCROLL:
	{
		int xDelta = 0;
		int yDelta;     // yDelta = new_pos - current_pos
		int yNewPos;    // new position

		switch (LOWORD(wParam))
		{
			// User clicked the scroll bar shaft above the scroll box.
		case SB_PAGEUP:
			yNewPos = yCurrentScroll - 50;
			break;

			// User clicked the scroll bar shaft below the scroll box.
		case SB_PAGEDOWN:
			yNewPos = yCurrentScroll + 50;
			break;

			// User clicked the top arrow.
		case SB_LINEUP:
			yNewPos = yCurrentScroll - 15;
			break;

			// User clicked the bottom arrow.
		case SB_LINEDOWN:
			yNewPos = yCurrentScroll + 15;
			break;

			// User dragged the scroll box.
		case SB_THUMBPOSITION:
			yNewPos = HIWORD(wParam);
			break;

		default:
			yNewPos = yCurrentScroll;
			break;
		}

		// New position must be between 0 and the screen height.
		yMaxScroll = (int)bxHeight2 - cysize;
		yNewPos = max(0, yNewPos);
		yNewPos = min(yMaxScroll, yNewPos);
		yCurrentScroll = min(yCurrentScroll, yMaxScroll);
		// If the current position does not change, do not scroll.
		if (yNewPos == yCurrentScroll)
			break;

		// Set the scroll flag to TRUE.
		fScroll = TRUE;

		// Determine the amount scrolled (in pixels).
		yDelta = abs(yNewPos - yCurrentScroll);

		// Reset the current scroll position.
		yCurrentScroll = yNewPos;

		// Scroll the window. (The system repaints most of the
		// client area when ScrollWindowEx is called; however, it is
		// necessary to call UpdateWindow in order to repaint the
		// rectangle of pixels that were invalidated.)

		// Reset the scroll bar.

		SetScrollBars(hwnd, WinRect, &si, bxWidth2, bxHeight2, cxsize, cysize, xMaxScroll, xMinScroll, &xCurrentScroll, yMaxScroll, yMinScroll, &yCurrentScroll, xDelta, yDelta);



		ScrollWindowEx(hwnd, -xDelta, -yDelta, (CONST RECT*) NULL,
			(CONST RECT*) NULL, (HRGN)NULL, (PRECT)NULL,
			SW_ERASE);

		InvalidateRect(hwnd, NULL, TRUE);
		break;
	}

	case WM_CLOSE:
	{

		ChangeClipboardChain(hwnd, hwndViewer);

		DeleteFile("temporarypicture.bmp");
		DeleteObject(hbmReturn);
		DeleteObject(hBitmap);

		DeleteObject(&bitmap);

		free(pbmi);
		BOOL libresult = FreeLibrary(hWinLib);
		if (libresult == 0)
		{
			MessageBox(hwnd, "Free Library Error", "Error", MB_OK);
		}
		Sleep(500);
		DestroyWindow(hwnd);
		return 0;
	}
	case WM_DESTROY:


		PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
		return 0;
	default:                      /* for messages that we don't deal with */
		return DefWindowProc(hwnd, message, wParam, lParam);

	}
	return 0;

}


#endif // WINPROC_H_INCLUDED







