/*
 * CopyRight (c) yestab123, shuzi, ZDJ
 * University course assignments at 2013
 */

#include <stdio.h>
#include <windows.h>
#include <conio.h>

COORD setCOORD;//��λ�ṹ��

extern HANDLE setHandleaa;

void setXY(int x,int y)
{
    setCOORD.X=x;
    setCOORD.Y=y;
    SetConsoleCursorPosition(setHandleaa,setCOORD);
}
