#pragma once
#include <memory>

namespace Util
{
    template <typename T>
    class Singleton
    {
    public:
        static std::shared_ptr<T> Instance()
        {
            static std::shared_ptr<T> Inst = std::make_shared<T>();
            return Inst;
        }

        Singleton(Singleton const&) = delete;
        Singleton& operator=(Singleton const&) = delete;
    protected:
        Singleton()
        {
        };
    };
}