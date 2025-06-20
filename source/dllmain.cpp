#ifdef IN_WINDOWS
#define IS_EXPORT
#endif // IN_WINDOWS

#include "dllmain.h"

#include "Coup_Enums.h"
#include "Coup_Master.h"
#include "Coup_Player.h"
#include <limits>
#include <memory>
#include <unordered_map>

uint32_t room_id = 0;
std::vector<uint32_t> room_id_recovery = {};
std::unordered_map<uint32_t, std::unique_ptr<COUP::Coup_Master>> Rooms = {};
std::vector<COUP::ROLE_IDENTITY> identities = {};

COUP_API uint32_t startGame(uint32_t num)
{
    uint32_t master_id;

    if (room_id_recovery.empty())
    {
        master_id = room_id;
        room_id++;
    }
    else
    {
        master_id = room_id_recovery.back();
        room_id_recovery.pop_back();
    }

    Rooms.emplace(master_id, std::make_unique<COUP::Coup_Master>(master_id, num));
    return master_id;
}

COUP_API void endGame(uint32_t room_id)
{
    auto master = Rooms.find(room_id);
    if (master != Rooms.end())
    {
        Rooms.erase(master);
        room_id_recovery.push_back(room_id);
    }
}

COUP_API bool addIntoGame(uint32_t room_id, uint32_t player_id)
{
    auto master = Rooms.find(room_id);
    if (master != Rooms.end())
    {
        return master->second->addPlayer(player_id);
    }
    return false;
}

COUP_API COUP::GAME_STATUS getGameStatus(uint32_t room_id)
{
    auto master = Rooms.find(room_id);
    if (master != Rooms.end())
    {
        return master->second->getGameStatus();
    }
    return COUP::GAME_STATUS::NONE;
}

COUP_API bool playerAction(uint32_t room_id, uint32_t src_player_id, uint32_t dst_player_id, COUP::ROLE_ACTION action, bool round_end)
{
    auto master = Rooms.find(room_id);
    if (master != Rooms.end())
    {
        return master->second->addAction(src_player_id, dst_player_id, action, round_end);
    }
    return false;
}

COUP_API uint8_t getPlayerCoins(uint32_t room_id, uint32_t player_id)
{
    auto master = Rooms.find(room_id);
    if (master != Rooms.end())
    {
        return master->second->getPlayer(player_id)->getCoinsNum();
    }
    return std::numeric_limits<uint8_t>::max();
}

COUP_API uint8_t getPlayerIdentityAliveNums(uint32_t room_id, uint32_t player_id)
{
    auto master = Rooms.find(room_id);
    if (master != Rooms.end())
    {
        return static_cast<uint8_t>(master->second->getPlayer(player_id)->getIdentitiesAlive().size());
    }
    return std::numeric_limits<uint8_t>::max();
}

COUP_API const COUP::ROLE_IDENTITY *getPlayerIdentitiesAlive(uint32_t room_id, uint32_t player_id)
{
    auto master = Rooms.find(room_id);
    if (master != Rooms.end())
    {
        identities = master->second->getPlayer(player_id)->getIdentitiesAlive();
        return identities.data();
    }
    return nullptr;
}

COUP_API uint8_t getPlayerIdentityDeadNums(uint32_t room_id, uint32_t player_id)
{
    auto master = Rooms.find(room_id);
    if (master != Rooms.end())
    {
        return static_cast<uint8_t>(master->second->getPlayer(player_id)->getIdentitiesDead().size());
    }
    return std::numeric_limits<uint8_t>::max();
}

COUP_API const COUP::ROLE_IDENTITY *getPlayerIdentitiesDead(uint32_t room_id, uint32_t player_id)
{
    auto master = Rooms.find(room_id);
    if (master != Rooms.end())
    {
        identities = master->second->getPlayer(player_id)->getIdentitiesDead();
        return identities.data();
    }
    return nullptr;
}

COUP_API void registerCallback(callbackFunc f)
{
    if (!f)
    {
        COUP::Coup_Master::registerCallback(f);
    }
}
