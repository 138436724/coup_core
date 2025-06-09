#pragma once

#include "Coup_Enums.h"
#include <iostream>

#ifdef MY_WINDOWS
#ifdef COUP_API
#define COUP_API __declspec(dllexport)
#else
#define COUP_API __declspec(dllimport)
#endif // COUP_API
#endif // MY_WINDOWS

#ifdef MY_LINUX
#define COUP_API __attribute__((visibility("default")))
#endif // MY_LINUX

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
	/*
	 * @brief start a game and get a room id
	 * @param uint32_t num - the number of players
	 * @return uint32_t - the id of the room
	 */
	COUP_API uint32_t startGame(uint32_t num);

	/*
	 * @brief add a player to a room
	 * @param uint32_t room_id - the id of the room
	 * @param uint32_t player_id - the id of the player
	 * @return COUP::ROLE_IDENTITY - player's intial identity
	 */
	COUP_API COUP::ROLE_IDENTITY *addIntoRoom(uint32_t room_id, uint32_t player_id);

	/*
	 * @brief player action turn
	 * @param uint32_t room_id - the id of the room
	 * @param uint32_t src_player_id - the id of the src player
	 * @param uint32_t dst_player_id - the id of the dst player
	 * @param COUP::ROLE_ACTION action - the action
	 * @param uint8_t coins - the number of coins, just use by CAPTAIN
	 * @param COUP::ROLE_IDENTITY identity - the identity, use by DOUBT
	 * @return COUP::ROLE_IDENTITY - if AMBASSADOR use skill, then return the identity of the player
	 */
	COUP_API COUP::ROLE_IDENTITY *playerAction(uint32_t room_id, uint32_t src_player_id, uint32_t dst_player_id, COUP::ROLE_ACTION action, uint8_t coins, COUP::ROLE_IDENTITY identity, bool round_end);

#ifdef __cplusplus
}
#endif // __cplusplus