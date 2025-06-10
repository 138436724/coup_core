#pragma once

#include "Coup_Enums.h"
#include <iostream>

#ifdef _WIN32
#ifdef COUP_API
#define COUP_API __declspec(dllexport)
#else
#define COUP_API __declspec(dllimport)
#endif // COUP_API
#endif // _WIN32

#ifdef __unix__
#define COUP_API __attribute__((visibility("default")))
#endif // __unix__

typedef uint8_t (*callbackFunc)(uint32_t room_id, uint32_t player_id, int identities_num, COUP::ROLE_IDENTITY *identities);

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
	/*
	 * @brief start a game and get a room id
	 * @param uint32_t num - the id of player
	 * @return uint32_t - the id of the room
	 */
	COUP_API uint32_t startGame(uint32_t num);

	/*
	 * @brief add a player to a room
	 * @param uint32_t room_id - the id of the room
	 * @param uint32_t player_id - the id of the player
	 * @return bool - true if success, false if failed
	 */
	COUP_API bool addIntoRoom(uint32_t room_id, uint32_t player_id);

	/*
	 * @brief player action turn
	 * @param uint32_t room_id - the id of the room
	 * @param uint32_t src_player_id - the id of the src player
	 * @param uint32_t dst_player_id - the id of the dst player
	 * @param COUP::ROLE_ACTION action - the action
	 * @param uint8_t coins - the number of coins, just use by CAPTAIN
	 * @param bool round_end - if true will do as the action chain
	 * @return bool - true if success, false if failed
	 */
	COUP_API bool playerAction(uint32_t room_id, uint32_t src_player_id, uint32_t dst_player_id, COUP::ROLE_ACTION action, uint8_t coins, bool round_end);

	/*
	 * @brief get player's coins
	 * @param uint32_t room_id - the id of the room
	 * @param uint32_t num - the id of player
	 * @return uint32_t - coins the player has
	 */
	COUP_API uint8_t getPlayerCoins(uint32_t room_id, uint32_t player_id);

	/*
	 * @brief get count of player's identity
	 * @param uint32_t room_id - the id of the room
	 * @param uint32_t num - the id of player
	 * @return uint32_t - the count of the player's identity
	 */
	COUP_API uint8_t getPlayerIdentityNums(uint32_t room_id, uint32_t player_id);

	/*
	 * @brief get player's identities.
	 * @param uint32_t room_id - the id of the room
	 * @param uint32_t num - the id of player
	 * @return uint32_t - the identities the player has
	 */
	COUP_API const COUP::ROLE_IDENTITY *getPlayerIdentities(uint32_t room_id, uint32_t player_id);

	/*
	 * @brief register callback function
	 * @param callbackFunc callback - callback function
	 */
	COUP_API void registerCallback(callbackFunc f);

#ifdef __cplusplus
}
#endif // __cplusplus