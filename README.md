
// yuvalgaddidi@gmail.com

# Coup – Board Game Simulation in C++

## 📌 Project Overview

This project is a full implementation of a simplified version of the board game "Coup", written in C++ using object-oriented programming principles. It includes multiple player roles with unique abilities, a turn-based system, a central bank, and both CLI and GUI interfaces.

The project was developed for an academic assignment at Ariel University.

---

## 📁 File Structure

```
.
├── Bank.hpp / Bank.cpp         # Manages the game's coin pool
├── Baron.hpp                  # Role: Baron (invest and sanction resistance)
├── Deck.hpp                   # Simulates role card drawing
├── Game.hpp / Game.cpp        # Core game logic, players, and turns
├── General.hpp                # Role: General (block coup, recover from arrest)
├── Governor.hpp               # Role: Governor (stronger tax, block tax)
├── Judge.hpp                  # Role: Judge (block bribe, punish sanctions)
├── Merchant.hpp               # Role: Merchant (bonus income and arrest cost)
├── Player.hpp / Player.cpp    # Represents players and their actions
├── Role.hpp                   # Base role class with virtual methods
├── Spy.hpp                    # Role: Spy (view coins, block arrest)
├── doctest.h                  # Unit testing framework
├── test.cpp                   # Full test suite for all game mechanics
├── main.cpp                   # Console-based game demo
├── main_gui.cpp               # Graphical interface using SFML
├── Makefile                   # Build automation
├── README.md                  # Project description and usage
├── submission.txt             # Student ID, GitHub link, commit hash
```

---

## 🧪 Features

- Full action system: gather, tax, bribe, arrest, sanction, coup
- Role-based behavior: 6 unique roles with passive and active abilities
- Real-time blocking logic
- Turn system with forced coup on 10 coins
- Player elimination and winner detection
- GUI using SFML
- Extensive test coverage with doctest
- Memory-safe (validated with valgrind)

---

## 🛠️ Build and Run Instructions

### 🔧 Requirements
- C++17 compiler (e.g. `g++`)
- [SFML](https://www.sfml-dev.org/) for GUI

### 🔨 Build Commands

```bash
make Main        # Compile and run console demo
make test        # Compile and run unit tests
make gui         # Compile and run GUI application
make valgrind    # Run valgrind on demo
make clean       # Remove compiled files
```

### ▶️ Run Examples

```bash
./Main           # Run the CLI game demo
./test           # Run all test cases
./gui_app        # Launch the graphical user interface
```

---

## ✅ Tests

The `test.cpp` file includes over 100 test cases covering:
- All player actions and role interactions
- Turn order and validation
- Illegal moves (e.g., acting out of turn, targeting eliminated players)
- Blocking mechanisms
- Edge cases and role-specific behavior

---

## 🖥️ GUI (SFML)

The GUI allows:
- Display of current turn and player roles
- Action buttons for all players
- Visual logging of each action or block
- Coin counters and elimination indicators
- Dynamic player input using mouse and buttons

**Notes:**
- Only 2–6 players supported.
- All players share the same screen.
- Blocks and reactions handled "in real time".

---

## 🧾 Submission Requirements Covered

✅ Clean modular C++ code  
✅ `Makefile` with all required commands  
✅ `main.cpp` for demo  
✅ `main_gui.cpp` for SFML demo  
✅ `test.cpp` with full test coverage  
✅ `README.md` with project explanation  
✅ `submission.txt` with ID, GitHub link, and commit hash  
✅ No memory leaks (`valgrind` clean)

---

## 📧 Contact

**Student:** Yuval Gaddidi  
**Email:** yuvalgaddidi@gmail.com

---

Enjoy the game!
