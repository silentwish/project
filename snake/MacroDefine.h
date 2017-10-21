#ifndef MACRODEFINE_H_INCLUDED
#define MACRODEFINE_H_INCLUDED

#pragma once

//////////////////////////////////////////////////////////////////////////

//��Ϸ״̬
#define			CM_UN_START				0					//ͣ������ʼ����
#define			CM_START_ED				1					//���ڽ�����Ϸ

//////////////////////////////////////////////////////////////////////////

#define			CM_READY_GAME			(WM_USER + 100)		//׼����Ϸ��Ϣ
#define			CM_START_GAME			(WM_USER + 101)		//��ʼ��Ϸ��Ϣ
#define			CM_MOVE_SNAKE			(WM_USER + 102)		//�ƶ�����
#define			CM_GAME_OVER			(WM_USER + 103)		//��Ϸ����
#define			CM_GAME_NEXT			(WM_USER + 104)		//��һ��
#define			CM_GAME_SUCCEED			(WM_USER + 105)		//ȫ�سɹ�
#define			CM_GAME_READY			(WM_USER + 106)		//��Ϸ׼��

//////////////////////////////////////////////////////////////////////////

#define			TMR_BEGIN				10000				//��ʼ���涨ʱ��
#define			TMR_PLAYING_READY		10001				//����׼����Ч
#define			TMR_PLAYING_GO			10002				//���ſ�ʼ��Ч
#define			TMR_IMPACE_TEST			10003				//��ײ�����ȷ�Բ���

#define			TMR_START_MOVE			10004				//�ƶ�����

//////////////////////////////////////////////////////////////////////////

#define			DR_UP					20000				//���ƶ�
#define			DR_DOWN					20001				//���ƶ�
#define			DR_LEFT					20002				//���ƶ�
#define			DR_RIGHT				20003				//���ƶ�

//////////////////////////////////////////////////////////////////////////

#define			GAME_AGAIN				30000				//����һ��
#define			GAME_MAIN				30001				//�����˵�
#define			GAME_EXIT				30003				//�˳���Ϸ


#endif // MACRODEFINE_H_INCLUDED
