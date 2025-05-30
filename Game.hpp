// yuvalgaddidi@gmail.com

#pragma once
#include <vector>
#include <string>
#include "Player.hpp"
#include "Role.hpp"
#include "Bank.hpp"

/**
 * @class Game
 * @brief Manages players, turns, and game state.
 */
class Game
{
private:
    std::vector<Player *> players;
    size_t currentPlayerIndex = 0;
    bool gameOver = false;
    Bank bank;

public:
    /**
     * @brief Adds a player to the game by reference.
     * @param p Reference to an existing Player object.
     */
    void addPlayer(Player &p);

    /**
     * @brief Validates that the player is allowed to act. Players with 10+ coins must coup.
     * @param p Player whose turn is being validated.
     * @throws std::runtime_error if player has 10 or more coins and did not coup.
     */
    void validateTurn(Player &p) const;

    /**
     * @brief Adds a player to the game by name and role (used by tests).
     * @param name Name of the player.
     * @param role Pointer to a dynamically allocated role.
     */
    void addPlayer(const std::string &name, Role *role);

    /**
     * @brief Eliminates a player from the game (used by tests).
     * @param p Reference to the player to eliminate.
     */
    void eliminate(Player &p);

    /**
     * @brief Returns the name of the player whose turn it is.
     * @return Name of current player.
     */
    std::string turn() const;

    /**
     * @brief Returns a vector of names of all active players.
     * @return Vector of player names.
     */
    std::vector<std::string> playersList() const;

    std::vector<Player *> getPlayers() const;

    /**
     * @brief Returns the name of the winner (if game is over).
     * @return Name of the winning player.
     * @throws std::runtime_error if game is not yet over.
     */
    std::string winner() const;

    /**
     * @brief Moves to the next player's turn.
     */
    void nextTurn();


    Player& getPlayer(const std::string& name);

    /**
     * @brief Gets the bank's current coin total.
     * @return Integer value of bank coin pool.
     */
    int bankCoins() const;

    /**
     * @brief Deducts coins from the bank.
     * @param amount Number of coins to deduct.
     */
    void withdrawFromBank(int amount);

    /**
     * @brief Adds coins to the bank.
     * @param amount Number of coins to add.
     */
    void depositToBank(int amount);

    /**
     * @brief Gets the player whose turn it is.
     * @return Reference to the current player.
     */
    Player &currentPlayer();
    Bank &getBank();
    const Bank &getBank() const;
    bool isTaxBlocked(Player &actor);
    bool isOver() const;

};
