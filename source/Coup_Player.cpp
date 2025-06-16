#include "Coup_Player.h"
#include <algorithm>
#include <ranges>

COUP::Coup_Player::Coup_Player(uint32_t id, const std::vector<ROLE_IDENTITY> &identities)
	: id(id), coins(2)
{
	std::ranges::transform(cards, identities, cards.begin(), [](const auto &card, ROLE_IDENTITY identity)
						   { return std::make_pair(identity, true); });
}

COUP::Coup_Player::~Coup_Player()
{
}

const uint8_t COUP::Coup_Player::getCoinsNum() const
{
	return coins;
}

const std::vector<COUP::ROLE_IDENTITY> COUP::Coup_Player::getIdentitiesAlive() const
{
	auto f = cards | std::views::filter([](const auto &card)
										{ return card.second == true; }) |
			 std::views::transform([](const auto &card)
								   { return card.first; }) |
			 std::ranges::to<std::vector>();
	return f;
}

const std::vector<COUP::ROLE_IDENTITY> COUP::Coup_Player::getIdentitiesDead() const
{
	auto f = cards | std::views::filter([](const auto &card)
										{ return card.second == false; }) |
			 std::views::transform([](const auto &card)
								   { return card.first; }) |
			 std::ranges::to<std::vector>();
	return f;
}

const bool COUP::Coup_Player::isPlayerAlive() const
{
	auto f = cards | std::views::filter([](const auto &card)
										{ return card.second == true; });
	return !f.empty();
}

const bool COUP::Coup_Player::isPlayerDead() const
{
	auto f = cards | std::views::filter([](const auto &card)
										{ return card.second == true; });
	return f.empty();
}

void COUP::Coup_Player::setCoinsNum(const uint8_t coins_num)
{
	coins = coins_num;
}

void COUP::Coup_Player::setIdentity(const std::vector<ROLE_IDENTITY> &the_identities)
{
	auto f = cards | std::ranges::views::filter([](const auto &card)
												{ return card.second == true; });
	std::ranges::transform(f, the_identities, f.begin(), [](const auto &card, ROLE_IDENTITY identity)
						   { return std::make_pair(identity, true); });
}

void COUP::Coup_Player::setIdentity(const ROLE_IDENTITY the_identity)
{
	auto iter = std::ranges::find_if(cards, [](const auto &card)
									 { return card.second == true; });
	if (iter != cards.end())
	{
		iter->first = the_identity;
	}
}

void COUP::Coup_Player::setIdentityAlive(ROLE_IDENTITY the_identity, bool alive)
{
	auto iter = std::ranges::find_if(cards, [the_identity](const auto &card)
									 { return card.first == the_identity; });
	if (iter != cards.end())
	{
		iter->second = alive;
	}
}

bool COUP::Coup_Player::checkIdentity(const ROLE_IDENTITY the_identity)
{
	auto f = cards | std::views::filter([the_identity](const auto &card)
										{ return card.second == true && card.first == the_identity; });
	return !f.empty();
}