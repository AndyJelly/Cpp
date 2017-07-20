
//实现不可赋值类
#pragma once

namespace Common
{
    class CNoncopyable
    {
        /*
        一.拷贝构造函数与赋值操作符
        class A;
        A a;
        A b1(a);    //拷贝构造函数的实例
        A b2 = a;  //赋值操作符的实例：
        二.拷贝构造函数的应用实例
        1.同一类型的对象显示或隐式初始化一个对象，如下：
        class A;
        A a;
        A b(a);
        2.作为函数的实参，如下：
        void work(const A& a);
        3.作为函数的返回值
        A& getA() const
        {
            ...
            return a;
        }
        如果用户没有显式定义复制构造函数或赋值操作符，编译器将会默认的合成一个复制构造 函数。
        合成复制构造函数与复制构造函数，或者合成赋值操作符与赋值操作符不同，编译器默认合成的函数的执行行为是"逐个成员初始化"，
        将新对象初始化为原对象的副本。逐个成员初始化不包括static成员，只负责初始化非static成员。
        */


    protected:
        //构造函数和析构函数是 protected的，只有子类可以访问；即只有子类可以创建CNoncopyable对象
        CNoncopyable() 
        {}
        ~CNoncopyable()
        {}

    private:
        //拷贝构造函数是私有的

        //拷贝构造函数，拷贝构造函数的名称必须与类名称一致，函数的形式参数是本类型的一个引用变量,且必须是引用。

        //深拷贝和浅拷贝：对于类中还有指针成员变量时，只用默认的拷贝构造函数拷贝会导致，两个对象的的指针指向同一块内存地址；
        //从而，一个类析构之后，另一个类使用指针成员变量将会引起崩溃；这就是浅拷贝。
        //可以自己申明一个自定义的拷贝构造函数，重新为该成员变量申请内存空间并进行赋值；这就是深拷贝
        CNoncopyable(const CNoncopyable&);

        //重载赋值运算符
        const CNoncopyable& operator = (const CNoncopyable&);
    };
}
