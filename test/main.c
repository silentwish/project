#include <graphics.h>

int main()

{

initgraph(640, 480);//打开一个长640像素，宽480像素的窗口



setcolor(GREEN);//设置绘图颜色为绿色

line(100, 100, 500, 200);//以（100，100）为起点，（500，200）为终点划一条直线



getch();//让屏幕暂停一下

closegraph();//关闭绘图窗口



return 0;

}
