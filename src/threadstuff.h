#ifndef _THREADSTUFF_H
#define _THREADSTUFF_H

#include <string>
#include <iostream>
#include <queue>
#include <map>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>

#include "thread.h"
#include "threadlead.h"

using namespace std;

class ThreadLead; // 前向声明

// 工作模块类
class ThreadStuff : public Thread {
public:
    ThreadStuff(const string& who, // 当前工作模块的名字，即唯一标识 
		float max_sec_delay = 0.1f); // 循环生产数据时最大延时的秒数,默认 0.1 s
    ~ThreadStuff();
    // 准备就绪后，开始模块间通信
    static void start_comm();
public:
    // 用于消费数据时通信使用的cond
    pthread_cond_t cond;
private:
    // 工作模块主线程的工作函数，负责 启动消费数据的子线程 和 生产数据
    virtual void run();
    // 负责从队列中取出数据的子线程
    static void* thread_msg(void* arg);
private:
    // 生产数据 和 消费数据 都依赖ThreadLead的队列 和 锁机制
    ThreadLead& tl;
    // 循环生产数据时最大延时的秒数
    float max_sec_delay;
    // 字典的迭代器，参与随机产生数据的接收者
    map<string, pthread_cond_t*>::const_iterator cit;
};
#endif // _THREADSTUFF_H
