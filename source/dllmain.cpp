#define COUP_API
#include "dllmain.h"

#include "Coup_Enums.h"
#include "Coup_Master.h"
#include "Coup_Player.h"
#include <unordered_map>

uint32_t room_id = 0;
std::unordered_map<uint32_t, COUP::Coup_Master *> Rooms;
std::vector<COUP::ROLE_IDENTITY> identities = {};

COUP_API uint32_t startGame(uint32_t num)
{
	auto master_id = room_id;
	COUP::Coup_Master *master = new COUP::Coup_Master(master_id, num);
	Rooms.insert({master_id, master});
	room_id++;
	return master_id;
}

COUP_API bool addIntoRoom(uint32_t room_id, uint32_t player_id)
{
	auto master = Rooms.at(room_id);
	return master->addPlayer(player_id);
}

COUP_API bool playerAction(uint32_t room_id, uint32_t src_player_id, uint32_t dst_player_id, COUP::ROLE_ACTION action, uint8_t coins, bool round_end)
{
	auto master = Rooms.at(room_id);
	return master->addAction(src_player_id, dst_player_id, action, coins, round_end);
}

COUP_API uint8_t getPlayerCoins(uint32_t room_id, uint32_t player_id)
{
	auto master = Rooms.at(room_id);
	return master->getPlayer(player_id)->getCoinsNum();
}

COUP_API uint8_t getPlayerIdentityNums(uint32_t room_id, uint32_t player_id)
{
	auto master = Rooms.at(room_id);
	return static_cast<uint8_t>(master->getPlayer(player_id)->getIdentities().size());
}

COUP_API const COUP::ROLE_IDENTITY *getPlayerIdentities(uint32_t room_id, uint32_t player_id)
{
	auto master = Rooms.at(room_id);
	identities = master->getPlayer(player_id)->getIdentities();
	return identities.data();
}

COUP_API void registerCallback(callbackFunc f)
{
	COUP::Coup_Master::registerCallback(f);
}
