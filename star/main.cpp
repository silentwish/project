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
    //画圆圈

    //paintstar(ege::getwidth()/2,ege::getheight()/2,ege::getheight()/2,0);
    //画实心五角星

    paintstar(ege::getwidth()/2,ege::getheight()/2,ege::getheight()/2,1);
    //空心五角星

    ege::line(ege::getwidth()/2,ege::getheight()/2,0,ege::getheight()/2);

    ege::getch();

    return 0;
}


void paintstar(double x,double y,double r,int mod) //x,y,为五角星外接圆圆心坐标，r 为外接圆半径，mod为 五角星样式
{
    switch(mod)
{
    case 0:  //实心五角星
{
    int pt[20];
    double r0=r*cos(ege::PI*2/5)/cos(ege::PI*1/5);
    //r0是五角星任意两点间都有连线时形成的五边形外接圆的半径
    for(int n=0;n<5;++n)
{
    pt[n*4]=(int)(-cos(ege::PI*2/5*n)*r+x);
    // 五角星各外顶点的横坐标
    pt[n*4+1]=(int)(sin(ege::PI*2/5*n)*r+y);
    // 五角星各外顶点的纵坐标
    pt[n*4+2]=(int)(-cos(ege::PI*2/5*n+ege::PI*1/5)*r0+x);// 五角星内五边形各顶点的横坐标
    pt[n*4+3]=(int)(sin(ege::PI*2/5*n+ege::PI*1/5)*r0+y);
    // 五角星内五边形各顶点的纵坐标
    }
    ege::fillpoly(10,pt);
}
    break;
    case 1:  //空心五角星
{
    int pt[10];
    for(int n=0;n<5;++n)
{
    pt[n*2]=(int)(-cos(ege::PI*4/5*n)*r+x);
    //各顶点横坐标
    pt[n*2+1]=(int)(sin(ege::PI*4/5*n)*r+y);
    //各顶点纵坐标
}
    ege::fillpoly(5,pt);
}
    break;
    default:
    break;
}
}
