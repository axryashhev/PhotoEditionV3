#ifndef LOADER_H_INCLUDED
#define LOADER_H_INCLUDED
#include <windows.h>
#include <wingdi.h>


#define ALPHA_SHIFT 24
#define RED_SHIFT   16
#define GREEN_SHIFT 8
#define BLUE_SHIFT  0
#define ALPHA_MASK  ((ARGB) 0xff << ALPHA_SHIFT)
#define MAKEARGB(a, r, g, B) \
	(((ARGB) ((a) & 0xff) << ALPHA_SHIFT)| \
	((ARGB) ((r) & 0xff) << RED_SHIFT)  | \
	((ARGB) ((g) & 0xff) << GREEN_SHIFT)| \
	((ARGB) ((B) & 0xff) << BLUE_SHIFT))

typedef DWORD ARGB;
typedef struct {

	ULONG token;
}gdiplusToken;


typedef struct {
	HBITMAP picture;
}gdiToken;
typedef struct
{
	int value;
}IntPtr;

typedef enum {
	Ok = 0,
	GenericError = 1,
	InvalidParameter = 2,
	OutOfMemory = 3,
	ObjectBusy = 4,
	InsufficientBuffer = 5,
	NotImplemented = 6,
	Win32Error = 7,
	WrongState = 8,
	Aborted = 9,
	FileNotFound = 10,
	ValueOverflow = 11,
	AccessDenied = 12,
	UnknownImageFormat = 13,
	FontFamilyNotFound = 14,
	FontStyleNotFound = 15,
	NotTrueTypeFont = 16,
	UnsupportedGdiplusVersion = 17,
	GdiplusNotInitialized = 18,
	PropertyNotFound = 19,
	PropertyNotSupported = 20,
	ProfileNotFound = 21
} Status;


enum DebugEventLevel
{
	DebugEventLevelFatal,
	DebugEventLevelWarning
};


typedef VOID(WINAPI* DebugEventProc)(DebugEventLevel level, CHAR* message);
typedef DWORD(WINAPI* NotificationHookProc)(OUT ULONG_PTR* token);
typedef VOID(WINAPI* NotificationUnhookProc)(ULONG_PTR token);




typedef struct {
	UINT32           GdiplusVersion;
	DebugEventProc DebugEventCallback;
	BOOL             SuppressBackgroundThread;
	BOOL             SuppressExternalCodecs;
} GdiplusStartupInput;

typedef struct {
	NotificationHookProc   NotificationHook;
	NotificationUnhookProc NotificationUnhook;
} GdiplusStartupOutput;

Status(WINAPI* GdiplusStartup)(gdiplusToken*, GdiplusStartupInput*, GdiplusStartupOutput*);
Status(WINAPI* GdipCreateBitmapFromFile)(WCHAR* filename, gdiplusToken* bitmap);
Status(WINAPI* GdipCreateHBITMAPFromBitmap)(ULONG bitmap, gdiToken* hbmReturn, ARGB background);
Status(WINAPI* GdipDisposeImage)(ULONG bitmap);
Status(WINAPI* GdiplusShutdown)(ULONG token);
HBITMAP hbmReturn;



HBITMAP MakeBitmapFromFile(HMODULE hWinLib, WCHAR* Name)
{

	GdiplusStartup = (Status(WINAPI*)(gdiplusToken*, GdiplusStartupInput*, GdiplusStartupOutput*))GetProcAddress(hWinLib, "GdiplusStartup");
	GdiplusStartupInput gdiplusStartupInput;
	gdiplusStartupInput.GdiplusVersion = 1;
	gdiplusStartupInput.DebugEventCallback = NULL;
	gdiplusStartupInput.SuppressBackgroundThread = FALSE;
	gdiplusStartupInput.SuppressExternalCodecs = FALSE;


	GdiplusStartupOutput* gdiplusStartupOutput;
	gdiplusStartupOutput = (GdiplusStartupOutput*)malloc(1000);

	gdiplusToken token;

	Status st = GdiplusStartup(&token, &gdiplusStartupInput, gdiplusStartupOutput);
	ULONG Shutdown = token.token;
	if (st != Ok)
	{
		MessageBox(NULL, "GDI Plus Startup Error", "ERROR", MB_OK);
	}



	//Status st;


	GdipCreateBitmapFromFile = (Status(WINAPI*)(WCHAR*, gdiplusToken*))GetProcAddress(hWinLib, "GdipCreateBitmapFromFile");

	gdiplusToken Bitmap;
	st = GdipCreateBitmapFromFile(Name, &Bitmap);

	if (st != Ok)
	{
		MessageBox(NULL, "GDI Plus Create Bitmap", "ERROR", MB_OK);
	}

	ULONG Map = Bitmap.token;

	gdiToken Image;

	GdipCreateHBITMAPFromBitmap = (Status(WINAPI*)(ULONG, gdiToken*, ARGB))GetProcAddress(hWinLib, "GdipCreateHBITMAPFromBitmap");
	ARGB rgb;
	rgb = MAKEARGB(0, 200, 200, 200); // set background color for transparency ie light gray

	st = GdipCreateHBITMAPFromBitmap(Map, &Image, rgb);
	hbmReturn = Image.picture;
	if (st != Ok)
	{
		MessageBox(NULL, "GDI Plus Create HBITMAP", "ERROR", MB_OK);
	}


	GdipDisposeImage = (Status(WINAPI*)(ULONG))GetProcAddress(hWinLib, "GdipDisposeImage");
	st = GdipDisposeImage(Map);

	if (st != Ok)
	{
		MessageBox(NULL, "GDI Plus Delete Bitmap", "ERROR", MB_OK);
	}

	free(gdiplusStartupOutput);

	GdiplusShutdown = (Status(WINAPI*)(ULONG))GetProcAddress(hWinLib, "GdiplusShutdown");
	st = GdiplusShutdown(Shutdown);

	if (st != Ok)
	{
		MessageBox(NULL, "GDI Plus Shutdown", "ERROR", MB_OK);
	}
	return hbmReturn;

}





#endif // LOADER_H_INCLUDED


