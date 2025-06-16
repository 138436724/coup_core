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
	  all_actions()
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
	for (auto &action : all_actions)
	{
		delete action;
	}
	all_actions.clear();
	all_identities.clear();
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
	// shuffleCards();
}

void COUP::Coup_Master::returnCards(const std::vector<ROLE_IDENTITY> &identities)
{
	for (auto &identity : identities)
	{
		all_identities.push_back(identity);
	}
}

void COUP::Coup_Master::handleINCOME(const Coup_Action *action)
{
	COUP::Coup_Player *src_player = getPlayer(action->src_player_id);
	src_player->setCoinsNum(src_player->getCoinsNum() + 1);
}

void COUP::Coup_Master::handleCOUP(const Coup_Action *action)
{
	COUP::Coup_Player *src_player = getPlayer(action->src_player_id);
	src_player->setCoinsNum(src_player->getCoinsNum() - 7);

	COUP::Coup_Player *dst_player = getPlayer(action->dst_player_id);
	std::vector<COUP::ROLE_IDENTITY> cards = dst_player->getIdentitiesAlive();

	uint8_t num_to_fold = callback(id, action->dst_player_id, cards.size(), cards.data());
	dst_player->setIdentityAlive(cards.at(num_to_fold), false);
}

void COUP::Coup_Master::handleTAX(const Coup_Action *action)
{
	COUP::Coup_Player *src_player = getPlayer(action->src_player_id);
	src_player->setCoinsNum(src_player->getCoinsNum() + 3);
}

void COUP::Coup_Master::handleEXCHANGE(const Coup_Action *action)
{
	COUP::Coup_Player *src_player = getPlayer(action->src_player_id);

	std::vector<COUP::ROLE_IDENTITY> cards_all = drawCards(2);
	std::vector<COUP::ROLE_IDENTITY> cards_had = src_player->getIdentitiesAlive();

	cards_all.insert(cards_all.end(), cards_had.begin(), cards_had.end());

	uint8_t num_to_fold1 = callback(id, action->src_player_id, cards_all.size(), cards_all.data());
	returnCards(cards_all[num_to_fold1]);
	cards_all.erase(cards_all.begin() + num_to_fold1);

	uint8_t num_to_fold2 = callback(id, action->src_player_id, cards_all.size(), cards_all.data());
	returnCards(cards_all[num_to_fold2]);
	cards_all.erase(cards_all.begin() + num_to_fold2);

	src_player->setIdentity(cards_all);
}

void COUP::Coup_Master::handleFOREIGN_AID(const Coup_Action *action)
{
	COUP::Coup_Player *src_player = getPlayer(action->src_player_id);
	src_player->setCoinsNum(src_player->getCoinsNum() + 2);
}

void COUP::Coup_Master::handleASSASSINATE(const Coup_Action *action)
{
	COUP::Coup_Player *src_player = getPlayer(action->src_player_id);
	src_player->setCoinsNum(src_player->getCoinsNum() - 3);

	COUP::Coup_Player *dst_player = getPlayer(action->dst_player_id);
	std::vector<COUP::ROLE_IDENTITY> cards = dst_player->getIdentitiesAlive();

	if (cards.size() == 1)
	{
		dst_player->setIdentityAlive(cards.at(0), false);
	}
	else
	{
		uint8_t num_to_fold = callback(id, action->dst_player_id, cards.size(), cards.data());
		dst_player->setIdentityAlive(cards.at(num_to_fold), false);
	}
}

void COUP::Coup_Master::handleSTEAL(const Coup_Action *action)
{
	COUP::Coup_Player *src_player = getPlayer(action->src_player_id);
	COUP::Coup_Player *dst_player = getPlayer(action->dst_player_id);
	if (dst_player->getCoinsNum() >= 2)
	{
		src_player->setCoinsNum(src_player->getCoinsNum() + 2);
		dst_player->setCoinsNum(dst_player->getCoinsNum() - 2);
	}
	else
	{
		src_player->setCoinsNum(src_player->getCoinsNum() + dst_player->getCoinsNum());
		dst_player->setCoinsNum(0);
	}
}

