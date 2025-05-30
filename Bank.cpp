// yuvalgaddidi@gmail.com
#include "Bank.hpp"
#include <stdexcept>

/**
 * @brief Constructor: initializes the bank with 50 coins.
 */
Bank::Bank() : total(50) {}

/**
 * @brief Subtracts coins from the bank. Throws if not enough.
 * @param amount number of coins to take
 */
void Bank::take(int amount) {
    if (amount > total) {
        throw std::runtime_error("Not enough coins in the bank.");
    }
    total -= amount;
}

/**
 * @brief Adds coins to the bank.
 * @param amount number of coins to add
 */
void Bank::add(int amount) {
    total += amount;
}

/**
 * @brief Returns the current number of coins in the bank.
 * @return total coins
 */
int Bank::getCoins() const {
    return total;
}
