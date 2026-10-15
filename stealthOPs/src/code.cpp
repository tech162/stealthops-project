#include <iostream>
#include <vector>    //to store map rows.
#include <string>
#include <cctype>
#include <cstdlib>
using namespace std;

// ======= CHARACTER BASE CLASS ======

class Character {

protected:
    int x, y;  //x is for horizontal postion ,y is for vertical postion.
    char symbol;

public:
    Character(int startX, int startY, char s) //constructor.
        {
            x= startX;
            y= startY;
            symbol= s;
        }

    virtual void move(char direction) = 0;

    int getX() const { return x; }
    int getY() const { return y; }
    char getSymbol() const { return symbol; }

    virtual ~Character() {}  //virtual destructor.
};

// ====== PLAYER CLASS =========

class Player : public Character {

private:
    bool hasTreasure;  //stores treasure status

public:
    Player(int startX, int startY)        //constructor.
        : Character(startX, startY, 'P'),   //calls base class constructor.
          hasTreasure(false) {}
                                         //run time polymorphism.
    void move(char direction) override    //overrides the virtual move() fun in base.
    {  
        direction = tolower(direction);    //converts uppercaseinput to lowercase.

        if (direction == 'w') y--;
        else if (direction == 's') y++;
        else if (direction == 'a') x--;
        else if (direction == 'd') x++;
    }

    void collectTreasure() {
        hasTreasure = true;
    }

    bool ownsTreasure() const {   //const used coz this fun does not modify object.
        return hasTreasure;
    }
};

// ====== GUARD CLASS =======

class Guard : public Character {   //inherits from base class.

private:
    int minX, maxX;     //minX(leftmost),maxX(rightmost).
    int direction;

public:
    Guard(int startX, int startY, int left, int right)
        : Character(startX, startY, 'G'){
            minX = left;
            maxX = right;
            direction = 1;    //guard initially moves right.

        }
          

    void move(char direction) override {
        (void)direction;

        x += this->direction;

        if (x >= maxX) {
            x = maxX;
            this->direction = -1;     //-1 =left.
        }
        else if (x <= minX) {
            x = minX;
            this->direction = 1;   //1=right.
        }
    }
};

// ===== SECURITY CAMERA CLASS =======

class SecurityCamera {

private:
    int x, y;

public:
    SecurityCamera(int cameraX, int cameraY){          //constructor.

        x =cameraX;
        y = cameraY;
    }

    int getX() const { return x; }
    int getY() const { return y; }

    bool detectsPlayer(const Player& player,
                       const vector<string>& map) const {

        if (player.getY() != y || player.getX() == x)
            return false;

        int playerX = player.getX();          //playerX is local variable.

        int step = (playerX > x) ? 1 : -1;     //conditional opr

        for (int col = x + step;
             col != playerX + step;          //help camera to check if wall blocks its view.
             col += step) {

            if (map[y][col] == '#')         //# represents a wall.
                return false;
        }

        return true;
    }
};

// ======= MAIN GAME CLASS ========

class Game {

private:
    vector<string> map;        //vector stores multiple elements, each string=one row.
    Player player;
    Guard guard;
    SecurityCamera camera;

    int movesLeft;
    bool gameOver;
    bool won;

    bool isWall(int x, int y) const {        //checking walls.
        return map[y][x] == '#';
    }

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
             << (player.ownsTreasure() ? "Collected" : "Not collected")
             << "\n";
    }

    void checkTreasure() {
        if (map[player.getY()][player.getX()] == 'T') {
            player.collectTreasure();
            map[player.getY()][player.getX()] = '.';
            cout << "\nTreasure collected!\n";
        }
    }

    void checkExit() {
        if (map[player.getY()][player.getX()] == 'E') {
            if (player.ownsTreasure()) {
                won = true;
                gameOver = true;
            } else {
                cout << "\nExit locked! Collect the treasure first.\n";
            }
        }
    }

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
          won(false) {}

    void run() {
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

            int oldX = player.getX();
            int oldY = player.getY();

            player.move(choice);

            if (isWall(player.getX(), player.getY())) {
                // Restore the previous position.
                // The player moved only one grid cell.
                // Apply the opposite movement to undo it.
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
    }
};

// ================= MAIN FUNCTION =================
int main() {
    Game game;
    game.run();

    return 0;
} 