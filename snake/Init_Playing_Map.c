#include "Init_Playing_Map.h"
#include <stdio.h>
//////////////////////////////////////////////////////////////////////////

void InitPlayingMap( HWND hwnd, HDC hdc, POINT ptFood, CMAP *gm_map, int level )
{
	HPEN hPen;
	HBRUSH hBrush;

	hPen = CreatePen( PS_SOLID, 3, RGB(128, 128, 128) );
	hBrush = CreateSolidBrush( RGB(192, 192, 192) );
	SelectObject( hdc, hPen );
	SelectObject( hdc, hBrush );

	DrawBrickWall( hwnd, hdc );
	DrawRandWall( hwnd, hdc, gm_map, level );
	DrawRandomFood( hdc, ptFood );

	DeleteObject( hPen );
	DeleteObject( hBrush );
}

//////////////////////////////////////////////////////////////////////////

//画四周砖块
void DrawBrickWall( HWND hwnd, HDC hdc )
{
	int x = 0, y = 0;

	RECT rect = {0};

	GetClientRect( hwnd, &rect );

	for( x; x < rect.right / 10 + 1; x++ )
	{
		RoundRect( hdc, x*10, 0, x*10+10, 10, 3, 3 );
		RoundRect( hdc, x*10, rect.bottom-10, x*10+10, rect.bottom, 3, 3 );
	}

	for( y; y < rect.bottom / 10 + 1; y++ )
	{
		RoundRect( hdc, 0, y*10, 10, y*10+10, 3, 3 );
		RoundRect( hdc, rect.right-10, y*10, rect.right, y*10+10, 3, 3 );
	}
}

//////////////////////////////////////////////////////////////////////////

//画线
void drawWallLine( HDC, int, int, int, int );

//画随机墙
void DrawRandWall( HWND hwnd, HDC hdc, CMAP *gm_map, int lev )
{
	int i = 0;
	RECT rect = {0};
	CLINE line = {0};

	GetClientRect( hwnd, &rect );

	for( i; i < 5; i++ )
	{
		line.x1 = gm_map[lev].line[i].x1 * rect.right;
		line.y1 = gm_map[lev].line[i].y1 * rect.bottom;
		line.x2 = gm_map[lev].line[i].x2 * rect.right;
		line.y2 = gm_map[lev].line[i].y2 * rect.bottom;

		drawWallLine(
			hdc,
			(int)(line.x1) + 10 - (int)line.x1 % 10,		//将坐标转换为整十数
			(int)(line.y1) + 10 - (int)line.y1 % 10,
			(int)(line.x2) + 10 - (int)line.x2 % 10,
			(int)(line.y2) + 10 - (int)line.y2 % 10
		);
	}
}

//////////////////////////////////////////////////////////////////////////

//绘制随机障碍物
void drawWallLine( HDC hdc, int x1, int y1, int x2, int y2 )
{
	int i = 0;
	int xPos = 0, yPos = 0;

	if( x2-x1 == 0 )
	{
		yPos = y1;
		for( i = 0; i < (y2-y1)/10; i++ )
		{
			RoundRect( hdc, x1, yPos, x1+10, yPos+10, 3, 3 );
			yPos += 10;
		}
	}

	if( y2-y1 == 0 )
	{
		xPos = x1;
		for( i = 0; i < (x2-x1)/10; i++ )
		{
			RoundRect( hdc, xPos, y1, xPos+10, y1+10, 3, 3 );
			xPos += 10;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

//绘制食物
void DrawRandomFood( HDC hdc, POINT ptFood )
{
	HPEN hPen;
	HBRUSH hBrush;

	hPen = GetStockObject( WHITE_PEN );
	hBrush = CreateSolidBrush( RGB(255, 0, 0) );
	SelectObject( hdc, hPen );
	SelectObject( hdc, hBrush );

	Ellipse( hdc, ptFood.x, ptFood.y, ptFood.x+10, ptFood.y+10 );

	DeleteObject( hPen );
	DeleteObject( hBrush );
}

//////////////////////////////////////////////////////////////////////////
