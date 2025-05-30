// yuvalgaddidi@gmail.com
#pragma once

/**
 * @brief Represents the game's coin bank.
 * Handles adding and removing coins during the game.
 */
class Bank {
private:
    int total;

public:
    Bank();

    // Subtracts coins from the bank. Throws if not enough.
    void take(int amount);

    // Adds coins to the bank.
    void add(int amount);

    // Returns the current amount of coins in the bank.
    int getCoins() const;
};
