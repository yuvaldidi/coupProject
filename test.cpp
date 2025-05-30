// yuvalgaddidi@gmail.com

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "Game.hpp"
#include "Judge.hpp"
#include "Spy.hpp"
#include "General.hpp"
#include "Governor.hpp"
#include "Merchant.hpp"
#include "Baron.hpp"

using namespace std;

/**
 * @test Game and Player creation
 */
TEST_CASE("Game and Player Creation") {
    Game g;
    CHECK_NOTHROW(g.addPlayer("benaya", new Judge()));
    CHECK_NOTHROW(g.addPlayer("yuval", new Spy()));
    CHECK_NOTHROW(g.addPlayer("eitan", new General()));
    CHECK_NOTHROW(g.addPlayer("chagai", new Governor()));

    SUBCASE("Duplicate player name should throw") {
        CHECK_THROWS(g.addPlayer("benaya", new Judge()));
    }

    SUBCASE("Empty name should throw") {
        CHECK_THROWS(g.addPlayer("", new Spy()));
    }

    SUBCASE("Nullptr role should throw") {
        CHECK_THROWS(g.addPlayer("keren", nullptr));
    }

    SUBCASE("Add player after game started should throw") {
        CHECK(g.turn() == "benaya");
        CHECK_THROWS(g.addPlayer("barbara", new Merchant()));
    }
}
/**
 * @test Turn Management
 */
TEST_CASE("Turn Management") {
    Game g;
    g.addPlayer("benaya", new Judge());
    g.addPlayer("yuval", new Spy());
    g.addPlayer("eitan", new General());
    g.addPlayer("chagai", new Governor());

    CHECK(g.turn() == "benaya");

    SUBCASE("Turn progresses after gather") {
        Player& a = g.getPlayer("benaya");
        Player& b = g.getPlayer("yuval");

        CHECK_NOTHROW(a.gather());
        CHECK(g.turn() == "yuval");

        CHECK_NOTHROW(b.gather());
        CHECK(g.turn() == "eitan");
    }

    SUBCASE("Turn skips eliminated players") {
        Player& a = g.getPlayer("benaya");
        Player& b = g.getPlayer("yuval");
        Player& c = g.getPlayer("eitan");
        Player& d = g.getPlayer("chagai");

        g.eliminate(b);
        g.eliminate(c);

        CHECK_NOTHROW(a.gather());
        CHECK(g.turn() == "chagai");

        CHECK_NOTHROW(d.gather());
        CHECK(g.turn() == "benaya");
    }

    SUBCASE("Only current player can act") {
        Player& a = g.getPlayer("benaya");
        Player& b = g.getPlayer("yuval");

        CHECK_THROWS(b.gather());
        CHECK_NOTHROW(a.gather());
        CHECK_NOTHROW(b.gather());
    }
}
/**
 * @test Basic Actions
 */
TEST_CASE("Basic Actions") {
    Game g;
    g.addPlayer("benaya", new Judge());
    g.addPlayer("yuval", new Spy());
    g.addPlayer("eitan", new General());
    g.addPlayer("chagai", new Governor());

    Player& benaya = g.getPlayer("benaya");
    Player& yuval = g.getPlayer("yuval");

    SUBCASE("Gather adds 1 coin") {
        CHECK(benaya.getCoins() == 1);
        CHECK_NOTHROW(benaya.gather());
        CHECK(benaya.getCoins() == 2);
    }

    SUBCASE("Tax adds 2 coins") {
        CHECK(benaya.getCoins() == 1);
        CHECK_NOTHROW(benaya.tax());
        CHECK(benaya.getCoins() == 3);
    }

    SUBCASE("Bribe costs 4 coins") {
        benaya.increaseCoins(4);
        CHECK_NOTHROW(benaya.bribe());
        CHECK(benaya.getCoins() == 0);
    }

    SUBCASE("Coup eliminates target and costs 7 coins") {
        benaya.increaseCoins(7);
        CHECK(yuval.isAlive());
        CHECK_NOTHROW(benaya.coup(yuval));
        CHECK_FALSE(yuval.isAlive());
        CHECK(benaya.getCoins() == 0);
    }

    SUBCASE("Sanction costs 3 coins and blocks target economy") {
        benaya.increaseCoins(3);
        CHECK_NOTHROW(benaya.sanction(yuval));
        CHECK(benaya.getCoins() == 0);
        CHECK_THROWS(yuval.gather());
        CHECK_THROWS(yuval.tax());
    }

    SUBCASE("Arrest takes 1 coin from target") {
        yuval.increaseCoins(2);
        CHECK(yuval.getCoins() == 3);
        CHECK_NOTHROW(benaya.arrest(yuval));
        CHECK(yuval.getCoins() == 2);
        CHECK(benaya.getCoins() == 2);
    }
}
/**
 * @test Role-specific Abilities
 */
