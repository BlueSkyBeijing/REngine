#pragma once

template<typename T>
class TSingleton
{
public:
    static T& GetInstance() {
        static T singletonValue;
        return singletonValue;
    }

private:
    //TSingleton()
    //{

    //}
    //~TSingleton()
    //{}
    //TSingleton(const TSingleton&)
    //{

    //}
    //TSingleton& operator=(const TSingleton&)
    //{

    //}
};