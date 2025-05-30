// yuvalgaddidi@gmail.com
#pragma once
#include "Role.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include <string>
#include <stdexcept>

/**
 * @brief Baron role:
 * - Can invest 3 coins to receive 6 coins.
 * - If sanctioned, gains 1 coin as compensation.
 */
class Baron : public Role
{
public:
    /**
     * @brief Returns the role name.
     */
    std::string name() const override
    {
        return "Baron";
    }

    /**
     * @brief Called when the baron is sanctioned – gains 1 coin as compensation.
     */
    void onSanction(Player &p, Player &, Game &) override
    {
        p.increaseCoins(1);
    }

    Role *clone() const override
    {
        return new Baron(*this);
    }

    /**
     * @brief Baron can invest: spend 3 coins to receive 6 coins.
     * Must be called explicitly in the game logic.
     */
    void invest(Player &p, Game &g)
    {
        if (p.getCoins() < 3)
        {
            throw std::runtime_error("Not enough coins to invest.");
        }
        p.decreaseCoins(3);
        g.getBank().add(3);
        g.getBank().take(6);
        p.increaseCoins(6);
        g.nextTurn();
    }
};
