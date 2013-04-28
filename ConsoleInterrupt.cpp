#include "ConsoleInterrupt.h"
#include "signal.h"

bool ConsoleInterrupt::m_bTriggered(false);

ConsoleInterrupt::ConsoleInterrupt()
{
    signal(SIGINT, sigint);
}

ConsoleInterrupt::~ConsoleInterrupt()
{
    
}

void ConsoleInterrupt::sigint(int nParam)
{
    m_bTriggered = true;
}

void ConsoleInterrupt::triggerThrow() const
{
    if (getTriggered())
    {
        throw Interrupted();
    }
}
