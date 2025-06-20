#pragma once

#include "Coup_Enums.h"
#include <cstdint>

namespace COUP
{
    class Coup_Action
    {
    public:
        uint32_t room_id;                   // 房间号
        uint32_t src_player_id;             // 发起者
        uint32_t dst_player_id;             // 接收者
        ROLE_ACTION action;                 // 发起行为
        const Coup_Action *previous_action; // 上一个动作

    public:
        Coup_Action(const uint32_t room_id, const uint32_t src_player_id, const uint32_t dst_player_id, const ROLE_ACTION action, Coup_Action *previous_action);
        ~Coup_Action();
    };
} // namespace COUP