#pragma once

#include "Coup_Action.h"
#include "Coup_Enums.h"
#include <deque>
#include <stack>
#include <unordered_map>
#include <vector>

typedef uint8_t (*callbackFunc)(uint32_t room_id, uint32_t player_id, int identities_num, COUP::ROLE_IDENTITY *identities);

namespace COUP
{
	class Coup_Player;
	class Coup_Master
	{
	private:
		uint32_t id;
		int num_players;
		GAME_STATUS game_status;
		std::unordered_map<uint32_t, Coup_Player *> all_players;
		std::deque<ROLE_IDENTITY> all_identities;
		std::deque<Coup_Action *> action_queue;
		static callbackFunc callback;

	public:
		Coup_Master(uint32_t id, int num);
		~Coup_Master();

		void shuffleCards();
		std::vector<ROLE_IDENTITY> drawCards(int num);
		void returnCards(const ROLE_IDENTITY identity);
		void returnCards(const std::vector<ROLE_IDENTITY> &identities);

		bool addPlayer(uint32_t player_id);
		Coup_Player *getPlayer(uint32_t player_id) const;

		void handleAction(const Coup_Action *action);

		bool addAction(uint32_t src_player_id, uint32_t dst_player_id, COUP::ROLE_ACTION action, uint8_t coins, bool round_end);
		const Coup_Action *getLastAction() const;

		static void registerCallback(callbackFunc f);
	};
} // namespace COUP
