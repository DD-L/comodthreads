// threadlead.cpp
#include "threadlead.h"

queue<data_t> ThreadLead::queue_lead;
size_t ThreadLead::instance_counter = 0;
ThreadLead* ThreadLead::instance = NULL;

ThreadLead* ThreadLead::get_instance() {
    if (!instance_counter) {
        instance = new ThreadLead;
        ++instance_counter;
        return instance;
    }
    if (instance) ++instance_counter;
    return instance;
}
void ThreadLead::release() {
    if (--instance_counter <= 0) {
        delete instance;
    }
}

void ThreadLead::run() {
    // 只允许运行一个实例过程
    if (is_working) return; 
    is_working = true;
    // 为了确保所有的工作模块先获得锁，有必要延迟以下
    sleep(1);
    write_log(who + " 要加锁;");
    pthread_mutex_lock(&g_mtx);
    write_log(who + " 持有了锁;");
    
    // 消息分发过程开始
    while (true) {    
        // 确保队列不为空
        if (queue_lead.empty()) {
            // 加此锁无意义，可不使用
            // pthread_mutex_lock(&mtx_empty_add);

            // 等待队列不为空
            pthread_cond_wait(&cond_empty_add, &mtx_empty_add);
            // pthread_mutex_unlock(&mtx_empty_add);
        }

        // 依照队列，通知对应模块来取数据
        write_log(who + " 通知" + queue_lead.front().receiver 
                      + "来取数据;");
        pthread_cond_signal(map_cond[queue_lead.front().receiver]);
        
        // 等待相应的模块把数据取出，保证了同一时间只有一个工作模块访问队列
        write_log(who + " 等待 cond_remove, 并释放了锁;");
        pthread_cond_wait(&g_cond_remove, &g_mtx);
        write_log(who + " 等到了 cond_remove, 并重新加上了锁;");

        cout << "---------------------------当前队列的长度:" 
             << queue_lead.size() << endl;
    }
}

bool ThreadLead::add_stuff(ThreadStuff* ts) {
    map_cond[ts->who] = &(ts->cond);
    // 因为键值重复会覆盖之前的值,所以必须作出判断:是否新模块被添加成功
    return map_cond.size() == ++stuff_counter;
}
void ThreadLead::add_work(const data_t data) {
    pthread_mutex_lock(&g_mtx_add);
    queue_lead.push(data);
    // 如果 之前队列为空，push()后为1，队列存在了数据，唤醒cond_add
    queue_lead.size() == 1 && pthread_cond_signal(&cond_empty_add); 
    pthread_mutex_unlock(&g_mtx_add);
}

void ThreadLead::get_value(data_t& data) {
    data = queue_lead.front();
    queue_lead.pop();
}

void ThreadLead::init_mutex(const ThreadStuff* ts) {
    write_log(ts->who + " thread_msg: 欲要加锁;");
    pthread_mutex_lock(&g_mtx);
    write_log(ts->who + " thread_msg: 已经持有锁;");
}
void ThreadLead::wait_cond(ThreadStuff* ts) {
    write_log(ts->who + " wait(自己的cond), 并释放锁;");
    pthread_cond_wait(&ts->cond, &g_mtx);
    write_log(ts->who + " 等到了自己的cond， 并重新持有锁;");
}
void ThreadLead::signal_remove(const ThreadStuff* ts, int flg/* = 0*/) {
    write_log(ts->who + " 数据已取出，唤醒 cond_remove;");
    flg ? pthread_cond_signal(&g_cond_remove)
        : pthread_cond_broadcast(&g_cond_remove);
}
