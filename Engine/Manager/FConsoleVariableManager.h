#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"

class FConsoleVariableManager
{
    friend class TSingleton<FConsoleVariableManager>;

public:

    void Init();
    void UnInit();

    template <class T>
    void BindCVar(const std::string& varname, T& var, const std::string& help = "");

    void ProcessCommand();

    void ProcessCommand(std::string& cmd);

    template <typename... Args>
    void BindCommand(const std::string& commandName, std::function<void(Args...)> fun, const std::string& help = "")
    {
        mConsole->bindCommand(commandName, fun, help);
    };

private:
    FConsoleVariableManager();
    virtual ~FConsoleVariableManager();

private:
    Virtuoso::QuakeStyleConsole* mConsole;
    std::string mCommandHistoryFileName;

};

//template <typename T>
class FConsoleVariable
{
public:
    template <typename... Args>
    FConsoleVariable(const std::string& commandName, std::function<void(Args...)> fun, const std::string& help = "")
    {
        TSingleton<FConsoleVariableManager>::GetInstance().BindCommand(commandName, fun, help);
    }
};