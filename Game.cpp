// yuvalgaddidi@gmail.com

#include "Game.hpp"
#include <stdexcept>
#include <iostream>

void Game::addPlayer(Player& p) {
    // Check for duplicate name
    for (Player* existing : players) {
        if (existing->getName() == p.getName()) {
            throw std::runtime_error("Duplicate player name: " + p.getName());
        }
    }
    players.push_back(&p);
}

void Game::validateTurn(Player& p) const {
    if (p.getCoins() >= 10) {
        throw std::runtime_error(p.getName() + " must perform coup with 10 or more coins");
    }
}

void Game::addPlayer(const std::string& name, Role* role) {
    Player* p = new Player(*this,name, role);
    addPlayer(*p);
}

void Game::eliminate(Player& p) {
    p.setAlive(false);
}

std::string Game::turn() const {
    if (players.empty()) {
        throw std::runtime_error("No players in game");
    }
    return players.at(currentPlayerIndex)->getName();
}

std::vector<std::string> Game::playersList() const {
    std::vector<std::string> active;
    for (Player* p : players) {
        if (p->isAlive()) {
            active.push_back(p->getName());
        }
    }
    return active;
}

std::vector<Player*> Game::getPlayers() const {
    return players;
}

std::string Game::winner() const {
    std::string name;
    int count = 0;
    for (Player* p : players) {
        if (p->isAlive()) {
            count++;
            name = p->getName();
        }
    }
    if (count != 1) {
        throw std::runtime_error("Game is not over");
    }
    return name;
}

void Game::nextTurn() {
    do {
        currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
    } while (!players[currentPlayerIndex]->isAlive());
}

int Game::bankCoins() const {
    return bank.getCoins();
}

void Game::withdrawFromBank(int amount) {
    bank.take(amount);
}

void Game::depositToBank(int amount) {
    bank.add(amount);
}

Player& Game::currentPlayer() {
    return *players.at(currentPlayerIndex);
}

Bank& Game::getBank() {
    return bank;
}

const Bank& Game::getBank() const {
    return bank;
}

bool Game::isTaxBlocked(Player& actor) {
    for (Player* p : players) {
        if (p->isAlive() && p != &actor && p->getRole()->canBlockTax(actor, *this)) {
            std::cout << p->getName() << " blocked " << actor.getName() << "'s tax.\n";
            return true;
        }
    }
    return false;
}
bool Game::isOver() const {
    int aliveCount = 0;
    for (const Player* p : players) {
        if (p->isAlive()) ++aliveCount;
    }
    return aliveCount <= 1;
}
