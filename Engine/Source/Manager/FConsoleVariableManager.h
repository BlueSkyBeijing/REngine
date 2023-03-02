#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"

class DLL_API FConsoleVariableManager
{
    friend class TSingleton<FConsoleVariableManager>;

public:

    void Init();
    void UnInit();

    template <class T>
    void BindCVar(const FString& varname, T& var, const FString& help = "");

    void ProcessCommand();

    void ProcessCommand(FString& cmd);

    template <typename... Args>
    void BindCommand(const FString& commandName, std::function<void(Args...)> fun, const FString& help = "")
    {
        mConsole->bindCommand(commandName, fun, help);
    };

private:
    FConsoleVariableManager();
    virtual ~FConsoleVariableManager();

private:
    Virtuoso::QuakeStyleConsole* mConsole;
    FString* mCommandHistoryFileName;

};

//template <typename T>
class FConsoleVariable
{
public:
    template <typename... Args>
    FConsoleVariable(const FString& commandName, std::function<void(Args...)> fun, const FString& help = "")
    {
        TSingleton<FConsoleVariableManager>::GetInstance().BindCommand(commandName, fun, help);
    }
};