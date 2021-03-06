// Dzwig.cpp : definiuje punkt wejścia dla aplikacji
//


#include "stdafx.h"
#include "Dzwig.h"
#include <windows.h>
#include <objidl.h>
#include <assert.h>
#include <gdiplus.h>
#include <time.h>
#include <vector>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define MAX_LOADSTRING 100


BOOL freeSpawnPoint; //czy spawnpoint jest pusty

class pudlo //klasa na pudelka
{
public:
	int bok;
	float masa;
	int top, bottom, left, right;
	bool zlapana;

	pudlo(int bok = 10)
	{
		this->bok = bok;
		masa = bok * wspmasy;
		bottom = 600;
		top = bottom - bok;
		left = 250;
		right = left + bok;
		zlapana = false;
		freeSpawnPoint = false;
	}
	~pudlo()
	{
		freeSpawnPoint = true;
	}
	float wspmasy = 0.4;
};

// Zmienne globalne:
HINSTANCE hInst;                                // bieżące wystąpienie
WCHAR szTitle[MAX_LOADSTRING];                  // Tekst paska tytułu
WCHAR szWindowClass[MAX_LOADSTRING];            // nazwa klasy okna głównego
INT y =	200;
INT x =	200;
pudlo *crate;
std::vector <pudlo> magazyn;


void OnPaint(HDC hdc)			//Cale rysowanie
{
	Graphics graphics(hdc);
	Font font(L"Arial", 25);
	SolidBrush brush(Color(255, 0, 0, 0));
	Pen pen(Color(255, 0, 0, 0), 6);
	Pen penCrateUnhooked(Color(255, 255, 0, 0), 3);
	Pen penCrateHooked(Color(255, 0, 255, 0), 3);
	Rectangle(hdc, 100, 150, 200, 600);
	Rectangle(hdc, 25, 50, 1000, 150);
	graphics.DrawLine(&pen, 200 + x, 150, 200 + x, 150 + y);
	graphics.DrawLine(&pen, 0, 600, 1500, 600);
	graphics.DrawLine((freeSpawnPoint ? &penCrateHooked : &penCrateUnhooked), 200, 600, 500, 600); //spaw point, zielone mozna, czerwone nie mozna

	if (crate != NULL)
	{
		if (!crate->zlapana) //rysowanie nowej skrzynki
		{
			graphics.DrawRectangle(&penCrateUnhooked, crate->left, crate->top, crate->bok, crate->bok);
		}
		else
			graphics.DrawRectangle(&penCrateHooked, 200+x-(crate->bok/2), 150+y, crate->bok, crate->bok);

	}
	for (int i = 0; i < magazyn.size(); i++) //rysowanie skrzynek z magazynu
	{
		if (!magazyn[i].zlapana)
		{
			graphics.DrawRectangle(&penCrateUnhooked, magazyn[i].left, magazyn[i].top, magazyn[i].bok, magazyn[i].bok);
		}
		else
			graphics.DrawRectangle(&penCrateHooked, 200 + x - (magazyn[i].bok / 2), 150 + y, magazyn[i].bok, magazyn[i].bok);
	}
}

void repaintWindow(HWND hWnd, HDC &hdc, PAINTSTRUCT &ps, RECT *drawArea)
{
	if (drawArea == NULL)
		InvalidateRect(hWnd, NULL, TRUE); // repaint all
	else
		InvalidateRect(hWnd, drawArea, TRUE); //repaint drawArea
	hdc = BeginPaint(hWnd, &ps);
	OnPaint(hdc);
	EndPaint(hWnd, &ps);
}

// Przekaż dalej deklaracje funkcji dołączonych w tym module kodu:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	Status st = GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	assert(st == Ok);
	if (st != Ok) return FALSE;

	freeSpawnPoint = true;

    // Zainicjuj ciągi globalne
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DZWIG, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Wykonaj inicjacje aplikacji:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DZWIG));

    MSG msg;

    // Główna pętla wiadomości:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }


    return (int) msg.wParam;
}



//
//  FUNKCJA: MyRegisterClass()
//
//  CEL: Rejestruje klasę okna.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{

    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DZWIG));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DZWIG);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNKCJA: InitInstance(HINSTANCE, int)
