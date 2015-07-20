#ifndef XAL_CLIENT_HPP
#define XAL_CLIENT_HPP

#include "Global.hpp"

#include "StateMachine.hpp"
#include "SetupState.hpp"

namespace xal
{

class Client
{
public:
    Client();

    void run();
private:

private:
    StateMachine m_machine;
};

}

#endif
