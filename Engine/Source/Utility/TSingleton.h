#pragma once

template<typename T>
class TSingleton
{
public:
    static T& GetInstance()
    {
        static T singletonInstance;
        return singletonInstance;
    }

    TSingleton(T&&) = delete;
    TSingleton(const T&) = delete;

    void operator= (const T&) = delete;

private:
    TSingleton() = default;
    virtual ~TSingleton() = default;
};
