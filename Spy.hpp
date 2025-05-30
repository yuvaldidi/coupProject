// yuvalgaddidi@gmail.com
#pragma once
#include "Role.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include <string>
#include <iostream>

/**
 * @brief Spy role:
 * - Can see another player's coin count.
 * - Can prevent them from using arrest on their next turn.
 * - This ability does not consume a turn or coins.
 */
class Spy : public Role
{
public:
    /**
     * @brief Returns the role name.
     */
    std::string name() const override
    {
        return "Spy";
    }

    /**
     * @brief Reveals the number of coins the target player has.
     */
    void revealCoins(Player &target)
    {
        std::cout << "[Spy] " << target.getName() << " has " << target.getCoins() << " coins." << std::endl;
    }

    /**
     * @brief Blocks the target from using arrest on their next turn.
     * Simulates arrest by setting lastArrested to self.
     */
    void blockArrest(Player &target)
    {
        target.setLastArrested(&target);  // ✅ prevents arrest
    }

    /**
     * @brief Clone this role.
     */
    Role *clone() const override
    {
        return new Spy(*this);
    }
};
