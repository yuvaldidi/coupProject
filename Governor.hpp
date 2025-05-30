// yuvalgaddidi@gmail.com
#pragma once
#include "Role.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include <string>

/**
 * @brief Governor role:
 * - Takes 3 coins instead of 2 when using tax.
 * - Can block other players from using tax.
 */
class Governor : public Role
{
public:
    /**
     * @brief Returns the role name.
     */
    std::string name() const override
    {
        return "Governor";
    }

    Role *clone() const override
    {
        return new Governor(*this);
    }

    /**
     * @brief Governor takes 3 coins from bank when taxing.
     */
    int onTax(Player &, Game &) override
    {
        return 3;
    }

    /**
     * @brief Governor can block tax from other players.
     */
    bool blockTax(Player &, Game &) override
    {
        return true;
    }

    /**
     * @brief Determine if this role can block tax.
     * @return True if blocking is possible.
     */
    bool canBlockTax(Player& actor, Game&) const override {
        return actor.isAlive(); // basic condition for allowing block
    }
};
