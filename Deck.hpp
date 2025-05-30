// yuvalgaddidi@gmail.com
#pragma once
#include "Role.hpp"
#include "Governor.hpp"
#include "Spy.hpp"
#include "Baron.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"
#include <cstdlib>
#include <ctime>

/**
 * @brief Deck class is responsible for assigning roles to players.
 * Each call to drawRole() returns a new instance of a random role.
 */
class Deck {
public:
    Deck() {
        std::srand(std::time(nullptr)); // Initialize random seed
    }

    /**
     * @brief Returns a pointer to a newly allocated random role.
     * Caller is responsible for deleting it.
     */
    Role* drawRole() {
        int r = std::rand() % 6;
        switch (r) {
            case 0: return new Governor();
            case 1: return new Spy();
            case 2: return new Baron();
            case 3: return new General();
            case 4: return new Judge();
            default: return new Merchant();
        }
    }
};
