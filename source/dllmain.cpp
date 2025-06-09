#define COUP_API
#include "dllmain.h"

#include "Coup_Enums.h"
#include "Coup_Master.h"
#include <unordered_map>

uint32_t room_id = 0;
std::unordered_map<uint32_t, COUP::Coup_Master *> Rooms;

COUP_API uint32_t startGame(uint32_t num)
{
	auto master_id = room_id;
	COUP::Coup_Master *master = new COUP::Coup_Master(master_id, num);
	Rooms.insert({master_id, master});
	room_id++;
	return master_id;
}

COUP_API COUP::ROLE_IDENTITY *addIntoRoom(uint32_t room_id, uint32_t player_id)
{
	auto master = Rooms.at(room_id);
	if (master->addPlayer(player_id))
	{
		// return nullptr;
	}
	return nullptr; // todo
}

COUP_API COUP::ROLE_IDENTITY *playerAction(uint32_t room_id, uint32_t src_player_id, uint32_t dst_player_id, COUP::ROLE_ACTION action, uint8_t coins, COUP::ROLE_IDENTITY identity, bool round_end)
{
	auto master = Rooms.at(room_id);
	master->addAction(src_player_id, dst_player_id, action, coins, identity, round_end);

	return nullptr;
}
