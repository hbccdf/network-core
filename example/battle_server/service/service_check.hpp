#pragma once

#define CHECK_BATTLE_MANAGER(name)  \
battle_manager* name = server.get_service<battle_manager>();    \
if (!name)                                                      \
{                                                               \
    LOG_ERROR("battle manager is not exist");                   \
    return;                                                     \
}

