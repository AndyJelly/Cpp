// assertDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
// crt_set_error_mode.c
#include <stdlib.h>
#include <assert.h>

// thread::get_id / this_thread::get_id
#include <iostream>       // std::cout
#include <thread>         // std::thread, std::thread::id, std::this_thread::get_id
#include <chrono>         // std::chrono::seconds

std::thread::id main_thread_id = std::this_thread::get_id();

void is_main_thread() {
    if (main_thread_id == std::this_thread::get_id())
        std::cout << "This is the main thread.\n";
    else
        std::cout << "This is not the main thread.\n";
}

int main()
{
    _set_error_mode(_OUT_TO_MSGBOX);
    assert(2 + 2 == 5);
    
    is_main_thread();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::thread th(is_main_thread);

    th.join();
    getchar();
}


/*


一、线程管理
1、thread变量一旦被detach，无法被join；否则将一起崩溃；
同样一个线程一旦被join之后，就无法被再次join，否则一样会引起崩溃；所以每次join之前需要joinable判断一次

2、起线程时，传递的参数是值传递的，线程内部修改变量对外不起作用；
可以使用std::ref(),以引用的方式传递，此时函数的参数也要改成引用传递；
线程变量只能通过std::move移动，不能直接赋值；move之后，原有的线程变量就为空的了；

3、std::thread::hardware_concurrency();获取CPU支持的最大并发数

二、数据竞争和互斥对象
1、std::mutex  通过互斥对象调用自身的lock、unlock函数，可以对数据进行加锁；然后当在程序锁住的部分发生崩溃时，unlock将会永远锁住。不建议使用。

2、使用std::lock_guard
std::mutex mu;

void share_print(std::string msg,int id)
{
    //mu.lock();
    std::lock_guard<std::mutex> guard(mu);
    std::cout << msg << id << std::endl;
    //mu.unlock();
}
这种情况，cout仍是全局可以访问的。在使用的过程中，最好将互斥量与保护的对象进行绑定。

3、如下，所保护对象m_f就完全在互斥对象的保护之下；任何线程不通过LofFile类，对m_f进行调用；
（1）所以LofFile类中，不要有任何接口返回m_f，否则就会破坏保护，m_f就会在互斥对象保护之外被使用，暴露在类之外。
（2）同时也不同将m_f，作为函数参数传递出去，否则该函数就会在互斥对象的保护之外，操作m_f；
class LofFile
{
public:
    LofFile()
    {
        m_f.open("log.txt");
    }

    void share_print(std::string msg, int id)
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        m_f << "From " << msg << " : " << id << std::endl;
    }
private:
    std::mutex  m_mutex;
    std::ofstream m_f;
};

4、死锁
（1）、死锁产生的一个原因，就是锁的调用顺序相反，
void share_print(std::string msg, int id)
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        std::lock_guard<std::mutex> locker2(m_mutex2);
        std::cout << "From " << msg << " : " << id << std::endl;
    }

    void share_print2(std::string msg, int id)
    {
        std::lock_guard<std::mutex> locker2(m_mutex2);
        std::lock_guard<std::mutex> locker(m_mutex);
        std::cout << "From " << msg << " : " << id << std::endl;
    }
在分别调用者两个函数时，两把锁的顺序是相反的，就会出现死锁；

(2)、标准库中，std::lock(),可以保证锁的顺序
        std::lock(m_mutex, m_mutex2);
        std::lock_guard<std::mutex> locker(m_mutex,std::adopt_lock);
        std::lock_guard<std::mutex> locker2(m_mutex2, std::adopt_lock);
此时在构造lock_guard对象时，需要传入另外一个参数。

*/

