#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
using namespace std;

// ======= CHARACTER BASE CLASS ======

class Character {

protected:
    int x, y;
    char symbol;

public:
    // Constructor: initializes the character's position and symbol.
    Character(int startX, int startY, char s) {
        x = startX;
        y = startY;
        symbol = s;
    }

    // Abstraction: pure virtual function makes Character an abstract class.
    virtual void move(char direction) = 0;

    // Const functions: return values without changing the object.
    int getX() const { return x; }
    int getY() const { return y; }
    char getSymbol() const { return symbol; }

    // Virtual destructor: supports safe destruction through a base pointer.
    virtual ~Character() {}
};

// ====== PLAYER CLASS =========

// Inheritance: Player gets common properties from Character.
class Player : public Character {

private:
    bool hasTreasure;  // Encapsulation: treasure status is private.

    // Friend function can access Player's private data.
    friend void showTreasureStatus(const Player& p);

public:
    // Constructor chaining: calls the Character constructor first.
    Player(int startX, int startY)
        : Character(startX, startY, 'P') {
        hasTreasure = false;
    }

    // Polymorphism: overrides Character's virtual move() function.
    void move(char direction) override {
        direction = tolower(direction);

        if (direction == 'w')
            y--;
        else if (direction == 's')
            y++;
        else if (direction == 'a')
            x--;
        else if (direction == 'd')
            x++;
    }

    void collectTreasure() {
        hasTreasure = true;
    }

    bool ownsTreasure() const {
        return hasTreasure;
    }
};

// ====== FRIEND FUNCTION ======

// Friend function: accesses private data without being a class member.
void showTreasureStatus(const Player& p) {
    cout << "\nTreasure status: ";

    if (p.hasTreasure)
        cout << "Collected\n";
    else
        cout << "Not collected\n";
}

// ====== GUARD CLASS =======

// Inheritance: Guard also derives from Character.
class Guard : public Character {

private:
    int minX, maxX;
    int direction;

public:
    // Constructor chaining: initializes the Character base class.
    Guard(int startX, int startY, int left, int right)
        : Character(startX, startY, 'G') {

        minX = left;
        maxX = right;
        direction = 1;
    }

    // Polymorphism: Guard provides its own version of move().
    void move(char direction) override {
        (void)direction;

        // this->direction refers to the guard's data member.
        x += this->direction;

        if (x >= maxX) {
            x = maxX;
            this->direction = -1;
        }
        else if (x <= minX) {
            x = minX;
            this->direction = 1;
        }
    }
};

// ===== SECURITY CAMERA CLASS =======

class SecurityCamera {

private:
    // Encapsulation: camera position is kept private.
    int x, y;

public:
    // Constructor: sets the camera's position.
    SecurityCamera(int cameraX, int cameraY) {
        x = cameraX;
        y = cameraY;
    }

    int getX() const { return x; }
    int getY() const { return y; }

    // Const references avoid copying the objects and prevent modification.
    bool detectsPlayer(const Player& player,
                       const vector<string>& map) const {

        if (player.getY() != y || player.getX() == x)
            return false;

        int playerX = player.getX();

        // Ternary operator: chooses the direction of camera scanning.
        int step = (playerX > x) ? 1 : -1;

        // Checks whether a wall blocks the camera's view.
        for (int col = x + step;
             col != playerX + step;
             col += step) {

            if (map[y][col] == '#')
                return false;
        }

        return true;
    }
};

// ======= MAIN GAME CLASS ========

class Game {

private:
    vector<string> map;

    // Composition: Game contains Player, Guard and SecurityCamera objects.
    Player player;
    Guard guard;
    SecurityCamera camera;

    int movesLeft;
    bool gameOver;
    bool won;

    // Static data member: shared by all Game objects.
    static int totalGames;

    // File handling: reads previously saved results.
    void showPreviousResults() {
        ifstream fin("game_history.txt");

        if (!fin) {
            cout << "\nNo previous mission history found.\n";
            return;
        }

        string line;

        cout << "\n===== PREVIOUS MISSION HISTORY =====\n";

        while (getline(fin, line)) {
            cout << line << '\n';
        }

        fin.close();
    }

    // File handling: appends the current result to a file.
    void saveResult() {
        ofstream fout("game_history.txt", ios::app);

        if (!fout) {
            throw runtime_error(
                "Unable to open game_history.txt for writing."
            );
        }

        fout << "Mission Result: "
             << (won ? "SUCCESS" : "FAILED / QUIT") << '\n';

        fout << "Moves Used: " << 35 - movesLeft << '\n';

        fout << "Treasure: "
             << (player.ownsTreasure()
                 ? "Collected" : "Not collected")
             << '\n';

        fout << "-----------------------------\n";

        // Exception handling: reports a file-writing failure.
        if (!fout) {
            throw runtime_error(
                "Unable to write mission result to game_history.txt."
            );
        }

        fout.close();
    }

    // Checks whether a map position contains a wall.
    bool isWall(int x, int y) const {
        if (map[y][x] == '#')
            return true;
        else
            return false;
    }

