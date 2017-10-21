#ifndef PLAYINGPROC_H_INCLUDED
#define PLAYINGPROC_H_INCLUDED

#pragma once

//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#include "DealBeginMenuMsg.h"
#include "GameOverDlgProc.h"
#include "Init_Playing_Map.h"
#include "MacroDefine.h"
#include "cu_map.h"
#include "resource.h"

//////////////////////////////////////////////////////////////////////////

typedef struct GameStatus
{
	TCHAR	szGameTitle[256];		//����������
	int		level;					//��ǰ��Ϸ����
	POINT	body[512];				//��������
	POINT	food;					//ʳ����������
	int		direction;				//��ǰ�ж�����
	int		len;					//������
	int		score;					//��Ϸ�÷�
}GM_STATUS;

//////////////////////////////////////////////////////////////////////////

LRESULT PlayingProc( HWND, UINT, WPARAM, LPARAM );

BOOL PointImpaceTest( HWND, POINT, int );		//�����ϰ������ײ���

void GetRandomFoodPlace( HWND, POINT* );		//�������ʳ������

LRESULT CALLBACK StartPlaying( HWND, UINT, WPARAM, LPARAM );		//��ʼ��Ϸ




#endif // PLAYINGPROC_H_INCLUDED
