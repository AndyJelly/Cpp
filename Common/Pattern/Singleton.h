//实现单例类模板

#pragma once

#include "Noncopyable.h"
#include <mutex>
#include <memory>

namespace Common
{
    //这里使用模板来封装单例的原因是，每次都是想new出来一个子类的对象；如果只是有一个单例的基类，所有子类继承与基类；
    //那么整个系统new出来的子类对象，都是基类的对象，而且是整个系统唯一的。

    /** @class
    *  @brief 设计模式的单件模式，用来取代全局变量
    *
    *  全局只有一份内存，当需要使用的时候才被分配内存
    *  使用的时候通过函数Instance()获取对象的引用
    *
    *
    * 从boost切换为std后，单例有BUG
    * 线程1进入Instance，调用static std::mutex mutex;初始化,切换到线程2
    * 线程2不会再进行static，会直接使用mutex，此时mutex未初始化完成，会崩溃
    * boost不会，boost中初始化mutex是不允许被中断切换的
    * 修改方法，将static变量从函数中移除，这样和全局变量一样，在程序启动时，就会优先初始化完成
    *  单件模式，基类使用的模型
    *  class MyClass : public CSingleton<MyClass>
    *  {
    *      //在定义一个模板类子类的时候，必须加上下面这句；否则在CMyClass以外就没法使用Instance()构造对象
    *      friend class CSingleton<MyClass>;
    *  private:
    *      MyClass() {}
    *  public:
    *      ...
    *  };
    */

     template <class T>
     class CSingleton : private CNoncopyable     //采用 private继承，  CNoncopyable所有的方法都变成CSingleton私有的，CSingleton的子类也没法使用
     {
         //将单例类实现成不可拷贝，所以单例类的初始化，需要有一个单独的初始化接口对外提供
     public:
         //供外界调用，创建CSingleton对象
         static T& Instance();
         
     protected:
         //构造函数只有子类可以调用
         CSingleton(){};

         static std::unique_ptr<T> m_instance_;
         static std::mutex m_mutex_;
     };

     //类模板的静态数据成员，必须在类模板定义之外初始化
     //类模板静态数据成员本身就是一个模板，它的定义不会引起内存被分配，只有对其实例化才会分配内存。
     template<class T>
     std::unique_ptr<T> CSingleton<T>::m_instance_ /* = nullptr*/;

     template<class T>
     std::mutex CSingleton<T>::m_mutex_;

     template<class T>
     inline T& CSingleton<T>::Instance()
     {
         if (m_instance_.get() == nullptr)
         {
             //还没有被初始化
             //! 加锁避免同时被创建两个实例
             std::lock_guard<std::mutex> locker(m_mutex_);
             //两个线程同时到来时，此时 m_instance_为空，都进入第一个if语句；线程1获取互斥量的使用权，线程2只有等待；
             //当线程1创建实例结束之后，离开 m_instance_已不为空了
             if (m_instance_.get() == nullptr)
             {
                 m_instance_ = std::unique_ptr<T>(new T);
             }
         }

         return *m_instance_.get();
     }


    //使用宏实现单例类
#define SINGLETON_IMPL(name)\
    public:\
    static name& Instance()\
              {\
        static std::unique_ptr<name> ptr_instance_;\
        static std::mutex mutex_intance_;\
        if (!ptr_instance_)\
                        {\
            std::lock_guard<std::mutex> lock(mutex_intance_);\
            if (!ptr_instance_)\
                                    {\
                ptr_instance_ = std::unique_ptr<name>(new name);\
                                    }\
                        }\
        return *ptr_instance_.get();\
              }

}