void COUP::Coup_Master::handleBLOCK(const Coup_Action *action)
{
	// 获取被阻止的操作
	const COUP::Coup_Action *pre_action = action->previous_action;
	if (pre_action && pre_action->action == COUP::ROLE_ACTION::ASSASSINATE)
	{
		COUP::Coup_Player *pre_src_player = getPlayer(pre_action->src_player_id);
		pre_src_player->setCoinsNum(pre_src_player->getCoinsNum() - 3);
	}
}

void COUP::Coup_Master::handleDOUBT(const Coup_Action *action)
{
	const COUP::Coup_Action *pre_action = action->previous_action;
	if (!pre_action)
	{
		return;
	}

	COUP::Coup_Player *src_player = getPlayer(action->src_player_id);
	COUP::Coup_Player *dst_player = getPlayer(action->dst_player_id);

	bool has_identity = false;
	std::vector<COUP::ROLE_IDENTITY> identity_was_doubt = {};

	// 推断身份牌
	if (pre_action->action == COUP::ROLE_ACTION::TAX)
	{
		identity_was_doubt.push_back(COUP::ROLE_IDENTITY::DUKE);
		has_identity = dst_player->checkIdentity(COUP::ROLE_IDENTITY::DUKE);
	}
	else if (pre_action->action == COUP::ROLE_ACTION::EXCHANGE)
	{
		identity_was_doubt.push_back(COUP::ROLE_IDENTITY::AMBASSADOR);
		has_identity = dst_player->checkIdentity(COUP::ROLE_IDENTITY::AMBASSADOR);
	}
	else if (pre_action->action == COUP::ROLE_ACTION::ASSASSINATE)
	{
		identity_was_doubt.push_back(COUP::ROLE_IDENTITY::ASSASSIN);
		has_identity = dst_player->checkIdentity(COUP::ROLE_IDENTITY::ASSASSIN);
	}
	else if (pre_action->action == COUP::ROLE_ACTION::STEAL)
	{
		identity_was_doubt.push_back(COUP::ROLE_IDENTITY::CAPTAIN);
		has_identity = dst_player->checkIdentity(COUP::ROLE_IDENTITY::CAPTAIN);
	}
	else if (pre_action->action == COUP::ROLE_ACTION::BLOCK)
	{
		// 继续往前推断
		const COUP::Coup_Action *pre_pre_action = pre_action->previous_action;
		if (!pre_pre_action)
		{
			return;
		}

		if (pre_pre_action->action == COUP::ROLE_ACTION::FOREIGN_AID)
		{
			identity_was_doubt.push_back(COUP::ROLE_IDENTITY::DUKE);
			has_identity = dst_player->checkIdentity(COUP::ROLE_IDENTITY::DUKE);
		}
		else if (pre_pre_action->action == COUP::ROLE_ACTION::ASSASSINATE)
		{
			identity_was_doubt.push_back(COUP::ROLE_IDENTITY::CONTESSA);
			has_identity = dst_player->checkIdentity(COUP::ROLE_IDENTITY::CONTESSA);
		}
		else if (pre_pre_action->action == COUP::ROLE_ACTION::STEAL)
		{
			identity_was_doubt.push_back(COUP::ROLE_IDENTITY::CAPTAIN);
			identity_was_doubt.push_back(COUP::ROLE_IDENTITY::AMBASSADOR);
			has_identity = dst_player->checkIdentity(COUP::ROLE_IDENTITY::CAPTAIN) || dst_player->checkIdentity(COUP::ROLE_IDENTITY::AMBASSADOR);
		}
	}

	if (has_identity && !identity_was_doubt.empty()) // 质疑失败
	{
		// src 翻开一张身份牌
		std::vector<COUP::ROLE_IDENTITY> src_cards = src_player->getIdentitiesAlive();
		if (src_cards.size() == 1)
		{
			src_player->setIdentityAlive(src_cards.at(0), false);
		}
		else
		{
			uint8_t num_to_fold = callback(id, action->src_player_id, src_cards.size(), src_cards.data());
			src_player->setIdentityAlive(src_cards.at(num_to_fold), false);
		}

		// dst 换一张
		std::vector<COUP::ROLE_IDENTITY> dst_cards = dst_player->getIdentitiesAlive();
		COUP::ROLE_IDENTITY card_to_change = drawCards(1).front();

		for (auto &identity : identity_was_doubt)
		{
			auto card = std::find(dst_cards.begin(), dst_cards.end(), identity);
			if (card != dst_cards.end())
			{
				returnCards(*card);
				dst_cards.erase(card);
				dst_cards.push_back(card_to_change);
				dst_player->setIdentity(dst_cards);
			}
		}

		// dst拥有对应身份牌，src质疑失败，处理上一步
		handleAction(pre_action);
	}
	else // 质疑成功
	{
		// dst 翻开一张身份牌 todo 不放回牌堆
		std::vector<COUP::ROLE_IDENTITY> cards = dst_player->getIdentitiesAlive();
		if (cards.size() == 1)
		{
			dst_player->setIdentityAlive(cards.at(0), false);
		}
		else
		{
			uint8_t num_to_fold = callback(id, action->dst_player_id, cards.size(), cards.data());
			dst_player->setIdentityAlive(cards.at(num_to_fold), false);
		}
		// dst没有对应身份牌，src质疑成功，跳过上一步
		handleAction(pre_action->previous_action);
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

	switch (action->action)
	{
	case COUP::ROLE_ACTION::INCOME:
		handleINCOME(action);
		break;
	case COUP::ROLE_ACTION::COUP:
		handleCOUP(action);
		break;
	case COUP::ROLE_ACTION::TAX:
		handleTAX(action);
		break;
	case COUP::ROLE_ACTION::EXCHANGE:
		handleEXCHANGE(action);
		break;
	case COUP::ROLE_ACTION::FOREIGN_AID:
		handleFOREIGN_AID(action);
		break;
	case COUP::ROLE_ACTION::ASSASSINATE:
		handleASSASSINATE(action);
		break;
	case COUP::ROLE_ACTION::STEAL:
		handleSTEAL(action);
		break;
	case COUP::ROLE_ACTION::BLOCK:
		handleBLOCK(action);
		break;
	case COUP::ROLE_ACTION::DOUBT:
		handleDOUBT(action);
		break;
	default:
		break;
	}
}

bool COUP::Coup_Master::addAction(uint32_t src_player_id, uint32_t dst_player_id, COUP::ROLE_ACTION action, bool round_end)
{
	int coins_num = getPlayer(src_player_id)->getCoinsNum();
	if ((action == COUP::ROLE_ACTION::COUP && coins_num < 7) ||
		(action == COUP::ROLE_ACTION::ASSASSINATE && coins_num < 3) ||
		(action != COUP::ROLE_ACTION::COUP && coins_num > 10) ||
		getPlayer(src_player_id)->isPlayerDead())
	{
		return false;
	}

	Coup_Action *now_action = nullptr;

	switch (action)
	{
	case COUP::ROLE_ACTION::INCOME:
	case COUP::ROLE_ACTION::FOREIGN_AID:
	case COUP::ROLE_ACTION::TAX:
	case COUP::ROLE_ACTION::EXCHANGE:
		dst_player_id = src_player_id;
	case COUP::ROLE_ACTION::COUP:
	case COUP::ROLE_ACTION::ASSASSINATE:
	case COUP::ROLE_ACTION::STEAL:
		now_action = new Coup_Action(id, src_player_id, dst_player_id, action, nullptr);
		break;

	case COUP::ROLE_ACTION::BLOCK:
	case COUP::ROLE_ACTION::DOUBT:
		now_action = new Coup_Action(id, src_player_id, dst_player_id, action, all_actions.back());
		break;
	default:
		break;
	}

	all_actions.push_back(now_action);

	if (round_end)
	{
		handleAction(now_action);
	}

	return true;
}

const COUP::Coup_Action *COUP::Coup_Master::getLastAction() const
{
	return all_actions.back();
}

void COUP::Coup_Master::registerCallback(callbackFunc f)
{
	Coup_Master::callback = f;
}

callbackFunc COUP::Coup_Master::callback = nullptr;