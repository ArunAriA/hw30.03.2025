#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

using namespace std;

const int SIZE = 10;
const int MAX_ENEMIES = 5;

class Player {
public:
    int x, y;
    int hp, attack, defense, coins;
    int crit;

    Player() : x(0), y(0), hp(10), attack(1), defense(0), coins(4), crit(2) {}
};

class Enemy {
public:
    int x, y;
    int hp, attack, defense, crit;

    Enemy(int x_, int y_) : x(x_), y(y_) {
        hp = 5 + rand() % 6;          
        attack = 1 + rand() % 4;      
        defense = rand() % 3;         
        crit = 1 + rand() % 3;        
    }
};

class Game {
private:
    char map[SIZE][SIZE];
    Player player;
    Enemy* enemies[MAX_ENEMIES];
    int enemyCount = 0;
    enum GameLevel { LEVEL_ONE, LEVEL_TWO ,LEVEL_THREE};
    GameLevel currentLevel = LEVEL_ONE;
    int moves = 0;
    bool loaded = false;

public:
    Game() {
        srand(time(0));
    }

    void createMap() {
        for (int y = 0; y < SIZE; y++)
            for (int x = 0; x < SIZE; x++)
                map[y][x] = '.';
    }

    void generateMaze() {
      
        for (int y = 0; y < SIZE; y++)
            for (int x = 0; x < SIZE; x++)
                map[y][x] = '#';

        int x = 0, y = 0;
        map[y][x] = '.'; 

        while (x < SIZE - 1) {
            x++;
            map[y][x] = ' ';
        }
        while (y < SIZE - 1) {
            y++;
            map[y][x] = '.';
        }

        for (int i = 0; i < SIZE * SIZE / 3; i++) {
            int rx = rand() % SIZE;
            int ry = rand() % SIZE;
            if (map[ry][rx] == '#') {
                map[ry][rx] = '.';
            }
        }

        map[0][0] = 'P';

        map[SIZE - 1][SIZE - 1] = 'X';

        spawnItemsLvl2();
    }

    void spawnItemsLvl2() {
        placeItems('C', 3);
        placeItems('H', 1);
        placeItems('D', 1);
        placeItems('A', 1);
        spawnEnemies(3);
    }


    void placePlayer() {
        player = Player();
        map[player.y][player.x] = 'P';
    }

    void spawnEnemies(int count) {
        enemyCount = 0;
        while (enemyCount < count) {
            int x = rand() % SIZE;
            int y = rand() % SIZE;
            if ((map[y][x] == '.' || map[y][x] == ' ') && !(x == player.x && y == player.y)) {
                map[y][x] = 'E';
                enemies[enemyCount++] = new Enemy(x, y);
            }

        }
    }

    void spawnItems() {
        placeItems('C', 5);
        placeItems('H', 3);
        placeItems('D', 2);
        placeItems('A', 3);
        spawnEnemies(5);
    }

    void placeItems(char symbol, int count) {
        int attempts = 0;
        while (count > 0 && attempts < 100) {
            int x = rand() % SIZE;
            int y = rand() % SIZE;
            if (map[y][x] == '.' && !(x == player.x && y == player.y)) {
                map[y][x] = symbol;
                count--;
            }
            attempts++;
        }
    }

    void printMap() {
        for (int y = 0; y < SIZE; y++) {
            for (int x = 0; x < SIZE; x++) {
                cout << map[y][x] << ' ';
            }
            cout << endl;
        }
        cout << "HP: " << player.hp << " | Atk: " << player.attack
            << " | Def: " << player.defense << " | Coins: " << player.coins
            << " | Crit: " << player.crit << endl;
    }

    int getPlayerHP() const {
        return player.hp;
    }


    void movePlayer() {
        char move;
        cout << "Ваш хід (W/A/S/D, M для збереження): ";
        cin >> move;

        if (move == 'm' || move == 'M') {
            saveGame();
            cout << "Гру збережено!" << endl;
            return;
        }

        int newX = player.x;
        int newY = player.y;

        if (move == 'W' || move == 'w') newY--;
        else if (move == 'S' || move == 's') newY++;
        else if (move == 'A' || move == 'a') newX--;
        else if (move == 'D' || move == 'd') newX++;
        else return;

        if (newX < 0 || newX >= SIZE || newY < 0 || newY >= SIZE) {
            cout << "Не можна вийти за межі карти!" << endl;
            return;
        }

        if (map[newY][newX] == '#') {
            cout << "Стіна! Туди не пройти." << endl;
            return;
        }


        map[player.y][player.x] = '.';

        player.x = newX;
        player.y = newY;

        processCell(newX, newY);

        map[player.y][player.x] = 'P';
    }

