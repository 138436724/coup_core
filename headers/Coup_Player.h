#pragma once

#include "Coup_Enums.h"
#include <array>
#include <vector>

namespace COUP
{
    class Coup_Player
    {
    private:
        uint32_t id;
        // std::string name;
        uint8_t coins;
        std::array<ROLE_IDENTITY, 2> cards;
        // bool is_alive;

    public:
        Coup_Player(uint32_t id, const std::vector<ROLE_IDENTITY> &identities);
        ~Coup_Player();

        // get player info
        const uint8_t getCoinsNum() const;
        const std::vector<ROLE_IDENTITY> getIdentities() const;
        const bool isPlayerDead() const;

        // set player info
        void setCoinsNum(const uint8_t coins_num);
        void setIdentity(const std::vector<ROLE_IDENTITY> &the_identities);
        void setIdentity(const ROLE_IDENTITY the_identity);
        void setIdentity();

        // handle event
        bool checkIdentity(const ROLE_IDENTITY the_identity);
    };

} // namespace COUP
