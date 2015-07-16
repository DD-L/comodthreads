// thread.cpp
#include <iostream>
#include "thread.h"

using namespace std;

void Thread::start() {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_t tid;
    if (pthread_create(&tid, &attr, run, this)) {
        cout << "线程创建失败!" << endl;
        exit(-1);
    }
}

void* Thread::run(void* arg) {
    static_cast<Thread *> (arg)->run();
    return NULL;
}
 
// DEBUG 模式下的 日志
void write_log(const string& msg/* = ""*/, long num/* = -1L*/) {
#ifdef _DEBUG
    cout << msg;
    if (num != -1) cout << num;
    cout << endl;
#endif
}
