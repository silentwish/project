#include <ege.h>
#include <time.h>
#include <math.h>

void paintstar(double x,double y,double r,int mod=0);


int main()
{
    ege::initgraph( 400, 400 );
    ege::setcolor( RGB(0xff, 0xff, 0xff) );
    ege::setfillcolor( RGB(0, 0, 0xff) );
    ege::setrendermode(ege::RENDER_MANUAL);

    ege::circle(ege::getwidth()/2,ege::getheight()/2,ege::getheight()/2);
    //��ԲȦ

    //paintstar(ege::getwidth()/2,ege::getheight()/2,ege::getheight()/2,0);
    //��ʵ�������

    paintstar(ege::getwidth()/2,ege::getheight()/2,ege::getheight()/2,1);
    //���������

    ege::line(ege::getwidth()/2,ege::getheight()/2,0,ege::getheight()/2);

    ege::getch();

    return 0;
}


void paintstar(double x,double y,double r,int mod) //x,y,Ϊ��������ԲԲ�����꣬r Ϊ���Բ�뾶��modΪ �������ʽ
{
    switch(mod)
{
    case 0:  //ʵ�������
{
    int pt[20];
    double r0=r*cos(ege::PI*2/5)/cos(ege::PI*1/5);
    //r0���������������䶼������ʱ�γɵ���������Բ�İ뾶
    for(int n=0;n<5;++n)
{
    pt[n*4]=(int)(-cos(ege::PI*2/5*n)*r+x);
    // ����Ǹ��ⶥ��ĺ�����
    pt[n*4+1]=(int)(sin(ege::PI*2/5*n)*r+y);
    // ����Ǹ��ⶥ���������
    pt[n*4+2]=(int)(-cos(ege::PI*2/5*n+ege::PI*1/5)*r0+x);// �����������θ�����ĺ�����
    pt[n*4+3]=(int)(sin(ege::PI*2/5*n+ege::PI*1/5)*r0+y);
    // �����������θ������������
    }
    ege::fillpoly(10,pt);
}
    break;
    case 1:  //���������
{
    int pt[10];
    for(int n=0;n<5;++n)
{
    pt[n*2]=(int)(-cos(ege::PI*4/5*n)*r+x);
    //�����������
    pt[n*2+1]=(int)(sin(ege::PI*4/5*n)*r+y);
    //������������
}
    ege::fillpoly(5,pt);
}
    break;
    default:
    break;
}
}
