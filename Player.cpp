// yuvalgaddidi@gmail.com

#include "Player.hpp"
#include "Spy.hpp"
#include "Baron.hpp"
#include "Governor.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"
#include "Game.hpp"
#include <stdexcept>

Player::Player(Game &game, const std::string &name, Role *role)
    : game(game), name(name), role(role), coins(1), alive(true), lastArrested(nullptr), underSanction(false)
{
    game.addPlayer(*this);
}

Player::Player(const Player &other)
    : game(other.game),
      name(other.name),
      role(other.role ? other.role->clone() : nullptr),
      coins(other.coins),
      alive(other.alive),
      lastArrested(other.lastArrested)
{
}

Player &Player::operator=(const Player &other)
{
    if (this != &other)
    {
        name = other.name;
        coins = other.coins;
        alive = other.alive;
        lastArrested = other.lastArrested;
        delete role;
        role = other.role ? other.role->clone() : nullptr;
    }
    return *this;
}

Player::~Player()
{
    delete role;
}

std::string Player::getName() const
{
    return name;
}

Role *Player::getRole() const
{
    return role;
}

int Player::getCoins() const
{
    return coins;
}

bool Player::isAlive() const
{
    return alive;
}

Player *Player::getLastArrested() const
{
    return lastArrested;
}

void Player::setAlive(bool status)
{
    alive = status;
}

void Player::setLastArrested(Player *target)
{
    lastArrested = target;
}

void Player::increaseCoins(int amount)
{
    coins += amount;
}

void Player::decreaseCoins(int amount)
{
    if (coins < amount)
    {
        throw std::runtime_error("Not enough coins");
    }
    coins -= amount;
}

void Player::receiveCoins(int amount)
{
    coins += amount;
}
bool Player::isUnderSanction() const {
    return underSanction;
}

void Player::setUnderSanction(bool status) {
    underSanction = status;
}

// ====== Game Actions ======

void Player::gather()
{
    if (game.turn() != name)
        throw std::runtime_error("Not your turn");

    if (isUnderSanction())
        throw std::runtime_error("Player is under sanction");
    Merchant *merchant = dynamic_cast<Merchant *>(role);
    if (merchant)
        merchant->grantBonusIfEligible(*this, game);

    game.validateTurn(*this);
    game.withdrawFromBank(1);
    increaseCoins(1);
    game.nextTurn();
}


void Player::tax() {
    if (game.turn() != name)
        throw std::runtime_error("Not your turn");

    if (underSanction)
        throw std::runtime_error("You are sanctioned and cannot tax");

    Merchant* merchant = dynamic_cast<Merchant*>(role);
    if (merchant)
        merchant->grantBonusIfEligible(*this, game);

    game.validateTurn(*this);

    for (Player* p : game.getPlayers()) {
        if (p != this && p->isAlive()) {
            Role* r = p->getRole();
            if (r && r->canBlockTax(*this, game)) {
                game.nextTurn();  // הפעולה נחסמה, לא קורה כלום
                return;
            }
        }
    }

    int amount = 2;
    if (dynamic_cast<Governor*>(role)) {
        amount = 3;
    }

    game.withdrawFromBank(amount);
    increaseCoins(amount);
    game.nextTurn();
}

void Player::bribe()
{
    if (game.turn() != name)
        throw std::runtime_error("Not your turn");

    Merchant *merchant = dynamic_cast<Merchant *>(role);
    if (merchant)
        merchant->grantBonusIfEligible(*this, game);

    game.validateTurn(*this);

    if (coins < 4)
        throw std::runtime_error("Not enough coins for bribe");

    decreaseCoins(4);
    game.depositToBank(4);  // Always pay the coins, even if blocked

    // Real-time block: if any Judge or General blocks – end the turn
    for (Player *p : game.getPlayers())
    {
        if (p != this && p->isAlive())
        {
            Role *r = p->getRole();
            if (r && r->blockBribe(*this, game))
            {
                game.nextTurn(); // Action was blocked – end turn
                return;
            }
        }
    }

    // Bribe successful – player allowed to perform another action
    // No need to call game.nextTurn() yet
}


void Player::arrest(Player &target)
{
    if (game.turn() != name)
        throw std::runtime_error("Not your turn");

    Merchant *merchant = dynamic_cast<Merchant *>(role);
    if (merchant)
        merchant->grantBonusIfEligible(*this, game);

    game.validateTurn(*this);

    if (&target == lastArrested)
        throw std::runtime_error("Cannot arrest the same player twice in a row");

    if (!target.isAlive())
        throw std::runtime_error("Cannot arrest a dead player");

    Role* targetRole = target.getRole();
    if (dynamic_cast<Merchant *>(targetRole)) {
        // Merchant - pays to bank (even 1 coin if needed)
        targetRole->onArrest(target, *this, game);
    } else {
        if (target.getCoins() < 1)
            throw std::runtime_error("Target has no coins");

        target.decreaseCoins(1);
        increaseCoins(1);
    }

    setLastArrested(&target);
    game.nextTurn();
}


