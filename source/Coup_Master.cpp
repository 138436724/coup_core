#include "Coup_Master.h"
#include "Coup_Player.h"
#include <algorithm>
#include <random>

COUP::Coup_Master::Coup_Master(uint32_t id, int num)
	: id(id),
	  num_players(num),
	  game_status(COUP::GAME_STATUS::JOIN),
	  all_players(),
	  all_identities{COUP::ROLE_IDENTITY::DUKE, COUP::ROLE_IDENTITY::ASSASSIN, COUP::ROLE_IDENTITY::CONTESSA, COUP::ROLE_IDENTITY::CAPTAIN, COUP::ROLE_IDENTITY::AMBASSADOR,
					 COUP::ROLE_IDENTITY::DUKE, COUP::ROLE_IDENTITY::ASSASSIN, COUP::ROLE_IDENTITY::CONTESSA, COUP::ROLE_IDENTITY::CAPTAIN, COUP::ROLE_IDENTITY::AMBASSADOR,
					 COUP::ROLE_IDENTITY::DUKE, COUP::ROLE_IDENTITY::ASSASSIN, COUP::ROLE_IDENTITY::CONTESSA, COUP::ROLE_IDENTITY::CAPTAIN, COUP::ROLE_IDENTITY::AMBASSADOR,
					 COUP::ROLE_IDENTITY::DUKE, COUP::ROLE_IDENTITY::ASSASSIN, COUP::ROLE_IDENTITY::CONTESSA, COUP::ROLE_IDENTITY::CAPTAIN, COUP::ROLE_IDENTITY::AMBASSADOR},
	  action_queue()
{
	shuffleCards();
}

COUP::Coup_Master::~Coup_Master()
{
	for (auto &player : all_players)
	{
		delete player.second;
	}
	all_players.clear();
	all_identities.clear();
	action_queue.clear();
}

void COUP::Coup_Master::shuffleCards()
{
	std::random_device device;
	std::mt19937 g(device());
	std::shuffle(all_identities.begin(), all_identities.end(), g);
}

std::vector<COUP::ROLE_IDENTITY> COUP::Coup_Master::drawCards(int num)
{
	std::vector<ROLE_IDENTITY> identities;

	for (int i = 0; i < num; i++)
	{
		identities.push_back(all_identities.front());
		all_identities.pop_front();
	}

	return identities;
}

void COUP::Coup_Master::returnCards(const std::vector<ROLE_IDENTITY> &identities)
{
	for (auto &identity : identities)
	{
		all_identities.push_back(identity);
	}
}

bool COUP::Coup_Master::addPlayer(uint32_t player_id)
{
	if (game_status == COUP::GAME_STATUS::FULL)
	{
		return false;
	}

	if (all_players.find(player_id) == all_players.end())
	{
		auto cards = drawCards(2);
		Coup_Player *player = new Coup_Player(player_id, cards);
		all_players.insert({player_id, player});

		if (all_players.size() == num_players)
		{
			game_status = COUP::GAME_STATUS::FULL;
		}

		return true;
	}
	return false;
}

COUP::Coup_Player *COUP::Coup_Master::getPlayer(uint32_t player_id) const
{
	return all_players.at(player_id);
}

void COUP::Coup_Master::handleAction(const Coup_Action *action)
{
	if (!action)
	{
		return;
	}

	COUP::Coup_Player *src_player = getPlayer(action->src_player_id);
	COUP::Coup_Player *dst_player = getPlayer(action->dst_player_id);

	const COUP::Coup_Action *pre_action = nullptr;
	const COUP::Coup_Action *pre_pre_action = nullptr;

	COUP::Coup_Player *pre_src_player = nullptr;
	COUP::Coup_Player *pre_dst_player = nullptr;
	COUP::Coup_Player *pre_pre_src_player = nullptr;
	COUP::Coup_Player *pre_pre_dst_player = nullptr;

	if (action->previous_action)
	{
		pre_action = action->previous_action;
		pre_src_player = getPlayer(pre_action->src_player_id);
		pre_dst_player = getPlayer(pre_action->dst_player_id);
	}

	if (pre_action && pre_action->previous_action)
	{
		pre_pre_action = pre_action->previous_action;
		pre_pre_src_player = getPlayer(pre_pre_action->src_player_id);
		pre_pre_dst_player = getPlayer(pre_pre_action->dst_player_id);
	}

	switch (action->action)
	{
	case COUP::ROLE_ACTION::INCOME:
		src_player->setCoinsNum(src_player->getCoinsNum() + 1);
		break;
	case COUP::ROLE_ACTION::COUP:
		src_player->setCoinsNum(src_player->getCoinsNum() - 7);
		break;

	case COUP::ROLE_ACTION::DUCK:
		src_player->setCoinsNum(src_player->getCoinsNum() + 3);
		break;

	case COUP::ROLE_ACTION::AMBASSADOR:
		dst_player->setIdentity(/*todo*/);
		break;

	case COUP::ROLE_ACTION::FOREIGE_AID:
		src_player->setCoinsNum(src_player->getCoinsNum() + 2);
		break;

	case COUP::ROLE_ACTION::ASSASSIN:
		src_player->setCoinsNum(src_player->getCoinsNum() - 3);
		dst_player->setIdentity(/*todo*/);
		break;

	case COUP::ROLE_ACTION::CAPTAIN:
		src_player->setCoinsNum(src_player->getCoinsNum() + action->coins);
		dst_player->setCoinsNum(dst_player->getCoinsNum() - action->coins);
		break;

	case COUP::ROLE_ACTION::BLOCK:
		if (pre_action && pre_action->action == COUP::ROLE_ACTION::ASSASSIN)
		{
			pre_src_player->setCoinsNum(pre_src_player->getCoinsNum() - 3);
		}
		break;

	case COUP::ROLE_ACTION::DOUBT:
		if (dst_player->checkIdentity(action->identity))
		{
			// 丢失一张身份 todo
			handleAction(pre_action); // dst拥有对应身份牌，src质疑失败，处理上一步
		}
		else
		{
			handleAction(pre_pre_action); // dst没有对应身份牌，src质疑成功，跳过上一步
		}
		break;

	default:
		break;
	}
}

bool COUP::Coup_Master::addAction(uint32_t src_player_id, uint32_t dst_player_id, COUP::ROLE_ACTION action, uint8_t coins, COUP::ROLE_IDENTITY identity, bool round_end)
{
	Coup_Action *now_action = nullptr;
	switch (action)
	{
	case COUP::ROLE_ACTION::INCOME:
	case COUP::ROLE_ACTION::COUP:
	case COUP::ROLE_ACTION::DUCK:
	case COUP::ROLE_ACTION::AMBASSADOR:
	case COUP::ROLE_ACTION::FOREIGE_AID:
	case COUP::ROLE_ACTION::ASSASSIN:
	case COUP::ROLE_ACTION::CAPTAIN:
		now_action = new Coup_Action(id, src_player_id, dst_player_id, action, coins, identity, nullptr);
		break;

	case COUP::ROLE_ACTION::BLOCK:
	case COUP::ROLE_ACTION::DOUBT:
		now_action = new Coup_Action(id, src_player_id, dst_player_id, action, coins, identity, action_queue.back());
		break;
	default:
		break;
	}

	action_queue.push_back(now_action);

	if (round_end)
	{
		handleAction(now_action);
	}

	return true;
}

const COUP::Coup_Action *COUP::Coup_Master::getLastAction() const
{
	return action_queue.back();
}