// yuvalgaddidi@gmail.com

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <stdexcept>
#include "Game.hpp"
#include "Deck.hpp"
#include "Player.hpp"
#include "Governor.hpp"
#include "Spy.hpp"
#include "Judge.hpp"
#include "Baron.hpp"
#include "General.hpp"
#include "Merchant.hpp"

using namespace std;

const int WIDTH = 1000;
const int HEIGHT = 800;
const int MAX_PLAYERS = 6;

enum class GuiState
{
    Start,
    AddPlayers,
    Playing,
    End
};

class Button
{
    sf::RectangleShape rect;
    sf::Text text;

public:
    string action;
    Player *target;

    // Public setter for rect color
    void setFillColor(const sf::Color &color)
    {
        rect.setFillColor(color);
    }

    Button(const string &label, float x, float y, sf::Font &font, const string &act, Player *t = nullptr)
        : action(act), target(t)
    {
        rect.setSize(sf::Vector2f(150, 30));
        rect.setFillColor(sf::Color(100, 100, 100));
        rect.setPosition(x, y);

        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(18);
        text.setFillColor(sf::Color::White);
        text.setPosition(x + 10, y + 5);
    }

    void draw(sf::RenderWindow &window) const
    {
        window.draw(rect);
        window.draw(text);
    }

    bool isClicked(sf::Vector2f point) const
    {
        return rect.getGlobalBounds().contains(point);
    }
};

void drawText(sf::RenderWindow &win, const string &str, float x, float y, int size, sf::Font &font, sf::Color col = sf::Color::White)
{
    sf::Text txt;
    txt.setFont(font);
    txt.setString(str);
    txt.setCharacterSize(size);
    txt.setFillColor(col);
    txt.setPosition(x, y);
    win.draw(txt);
}

void drawPlayers(sf::RenderWindow &win, const Game &game, sf::Font &font)
{
    int y = 500;
    for (Player *p : game.getPlayers())
    {
        if (p->isAlive())
        {
            drawText(win, p->getName() + " - " + to_string(p->getCoins()) + " coins", 50, y, 18, font, sf::Color::Cyan);
            y += 25;
        }
    }
}

