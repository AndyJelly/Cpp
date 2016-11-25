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
