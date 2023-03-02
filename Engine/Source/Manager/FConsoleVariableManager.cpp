#include "PrecompiledHeader.h"

#include "FConsoleVariableManager.h"

FConsoleVariableManager::FConsoleVariableManager() :
    mCommandHistoryFileName(new FString("Saved\\CommandHistory.txt"))
{
}

FConsoleVariableManager::~FConsoleVariableManager()
{
}

void FConsoleVariableManager::Init()
{
    mConsole = new Virtuoso::QuakeStyleConsole;
    std::function <void(void)> printHistory = [this]()
    {
        for (int i = 0; i < this->mConsole->historyBuffer().size(); i++)
        {
            std::clog << this->mConsole->historyBuffer()[i] << std::endl;
        }
    };

    mConsole->bindCommand("printHistory", printHistory, "Print the current command history buffer for the console.");

    mConsole->loadHistoryBuffer(*mCommandHistoryFileName);
}

void FConsoleVariableManager::UnInit()
{
    mConsole->saveHistoryBuffer(*mCommandHistoryFileName);

    delete mConsole;
    mConsole = nullptr;

    delete mCommandHistoryFileName;
    mCommandHistoryFileName = nullptr;
}

template <class T>
void FConsoleVariableManager::BindCVar(const FString& str, T& var, const FString& help)
{
    mConsole->bindCVar(str, var, help);
}

void FConsoleVariableManager::ProcessCommand()
{
    std::cout << "\nplease input your command:" << std::endl;
    mConsole->commandExecute(std::cin, std::clog);
}

void FConsoleVariableManager::ProcessCommand(FString& cmd)
{
    mConsole->commandExecute(cmd, std::clog);
}