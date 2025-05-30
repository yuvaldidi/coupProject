// yuvalgaddidi@gmail.com
#pragma once
#include <string>

class Player;
class Game;

/**
 * @brief Abstract base class for all roles in the game.
 * Each role can override specific behaviors and block certain actions.
 */
class Role
{
public:
    virtual ~Role() = default;

    /**
     * @brief Creates a deep copy of the role.
     * @return Pointer to a new copy of the derived role.
     */
    virtual Role *clone() const = 0;

    // Returns the name of the role (e.g., "Governor", "Spy", etc.)
    virtual std::string name() const = 0;

    // Called when the player attempts to take tax.
    virtual int onTax(Player &, Game &) { return 2; }

    // Called when the player is sanctioned.
    virtual void onSanction(Player &, Player &, Game &) {}

    // Called when the player is arrested.
    virtual void onArrest(Player &, Player &, Game &) {}

    // Called when another player tries to tax and this role can block.
    virtual bool blockTax(Player &, Game &) { return false; }

    // Called when another player uses bribe and this role can block.
    virtual bool blockBribe(Player &, Game &) { return false; }

    // Called when another player tries to coup and this role can block.
    virtual bool blockCoup(Player &, Player &, Game &) { return false; }

    virtual bool canBlockTax(Player &, Game &)const { return false; }
    virtual bool canBlockBribe(Player &, Game &)const { return false; }
    virtual bool canBlockCoup(Player &, Player &, Game &) const { return false; }
};
