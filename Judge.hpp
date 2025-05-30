// yuvalgaddidi@gmail.com
#pragma once
#include "Role.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include <string>
#include <stdexcept>

/**
 * @brief Judge role:
 * - Can block bribe, causing the briber to lose the 4 coins.
 * - If sanctioned, the attacker pays an extra coin to the bank.
 */
class Judge : public Role
{
public:
    /**
     * @brief Returns the role name.
     */
    std::string name() const override
    {
        return "Judge";
    }

    Role *clone() const override
    {
        return new Judge(*this);
    }

    /**
     * @brief Blocks bribe – the attacker loses the 4 coins regardless.
     */
    bool blockBribe(Player &, Game &) override
    {
        // The coins are already paid in bribe, so just return true to block the second action.
        return true;
    }

    /**
     * @brief If sanctioned, attacker pays one more coin to the bank.
     */
    void onSanction(Player &, Player &attacker, Game &g) override
    {
        if (attacker.getCoins() < 1)
        {
            throw std::runtime_error("Attacker doesn't have extra coin for Judge sanction penalty.");
        }
        attacker.decreaseCoins(1);
        g.getBank().add(1);
    }
};
