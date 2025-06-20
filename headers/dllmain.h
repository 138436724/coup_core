#pragma once

#ifdef IN_WASM
#include "emscripten/emscripten.h"
#endif // IN_WASM

#include "Coup_Enums.h"
#include <cstdint>

#ifdef IN_WINDOWS
#ifdef IS_EXPORT
#define COUP_API __declspec(dllexport)
#else
#define COUP_API __declspec(dllimport)
#endif // IS_EXPORT
#endif // IN_WINDOWS

#ifdef IN_LINUX
#define COUP_API __attribute__((visibility("default")))
#endif // IN_LINUX

#ifdef IN_WASM
#define COUP_API EMSCRIPTEN_KEEPALIVE
#endif // IN_WASM

using callbackFunc = uint8_t (*)(uint32_t, uint32_t, int, COUP::ROLE_IDENTITY *);

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
    /*
     * @brief start a game and get a room id.
     * @param uint32_t num - the id of player.
     * @return uint32_t - the id of the room.
     */
    COUP_API uint32_t startGame(uint32_t num);

    /*
     * @brief end game force
     * @param uint32_t num - the id of the room
     */
    COUP_API void endGame(uint32_t room_id);

    /*
     * @brief add a player to a room.
     * @param uint32_t room_id - the id of the room.
     * @param uint32_t player_id - the id of the player.
     * @return bool - true if success, false if failed.
     */
    COUP_API bool addIntoGame(uint32_t room_id, uint32_t player_id);

    /*
     * @brief get game status.
     * @param uint32_t room_id - the id of the room.
     * @return int - the status of the room.
     */
    COUP_API COUP::GAME_STATUS getGameStatus(uint32_t room_id);

    /*
     * @brief player action turn.
     * @param uint32_t room_id - the id of the room.
     * @param uint32_t src_player_id - the id of the src player.
     * @param uint32_t dst_player_id - the id of the dst player, if the ACTION has no dst, the dst need same with src.
     * @param COUP::ROLE_ACTION action - the action.
     * @param bool round_end - if true will do as the action chain.
     * @return bool - true if success, false if failed.
     */
    COUP_API bool playerAction(uint32_t room_id, uint32_t src_player_id, uint32_t dst_player_id, COUP::ROLE_ACTION action, bool round_end);

    /*
     * @brief get player's coins.
     * @param uint32_t room_id - the id of the room.
     * @param uint32_t num - the id of player.
     * @return uint32_t - coins the player has, if return max of uint8_t means passed arguments incorrectly.
     */
    COUP_API uint8_t getPlayerCoins(uint32_t room_id, uint32_t player_id);

    /*
     * @brief get count of player's alive identity.
     * @param uint32_t room_id - the id of the room.
     * @param uint32_t num - the id of player.
     * @return uint32_t - the count of the player's identity, if return max of uint8_t means passed arguments incorrectly.
     */
    COUP_API uint8_t getPlayerIdentityAliveNums(uint32_t room_id, uint32_t player_id);

    /*
     * @brief get player's alive identities.
     * @param uint32_t room_id - the id of the room.
     * @param uint32_t num - the id of player.
     * @return int* - the identities the player has, return nullptr means passed arguments incorrectly.
     */
    COUP_API const COUP::ROLE_IDENTITY *getPlayerIdentitiesAlive(uint32_t room_id, uint32_t player_id);

    /*
     * @brief get count of player's dead identity.
     * @param uint32_t room_id - the id of the room.
     * @param uint32_t num - the id of player.
     * @return uint32_t - the count of the player's identity, if return max of uint8_t means passed arguments incorrectly.
     */
    COUP_API uint8_t getPlayerIdentityDeadNums(uint32_t room_id, uint32_t player_id);

    /*
     * @brief get player's dead identities.
     * @param uint32_t room_id - the id of the room.
     * @param uint32_t num - the id of player.
     * @return int* - the identities the player has, return nullptr means passed arguments incorrectly.
     */
    COUP_API const COUP::ROLE_IDENTITY *getPlayerIdentitiesDead(uint32_t room_id, uint32_t player_id);

    /*
     * @brief register callback function.
     * @param callbackFunc callback - callback function.
     */
    COUP_API void registerCallback(callbackFunc f);

#ifdef __cplusplus
}
#endif // __cplusplus