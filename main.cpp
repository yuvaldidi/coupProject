// yuvalgaddidi@gmail.com

#include <iostream>
#include "Game.hpp"
#include "Player.hpp"
#include "Governor.hpp"
#include "Spy.hpp"
#include "Judge.hpp"
#include "Baron.hpp"
#include "General.hpp"
#include "Merchant.hpp"

using namespace std;

void printGameState(Game &game)
{
    cout << "\n=== Current Players & Coins ===" << endl;
    for (Player *p : game.getPlayers())
    {
        if (p->isAlive())
        {
            cout << p->getName() << " - " << p->getCoins() << " coins" << endl;
        }
    }
    cout << "===============================\n"
         << endl;
}

int main()
{
    Game game;

    // Create players with specific roles
    Governor gov;
    Spy spy;
    Judge judge;
    Baron baron;
    General general;
    Merchant merchant;

    Player benaya(game, "benaya", new Governor());
    Player yuval(game, "yuval", new Spy());
    Player eitan(game, "eitan", new Judge());
    Player chagai(game, "chagai", new Baron());
    Player keren(game, "keren", new General());
    Player hila(game, "hila", new Merchant());

    // Print initial state
    cout << "Turn order: ";
    for (Player *p : game.getPlayers())
    {
        cout << p->getName() << " ";
    }
    cout << "\n";
    printGameState(game);

    // --- Turn: benaya ---
    cout << "--- Turn: " << game.turn() << " ---" << endl;
    try
    {
        benaya.tax(); // Governor gets 3
        cout << "benaya performs tax\nAction succeeded.\n";
    }
    catch (exception &e)
    {
        cout << "Action failed: " << e.what() << endl;
    }
    printGameState(game);

    // --- Turn: yuval ---
    cout << "--- Turn: " << game.turn() << " ---" << endl;
    try
    {
        yuval.gather(); // Merchant doesn't activate yet
        cout << "yuval performs gather\nAction succeeded.\n";
    }
    catch (exception &e)
    {
        cout << "Action failed: " << e.what() << endl;
    }
    printGameState(game);

    // --- Turn: eitan ---
    cout << "--- Turn: " << game.turn() << " ---" << endl;
    try
    {
        eitan.bribe(); // Judge, but not enough coins
        cout << "eitan attempts bribe\nAction succeeded.\n";
    }
    catch (exception &e)
    {
        cout << "Action failed: " << e.what() << endl;
    }
    printGameState(game);

    try
    {
        eitan.invest(game); // Not a Baron – should fail
        cout << "eitan performs invest\nAction succeeded.\n";
    }
    catch (exception &e)
    {
        cout << "Action failed: " << e.what() << endl;
    }
    printGameState(game);

    try
    {
        eitan.gather(); // Legal, gives 1
        cout << "eitan performs gather\nAction succeeded.\n";
    }
    catch (exception &e)
    {
        cout << "Action failed: " << e.what() << endl;
    }
    printGameState(game);

    // --- Turn: chagai ---
    cout << "--- Turn: " << game.turn() << " ---" << endl;
    try
    {
        chagai.invest(game); // Baron pays 3 gets 6
        cout << "chagai performs invest\nAction succeeded.\n";
    }
    catch (exception &e)
    {
        cout << "Action failed: " << e.what() << endl;
    }
    printGameState(game);

    // --- Turn: keren ---
    cout << "--- Turn: " << game.turn() << " ---" << endl;
    try
    {
        keren.arrest(hila); // General will respond later
        cout << "keren performs arrest on hila\nAction succeeded.\n";
    }
    catch (exception &e)
    {
        cout << "Action failed: " << e.what() << endl;
    }
    printGameState(game);

    // --- Turn: hila ---
    cout << "--- Turn: " << game.turn() << " ---" << endl;
    try
    {
        hila.gather(); // Merchant may get bonus
        cout << "hila performs gather\nAction succeeded.\n";
    }
    catch (exception &e)
    {
        cout << "Action failed: " << e.what() << endl;
    }
    printGameState(game);

    // === Illegal actions (out of turn / bad state) ===
    cout << "--- Invalid Action: benaya bribe out of turn ---" << endl;
    try
    {
        benaya.bribe();
        cout << "Action succeeded (should not happen).\n";
    }
    catch (exception &e)
    {
        cout << "Expected failure: " << e.what() << "\n";
    }

    cout << "--- Invalid Action: eitan coup with <10 coins ---" << endl;
    try
    {
        eitan.coup(benaya);
        cout << "Action succeeded (should not happen).\n";
    }
    catch (exception &e)
    {
        cout << "Expected failure: " << e.what() << "\n";
    }

    cout << "--- Invalid Action: yuval gather under sanction ---" << endl;
    try
    {
        yuval.gather();
        cout << "Action succeeded (should not happen).\n";
    }
    catch (exception &e)
    {
        cout << "Expected failure: " << e.what() << "\n";
    }

    printGameState(game);

    // --- Turn: keren performs sanction on yuval ---
    cout << "--- Turn: " << game.turn() << " ---" << endl;
    try
    {
        keren.sanction(yuval);
        cout << "keren performs sanction on yuval\nAction succeeded.\n";
    }
    catch (exception &e)
    {
        cout << "Action failed: " << e.what() << endl;
    }
    printGameState(game);

    // --- Turn: hila performs arrest on benaya ---
    cout << "--- Turn: " << game.turn() << " ---" << endl;
    try
    {
        hila.arrest(benaya);
        cout << "hila performs arrest on benaya\nAction succeeded.\n";
    }
    catch (exception &e)
    {
        cout << "Action failed: " << e.what() << endl;
    }
    printGameState(game);

    // --- Turn: benaya tries tax while sanctioned (blocked by governor himself) ---
    cout << "--- Turn: " << game.turn() << " ---" << endl;
    try
    {
        benaya.tax(); // Governor can't be sanctioned, but for completeness we try again
        cout << "benaya performs tax\nAction succeeded.\n";
    }
    catch (exception &e)
    {
        cout << "Action failed: " << e.what() << endl;
    }
    printGameState(game);

    // --- Turn: yuval tries gather under sanction ---
    cout << "--- Turn: " << game.turn() << " ---" << endl;
    try
    {
        yuval.gather(); // Should fail due to sanction
        cout << "yuval performs gather\nAction succeeded.\n";
    }
    catch (exception &e)
    {
        cout << "Expected failure: " << e.what() << endl;
    }
    printGameState(game);

    // --- Turn: eitan performs bribe with success ---
    cout << "--- Turn: " << game.turn() << " ---" << endl;
    try
    {
        // קודם נטען אותו למעל 4
        eitan.gather(); // 3
        eitan.gather(); // 4
        eitan.gather(); // 5
        eitan.bribe();  // success
        cout << "eitan performs bribe\nAction succeeded.\n";
        // אפשר להוסיף פעולה שנייה אם רוצים, או להשאיר כך
    }
    catch (exception &e)
    {
        cout << "Action failed: " << e.what() << endl;
    }
    printGameState(game);

    // --- Turn: chagai performs invest with 3 coins ---
    cout << "--- Turn: " << game.turn() << " ---" << endl;
    try
    {
        chagai.gather(); // 2
        chagai.gather(); // 3
        chagai.invest(game);
        cout << "chagai performs invest\nAction succeeded.\n";
    }
    catch (exception &e)
    {
        cout << "Action failed: " << e.what() << endl;
    }
    printGameState(game);

    // --- Turn: keren performs coup on yuval (General will block it) ---
    cout << "--- Turn: " << game.turn() << " ---" << endl;
    try
    {
        // נאפשר לקורן לצבור 7
        keren.gather();
        keren.gather();
        keren.gather();
        keren.gather();
        keren.gather();
        keren.gather();
        keren.gather();    // 8
        keren.coup(yuval); // General blocks himself
        cout << "keren performs coup on yuval\nAction attempted.\n";
    }
    catch (exception &e)
    {
        cout << "Expected failure or block: " << e.what() << endl;
    }
    printGameState(game);

    // --- Turn: benaya performs gather to reach 5 coins ---
    while (benaya.getCoins() < 5 && game.turn() == "benaya")
    {
        try
        {
            benaya.gather();
            cout << "benaya gathers\n";
        }
        catch (exception &e)
        {
            cout << "benaya gather failed: " << e.what() << endl;
        }
        printGameState(game);
    }

    // --- Turn: yuval (Spy) blocks arrest on benaya ---
    while (game.turn() != "eitan")
        game.nextTurn();
    try
    {
        yuval.blockArrest(benaya);
        cout << "yuval blocks arrest on benaya\n";
    }
    catch (exception &e)
    {
        cout << "Spy block failed: " << e.what() << endl;
    }

    // --- Turn: eitan tries to arrest benaya (should be blocked by Spy) ---
    try
    {
        eitan.arrest(benaya);
        cout << "eitan arrests benaya\n";
    }
    catch (exception &e)
    {
        cout << "eitan arrest failed: " << e.what() << endl;
    }
    printGameState(game);

    // --- Turn: chagai performs tax and benaya (Governor) blocks him ---
    while (game.turn() != "chagai")
        game.nextTurn();
    try
    {
        chagai.tax();
        cout << "chagai performs tax\n";
    }
    catch (exception &e)
    {
        cout << "chagai tax failed: " << e.what() << endl;
    }
    try
    {
        benaya.blockTax(chagai, game);
        cout << "benaya blocks tax from chagai\n";
    }
    catch (exception &e)
    {
        cout << "blockTax failed: " << e.what() << endl;
    }
    printGameState(game);

    // --- Turn: hila reaches 10 coins to trigger coup ---
    while (hila.getCoins() < 10 && game.turn() == "hila")
    {
        try
        {
            hila.gather();
            cout << "hila gathers\n";
        }
        catch (exception &e)
        {
            cout << "hila gather failed: " << e.what() << endl;
        }
        printGameState(game);
    }

    // --- Turn: hila must perform coup on yuval ---
    try
    {
        hila.coup(yuval);
        cout << "hila performs coup on yuval\n";
    }
    catch (exception &e)
    {
        cout << "hila coup failed: " << e.what() << endl;
    }
    printGameState(game);

    // --- Turn: keren arrests eitan ---
    while (game.turn() != "keren")
        game.nextTurn();
    try
    {
        keren.arrest(eitan);
        cout << "keren arrests eitan\n";
    }
    catch (exception &e)
    {
        cout << "keren arrest failed: " << e.what() << endl;
    }
    printGameState(game);

    // --- Turn: hila tries to arrest eitan again (should fail – same as last arrest) ---
    while (game.turn() != "hila")
        game.nextTurn();
    try
    {
        hila.arrest(eitan);
        cout << "hila arrests eitan again\n";
    }
    catch (exception &e)
    {
        cout << "Expected failure (double arrest): " << e.what() << endl;
    }

    // --- Turn: benaya coups eitan (eliminates him) ---
    while (game.turn() != "benaya")
        game.nextTurn();
    try
    {
        benaya.coup(eitan);
        cout << "benaya coups eitan\n";
    }
    catch (exception &e)
    {
        cout << "benaya coup failed: " << e.what() << endl;
    }
    printGameState(game);

    // --- Turn: yuval tries to arrest eitan (already dead) ---
    while (game.turn() != "yuval")
        game.nextTurn();
    try
    {
        yuval.arrest(eitan);
        cout << "yuval arrests eitan (should fail)\n";
    }
    catch (exception &e)
    {
        cout << "Expected failure (arrest on dead player): " << e.what() << endl;
    }
    printGameState(game);

    // --- Turn: yuval (Spy) reveals coins of chagai ---
    while (game.turn() != "yuval")
        game.nextTurn();
    try
    {
        yuval.revealCoins(chagai);
        cout << "yuval (Spy) reveals coins of chagai\n";
    }
    catch (exception &e)
    {
        cout << "Spy revealCoins failed: " << e.what() << endl;
    }

    // --- Turn: keren sanctions chagai (Baron) to trigger compensation ---
    while (game.turn() != "keren")
        game.nextTurn();
    try
    {
        keren.sanction(chagai);
        cout << "keren sanctions chagai\n";
    }
    catch (exception &e)
    {
        cout << "Sanction failed: " << e.what() << endl;
    }
    printGameState(game);

    // --- Turn: hila performs arrest on keren (General) to test refund ---
    keren.increaseCoins(1); // נותנים לה מטבע אחד כדי לבדוק את הפיצוי
    cout << "Gave keren (General) 1 coin before arrest\n";
    printGameState(game);

    while (game.turn() != "hila")
        game.nextTurn();
    try
    {
        hila.arrest(keren);
        cout << "hila arrests keren (General)\n";
    }
    catch (exception &e)
    {
        cout << "Arrest failed: " << e.what() << endl;
    }
    printGameState(game);

    // === Attempting to declare winner ===
    cout << "=== Attempting to declare winner ===" << endl;
    try
    {
        string win = game.winner();
        cout << "Winner: " << win << "\n"
             << endl;
    }
    catch (const exception &e)
    {
        cout << "Winner: Expected failure: " << e.what() << "\n"
             << endl;
    }

    return 0;
}
