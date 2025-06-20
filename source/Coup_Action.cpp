﻿#include "Coup_Action.h"

COUP::Coup_Action::Coup_Action(const uint32_t room_id, const uint32_t src_player_id, const uint32_t dst_player_id, const ROLE_ACTION action, Coup_Action *previous_action)
    : room_id(room_id), src_player_id(src_player_id), dst_player_id(dst_player_id), action(action), previous_action(previous_action)
{
}

COUP::Coup_Action::~Coup_Action()
{
    previous_action = nullptr;
}
