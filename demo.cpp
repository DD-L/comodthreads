// 演示程序 
#include <cstdio>

#include "threadstuff.h"

int main(int argc, char* argv[]) {
    Thread* ts1 = new ThreadStuff("模块1"); 
    Thread* ts2 = new ThreadStuff("模块2", 0.8);
    Thread* ts3 = new ThreadStuff("模块3", 3);
    
    // 可随意增加工作模块
    Thread* ts4 = new ThreadStuff("模块4", 5.1f);
    // 模块标识符可任意写，只要不和其他的重复
    Thread* ts5 = new ThreadStuff("模五 ", 3);
    
    ts1->start();
    ts2->start();
    ts3->start();
    ts4->start(); 
    ts5->start();

    //  准备就绪开始通信
    ThreadStuff::start_comm();
    getchar();
    delete ts1;
    delete ts2;
    delete ts3;
    delete ts4; delete ts5;
    return 0;
}