    void processCell(int x, int y) {
        char cell = map[y][x];
        switch (cell) {
        case 'C':
            player.coins++;
            cout << "Монета зібрана!" << endl;
            break;  

        case 'H':
            player.hp += 3;
            cout << "Здоров'я +3!" << endl;
            break;

        case 'D':
            player.defense++;
            cout << "Броня +1!" << endl;
            break;

        case 'A':
            player.attack++;
            cout << "Атака +1!" << endl;
            break;

        case 'X':
            currentLevel = LEVEL_THREE;
            break;

        case 'E': {
            Enemy* enemy = nullptr;
            int enemyIndex = -1;
            for (int i = 0; i < enemyCount; i++) {
                if (enemies[i] && enemies[i]->x == x && enemies[i]->y == y) {
                    enemy = enemies[i];
                    enemyIndex = i;
                    break;
                }
            }

            if (enemy) {
                cout << "Битва почалась!" << endl;

                int playerScore = (player.hp + player.attack + player.defense + player.crit) / 4;
                int enemyScore = (enemy->hp + enemy->attack + enemy->defense + enemy->crit) / 4;

                cout << "Ваш показник: " << playerScore << ", Показник ворога: " << enemyScore << endl;

                if (playerScore >= enemyScore) {
                    cout << "Ви перемогли ворога!" << endl;
                    enemy->hp = 0;
                    map[y][x] = '.';
                    delete enemies[enemyIndex];
                    enemies[enemyIndex] = nullptr;
                }
                else {
                    cout << "Ворог переміг вас!" << endl;
                    int damage = enemy->attack - player.defense;
                    if (damage < 1) damage = 1;
                    player.hp -= damage;
                    cout << "Ви отримали " << damage << " шкоди!" << endl;

                    if (player.hp <= 0) {
                        cout << "Ваше здоров'я вичерпано!" << endl;
                    }
                    player.x = x;
                    player.y = y;
                }

                if (currentLevel == LEVEL_TWO && map[y][x] == 'X') {
                    cout << "Вітаємо! Ви пройшли другий рівень та всю гру!" << endl;
                    player.hp = 0;
                    return;
                }
            }
            break;
        }
        default:
            break;
        }
    }


    void saveGame() {
        ofstream file("save.txt");
        if (!file) {
            cout << "Помилка збереження!" << endl;
            return;
        }

        file << player.x << " " << player.y << " " << player.hp << " " << player.attack << " "
            << player.defense << " " << player.coins << " " << player.crit << endl;

        file << static_cast<int>(currentLevel) << endl;  

        for (int y = 0; y < SIZE; y++) {
            for (int x = 0; x < SIZE; x++) {
                file << map[y][x];
            }
            file << endl;
        }

        file << enemyCount << endl;
        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i]) {
                file << enemies[i]->x << " " << enemies[i]->y << " " << enemies[i]->hp << " "
                    << enemies[i]->attack << " " << enemies[i]->defense << " " << enemies[i]->crit << endl;
            }
        }

        file.close();
    }


    bool loadGame() {
        ifstream file("save.txt");
        if (!file) {
            cout << "Не найден файл сохранения!" << endl;
            return false;
        }
        
        for (int i = 0; i < enemyCount; i++) {
            if (enemies[i]) {
                delete enemies[i];
                enemies[i] = nullptr;
            }
        }
        enemyCount = 0;

        file >> player.x >> player.y >> player.hp >> player.attack >> player.defense >> player.coins >> player.crit;

        int levelInt;
        file >> levelInt;
        currentLevel = static_cast<GameLevel>(levelInt);

        string line;
        for (int y = 0; y < SIZE; y++) {
            file >> line;
            for (int x = 0; x < SIZE; x++) {
                map[y][x] = line[x];
            }
        }

        int newEnemyCount;
        file >> newEnemyCount;
        enemyCount = newEnemyCount;

        for (int i = 0; i < enemyCount; i++) {
            int x, y, hp, attack, defense, crit;
            file >> x >> y >> hp >> attack >> defense >> crit;
            enemies[i] = new Enemy(x, y);
            enemies[i]->hp = hp;
            enemies[i]->attack = attack;
            enemies[i]->defense = defense;
            enemies[i]->crit = crit;
        }

        for (int y = 0; y < SIZE; y++) {
            for (int x = 0; x < SIZE; x++) {
                if (map[y][x] == 'E') {
                    bool enemyFound = false;
                    for (int i = 0; i < enemyCount; i++) {
                        if (enemies[i] && enemies[i]->x == x && enemies[i]->y == y) {
                            enemyFound = true;
                            break;
                        }
                    }
                    if (!enemyFound) {
                        map[y][x] = '.';
                    }
                }
            }
        }

        for (int y = 0; y < SIZE; y++) {
            for (int x = 0; x < SIZE; x++) {
                if (map[y][x] == 'P' && (x != player.x || y != player.y)) {
                    map[y][x] = '.';
                }
            }
        }
        map[player.y][player.x] = 'P';

        loaded = true;
        return true;
    }



    void play() {
        if (!loaded) {  
            if (currentLevel == LEVEL_ONE) {
                createMap();
                placePlayer();
                spawnItems();
            }
            else if (currentLevel == LEVEL_TWO) {
                generateMaze();
                player.x = 0;
                player.y = 0;
                map[player.y][player.x] = 'P';
            }
        }
        else {
        }

        while (player.hp > 0) {
            printMap();
            movePlayer();
            moves++;

            if (currentLevel == LEVEL_ONE && player.coins >= 5) {
                cout << "Переходим на другий рівень!" << endl;
                currentLevel = LEVEL_TWO;

                for (int i = 0; i < enemyCount; i++) {
                    if (enemies[i]) {
                        delete enemies[i];
                        enemies[i] = nullptr;
                    }
                }
                enemyCount = 0;

                generateMaze();

                player.x = 0;
                player.y = 0;
                map[player.y][player.x] = 'P';
            }

            if (currentLevel == LEVEL_THREE)
            {
                system("cls");
                cout << "Гру завершено! За " << moves << " ходів!!!" << endl;
                break;
            }
        }
    }



};
int main() {
    Game game;

    cout << "Завантажити збереження? (Y/N): ";
    char choice;
    cin >> choice;

    if (choice == 'Y' || choice == 'y') {
        if (!game.loadGame()) {
            cout << "Не вдалося завантажити збереження. Нова гра.\n";
        }
        cout << "save done"<<endl;
    }

    game.play();  

    return 0;
}
