#ifndef _THREAD_H
#define _THREAD_H

#include <string>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
using namespace std;

///////////////////////////////////////////
// 所用模块的抽象基类
class Thread {
    public:
        Thread(const string& name) : who(name) {}
        virtual ~Thread() {}
        // 线程模块启动函数
        void start();
    public:
        // 自己的名字
        const string who;
    private:
        // 线程处理函数
        static void* run(void* arg);
        // 线程工作接口
	virtual void run() = 0;
};

////////////////////////////////////////////

// 数据的类型 data_t
// 说明:成员是 std::string 和 int 类型,所以不必担心data_t的浅拷贝问题
typedef struct {
    string sender;  // 数据的发送者
    string receiver;// 数据的接收者
    int value;      // 附加数据
} data_t;


// DEBUG 模式下的 日志
void write_log(const string& msg = "", long num = -1L);


#endif // _THREAD_H
