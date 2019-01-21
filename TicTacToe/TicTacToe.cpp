// TicTacToe.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TicTacToe.h"
#include "windowsx.h"
#include "windef.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
static const int CELL_SIZE = 100;				// Cell size 
static const int GRID_SIZE = 3;					// Grid size (3x3)
HBRUSH redBrush, blueBrush;						// Custom brush
int playerTurn = 1;
int gameboard[9] = { 0,0,0, 0,0,0, 0,0,0 };
int winner = 0;
int wins[3];
HANDLE img1, img2;


// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TICTACTOE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TICTACTOE));
	
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
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
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TICTACTOE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	//wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TICTACTOE);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

BOOL GetGameBoard(HWND hWnd, RECT * pRect) {

	RECT rc;
	if (GetClientRect(hWnd, &rc)) {
		
		int width = rc.right - rc.left;
		int height = rc.bottom - rc.top;

		pRect->left = (width - CELL_SIZE * 3) / 2;
		pRect->top = (height - CELL_SIZE * 3) / 2;

		pRect->right = pRect->left + CELL_SIZE * 3;
		pRect->bottom = pRect->top + CELL_SIZE * 3;
		
		return TRUE;
	}

	SetRectEmpty(pRect);
	return FALSE;
	
}

/*
	012
	345
	678
*/

int GetWinner(int win[3]) {
	int cells[] = { 0,1,2, 3,4,5, 6,7,8, 0,3,6, 1,4,7, 2,5,8, 0,4,8, 2,4,6 };
	
	for (int i = 0; i < ARRAYSIZE(cells); i += 3)
	{
		if (0 != gameboard[cells[i]] && gameboard[cells[i]] == gameboard[cells[i + 1]] && gameboard[cells[i]] == gameboard[cells[i + 2]])
		{
			win[0] = cells[i];
			win[1] = cells[i+1];
			win[2] = cells[i+2];
			return gameboard[cells[i]];
		}

	}

	for (int i : gameboard)
		if (i == 0)return 0;
	 
	return 3;
}

void ShowTurn(HWND hwnd, HDC hdc) {

	static const WCHAR play1[] = L"Player 1's turn";
	static const WCHAR play2[] = L"Player 2's turn";

	static const WCHAR * output = NULL;

	switch (winner)
	{
		case 0: // play game
			output = (playerTurn == 1) ? play1 : play2;
			break;
		case 1: // player 1 wins
			output = L"Player 1 wins!";
			break;
		case 2: //player 2 wins
			output = L"Player 2 wins!";
			break;
		case 3: //its a draw
			output = L"Its a draw...";
			break;
	}

	RECT rc;
	if (NULL != output && GetClientRect(hwnd, &rc))
	{
		rc.top = rc.bottom - 48;
		FillRect(hdc, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH));
		SetTextColor(hdc, BLACK_BRUSH);
		SetBkMode(hdc, TRANSPARENT);
		
		DrawText(hdc, output, lstrlen(output), &rc, DT_CENTER);
	}
}

void DrawLine(HDC hdc, int x1, int y1, int x2, int y2) {
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}

BOOL GetCellRect(HWND hWnd, int index, RECT * pRect) {

	RECT rcBoard;
	SetRectEmpty(pRect);
	if (index < 0 || index > 8)
		return FALSE;

	if (GetGameBoard(hWnd, &rcBoard))
	{
		int row = index / 3;
		int col = index % 3;

		pRect->left = rcBoard.left + col * CELL_SIZE + 1;
		pRect->top = rcBoard.top + row * CELL_SIZE + 1;
		pRect->right = pRect->left + CELL_SIZE - 1;
		pRect->bottom = pRect->top + CELL_SIZE - 1;

		return TRUE;
	}

	return false;
}