void Player::sanction(Player &target)
{
    if (game.turn() != name)
        throw std::runtime_error("Not your turn");

    Merchant *merchant = dynamic_cast<Merchant *>(role);
    if (merchant)
        merchant->grantBonusIfEligible(*this, game);

    game.validateTurn(*this);

    if (coins < 3)
        throw std::runtime_error("Not enough coins to sanction");
    if (!target.isAlive())
        throw std::runtime_error("Cannot sanction a dead player");

    decreaseCoins(3);
    game.depositToBank(3);

    target.setUnderSanction(true);

    // הפעלת תגובה מיוחדת של התפקיד (אם קיימת)
    Role* targetRole = target.getRole();
    if (targetRole)
    {
        targetRole->onSanction(target, *this, game);
    }

    game.nextTurn();
}



void Player::coup(Player &target)
{
    if (game.turn() != name)
        throw std::runtime_error("Not your turn");

    if (!target.isAlive())
        throw std::runtime_error("Target already eliminated");

    if (coins < 7)
        throw std::runtime_error("Not enough coins to perform coup");

    Merchant *merchant = dynamic_cast<Merchant *>(role);
    if (merchant)
        merchant->grantBonusIfEligible(*this, game);

    game.validateTurn(*this);

    // שלב 1: תשלום והפקדה
    decreaseCoins(7);
    game.depositToBank(7);

    // שלב 2: חסימה בזמן אמת
    for (Player *p : game.getPlayers())
    {
        if (p != this && p->isAlive())
        {
            Role *r = p->getRole();
            if (r && r->canBlockCoup(*this, target, game))
            {
                if (p->getCoins() < 5)
                    continue; // אין לו מטבעות לחסום בפועל

                p->decreaseCoins(5);
                game.depositToBank(5);
                increaseCoins(7); // מחזירים את הכסף ששילמתי
                game.nextTurn();
                return;
            }
        }
    }

    // שלב 3: לא נחסם → הדחה
    game.eliminate(target);
    game.nextTurn();
}


// ====== Role Abilities ======

void Player::revealCoins(Player &target)
{
    Spy *spy = dynamic_cast<Spy *>(role);
    if (!spy)
    {
        throw std::runtime_error("Player is not a Spy");
    }
    spy->revealCoins(target);
}

void Player::blockArrest(Player &target)
{
    Spy *spy = dynamic_cast<Spy *>(role);
    if (!spy)
    {
        throw std::runtime_error("Player is not a Spy");
    }
    spy->blockArrest(target);
}

void Player::invest(Game &game)
{
    Baron *baronPtr = dynamic_cast<Baron *>(role);
    if (!baronPtr)
    {
        throw std::runtime_error("Player is not a Baron");
    }
    baronPtr->invest(*this, game);
}

void Player::blockTax(Player &p, Game &game)
{
    Governor *govPtr = dynamic_cast<Governor *>(role);
    if (!govPtr)
    {
        throw std::runtime_error("Player is not a Governor");
    }
    govPtr->blockTax(p, game);
}

void Player::blockBribe(Player &p, Game &game)
{
    if (Judge *judgePtr = dynamic_cast<Judge *>(role))
    {
        judgePtr->blockBribe(p, game);
    }
    else if (General *generalPtr = dynamic_cast<General *>(role))
    {
        generalPtr->blockBribe(p, game);
    }
    else
    {
        throw std::runtime_error("Player cannot block bribe");
    }
}

void Player::blockCoup(Player &attacker, Player &target, Game &game)
{
    if (!isAlive())
    {
        throw std::runtime_error("Dead player cannot block coup");
    }

    General *generalPtr = dynamic_cast<General *>(role);
    if (!generalPtr)
    {
        throw std::runtime_error("Only General can block coup");
    }

    if (coins < 5)
    {
        throw std::runtime_error("Not enough coins to block coup");
    }

    if (!target.isAlive())
    {
        throw std::runtime_error("Cannot block coup on a dead player");
    }

    // ביצוע החסימה
    decreaseCoins(5);
    game.depositToBank(5);
    attacker.increaseCoins(7); // מחזירים את המטבעות לתוקף

    std::cout << "[Block] " << name << " blocked coup on " << target.getName()
              << " by " << attacker.getName() << std::endl;
}
