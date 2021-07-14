// assertDemo.cpp : 定义控制台应用程序的入口点。
//magnet:?xt=urn:btih:2D11F1BD4FDA2A759F0E2F230C86E23F9B1364F3

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

5、使用unique_lock加锁
（1）、只对部分代码进行加锁
        std::unique_lock<std::mutex> locker(m_mutex);
        std::cout << "From " << msg << " : " << id << std::endl;
        locker.unlock();
（2）、延迟锁定，在构造的时候，传入std::defer_lock即可
        std::unique_lock<std::mutex> locker(m_mutex,std::defer_lock);
        //...do something 
        locker.lock();
        std::cout << "From " << msg << " : " << id << std::endl;
        locker.unlock();
（3）、unique_lock在unlock之后，还可以再次被lock

（4）、lock_guard/unique_lock都不可以被复制，unique_lock可以被移动，一旦unique_lock被move之后，那么mutex的控制权也将会响应的转移
unique_lock消耗更多的计算机性能。


6、
        if (!m_f.is_open())
        {
            std::unique_lock<std::mutex> locker(m_mutex_open, std::defer_lock);
            m_f.open("log.txt");
        }
这段并非绝对的线程安全；一个线程A到来，文件未被打开，锁住并打开文件；同时线程B到来时，文件还未被打开，进入if语句块；同时线程A打开了文件，并离开if语句块，
此时就会释放mutex的使用权；刚好线程B试图获取mutex的使用权；此时刚好获取成功，又会打开一次文件。换成如下

        {
            std::unique_lock<std::mutex> locker(m_mutex_open, std::defer_lock);
            if (!m_f.is_open())
            {
                m_f.open("log.txt");
            }
        }
也可以使用标准库提供的std::once_flag 和std::call_once，以及Lambda函数实现
std::once_flag m_flag;
std::call_once(m_flag, [&](){m_f.open("log.txt"); });


三、条件变量
1、使用条件变量等待
（1）、条件变量，wait的时候不，需要对mutex多次加解锁，所以只能使用unique_lock变量作为参数传递进去；
（2）、wait的时候，可以增加一个Lambda函数作为参数传进去，当Lambda函数返回失败时，将会一直等待。
void function3()
{
    int data = 0;
    while (data != 1)
    {
        std::unique_lock<std::mutex> locker(mu);
        cond.wait(locker, [](){return !q.empty(); });  //wait函数内会解锁mutex对象，这里需要重复的加解锁，所以只能使用unique_lock；同时这里也会出现伪激活（自己激活自己），可以在wait增加一个lambda函数,当q为空时，将会一直等待下去

        data = q.back();  //取出最后一个元素值
        q.pop_back();  //弹出最后一个元素
        locker.unlock();
        std::cout << "t2 got a value from t1: " << data << std::endl;
    }
}

四、Future、Promise子线程和父线程之间数据的传递
父线程中获取子线程中的变量
    int x;
    std::future<int> fu = std::async(std::launch::deferred,factorial, 4);
    x = fu.get();
（1）、future的get函数将会等待子线程执行结束，并获取子线程的返回值；同时get只能被调用一次，否则会出现崩溃
（2）、使用std::async构造std::future对象，并不一定会立即起子线程；取决于构造时的第一个参数，如果第一个参数是std::launch::deferred，则会一直等到get被
调用时才会去执行函数factorial，并返回factorial的执行结果，此时在同一个线程中执行factorial；
如果第一个参数是std::launch::async，则会另起一个线程执行factorial；

子线程中获取父线程中的变量
    std::promise<int> p;
    std::future<int> f = p.get_future();
    std::future<int> fu = std::async(std::launch::async,factorial, std::ref(f));	//子线程从父线程中拿数据
    p.set_value(4);
    x = fu.get();	//父线程从子线程中拿数据
（1）、定义std::promise之后，就必须调用set_value，否则，子线程拿到父线程的std::future变量，进行get的时候，就会跑出异常；
（2）、std::promise、std::future只能move不能复制拷贝
（3）、当有多个线程，需要拿到父线程不同的数据执行时，一种方式是构造多个std::future和std::promise，进行多次set_value；
还有一种方式是使用std::shared_future<int> sf = f.share()；这样sf就可以进行拷贝复制了，这是子线程的参数可以不使用引用传参；同时只需要一次set_value。
    std::promise<int> p;
    std::future<int> f = p.get_future();
    std::shared_future<int> sf = f.share();
    std::future<int> fu1 = std::async(std::launch::deferred, factorial, sf);
    std::future<int> fu2 = std::async(std::launch::deferred, factorial, sf);
    std::future<int> fu3 = std::async(std::launch::deferred, factorial, sf);
    std::future<int> fu4 = std::async(std::launch::deferred, factorial, sf);
    p.set_value(4);
    int x1 = fu1.get();
    int x2 = fu2.get();
    int x3 = fu3.get();
    int x4 = fu4.get();

    std::cout << "Get from child is: " << x1 << std::endl;
    std::cout << "Get from child is: " << x2 << std::endl;
    std::cout << "Get from child is: " << x3 << std::endl;
    std::cout << "Get from child is: " << x4 << std::endl;

五、std::packaged_task
int factorial(int N)
{
    int res = 1;
   
    for (int i = N; i > 1; --i)
    {
        res *= i;
    }
    std::cout << "Result is: " << res << std::endl;
    return res;

}

std::packaged_task<int()> t(std::bind(factorial,6));      
    //可以异步获取，可调用对象的执行结果(不用限制只能从父线程中获取子线程的执行结果，可以是两个完全不相干的线程)
    //创建时只能传递一个参数，如果可调用对象需要传递参数时，可以使用std::bind获取一个函数对象

    std::future<int> ret = t.get_future();
    int value = ret.get();          //等待任务执行完成并获取结果
*/

