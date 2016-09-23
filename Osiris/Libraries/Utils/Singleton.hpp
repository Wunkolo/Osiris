#pragma once
#include <memory>
#include "NonCopyable.hpp"

namespace Util
{
    template <typename T>
    class Singleton : private Util::NonCopyable<Singleton<T> >
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