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

private:
    FConsoleVariableManager();
    virtual ~FConsoleVariableManager();

private:
    Virtuoso::QuakeStyleConsole* mConsole;
    std::string mCommandHistoryFileName;

};
