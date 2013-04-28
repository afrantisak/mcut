#pragma once
#include <exception>

class ConsoleInterrupt
{
    public:
        ConsoleInterrupt();
        ~ConsoleInterrupt();
        
        bool getTriggered() const { return m_bTriggered; }

        class Interrupted : public std::exception {};
        
        // throws Interrupted() if triggered;
        void triggerThrow() const;
        
    private:
        static bool m_bTriggered;
        
        static void sigint(int nParam);
};

