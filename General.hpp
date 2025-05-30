// yuvalgaddidi@gmail.com
#pragma once
#include "Role.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include <string>

/**
 * @brief General role:
 * - Can block a coup against himself or others by paying 5 coins.
 * - If arrested, gets back the coin that was taken.
 */
class General : public Role
{
public:
    /**
     * @brief Returns the role name.
     */
    std::string name() const override
    {
        return "General";
    }

    /**
     * @brief Clone this role.
     */
    Role *clone() const override
    {
        return new General(*this);
    }

    /**
     * @brief When arrested, General regains the stolen coin.
     */
    void onArrest(Player &p, Player &by, Game &) override
    {
        p.increaseCoins(1);
        by.decreaseCoins(1);
    }

    /**
     * @brief Indicates that the General can block a coup.
     */
    bool canBlockCoup(Player &, Player &, Game &) const override
    {
        return true;
    }
};
