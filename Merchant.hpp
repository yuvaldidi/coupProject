// yuvalgaddidi@gmail.com
#pragma once
#include "Role.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include <string>
#include <stdexcept>

/**
 * @brief Merchant role:
 * - At the start of their turn, if they have at least 3 coins, they get 1 bonus coin.
 * - When arrested, loses 2 coins to the bank instead of giving 1 to the attacker.
 */
class Merchant : public Role
{
public:
    /**
     * @brief Returns the role name.
     */
    std::string name() const override
    {
        return "Merchant";
    }

    Role *clone() const override
    {
        return new Merchant(*this);
    }

    void onArrest(Player &p, Player &, Game &g) override
    {
        int toPay = std::min(2, p.getCoins());
        if (toPay == 0)
        {
            throw std::runtime_error("Merchant has no coins to pay the penalty.");
        }
        p.decreaseCoins(toPay);
        g.getBank().add(toPay);
    }

    /**
     * @brief Should be called at the start of the merchant's turn.
     * If the merchant has 3 or more coins, they receive 1 bonus coin.
     */
    void grantBonusIfEligible(Player &p, Game &g)
    {
        if (p.getCoins() >= 3)
        {
            g.getBank().take(1);
            p.increaseCoins(1);
        }
    }
};
