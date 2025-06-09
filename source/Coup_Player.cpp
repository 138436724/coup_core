#include "Coup_Player.h"
#include <ranges>

COUP::Coup_Player::Coup_Player(uint32_t id, const std::vector<ROLE_IDENTITY> &identities)
	: id(id), coins(2)
{
	setIdentity(identities);
}

COUP::Coup_Player::~Coup_Player()
{
}

const uint8_t COUP::Coup_Player::getCoinsNum() const
{
	return coins;
}

const std::vector<COUP::ROLE_IDENTITY> COUP::Coup_Player::getIdentities() const
{
	auto f = cards | std::views::filter([](ROLE_IDENTITY identity)
										{ return identity != ROLE_IDENTITY::NO_IDENTITY; });
	std::vector<ROLE_IDENTITY> identities(f.begin(), f.end());
	return identities;
}

const bool COUP::Coup_Player::isPlayerDead() const
{
	return cards[0] == ROLE_IDENTITY::NO_IDENTITY && cards[1] == ROLE_IDENTITY::NO_IDENTITY;
}

void COUP::Coup_Player::setCoinsNum(const uint8_t coins_num)
{
	coins = coins_num;
}

void COUP::Coup_Player::setIdentity(const std::vector<ROLE_IDENTITY> &the_identities)
{
	cards[0] = the_identities[0];
	cards[1] = the_identities[1];
}

void COUP::Coup_Player::setIdentity(const ROLE_IDENTITY the_identity)
{
	cards[0] = the_identity;
	cards[1] = ROLE_IDENTITY::NO_IDENTITY;
}

void COUP::Coup_Player::setIdentity()
{
	cards[0] = ROLE_IDENTITY::NO_IDENTITY;
	cards[1] = ROLE_IDENTITY::NO_IDENTITY;
}

bool COUP::Coup_Player::checkIdentity(const ROLE_IDENTITY the_identity)
{
	return cards[0] == the_identity || cards[1] == the_identity;
}