#include "StateManager.h"

StateManager& StateManager::getInstance()
{
    static StateManager manager;
    return manager;
}
