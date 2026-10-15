# 🕵️ StealthOps: The Heist

A turn-based stealth heist game written in **C++** that runs in the terminal. Sneak through a guarded building, steal the treasure, and escape through the exit before you run out of moves.

---

## 🎮 Gameplay

You play as a thief (`P`). Every move you make, the guard takes a step too, so you have to time your moves carefully.

**Mission:** collect the treasure (`T`) and reach the exit (`E`) within **35 moves**.

### Map Legend

| Symbol | Meaning |
|--------|---------|
| `P` | Player |
| `G` | Guard (patrols left and right) |
| `C` | Security camera (fixed position) |
| `T` | Treasure |
| `E` | Exit |
| `#` | Wall |
| `.` | Path |

### Controls

| Key | Action |
|-----|--------|
| `W` | Move up |
| `S` | Move down |
| `A` | Move left |
| `D` | Move right |
| `Q` | Quit the mission |

Input is not case-sensitive.

### Rules

- Walking into a wall does **not** use up a move.
- The exit stays **locked** until you have collected the treasure.
- The **guard** patrols horizontally between two points and catches you if you end up right next to it (distance of 1 cell).
- The **security camera** watches along its row. It spots you if you are in the same row with no wall between you and it. Walls block its view.
- You have **35 moves** in total.
- Previous mission results are displayed at startup if a history file is available.
- After each mission, the result, moves used, and treasure status are saved to `game_history.txt`.
- If saving a mission result fails, the program reports the error using exception handling.

### Win / Lose

- ✅ **Win:** reach the exit after collecting the treasure
- ❌ **Lose:** the guard catches you, the camera spots you, or you run out of moves

---

## 🗺️ Starting Map

```text
############
#....#....E#
#.#..#.#...#
#.#.....#..#
#..........#
###.###.#..#
#...#...#..#
#....#..T..#
#..........#
############
```

Player starts at the top-left, the guard patrols row 4, and the camera sits at the bottom-left.

---

## 🧱 Project Structure

```text
stealthOPs/
├── src/
│   └── code.cpp
└── game_history.txt  (created when mission results are saved)
```

`game_history.txt` is created in the program's current working directory when mission results are saved. It stores the results between runs.

### Classes

| Class | Description |
|-------|-------------|
| `Character` | Abstract base class holding position (`x`, `y`) and symbol, with a pure virtual `move()` |
| `Player` | Inherits `Character`. Moves with W/A/S/D and tracks whether the treasure is collected |
| `Guard` | Inherits `Character`. Patrols automatically between a left and right boundary |
| `SecurityCamera` | Detects the player along its row, with wall blocking |
| `Game` | Owns the map and all objects, runs the game loop, and checks treasure, exit and detection |

### Mission History Functions

The `Game` class also contains two functions for handling mission history:

- **`showPreviousResults()`** reads and displays previous mission results using `ifstream`.
- **`saveResult()`** appends the current mission result, moves used, and treasure status to `game_history.txt` using `ofstream`.

If the history file cannot be written, the program throws a `runtime_error`.

---

## 🧠 OOP Concepts Used

- **Abstraction:** `Character` is an abstract class with a pure virtual function
- **Inheritance:** `Player` and `Guard` derive from `Character`
- **Polymorphism:** `move()` is overridden differently in `Player` and `Guard`
- **Encapsulation:** private data members with public getters
- **Virtual destructor** in the base class
- **Composition:** `Game` contains `Player`, `Guard` and `SecurityCamera` objects
- **`const` member functions** for functions that do not modify the object
- **STL:** `vector<string>` used to store the map
- **File handling:** `ifstream` reads previous mission results and `ofstream` saves new results
- **Exception handling:** `try`, `catch`, and `runtime_error` handle file-writing errors
- **Conditional statements:** `if-else` checks movement, walls, treasure, exit and detection
- **Loops:** `while` runs the main game loop, and `for` checks the camera's line of sight
- **Standard library headers:** `<fstream>` supports file handling and `<stdexcept>` provides runtime exceptions

---

## ⚙️ Requirements

- A C++ compiler with C++11 or later (e.g. **g++ / MinGW**)
- VS Code (optional)

---

## ▶️ How to Run

**1. Clone the repository**

```bash
git clone https://github.com/<tech162>/stealthOPs.git
cd stealthOPs
```

**2. Compile**

```bash
g++ -std=c++11 src/code.cpp -o src/code.exe
```

**3. Run**

```bash
./src/code.exe
```

On Windows (Command Prompt / PowerShell):

```bash
src\code.exe
```

**Note:** The game may create `game_history.txt` in the current working directory to store mission results. Keep this file if you want to retain the history between runs.

---

## 👥 Team

| Name | Role |
|------|------|
| _Tejal Bonde_ | _7_ |
| _Srushti Gaikwad_ | _14_ |
| _Payal Lokhande_ | _34_ |
| _Siddhi Shelar_ | _59_ |

---

## 🏫 About

Developed as an Object-Oriented Programming course project for B.Tech Information Technology, Usha Mittal Institute of Technology, SNDT Women's University.

---

## 🚀 Future Improvements

- More levels and bigger maps
- Smarter guard AI (chasing the player)
- Rotating camera with a vision range
- Score and timer system
- Web frontend (HTML, CSS, JavaScript)

---

## 📄 License

This project is for educational purposes.
