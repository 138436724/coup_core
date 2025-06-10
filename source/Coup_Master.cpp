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

void COUP::Coup_Master::returnCards(const ROLE_IDENTITY identity)
{
	all_identities.push_back(identity);
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
		std::vector<COUP::ROLE_IDENTITY> cards = drawCards(2);
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

	bool has_identity = false;
	std::vector<COUP::ROLE_IDENTITY> identity_was_doubt = {};

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
		if (true)
		{
			std::vector<COUP::ROLE_IDENTITY> cards_drawed = drawCards(2);
			std::vector<COUP::ROLE_IDENTITY> cards_had = dst_player->getIdentities();

			std::vector<COUP::ROLE_IDENTITY> cards_all(cards_drawed.begin(), cards_drawed.end());
			cards_all.insert(cards_all.end(), cards_had.begin(), cards_had.end());

			// send cards and choose how todo ?
			uint8_t num_to_fold1 = callback(id, action->dst_player_id, cards_all.size(), cards_all.data());
			cards_all.erase(cards_all.begin() + num_to_fold1);

			uint8_t num_to_fold2 = callback(id, action->dst_player_id, cards_all.size(), cards_all.data());
			cards_all.erase(cards_all.begin() + num_to_fold2);

			dst_player->setIdentity(cards_all);
		}
		break;

	case COUP::ROLE_ACTION::FOREIGE_AID:
		src_player->setCoinsNum(src_player->getCoinsNum() + 2);
		break;

	case COUP::ROLE_ACTION::ASSASSIN:
		src_player->setCoinsNum(src_player->getCoinsNum() - 3);
		if (dst_player->getIdentities().size() == 1 || dst_player->getIdentities().size() == 0)
		{
			std::vector<COUP::ROLE_IDENTITY> cards = dst_player->getIdentities();
			returnCards(cards);
			dst_player->setIdentity();
		}
		else if (dst_player->getIdentities().size() == 2)
		{
			std::vector<COUP::ROLE_IDENTITY> cards = dst_player->getIdentities();
			uint8_t num_to_fold = callback(id, action->dst_player_id, cards.size(), cards.data());

			returnCards(cards.at(num_to_fold));
			dst_player->setIdentity(cards.at(1 - num_to_fold));
		}
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
		// 推断身份牌
		if (pre_action && pre_action->action == COUP::ROLE_ACTION::BLOCK)
		{
			// 继续往前推断
			if (pre_pre_action && pre_pre_action->action == COUP::ROLE_ACTION::FOREIGE_AID)
			{
				identity_was_doubt.push_back(COUP::ROLE_IDENTITY::DUKE);
				has_identity = dst_player->checkIdentity(COUP::ROLE_IDENTITY::DUKE);
			}
			else if (pre_pre_action && pre_pre_action->action == COUP::ROLE_ACTION::ASSASSIN)
			{
				identity_was_doubt.push_back(COUP::ROLE_IDENTITY::CONTESSA);
				has_identity = dst_player->checkIdentity(COUP::ROLE_IDENTITY::CONTESSA);
			}
			else if (pre_pre_action && pre_pre_action->action == COUP::ROLE_ACTION::CAPTAIN)
			{
				identity_was_doubt.push_back(COUP::ROLE_IDENTITY::CAPTAIN);
				identity_was_doubt.push_back(COUP::ROLE_IDENTITY::AMBASSADOR);
				has_identity = dst_player->checkIdentity(COUP::ROLE_IDENTITY::CAPTAIN) || dst_player->checkIdentity(COUP::ROLE_IDENTITY::AMBASSADOR);
			}
		}
		else if (pre_action && pre_action->action == COUP::ROLE_ACTION::DUCK)
		{
			identity_was_doubt.push_back(COUP::ROLE_IDENTITY::DUKE);
			has_identity = dst_player->checkIdentity(COUP::ROLE_IDENTITY::DUKE);
		}
		else if (pre_action && pre_action->action == COUP::ROLE_ACTION::AMBASSADOR)
		{
			identity_was_doubt.push_back(COUP::ROLE_IDENTITY::AMBASSADOR);
			has_identity = dst_player->checkIdentity(COUP::ROLE_IDENTITY::AMBASSADOR);
		}
		else if (pre_action && pre_action->action == COUP::ROLE_ACTION::ASSASSIN)
		{
			identity_was_doubt.push_back(COUP::ROLE_IDENTITY::ASSASSIN);
			has_identity = dst_player->checkIdentity(COUP::ROLE_IDENTITY::ASSASSIN);
		}
		else if (pre_action && pre_action->action == COUP::ROLE_ACTION::CAPTAIN)
		{
			identity_was_doubt.push_back(COUP::ROLE_IDENTITY::CAPTAIN);
			has_identity = dst_player->checkIdentity(COUP::ROLE_IDENTITY::CAPTAIN);
		}

		if (has_identity && identity_was_doubt.empty())
		{
			// src 丢失一张身份牌
			std::vector<COUP::ROLE_IDENTITY> src_cards = src_player->getIdentities();
			if (src_cards.size() == 1 || src_cards.size() == 0)
			{
				returnCards(src_cards);
				src_player->setIdentity();
			}
			else if (src_cards.size() == 2)
			{
				uint8_t num_to_fold = callback(id, action->dst_player_id, src_cards.size(), src_cards.data());
				returnCards(src_cards.at(num_to_fold));
				src_player->setIdentity(src_cards.at(1 - num_to_fold));
			}

			// dst 换一张
			std::vector<COUP::ROLE_IDENTITY> dst_cards = dst_player->getIdentities();
			COUP::ROLE_IDENTITY cards_to_change = drawCards(1).front();

			int need_to_change = -1;

			for (size_t i = 0; i < dst_cards.size(); i++)
			{
				if (std::find(identity_was_doubt.begin(), identity_was_doubt.end(), dst_cards[i]) != identity_was_doubt.end())
				{
					need_to_change = i;
					break;
				}
			}

			returnCards(dst_cards[need_to_change]);
			dst_cards[need_to_change] = cards_to_change;
			dst_player->setIdentity(dst_cards);

			// 继续处理
			handleAction(pre_action); // dst拥有对应身份牌，src质疑失败，处理上一步
		}
		else
		{
			// dst 丢失一张身份牌
			std::vector<COUP::ROLE_IDENTITY> cards = dst_player->getIdentities();
			if (cards.size() == 1 || cards.size() == 0)
			{
				returnCards(cards);
				dst_player->setIdentity();
			}
			else if (cards.size() == 2)
			{
				uint8_t num_to_fold = callback(id, action->dst_player_id, cards.size(), cards.data());
				returnCards(cards.at(num_to_fold));
				dst_player->setIdentity(cards.at(1 - num_to_fold));
			}
			// 继续处理
			handleAction(pre_pre_action); // dst没有对应身份牌，src质疑成功，跳过上一步
		}
		break;

	default:
		break;
	}
}

bool COUP::Coup_Master::addAction(uint32_t src_player_id, uint32_t dst_player_id, COUP::ROLE_ACTION action, uint8_t coins, bool round_end)
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
		now_action = new Coup_Action(id, src_player_id, dst_player_id, action, coins, nullptr);
		break;

	case COUP::ROLE_ACTION::BLOCK:
	case COUP::ROLE_ACTION::DOUBT:
		now_action = new Coup_Action(id, src_player_id, dst_player_id, action, coins, action_queue.back());
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

void COUP::Coup_Master::registerCallback(callbackFunc f)
{
	Coup_Master::callback = f;
}

callbackFunc COUP::Coup_Master::callback = nullptr;