void addToLog(vector<string> &log, const string &entry)
{
    log.push_back(entry);
    if (log.size() > 10)
        log.erase(log.begin());
}
int main()
{
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Coup GUI");
    sf::Font font;
    font.loadFromFile("arial.ttf");

    GuiState state = GuiState::Start;
    Game game;
    GuiState previousState = GuiState::Playing;
    string pendingAction = "";
    Player *actionSource = nullptr;

    Deck deck;
    vector<unique_ptr<Player>> createdPlayers;
    vector<Button> buttons;
    vector<string> log;
    string message = "Welcome!";
    string nameInput = "";
    bool nameFieldFocused = false;
    string pendingAction = "";
    bool waitingForTarget = false;
    bool waitingForBlock = false;
    Player *blockSource = nullptr;
    string blockableAction = "";
    Player *blockableTarget = nullptr;
    Player *pendingTarget = nullptr;
    Player *current = nullptr;
    string winnerName;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::TextEntered && state == GuiState::AddPlayers && nameFieldFocused)
            {
                if (event.text.unicode < 128)
                {
                    char c = static_cast<char>(event.text.unicode);
                    if (c == '\b' && !nameInput.empty())
                        nameInput.pop_back();
                    else if (isalnum(c))
                        nameInput += c;
                }
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                if (state == GuiState::AddPlayers)
                {
                    sf::FloatRect nameBox(300, 200, 300, 35);
                    nameFieldFocused = nameBox.contains(mousePos);
                }

                for (Button &b : buttons)
                {
                    if (!b.isClicked(mousePos))
                        continue;

                    if (state == GuiState::Start && b.action == "start")
                    {
                        state = GuiState::AddPlayers;
                        message = "Enter player name:";
                        break;
                    }
                    else if (state == GuiState::AddPlayers)
                    {
                        if (b.action == "add" && !nameInput.empty())
                        {
                            string trimmed = nameInput;
                            trimmed.erase(trimmed.find_last_not_of(" \n\r\t") + 1);
                            trimmed.erase(0, trimmed.find_first_not_of(" \n\r\t"));
                            bool exists = false;
                            for (Player *p : game.getPlayers())
                            {
                                if (p->getName() == trimmed)
                                {
                                    exists = true;
                                    break;
                                }
                            }
                            if (!exists)
                            {
                                try
                                {
                                    Player *pl = new Player(game, trimmed, deck.drawRole());
                                    createdPlayers.emplace_back(pl);
                                    game.addPlayer(*pl);
                                    addToLog(log, "Added " + trimmed + " (" + pl->getRole()->name() + ")");
                                    nameInput = "";
                                    message = "Player added!";
                                }
                                catch (const std::exception &e)
                                {
                                    message = string("Error: ") + e.what();
                                }
                            }
                            else
                            {
                                message = "Name already taken.";
                            }
                        }
                        else if (b.action == "begin" && game.getPlayers().size() >= 2)
                        {
                            state = GuiState::Playing;
                            message = "Game started!";
                        }
                    }
                    else if (state == GuiState::Playing)
                    {
                        current = &game.currentPlayer();
                        Role *role = current->getRole();

                        if (waitingForTarget && b.action == "target")
                        {
                            Player *target = b.target;
                            try
                            {
                                if (pendingAction == "arrest")
                                {
                                    addToLog(log, current->getName() + " arrests " + target->getName());
                                    current->arrest(*target);
                                    blockableAction = "arrest";
                                    blockableTarget = target;
                                    blockSource = current;
                                    waitingForBlock = true;
                                }
                                else if (pendingAction == "sanction")
                                {
                                    addToLog(log, current->getName() + " sanctions " + target->getName());
                                    current->sanction(*target);
                                    blockableAction = "sanction";
                                    blockableTarget = target;
                                    blockSource = current;
                                    waitingForBlock = true;
                                }
                                else if (pendingAction == "coup")
                                {
                                    addToLog(log, current->getName() + " attempts coup on " + target->getName());
                                    current->coup(*target);
                                    blockableAction = "coup";
                                    blockableTarget = target;
                                    blockSource = current;
                                    waitingForBlock = true;
                                }
                                else if (pendingAction == "block_arrest" && dynamic_cast<Spy *>(role))
                                {
                                    addToLog(log, current->getName() + " blocks arrest on " + target->getName());
                                    dynamic_cast<Spy *>(role)->blockArrest(*target);
                                    game.nextTurn();
                                }
                                else if (pendingAction == "block_coup" && dynamic_cast<General *>(role))
                                {
                                    addToLog(log, current->getName() + " blocks coup from " + target->getName());
                                    dynamic_cast<General *>(role)->blockCoup(*target, *current, game);
                                    game.nextTurn();
                                }
                                else if (pendingAction == "block_bribe" && dynamic_cast<Judge *>(role))
                                {
                                    addToLog(log, current->getName() + " blocks bribe from " + target->getName());
                                    dynamic_cast<Judge *>(role)->blockBribe(*target, game);
                                    game.nextTurn();
                                }
                                else if (pendingAction == "block_tax" && dynamic_cast<Governor *>(role))
                                {
                                    addToLog(log, current->getName() + " blocks tax from " + target->getName());
                                    dynamic_cast<Governor *>(role)->onTax(*target, game);
                                    game.nextTurn();
                                }
                                else if (pendingAction == "reveal_target" && dynamic_cast<Spy *>(role))
                                {
                                    dynamic_cast<Spy *>(role)->revealCoins(*target);
                                    message = target->getName() + "'s coins revealed.";
                                    addToLog(log, current->getName() + " revealed " + target->getName() + "'s coins.");
                                }
                            }
                            catch (const std::exception &e)
                            {
                                message = string("Error: ") + e.what();
                            }

                            pendingAction = "";
                            waitingForTarget = false;
                            if (!waitingForBlock && !game.isOver())
                                game.nextTurn();
                        }
                    }
                    else if (b.action == "gather")
                    {
                        try
                        {
                            addToLog(log, current->getName() + " gathers 1 coin");
                            current->gather();
                            if (!waitingForBlock && !game.isOver())
                                game.nextTurn();
                        }
                        catch (const std::exception &e)
                        {
                            message = string("Error: ") + e.what();
                        }
                    }
                    else if (b.action == "tax")
                    {
                        try
                        {
                            addToLog(log, current->getName() + " attempts tax");
                            current->tax();
                            blockableAction = "tax";
                            blockableTarget = current;
                            blockSource = current;
                            waitingForBlock = true;
                        }
                        catch (const std::exception &e)
                        {
                            message = string("Error: ") + e.what();
                        }
                    }
                    else if (b.action == "bribe")
                    {
                        try
                        {
                            addToLog(log, current->getName() + " attempts bribe");
                            current->bribe();
                            blockableAction = "bribe";
                            blockableTarget = current;
                            blockSource = current;
                            waitingForBlock = true;
                        }
                        catch (const std::exception &e)
                        {
                            message = string("Error: ") + e.what();
                        }
                    }
                    else if (b.action == "invest")
                    {
                        Role *role = current->getRole();
                        if (dynamic_cast<Baron *>(role))
                        {
                            try
                            {
                                dynamic_cast<Baron *>(role)->invest(*current, game);
                                addToLog(log, current->getName() + " invests (Baron)");
                                game.nextTurn();
                            }
                            catch (const std::exception &e)
                            {
                                message = string("Error: ") + e.what();
                            }
                        }
                    }
                    else if (b.action == "reveal")
                    {
                        pendingAction = "reveal_target";
                        waitingForTarget = true;
                    }
                    else if (b.action.find("target_") == 0)
                    {
                        pendingTarget = b.target;
                        if (pendingAction == "bribe" || pendingAction == "coup")
                        {
                            blockableAction = pendingAction;
                            blockSource = actionSource;
                            waitingForBlock = true;
                            state = GuiState::Playing;
                            log.push_back(actionSource->getName() + " tried " + pendingAction + " on " + pendingTarget->getName());
                        }
                        else if (pendingAction == "arrest")
                        {
                            actionSource->arrest(*pendingTarget);
                            log.push_back(actionSource->getName() + " arrested " + pendingTarget->getName());
                            state = GuiState::Playing;
                        }
                        else if (pendingAction == "sanction")
                        {
                            actionSource->sanction(*pendingTarget);
                            log.push_back(actionSource->getName() + " sanctioned " + pendingTarget->getName());
                            state = GuiState::Playing;
                        }
                    }
                    else if (b.action == "block_arrest")
                    {
                        pendingAction = "block_arrest";
                        waitingForTarget = true;
                    }
                    else if (b.action == "block_coup")
                    {
                        pendingAction = "block_coup";
                        waitingForTarget = true;
                    }
                    else if (b.action == "block_bribe")
                    {
                        pendingAction = "block_bribe";
                        waitingForTarget = true;
                    }
                    else if (b.action == "block_tax")
                    {
                        pendingAction = "block_tax";
                        waitingForTarget = true;
                    }
                    else if (b.action == "arrest")
                    {
                        pendingAction = "arrest";
                        waitingForTarget = true;
                    }
                    else if (b.action == "sanction")
                    {
                        pendingAction = "sanction";
                        waitingForTarget = true;
                    }
                    else if (b.action == "coup")
                    {
                        pendingAction = "coup";
                        waitingForTarget = true;
                    }
                    else if (b.action == "allow")
                    {
                        addToLog(log, "Action allowed: " + blockableAction);
                        blockableAction = "";
                        blockableTarget = nullptr;
                        blockSource = nullptr;
                        waitingForBlock = false;
                        if (!game.isOver())
                            game.nextTurn();
                    }
                }
            }
        }

        window.clear(sf::Color::Black);
        buttons.clear();

        drawText(window, message, 40, 30, 24, font);

        if (state == GuiState::Start)
        {
            buttons.emplace_back("Start Game", 450, 300, font, "start");
        }
        else if (state == GuiState::AddPlayers)
        {
            sf::RectangleShape inputBox(sf::Vector2f(300, 35));
            inputBox.setPosition(300, 200);
            inputBox.setFillColor(nameFieldFocused ? sf::Color(60, 60, 60) : sf::Color(40, 40, 40));
            inputBox.setOutlineThickness(2);
            inputBox.setOutlineColor(sf::Color::White);
            window.draw(inputBox);

            sf::Text inputText(nameInput, font, 20);
            inputText.setPosition(310, 205);
            inputText.setFillColor(sf::Color::White);
            window.draw(inputText);

            buttons.emplace_back("Add", 300, 260, font, "add");
            if (game.getPlayers().size() >= 2)
                buttons.emplace_back("Start Round", 300, 310, font, "begin");
            drawPlayers(window, game, font);
        }
        else if (state == GuiState::Playing)
        {
            current = &game.currentPlayer();
            drawText(window, "Current turn: " + current->getName(), 40, 90, 22, font, sf::Color::Yellow);
            drawPlayers(window, game, font);

            int sx = 30, sy = 100;
            for (Player *p : game.getPlayers())
            {
                if (!p->isAlive())
                    continue;

                Role *r = p->getRole();
                drawText(window, p->getName(), sx, sy, 18, font, p == current ? sf::Color::Green : sf::Color::White);

                vector<pair<string, string>> actions = {
                    {"Gather", "gather"}, {"Tax", "tax"}, {"Bribe", "bribe"}, {"Arrest", "arrest"}, {"Sanction", "sanction"}, {"Coup", "coup"}};

                if (dynamic_cast<Baron *>(r))
                    actions.emplace_back("Invest", "invest");
                if (dynamic_cast<Spy *>(r))
                    actions.emplace_back("Reveal", "reveal");
                if (dynamic_cast<Spy *>(r))
                    actions.emplace_back("Block Arrest", "block_arrest");
                if (dynamic_cast<General *>(r))
                    actions.emplace_back("Block Coup", "block_coup");
                if (dynamic_cast<Governor *>(r))
                    actions.emplace_back("Block Tax", "block_tax");
                if (dynamic_cast<Judge *>(r))
                    actions.emplace_back("Block Bribe", "block_bribe");
                int y = sy + 20;
                for (auto &[label, act] : actions)
                {
                    sf::Color col = (act.find("block") != string::npos || act == "reveal" || act == "invest") ? sf::Color(80, 80, 160) : sf::Color(100, 100, 100);
                    buttons.emplace_back(label, sx, y, font, act, p);
                    buttons.back().setFillColor(col);
                    y += 35;
                }
                sx += 160;
            }

            if (waitingForTarget)
            {
                int tx = 850, ty = 100;
                for (Player *p : game.getPlayers())
                {
                    if (p->isAlive() && p != current)
                    {
                        buttons.emplace_back(p->getName(), tx, ty, font, "target", p);
                        ty += 40;
                    }
                }
                drawText(window, "Choose target for: " + pendingAction, 830, 70, 18, font, sf::Color::Cyan);
            }

            if (waitingForBlock)
            {
                drawText(window, "Block or Allow?", 750, 500, 20, font, sf::Color::Red);
                buttons.emplace_back("Allow", 750, 540, font, "allow");
            }
        }
        else if (state == GuiState::End)
        {
            drawText(window, "Winner: " + winnerName, 400, 200, 30, font, sf::Color::Green);
            buttons.emplace_back("Play Again", 450, 300, font, "again");
        }

        int logY = HEIGHT - 180;
        drawText(window, "Log:", 50, logY, 20, font, sf::Color::White);
        for (const string &entry : log)
        {
            logY += 20;
            drawText(window, entry, 50, logY, 18, font, sf::Color(180, 180, 180));
        }

        for (const Button &b : buttons)
            b.draw(window);
        window.display();
    }

    return 0;
}
