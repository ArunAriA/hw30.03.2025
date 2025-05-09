#include <iostream>
using namespace std;

class BaseGame {
public:
    virtual void play() = 0;
};

class Game : public BaseGame {
private:
    char map[10][10];
    int playerX, playerY;
    int hp, attack, defense, coins;

    void printMap() {
        for (int y = 0; y < 10; y++) {
            for (int x = 0; x < 10; x++) {
                cout << map[y][x] << ' ';
            }
            cout << endl;
        }
        cout << "HP: " << hp << " | Atc: " << attack << " | Def: " << defense << " | Coins: " << coins << endl;
    }

    void placeItems(char item, int count) {
        while (count > 0) {
            int x = rand() % 10;
            int y = rand() % 10;
            if (map[y][x] == '.' && !(x == playerX && y == playerY)) {
                map[y][x] = item;
                count--;
            }
        }
    }

    void init() {
        srand(time(0));
        hp = 10;
        attack = 1;
        defense = 0;
        coins = 0;
        playerX = 0;
        playerY = 0;

        for (int y = 0; y < 10; y++)
            for (int x = 0; x < 10; x++)
                map[y][x] = '.';

        map[playerY][playerX] = 'P';

        placeItems('*', 5);
        placeItems('E', 5);
        placeItems('+', 3);
        placeItems('D', 3);
        placeItems('A', 3);
    }

    void processCell(char cell) {
        if (cell == 'C') {
            coins++;
            cout << "Подобрана монета!" << endl;
        }
        else if (cell == 'H') {
            hp += 3;
            cout << "Вы исцелились! HP +3" << endl;
        }
        else if (cell == 'D') {
            defense++;
            cout << "Броня экипирована! Def +1" << endl;
        }
        else if (cell == 'A') {
            attack++;
            cout << "Усиление атаки! Atk +1" << endl;
        }
        else if (cell == 'E') {
            int damage = 3 - defense;
            if (damage < 1) damage = 1;
            hp -= damage;
            cout << "Вас атаковали! Урона получено: " << damage << endl;
        }
    }
public:
    void play() override {
        init();

        while (hp > 0) {
            printMap();
            cout << "Ходить (w/a/s/d): " << endl;
            char move;
            cin >> move;

            int newX = playerX;
            int newY = playerY;

            if (move == 'w') newY--;
            else if (move == 's') newY++;
            else if (move == 'a') newX--;
            else if (move == 'd') newX++;
            else continue;

            if (newX < 0 || newX >= 10 || newY < 0 || newY >= 10) {
                cout << "Нельзя выйти за границу карты!" << endl;
                continue;
            }

            char cell = map[newY][newX];
            processCell(cell);

            map[playerY][playerX] = '.';
            playerX = newX;
            playerY = newY;
            map[playerY][playerX] = 'P';
        }

        cout << "Гру завершено. Монети зібрано: " << coins << endl;
    }
};

int main() {
    BaseGame* game = new Game();
    game->play();
    delete game;


    return 0;
}