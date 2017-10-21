#ifndef INIT_PLAYING_MAP_H_INCLUDED
#define INIT_PLAYING_MAP_H_INCLUDED

#pragma once

//////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <windows.h>

#include "cu_map.h"

#include "MacroDefine.h"

//////////////////////////////////////////////////////////////////////////

void InitPlayingMap( HWND, HDC, POINT, CMAP *, int );

//////////////////////////////////////////////////////////////////////////

void DrawBrickWall( HWND, HDC );				//��������ש��

void DrawRandWall( HWND, HDC, CMAP*, int );		//��������ϰ���

void DrawRandomFood( HDC, POINT );				//����ʳ��




#endif // INIT_PLAYING_MAP_H_INCLUDED
