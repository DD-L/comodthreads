#ifndef _THREADLEAD_H
#define _THREADLEAD_H

#include <string>
#include <iostream>
#include <queue>
#include <map>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>

#include "thread.h"
#include "threadstuff.h"
using namespace std;

class ThreadStuff;

// 通信模块类，负责维护一个队列，并向工作模块分发消息
class ThreadLead : public Thread {
private:
    // 数据队列
    static queue<data_t> queue_lead;     
    // 队列刚刚不为空时用到的 锁和条件变量
    pthread_mutex_t mtx_empty_add;
    pthread_cond_t cond_empty_add;

    // g_mtx_add 用来确保 三个工作模块生产数据时 不同时添加数据到队列
    pthread_mutex_t g_mtx_add;

    // g_mtx 和 g_mtx_remove 用来协调 工作模块消费数据 和 ThreadLead模块分发指令. 
    pthread_mutex_t g_mtx;
    pthread_mutex_t g_mtx_remove;
    // 通知ThreadLead模块有数据已经取出.g_cond_remove和g_mtx_remove配合使用,保证了队列同时只有一个工作模块访问
    pthread_cond_t g_cond_remove;

    // map_cond 用来存放工作模块相应的cond
    map<string, pthread_cond_t*> map_cond;
    // 统计工作模块个数的计数器
    size_t stuff_counter;
    // 单例模式用到的计数器
    static int instance_counter;
    // 唯一实例
    static ThreadLead* instance;
    // 实例的 消息分配模块是否已经在运行
    bool is_working;
public: 
    // 以下成员是提供给 工作模块 的操作函数
    
    // 获取当前类的唯一实例，失败返回 NULL
    static ThreadLead* get_instance();
    // 释放实例
    static void release();
    // 添加工作模块, 成功返回 true
    bool add_stuff(ThreadStuff* ts);
    // 工作模块添加数据到队列
    void add_work(const data_t data);
    // 工作模块从队列取出数据
    void get_value(data_t& data);
    // 工作模块锁机制初始化
    void init_mutex(const ThreadStuff* ts);
    // 工作模块等待消费数据信号，阻塞
    void wait_cond(ThreadStuff* ts);
    // 取出数据后，反馈给ThreadLead模块
    void signal_remove(const ThreadStuff* ts, int flg = 0);
    // 返回指向字典map_cond的常的引用
    const map<string, pthread_cond_t*>& get_map_cond() {
	return map_cond;
    }
private:
    ThreadLead(const string& who = "通信模块") 
            : Thread(who), stuff_counter(0), is_working(false) {
        srand(time(0));
        pthread_mutex_init(&mtx_empty_add, NULL);
        pthread_cond_init(&cond_empty_add, NULL);
        pthread_mutex_init(&g_mtx_add, NULL);
        pthread_mutex_init(&g_mtx, NULL);
        pthread_mutex_init(&g_mtx_remove, NULL);
        pthread_cond_init(&g_cond_remove, NULL);
    }
    ~ThreadLead() {
        is_working = false;
        pthread_cond_destroy(&cond_empty_add);
        pthread_mutex_destroy(&mtx_empty_add);
        pthread_cond_destroy(&g_cond_remove);

        pthread_mutex_destroy(&g_mtx);
        pthread_mutex_destroy(&g_mtx_add);
        pthread_mutex_destroy(&g_mtx_remove);
        write_log("队列的长度:", queue_lead.size());
    }
    // 线程工作函数
    virtual void run();
};
#endif // THREADLEAD
