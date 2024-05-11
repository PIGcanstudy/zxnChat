#ifndef SINGLETON_H
#define SINGLETON_H
#include <memory>
#include <mutex>
#include <iostream>
#include "global.h"
template<class T>
class Singleton
{
protected:
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    static std::shared_ptr<T> _ins;
public:
    static std::shared_ptr<T> GetInstance(){
        static std::once_flag flag;
        std::call_once(flag,[&](){
            //错误用法
            //_ins = std::make_shared<T>();

            //正确用法
            _ins = std::shared_ptr<T>(new T);
        });
        return _ins;
    }

    void PrintAddress(){
        std::cout << _ins.get() << std::endl;
    }

    ~Singleton(){
        std::cout << "this is destruct Singleton and destructed successed!" << std::endl;
    }
};


template<typename T>
std::shared_ptr<T> Singleton<T>::_ins = nullptr;

#endif // SINGLETON_H
