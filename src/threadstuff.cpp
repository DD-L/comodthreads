// threadstuff.cpp
#include <cstring>
#include <cstdio>
#include "threadstuff.h"
ThreadStuff::ThreadStuff(const string& who, 
                        float max_sec_delay/* = 0.1*/)
            : Thread(who), 
              // 创建 ThreadLead唯一实例的引用
              tl(* ThreadLead::get_instance()) {
    pthread_cond_init(&cond, NULL); 

    // 将自己添加到tl
    if (!tl.add_stuff(this)) {
	cout << "\nError: " << who 
	     << " 模块创建失败! 可能是模块标识符\"" 
	     << who << "\" 重复。程序退出.\n\n";
	exit(-1);	
    }
    this->max_sec_delay = max_sec_delay > -0.000001f ? max_sec_delay : 0.0f;
    // 初始化迭代器
    cit = tl.get_map_cond().begin();
}

ThreadStuff::~ThreadStuff() {
    pthread_cond_destroy(&cond);
    ThreadLead::release();
}

void ThreadStuff::start_comm() {
    ThreadLead::get_instance()->start();
}

void ThreadStuff::run() {
    // 创建子线程, 用于从队列取出自己的数据
    pthread_t tid;
    pthread_create(&tid, NULL, thread_msg, this);

    // 生产数据
    while (true) {
        // 产生数据

	// 随机产生接收者
	int i = rand() % tl.get_map_cond().size();
	cit = tl.get_map_cond().begin();
	while (i-- > 0) cit++;
        // 此处没有过滤 发送者和接收者 相同的情况。

        data_t data;
        data.sender = who;
        data.receiver = cit->first;
        data.value = rand() % 100;
        // 添加数据到队列
        tl.add_work(data);
        // 随机延时
        usleep((rand() % int(max_sec_delay * 1000000)));
    }
}
void* ThreadStuff::thread_msg(void* arg) {
    pthread_detach(pthread_self());
    ThreadStuff* THIS = (ThreadStuff *)arg;
    write_log(THIS->who + " thread_msg 线程启动；");
    
    // 为了防止交叉死锁现象,确保在进入循环前,要先比ThreadLead模块持有锁
               
    // 初始化锁机制
    THIS->tl.init_mutex(THIS);
    
    // 消费数据
    while (true) {
        // 等待属于自己的数据出现
        THIS->tl.wait_cond(THIS); 
        
        // 得到通知后，取出数据
        data_t data;
        THIS->tl.get_value(data);
        // 使用数据
        cout << '\"' << data.receiver << "\"得到来自\"" 
             << data.sender << "\"的数据: " << data.value << endl;
            
        // 已经取出，反馈给 ThreadLead, 使其可以继续通知下一个接收者去取数据
        THIS->tl.signal_remove(THIS); //pthread_cond_broadcast(&g_cond_remove);
    }
    return NULL;
};
