#include "PrecompiledHeader.h"

#include "FConsoleVariableManager.h"

FConsoleVariableManager::FConsoleVariableManager() :
    mCommandHistoryFileName("Saved\\CommandHistory.txt")
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

    mConsole->loadHistoryBuffer(mCommandHistoryFileName);
}

void FConsoleVariableManager::UnInit()
{
    mConsole->saveHistoryBuffer(mCommandHistoryFileName);

    delete mConsole;
    mConsole = nullptr;

}

template <class T>
void FConsoleVariableManager::BindCVar(const std::string& str, T& var, const std::string& help)
{
    mConsole->bindCVar(str, var, help);
}

void FConsoleVariableManager::ProcessCommand()
{
    std::cout << "\nplease input your command:" << std::endl;
    mConsole->commandExecute(std::cin, std::clog);
}