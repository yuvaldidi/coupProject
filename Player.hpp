// yuvalgaddidi@gmail.com

#pragma once
#include <string>
#include "Role.hpp"

class Game;

/**
 * @class Player
 * @brief Represents a player in the game, including role, coin balance, and state.
 */
class Player
{
private:
    Game &game; // Reference to the game the player belongs to
    std::string name;
    Role *role;
    int coins;
    bool alive;
    Player *lastArrested;
    bool underSanction;

public:
    Player(Game &game, const std::string &name, Role *role);

    // Rule of Three
    Player(const Player &other);
    Player &operator=(const Player &other);
    ~Player();

    std::string getName() const;
    Role *getRole() const;
    int getCoins() const;
    bool isAlive() const;
    Player *getLastArrested() const;
    bool isUnderSanction() const;
    void setUnderSanction(bool status);

    void setAlive(bool status);
    void setLastArrested(Player *target);
    void increaseCoins(int amount);
    void decreaseCoins(int amount);
    void receiveCoins(int amount);

    // Game actions
    void gather();                 ///< Gain 1 coin (can be blocked by sanction)
    void tax();                    ///< Gain 2 coins, or 3 if Governor
    void bribe();    ///< Pay 4 coins to perform another action
    void arrest(Player &target);   ///< Take 1 coin from target
    void sanction(Player &target); ///< Block target from gather/tax, costs 3 coins
    void coup(Player &target);     ///< Eliminate a player, costs 7 coins

    // Role abilities
    void revealCoins(Player &target);
    void blockArrest(Player &target);
    void invest(Game &game);
    void blockTax(Player &p, Game &game);
    void blockBribe(Player &p, Game &game);
    void blockCoup(Player &attacker, Player &target, Game &game);
};