TEST_CASE("Role-specific Abilities") {
    Game g;
    g.addPlayer("benaya", new Baron());
    g.addPlayer("yuval", new Merchant());
    g.addPlayer("eitan", new Governor());
    g.addPlayer("chagai", new General());
    g.addPlayer("keren", new Spy());
    g.addPlayer("hila", new Judge());

    Player& benaya = g.getPlayer("benaya");
    Player& yuval = g.getPlayer("yuval");
    Player& eitan = g.getPlayer("eitan");
    Player& chagai = g.getPlayer("chagai");
    Player& keren = g.getPlayer("keren");
    Player& hila = g.getPlayer("hila");

    SUBCASE("Baron invest: pays 3, gains 6") {
        benaya.increaseCoins(3);
        CHECK_NOTHROW(benaya.invest(g));
        CHECK(benaya.getCoins() == 6);
    }

    SUBCASE("Governor gets 3 coins on tax") {
        CHECK(eitan.getCoins() == 1);
        CHECK_NOTHROW(eitan.tax());
        CHECK(eitan.getCoins() == 4);
    }

    SUBCASE("Governor blocks tax") {
        CHECK_NOTHROW(yuval.tax());  // merchant
        CHECK(yuval.getCoins() == 3);
        CHECK_NOTHROW(eitan.blockTax(yuval, g));
        CHECK(yuval.getCoins() == 1); // חזרה למצב של לפני
    }

    SUBCASE("Spy can reveal coins") {
        std::stringstream buffer;
        std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
        CHECK_NOTHROW(keren.revealCoins(benaya));
        std::cout.rdbuf(old);
        CHECK(buffer.str().find("benaya has") != std::string::npos);
    }

    SUBCASE("Spy blocks arrest") {
        CHECK_NOTHROW(keren.blockArrest(benaya));
        CHECK_THROWS(benaya.arrest(keren));
    }

    SUBCASE("General blocks coup and refunds target") {
        yuval.increaseCoins(7);
        CHECK_NOTHROW(yuval.coup(chagai)); // ג'נרל הוא chagai
        CHECK(chagai.isAlive()); // נבלם
        CHECK(yuval.getCoins() == 0);
    }

    SUBCASE("General refund on arrest") {
        chagai.increaseCoins(2);
        CHECK_NOTHROW(benaya.arrest(chagai));
        CHECK(chagai.getCoins() == 2); // קיבל חזרה
    }

    SUBCASE("Judge blocks bribe") {
        yuval.increaseCoins(4);
        CHECK_NOTHROW(yuval.bribe());
        CHECK_NOTHROW(hila.blockBribe(yuval, g));
        CHECK(yuval.getCoins() == 0);
    }

    SUBCASE("Judge increases sanction cost") {
        yuval.increaseCoins(4); // תשלום + קנס
        CHECK_NOTHROW(yuval.sanction(hila));
        CHECK(yuval.getCoins() == 0);
    }

    SUBCASE("Merchant bonus at start of turn") {
        yuval.increaseCoins(2); // שיהיו לו 3 בתחילת תור
        CHECK_NOTHROW(yuval.gather());
        CHECK(yuval.getCoins() == 5); // gather + bonus
    }

    SUBCASE("Merchant pays to bank on arrest") {
        yuval.increaseCoins(1); // פחות מ-2, אז יאבד רק מה שיש
        CHECK_NOTHROW(benaya.arrest(yuval));
        CHECK(yuval.getCoins() == 0);
    }
}
/**
 * @test Blocking Actions
 */