//
//   CEL: Zapisuje dojście wystąpienia i tworzy okno główne
//
//   KOMENTARZE:
//
//        W tej funkcji dojście wystąpienia jest zapisywane w zmiennej globalnej i
//        jest tworzone i wyświetlane okno główne programu.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

   hInst = hInstance; // Przechowuj dojście wystąpienia w zmiennej globalnej
   srand(time(NULL));

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, 3);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNKCJA: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  CEL: Przetwarza wiadomości dla okna głównego.
//
//  WM_COMMAND — przetwarzaj menu aplikacji
//  WM_PAINT — pomaluj okno główne
//  WM_DESTROY — opublikuj komunikat o rezygnacji i wróć


//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
		int wmId, wmEvent;		
		PAINTSTRUCT ps;
		HDC hdc;

    switch (message)
    {

	case WM_KEYDOWN:		//eventy podczas klikania przyciskow
		switch (wParam)
		{
		case VK_UP:
			if (y-5 > 0 && y <= 450)
			{
				y=y-5;
			}
			repaintWindow(hWnd, hdc, ps, NULL);
			break;
		case VK_DOWN:
			if (y > 0 && y+5 < 450)
			{
				y= y+5;
			}
			repaintWindow(hWnd, hdc, ps, NULL);
			break;
		case VK_LEFT:
			if (x -5 > 0 && x <= 700)
			{
				x = x -5;
			}
			repaintWindow(hWnd, hdc, ps, NULL);
			break;
		case VK_RIGHT:
			if (x > 0 && x+5 < 700)
			{
				x = x+5;
			}
			repaintWindow(hWnd, hdc, ps, NULL);
			break;
		case VK_SPACE: //lapanie i puszczanie pudelek spacja
		{
			if (magazyn.size() > 0) //...z magazynu
			{
				for (int i = 0; i < magazyn.size(); i++)
				{
					if (magazyn[i].zlapana != true && 200 + x >= magazyn[i].left && 200 + x <= magazyn[i].right && 150 + y >= magazyn[i].top && 150+y <=magazyn[i].bottom)
						magazyn[i].zlapana = true;
					else if (magazyn[i].zlapana == true)
					{
						magazyn[i].zlapana = false;
						magazyn[i].left = 200 + x - (magazyn[i].bok / 2);
						magazyn[i].right = magazyn[i].left + magazyn[i].bok;
						magazyn[i].bottom = 600;
						magazyn[i].top = magazyn[i].bottom - magazyn[i].bok;
						for (int j = 0; j < magazyn.size(); j++)
						{
							if (j == i)
							{
								if (j + 1 == magazyn.size())
									break;
								else
									j++;
							}
							if (magazyn[i].left < magazyn[j].right && magazyn[i].right > magazyn[j].left)
							{
								magazyn[i].bottom = magazyn[j].top;
								magazyn[i].top = magazyn[i].bottom - magazyn[i].bok;
							}
						}
						if (magazyn[i].left > 500)
						{
							freeSpawnPoint = true;
						}
						else
							freeSpawnPoint = false;
					}
				}
			}
			if (crate != NULL) //..z miejsca spawnu
			{
				if(crate->zlapana != true && 200+x>=crate->left && 200 + x <= crate->right && 150 + y >= crate->top)
					crate->zlapana = true;
				else if (crate->zlapana == true)
				{
					crate->zlapana = false;
					crate->left = 200 + x - (crate->bok / 2);
					crate->right = crate->left + crate->bok;
					for (int i = 0; i < magazyn.size(); i++)
					{
						if (crate->left < magazyn[i].right && crate->right > magazyn[i].left)
						{
							crate->bottom = magazyn[i].top;
							crate->top = crate->bottom - crate->bok;
						}
					}
					if (crate->left > 500)
					{
						freeSpawnPoint = true;
						magazyn.push_back(*crate);
						delete crate;
					}
				}
			}
			repaintWindow(hWnd, hdc, ps, NULL);
			break;
		}
		case VK_CONTROL://tworzenie pudelek prawym ctrl
		{
			if (freeSpawnPoint==false)
			{
				if (crate != NULL)
				{
					delete crate;
					freeSpawnPoint = true;
					repaintWindow(hWnd, hdc, ps, NULL);
				}
				
			}
			if(freeSpawnPoint==true)
				crate = new pudlo((rand() % 200) + 10);
			repaintWindow(hWnd, hdc, ps, NULL);
			break;
		}

		}
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Analizuj zaznaczenia menu:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

//Procedura obsługi wiadomości dla okna informacji o programie.
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