int GetCellNumber(HWND hWnd, int x, int y) {
	POINT pt = { x, y };
	RECT rc;

	if (GetGameBoard(hWnd, &rc)) {

		if (PtInRect(&rc, pt)) {
			//user clicked inside rectangle
			x = pt.x - rc.left;
			y = pt.y - rc.top;
			int col = x / CELL_SIZE;
			int row = y / CELL_SIZE;
			int index = col + row * 3;

			return index;
		}
	}
	return -1;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	
	case WM_CREATE:
		{
			redBrush = CreateSolidBrush(RGB(255, 0, 0));
			blueBrush = CreateSolidBrush(RGB(0, 0, 255));

			//load image
			
			//img1 = LoadImage(hInst, MAKEINTRESOURCE(IDB_PNG1), 0, 20, 20, LR_MONOCHROME);
			//img2 = LoadImage(hInst, MAKEINTRESOURCE(IDB_PNG2), 0, 20, 20, LR_MONOCHROME);
			
		}
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{

		case ID_FILE_NEWGAME:
			{
				int ret = MessageBox(hWnd, L"Do you want to start a new game?", L"New Game", MB_YESNO | MB_ICONQUESTION);
				if (IDYES == ret)
				{
					//reset game
					playerTurn = 1;
					winner = 0;
					ZeroMemory(gameboard, sizeof(gameboard));
					//force paint message
					InvalidateRect(hWnd, NULL, TRUE); //force wm paint
					UpdateWindow(hWnd);

				}
			}
			break;

		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_GETMINMAXINFO:
		{
			MINMAXINFO * pMinMax = (MINMAXINFO*)lParam;

			pMinMax->ptMinTrackSize.x = CELL_SIZE * 5;
			pMinMax->ptMinTrackSize.y = CELL_SIZE * 5;
			
		}
		break;

	case WM_LBUTTONDOWN:
		{

		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		if (0 == playerTurn)
		{
			break;
		}

		int index = GetCellNumber(hWnd, xPos, yPos);
		HDC hdc = GetDC(hWnd);

		if (index != -1) 
		{
			RECT rcCell;
			if (gameboard[index]==0 && GetCellRect(hWnd, index, &rcCell))
			{
				gameboard[index] = playerTurn;
				FillRect(hdc, &rcCell, playerTurn==1 ? redBrush : blueBrush);
				//DrawIcon(hdc, rcCell.left, rcCell.top, FromHandle);

				//check for winner
				winner = GetWinner(wins);
				if (winner == 1 || winner == 2)
				{
					//we have a winner
					//ShowTurn(hWnd, hdc);
					MessageBox(hWnd, (winner == 1) ? L"Player one wins!" : L"Player two wins!", 
							   L"Winner",
							   MB_OK | MB_ICONINFORMATION);
					playerTurn = 0;
				}
				else if (3 == winner)
				{
					//draw
					//ShowTurn(hWnd, hdc);
					MessageBox(hWnd, L"It's a draw",
						L"Tie Game",
						MB_OK | MB_ICONEXCLAMATION);
					playerTurn = 0;
				}
				else if (winner == 0)
				{
					playerTurn = playerTurn == 1 ? 2 : 1;
					//ShowTurn(hWnd, hdc);
				}
				//show turn
				ShowTurn(hWnd, hdc); 

			}
		}

		ReleaseDC(hWnd, hdc);

		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		RECT rc;
		if( GetGameBoard(hWnd, &rc) ) 
		{

			RECT rcClient;
			if (GetClientRect(hWnd, &rcClient))
			{
				const WCHAR szPlayer1[] = L"Player 1";
				const WCHAR szPlayer2[] = L"Player 2";
				//SetBkMode(hdc, RGB(255,0,0));
				TextOut(hdc, rc.left, rc.top - 32, szPlayer1, ARRAYSIZE(szPlayer1));
				//SetBkMode(hdc, RGB(0, 0, 255));
				TextOut(hdc, rc.right-64, rc.top - 32, szPlayer2, ARRAYSIZE(szPlayer2));
			}
			
			Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
			for (int i = 1; i < GRID_SIZE; i++) 
			{
				DrawLine(hdc, rc.left + (i*CELL_SIZE), rc.top, rc.left + (i*CELL_SIZE), rc.bottom);
				DrawLine(hdc, rc.left, rc.top + (i*CELL_SIZE), rc.right, rc.top + (i*CELL_SIZE));
			}
		}

		ShowTurn(hWnd, hdc);

		RECT rcCell;
		for (int i = 0; i < ARRAYSIZE(gameboard); i++)
		{
			if (gameboard[i] != 0 && GetCellRect(hWnd, i, &rcCell))
			{
				FillRect(hdc, &rcCell, gameboard[i]==1 ? redBrush : blueBrush);
			}
		}
	

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		DeleteObject(redBrush);
		DeleteObject(blueBrush);
		DeleteObject(img1);
		DeleteObject(img2);		
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
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