TEST_CASE("Blocking Actions") {
    Game g;
    g.addPlayer("benaya", new Baron());
    g.addPlayer("yuval", new Governor());
    g.addPlayer("eitan", new General());
    g.addPlayer("chagai", new Spy());
    g.addPlayer("keren", new Merchant());
    g.addPlayer("hila", new Judge());

    Player& benaya = g.getPlayer("benaya");
    Player& yuval = g.getPlayer("yuval");
    Player& eitan = g.getPlayer("eitan");
    Player& chagai = g.getPlayer("chagai");
    Player& keren = g.getPlayer("keren");
    Player& hila = g.getPlayer("hila");

    SUBCASE("Governor blocks tax") {
        CHECK_NOTHROW(benaya.tax()); // בנאיה = ברון
        CHECK(benaya.getCoins() == 3);
        CHECK_NOTHROW(yuval.blockTax(benaya, g));
        CHECK(benaya.getCoins() == 1);
    }

    SUBCASE("Spy blocks arrest") {
        CHECK_NOTHROW(chagai.blockArrest(benaya));
        CHECK_THROWS(benaya.arrest(chagai));
    }

    SUBCASE("General blocks coup") {
        yuval.increaseCoins(7);
        CHECK_NOTHROW(yuval.coup(eitan));
        CHECK(eitan.isAlive());
        CHECK(yuval.getCoins() == 0);
    }

    SUBCASE("Judge blocks bribe") {
        benaya.increaseCoins(4);
        CHECK_NOTHROW(benaya.bribe());
        CHECK_NOTHROW(hila.blockBribe(benaya, g));
        CHECK(benaya.getCoins() == 0);
    }

    SUBCASE("Judge increases sanction cost") {
        benaya.increaseCoins(4); // עלות 3 + 1 עונש
        CHECK_NOTHROW(benaya.sanction(hila));
        CHECK(benaya.getCoins() == 0);
    }

    SUBCASE("General refund on arrest") {
        eitan.increaseCoins(2);
        CHECK_NOTHROW(benaya.arrest(eitan));
        CHECK(eitan.getCoins() == 2); // קיבל חזרה
    }

    SUBCASE("Merchant pays to bank on arrest") {
        keren.increaseCoins(2);
        CHECK_NOTHROW(benaya.arrest(keren));
        CHECK(keren.getCoins() == 0); // שילם שניים לבנק
    }
}
/**
 * @test Edge Cases
 */
TEST_CASE("Edge Cases") {
    Game g;
    g.addPlayer("benaya", new General());
    g.addPlayer("yuval", new Governor());
    g.addPlayer("eitan", new Spy());
    g.addPlayer("chagai", new Merchant());

    Player& benaya = g.getPlayer("benaya");
    Player& yuval = g.getPlayer("yuval");
    Player& eitan = g.getPlayer("eitan");
    Player& chagai = g.getPlayer("chagai");

    SUBCASE("Player must coup if has 10 coins") {
        benaya.increaseCoins(10);
        CHECK_THROWS(benaya.bribe());
    }

    SUBCASE("Sanction blocks gather and tax") {
        yuval.increaseCoins(3);
        CHECK_NOTHROW(yuval.sanction(benaya));
        CHECK_THROWS(benaya.gather());
        CHECK_THROWS(benaya.tax());
    }

    SUBCASE("Spy ability doesn't require turn") {
        CHECK_NOTHROW(eitan.revealCoins(benaya));
    }

    SUBCASE("Only one action per turn") {
        CHECK_NOTHROW(benaya.gather());
        CHECK_THROWS(benaya.tax());
    }

    SUBCASE("Cannot act on eliminated player") {
        benaya.increaseCoins(7);
        CHECK_NOTHROW(benaya.coup(chagai));
        CHECK_THROWS(benaya.arrest(chagai));
        CHECK_THROWS(benaya.sanction(chagai));
    }

    SUBCASE("Eliminated player cannot act") {
        yuval.increaseCoins(7);
        CHECK_NOTHROW(yuval.coup(eitan));
        CHECK_THROWS(eitan.gather());
        CHECK_THROWS(eitan.tax());
        CHECK_THROWS(eitan.sanction(benaya));
    }
}
/**
 * @test Game Ending and Winner
 */
TEST_CASE("Game Ending and Winner") {
    Game g;
    g.addPlayer("benaya", new General());
    g.addPlayer("yuval", new Governor());
    g.addPlayer("eitan", new Spy());

    Player& benaya = g.getPlayer("benaya");
    Player& yuval = g.getPlayer("yuval");
    Player& eitan = g.getPlayer("eitan");

    SUBCASE("Calling winner before game ends throws") {
        CHECK_THROWS(g.winner());
    }

    SUBCASE("Eliminated players can't win") {
        yuval.increaseCoins(7);
        CHECK_NOTHROW(yuval.coup(benaya));

        eitan.increaseCoins(7);
        CHECK_NOTHROW(eitan.coup(yuval));

        CHECK(g.winner() == "eitan");
    }

    SUBCASE("Winner is the last alive player") {
        benaya.increaseCoins(7);
        CHECK_NOTHROW(benaya.coup(yuval));

        benaya.increaseCoins(7);
        CHECK_NOTHROW(benaya.coup(eitan));

        CHECK(g.winner() == "benaya");
    }
}