    // Displays the map using a copy, leaving the original map unchanged.
    void displayMap() const {
        vector<string> board = map;

        board[camera.getY()][camera.getX()] = 'C';
        board[guard.getY()][guard.getX()] = 'G';
        board[player.getY()][player.getX()] = 'P';

        cout << "\n";

        for (const string& row : board)
            cout << row << '\n';

        cout << "\nMoves remaining: " << movesLeft;

        cout << "\nTreasure: "
             << (player.ownsTreasure()
                 ? "Collected" : "Not collected")
             << "\n";
    }

    // Checks whether the player collects the treasure.
    void checkTreasure() {
        if (map[player.getY()][player.getX()] == 'T') {
            player.collectTreasure();

            map[player.getY()][player.getX()] = '.';

            cout << "\nTreasure collected!\n";
        }
    }

    // Checks whether the player can escape.
    void checkExit() {
        if (map[player.getY()][player.getX()] == 'E') {
            if (player.ownsTreasure()) {
                won = true;
                gameOver = true;
            }
            else {
                cout << "\nExit locked! Collect the treasure first.\n";
            }
        }
    }

    // Checks if the guard or camera detects the player.
    void checkDetection() {
        int distance =
            abs(player.getX() - guard.getX()) +
            abs(player.getY() - guard.getY());

        if (distance <= 1) {
            cout << "\nThe guard caught you! Mission failed.\n";

            gameOver = true;
            won = false;
            return;
        }

        if (camera.detectsPlayer(player, map)) {
            cout << "\nThe security camera spotted you! Mission failed.\n";

            gameOver = true;
            won = false;
        }
    }

public:
    // Static member function: can be called using the class name.
    static int getTotalGames() {
        return totalGames;
    }

    // Game constructor - kept with its initializer list.
    Game()
        : map({
            "############",
            "#....#....E#",
            "#.#..#.#...#",
            "#.#.....#..#",
            "#..........#",
            "###.###.#..#",
            "#...#...#..#",
            "#....#..T..#",
            "#..........#",
            "############"
          }),
          player(1, 1),
          guard(4, 4, 3, 9),
          camera(1, 7),
          movesLeft(35),
          gameOver(false),
          won(false) {

        totalGames++;  // Increments the shared game-object count.
    }

    // Main function that controls the gameplay.
    void run() {

        showPreviousResults();

        cout << "====================================\n";
        cout << "       STEALTHOPS: THE HEIST\n";
        cout << "====================================\n";

        cout << "\nMISSION: Collect the treasure and escape!\n";
        cout << "Avoid the guard and security camera.\n";
        cout << "You have 35 moves.\n";

        cout << "\nCONTROLS:\n";
        cout << "W = Up | S = Down\n";
        cout << "A = Left | D = Right\n";
        cout << "Q = Quit\n";

        cout << "\nSYMBOLS:\n";
        cout << "P = Player, G = Guard, C = Camera\n";
        cout << "T = Treasure, E = Exit\n";
        cout << "# = Wall, . = Path\n";

        while (!gameOver && movesLeft > 0) {

            displayMap();

            char choice;

            cout << "\nYour move (W/A/S/D/Q): ";
            cin >> choice;

            choice = tolower(choice);

            if (choice == 'q') {
                cout << "\nYou left the mission.\n";
                gameOver = true;
                break;
            }

            if (choice != 'w' && choice != 'a' &&
                choice != 's' && choice != 'd') {

                cout << "Invalid input! Try W, A, S or D.\n";
                continue;
            }

            player.move(choice);

            // If a wall is hit, reverse the move to return to the old position.
            if (isWall(player.getX(), player.getY())) {

                char opposite =
                    choice == 'w' ? 's' :
                    choice == 's' ? 'w' :
                    choice == 'a' ? 'd' : 'a';

                player.move(opposite);

                cout << "A wall blocks your path!\n";
                continue;
            }

            movesLeft--;

            checkTreasure();
            checkExit();

            if (gameOver)
                break;

            guard.move(' ');

            checkDetection();

            if (gameOver)
                break;
        }

        displayMap();

        cout << "\n====================================\n";

        if (won) {
            cout << "MISSION SUCCESS!\n";
            cout << "You collected the treasure and escaped!\n";
        }
        else if (movesLeft == 0 && !won) {
            cout << "MISSION FAILED!\n";
            cout << "You ran out of moves.\n";
        }
        else {
            cout << "GAME OVER!\n";
        }

        cout << "====================================\n";

        // Demonstrates the friend function.
        showTreasureStatus(player);

        // Demonstrates the static member function.
        cout << "Game objects created: "
             << Game::getTotalGames() << '\n';

        saveResult();
    }
};

// Definition of the static data member.
int Game::totalGames = 0;

// ================= MAIN FUNCTION =================

int main() {

    // Exception handling: catches errors and displays the message.
    try {
        Game game;
        game.run();
    }
    catch (const exception& e) {
        cerr << "\nException: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
