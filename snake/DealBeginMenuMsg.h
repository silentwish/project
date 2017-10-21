#ifndef DEALBEGINMENUMSG_H_INCLUDED
#define DEALBEGINMENUMSG_H_INCLUDED

#pragma once

//////////////////////////////////////////////////////////////////////////

#include <windows.h>

#include "Init_Begin_UI.h"
#include "HelpDialogProc.h"
#include "MacroDefine.h"
#include "PlayingProc.h"
#include "resource.h"

//////////////////////////////////////////////////////////////////////////

LRESULT DealBeginMenuMsg( HWND, UINT, WPARAM, LPARAM );

//////////////////////////////////////////////////////////////////////////

//���������Ϣ
int DealMouseMove( HWND );

//���Ʋ˵�ѡ�����
void DrawSelectedBox( HWND, int );

//����˵������Ϣ
void DealMenuClick( int, HWND, HINSTANCE );



#endif // DEALBEGINMENUMSG_H_INCLUDED
