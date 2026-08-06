// ============================================================
// LAST DAWN: FALL OF HUMANITY - C++ Terminal Edition
// By Aanand Kumar
// GitHub: github.com/aanand565336-spec/Cpp-Learning
// ============================================================
// A text-based survival horror game with:
// - 9-region open world map
// - Zombie AI (walkers + runners)
// - Inventory system (torch, first aid, bat, key)
// - Combat system
// - Story cutscenes
// ============================================================

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <limits>

using namespace std;

// ===== Forward declarations =====
class Player;
class Zombie;
class World;
class Game;

// ===== Enums =====
enum TileType { WALL, FLOOR, DOOR, DOOR_OPEN, OUTSIDE, BLOOD, RUBBLE, SAFE, FOREST, WATER, SAND, FARM, DIRT, INDUSTRIAL, CITY, BUILDING, LAB, STREET };
enum RegionID { NORTHERN_FOREST, DOWNTOWN, ABANDONED_SUBURBS, RIVER_SIDE, EAST_HARBOR, INDUSTRIAL_ZONE, MILITARY_BASE, UNDERGROUND_LAB, SOUTH_FARMLANDS };
enum GameState { PLAYING, WON, LOST };

// ===== Region struct =====
struct Region {
    string name;
    int cx, cy, r;
    TileType groundType;
};

// ===== Marker struct (map locations) =====
struct Marker {
    int x, y;
    string type;
    string label;
};

// ===== Item struct =====
struct Item {
    int x, y;
    string type;
    string name;
    string emoji;
    bool collected;
};

// ===== Player class =====
class Player {
public:
    int px, py;
    int hp;
    int maxHp;
    bool hasTorch;
    bool torchOn;
    int hasAid;
    bool hasBat;
    bool hasKey;
    int kills;
    int attackPower;
    
    Player(int startX, int startY) : px(startX), py(startY), hp(100), maxHp(100),
        hasTorch(false), torchOn(false), hasAid(0), hasBat(false), hasKey(false),
        kills(0), attackPower(12) {}
    
    void takeDamage(int dmg) {
        hp -= dmg;
        if (hp < 0) hp = 0;
    }
    
    void heal(int amount) {
        if (hasAid > 0 && hp < maxHp) {
            hasAid--;
            hp = min(hp + amount, maxHp);
            cout << "🩹 Healed! HP: " << hp << "/" << maxHp << endl;
        } else if (hasAid == 0) {
            cout << "No first aid kits!" << endl;
        } else {
            cout << "HP already full!" << endl;
        }
    }
    
    void displayStatus() {
        cout << "\n========== STATUS ==========" << endl;
        cout << "HP: " << hp << "/" << maxHp << " [";
        int bars = hp / 10;
        for (int i = 0; i < 10; i++) cout << (i < bars ? "█" : "░");
        cout << "]" << endl;
        cout << "Torch: " << (hasTorch ? (torchOn ? "ON" : "OFF") : "Not found") << endl;
        cout << "First Aid: " << hasAid << " kits" << endl;
        cout << "Weapon: " << (hasBat ? "Baseball Bat (+25 DMG)" : "Fists") << endl;
        cout << "Safe House Key: " << (hasKey ? "YES" : "NO") << endl;
        cout << "Zombies Killed: " << kills << endl;
        cout << "============================" << endl;
    }
    
    int getAttackPower() {
        return hasBat ? 25 : attackPower;
    }
};

// ===== Zombie class =====
class Zombie {
public:
    int x, y;
    int hp;
    int maxHp;
    bool isRunner;
    bool isChasing;
    int attackCooldown;
    
    Zombie(int startX, int startY) : x(startX), y(startY), hp(30 + rand() % 20),
        isRunner(rand() % 100 > 70), isChasing(false), attackCooldown(0) {
        maxHp = hp;
    }
    
    void update(Player& player, World& world);
    
    void display() {
        cout << (isRunner ? "🏃 Runner" : "🧟 Walker") << " HP:" << hp << "/" << maxHp
             << (isChasing ? " [CHASING]" : " [idle]") << endl;
    }
    
    char getSymbol() const {
        return isRunner ? 'R' : 'Z';
    }
};

// ===== World class =====
class World {
public:
    static constexpr int WORLD_W = 90;
    static constexpr int WORLD_H = 60;
    vector<vector<TileType>> tiles;
    vector<Region> regions;
    vector<Marker> markers;
    vector<Item> items;
    
    World() {
        srand(time(0));
        generateWorld();
    }
    
    void generateWorld() {
        // Initialize with outside
        tiles.resize(WORLD_H, vector<TileType>(WORLD_W, OUTSIDE));
        
        // Define regions
        regions = {
            {"NORTHERN FOREST", 24, 12, 16, FOREST},
            {"DOWNTOWN", 60, 15, 15, CITY},
            {"ABANDONED SUBURBS", 15, 32, 14, OUTSIDE},
            {"RIVER SIDE", 40, 32, 13, OUTSIDE},
            {"EAST HARBOR", 80, 26, 12, SAND},
            {"INDUSTRIAL ZONE", 65, 40, 14, INDUSTRIAL},
            {"MILITARY BASE", 17, 51, 12, DIRT},
            {"UNDERGROUND LAB", 42, 49, 9, LAB},
            {"SOUTH FARMLANDS", 68, 52, 15, FARM},
        };
        
        // Apply regions
        for (int y = 0; y < WORLD_H; y++) {
            for (int x = 0; x < WORLD_W; x++) {
                for (const auto& r : regions) {
                    int dist = sqrt((x - r.cx) * (x - r.cx) + (y - r.cy) * (y - r.cy));
                    if (dist < r.r) {
                        tiles[y][x] = r.groundType;
                        break;
                    }
                }
                // Sea on east edge
                if (x > WORLD_W - 6) tiles[y][x] = WATER;
                else if (x > WORLD_W - 9 && tiles[y][x] != WATER) tiles[y][x] = SAND;
            }
        }
        
        // River
        for (int y = 0; y < WORLD_H; y++) {
            int rx = 48 + sin(y * 0.15) * 4;
            for (int dx = -1; dx <= 1; dx++) {
                int x = rx + dx;
                if (x >= 0 && x < WORLD_W) tiles[y][x] = WATER;
            }
        }
        
        // Define markers
        markers = {
            {15, 33, "start", "Player Start"},
            {24, 9, "safehouse", "Safe House"},
            {9, 29, "safehouse", "Safe House"},
            {81, 22, "safehouse", "Safe House"},
            {70, 57, "safehouse", "Safe House"},
            {60, 12, "hospital", "Hospital"},
            {64, 41, "weapon", "Weapon Cache"},
            {26, 19, "vehicle", "Vehicle"},
            {41, 31, "vehicle", "Vehicle"},
            {42, 49, "lab", "Underground Entrance"},
            {12, 37, "story", "Story Mission"},
            {17, 47, "story", "Story Mission"},
            {30, 20, "side", "Side Mission"},
            {38, 35, "enemy", "Enemy Camp"},
            {59, 34, "enemy", "Enemy Camp"},
            {79, 24, "horde", "Zombie Horde"},
        };
        
        // Items
        items = {
            {16, 31, "torch", "Torch", "🔦", false},
            {14, 33, "firstaid", "First Aid Kit", "🩹", false},
            {62, 41, "bat", "Baseball Bat", "🏏", false},
            {79, 24, "key", "Safe House Key", "🔑", false},
            {64, 41, "firstaid", "First Aid Kit", "🩹", false},
        };
    }
    
    bool isSolid(int x, int y) {
        if (x < 0 || x >= WORLD_W || y < 0 || y >= WORLD_H) return true;
        TileType t = tiles[y][x];
        return t == WALL || t == DOOR || t == RUBBLE || t == WATER || t == BUILDING;
    }
    
    string getRegionName(int x, int y) {
        for (const auto& r : regions) {
            int dist = sqrt((x - r.cx) * (x - r.cx) + (y - r.cy) * (y - r.cy));
            if (dist < r.r) return r.name;
        }
        return "WILDERNESS";
    }
    
    void displayMap(int playerX, int playerY) {
        cout << "\n╔══════════════════════════════════════════╗\n";
        cout << "║     LAST DAWN — WORLD MAP                ║\n";
        cout << "╚══════════════════════════════════════════╝\n\n";
        
        // Show nearby area (15x10 around player)
        int viewW = 15, viewH = 10;
        int sx = max(0, playerX - viewW / 2);
        int sy = max(0, playerY - viewH / 2);
        
        for (int y = sy; y < min(WORLD_H, sy + viewH); y++) {
            for (int x = sx; x < min(WORLD_W, sx + viewW); x++) {
                if (x == playerX && y == playerY) {
                    cout << "@";
                } else {
                    char c = getTileChar(x, y);
                    cout << c;
                }
            }
            cout << endl;
        }
        
        cout << "\n@ = You  # = Wall  . = Outside  ~ = Water  T = Tree" << endl;
        cout << "B = Building  R = Road  * = Blood  X = Rubble" << endl;
        cout << "Region: " << getRegionName(playerX, playerY) << endl;
    }
    
    char getTileChar(int x, int y) {
        TileType t = tiles[y][x];
        switch (t) {
            case WALL: return '#';
            case FLOOR: return '.';
            case DOOR: return 'D';
            case DOOR_OPEN: return 'd';
            case OUTSIDE: return '.';
            case BLOOD: return '*';
            case RUBBLE: return 'X';
            case SAFE: return 'S';
            case FOREST: return 'T';
            case WATER: return '~';
            case SAND: return ':';
            case FARM: return 'F';
            case DIRT: return 'D';
            case INDUSTRIAL: return 'I';
            case CITY: return 'C';
            case BUILDING: return 'B';
            case LAB: return 'L';
            case STREET: return 'R';
            default: return '?';
        }
    }
    
    void displayFullMap(int playerX, int playerY) {
        cout << "\n╔══════════════════════════════════════════════════╗\n";
        cout << "║   LAST DAWN: FALL OF HUMANITY — FULL WORLD MAP   ║\n";
        cout << "╚══════════════════════════════════════════════════╝\n";
        cout << "Legend: @=You  S=SafeHouse  H=Hospital  W=Weapon  V=Vehicle\n";
        cout << "        !=Story  ?=Side  X=Enemy  Z=Horde\n\n";
        
        // Print coordinate header
        cout << "   ";
        for (int x = 0; x < WORLD_W; x++) {
            if (x % 10 == 0) cout << (x / 10);
            else cout << " ";
        }
        cout << endl;
        
        for (int y = 0; y < WORLD_H; y++) {
            cout << setw(2) << y << " ";
            for (int x = 0; x < WORLD_W; x++) {
                // Check if player is here
                if (x == playerX && y == playerY) {
                    cout << "@";
                    continue;
                }
                // Check if a marker is here
                bool markerFound = false;
                for (const auto& m : markers) {
                    if (m.x == x && m.y == y) {
                        if (m.type == "start") cout << "S";
                        else if (m.type == "safehouse") cout << "S";
                        else if (m.type == "hospital") cout << "H";
                        else if (m.type == "weapon") cout << "W";
                        else if (m.type == "vehicle") cout << "V";
                        else if (m.type == "lab") cout << "L";
                        else if (m.type == "story") cout << "!";
                        else if (m.type == "side") cout << "?";
                        else if (m.type == "enemy") cout << "X";
                        else if (m.type == "horde") cout << "Z";
                        else cout << getTileChar(x, y);
                        markerFound = true;
                        break;
                    }
                }
                if (!markerFound) cout << getTileChar(x, y);
            }
            cout << endl;
        }
        
        cout << "\nRegions:" << endl;
        for (const auto& r : regions) {
            cout << "  " << r.name << " (" << r.cx << "," << r.cy << ")" << endl;
        }
    }
};

// ===== Zombie AI update =====
void Zombie::update(Player& player, World& world) {
    if (attackCooldown > 0) attackCooldown--;
    
    int dx = player.px - x;
    int dy = player.py - y;
    int dist = sqrt(dx * dx + dy * dy);
    
    int seeRange = player.torchOn ? 8 : 5;
    int speed = isRunner ? 2 : 1;
    
    if (dist < seeRange || isChasing) {
        isChasing = true;
        // Move toward player
        if (abs(dx) > abs(dy)) {
            int nx = x + (dx > 0 ? speed : -speed);
            if (!world.isSolid(nx, y)) x = nx;
        } else {
            int ny = y + (dy > 0 ? speed : -speed);
            if (!world.isSolid(x, ny)) y = ny;
        }
        
        // Attack if close
        if (dist <= 1 && attackCooldown == 0) {
            int dmg = isRunner ? 15 : 10;
            player.takeDamage(dmg);
            attackCooldown = 3;
            cout << "🧟 Zombie attacks you for " << dmg << " damage!" << endl;
        }
    } else {
        // Wander
        if (rand() % 5 == 0) {
            int dir = rand() % 4;
            int nx = x, ny = y;
            switch (dir) {
                case 0: ny--; break;
                case 1: ny++; break;
                case 2: nx--; break;
                case 3: nx++; break;
            }
            if (!world.isSolid(nx, ny)) { x = nx; y = ny; }
        }
    }
}

// ===== Game class =====
class Game {
private:
    World world;
    Player player;
    vector<Zombie> zombies;
    GameState state;
    int turnCount;
    bool objectivesDone[4]; // torch, aid, kill, safehouse
    
public:
    Game() : player(15, 32), state(PLAYING), turnCount(0) {
        fill(objectivesDone, objectivesDone + 4, false);
        spawnZombies();
    }
    
    void spawnZombies() {
        // Enemy camps
        int camps[][3] = {{38, 35, 5}, {59, 34, 5}, {79, 24, 10}};
        for (auto& camp : camps) {
            for (int i = 0; i < camp[2]; i++) {
                int x = camp[0] + (rand() % 6 - 3);
                int y = camp[1] + (rand() % 6 - 3);
                x = max(1, min(World::WORLD_W - 1, x));
                y = max(1, min(World::WORLD_H - 1, y));
                zombies.emplace_back(x, y);
            }
        }
        // Scattered roamers
        for (int i = 0; i < 15; i++) {
            zombies.emplace_back(rand() % World::WORLD_W, rand() % World::WORLD_H);
        }
    }
    
    void showIntro() {
        cout << "\n";
        cout << "╔══════════════════════════════════════════════╗\n";
        cout << "║   LAST DAWN: FALL OF HUMANITY                ║\n";
        cout << "║   C++ Terminal Edition                        ║\n";
        cout << "║   By Aanand Kumar                             ║\n";
        cout << "╚══════════════════════════════════════════════╝\n\n";
        
        cout << "CHAPTER 1 — THE FIRST NIGHT\n\n";
        cout << "2038. Everything was normal.\n";
        cout << "People going to work. Children playing.\n";
        cout << "Then... the hospital happened.\n\n";
        cout << "First, a few people got 'sick.'\n";
        cout << "Then they started biting each other.\n";
        cout << "Within hours, the entire city fell into chaos.\n\n";
        cout << "Aarav, 20 years old, made it home.\n";
        cout << "His little sister was gone.\n";
        cout << "On the table, a note:\n";
        cout << "\"Mujhe military shelter le ja rahe hain.\n";
        cout << " Agar tum zinda ho... mujhe dhoondhna.\"\n\n";
        cout << "Press Enter to continue...";
        cin.ignore();
    }
    
    void showObjectives() {
        cout << "\n========== OBJECTIVES ==========\n";
        cout << "1. " << (objectivesDone[0] ? "[✓]" : "[ ]") << " Find the torch\n";
        cout << "2. " << (objectivesDone[1] ? "[✓]" : "[ ]") << " Find a first aid kit\n";
        cout << "3. " << (objectivesDone[2] ? "[✓]" : "[ ]") << " Kill a zombie\n";
        cout << "4. " << (objectivesDone[3] ? "[✓]" : "[ ]") << " Reach the East Harbor safe house\n";
        cout << "================================\n\n";
    }
    
    void displayNearby() {
        cout << "\n--- Nearby Area ---\n";
        int viewR = 5;
        for (int dy = -viewR; dy <= viewR; dy++) {
            for (int dx = -viewR; dx <= viewR; dx++) {
                int x = player.px + dx;
                int y = player.py + dy;
                if (x == player.px && y == player.py) {
                    cout << "@";
                } else if (x < 0 || x >= World::WORLD_W || y < 0 || y >= World::WORLD_H) {
                    cout << "#";
                } else {
                    // Check for zombie
                    bool zFound = false;
                    for (const auto& z : zombies) {
                        if (z.x == x && z.y == y) { cout << z.getSymbol(); zFound = true; break; }
                    }
                    if (!zFound) {
                        // Check for item
                        bool iFound = false;
                        for (const auto& it : world.items) {
                            if (it.x == x && it.y == y && !it.collected) {
                                cout << "$";
                                iFound = true;
                                break;
                            }
                        }
                        if (!iFound) cout << world.getTileChar(x, y);
                    }
                }
            }
            cout << endl;
        }
        cout << "@ = You  Z = Walker  R = Runner  $ = Item\n";
        cout << "# = Wall  . = Ground  ~ = Water  T = Tree\n";
        cout << "Region: " << world.getRegionName(player.px, player.py) << "\n";
        
        // Show zombies nearby
        int nearbyZ = 0;
        for (const auto& z : zombies) {
            int dist = sqrt((z.x - player.px) * (z.x - player.px) + (z.y - player.py) * (z.y - player.py));
            if (dist <= 5) nearbyZ++;
        }
        cout << "Zombies nearby: " << nearbyZ << "\n";
    }
    
    void showActions() {
        cout << "\n--- Actions ---\n";
        cout << "1. Move (WASD or N/S/E/W)\n";
        cout << "2. Attack nearby zombie\n";
        cout << "3. " << (player.hasTorch ? "Toggle torch" : "(Need torch)") << "\n";
        cout << "4. Use first aid (" << player.hasAid << " kits)\n";
        cout << "5. View map\n";
        cout << "6. View full world map\n";
        cout << "7. Check status\n";
        cout << "8. Pick up / Interact\n";
        cout << "9. Wait (skip turn)\n";
        cout << "0. Quit\n";
        cout << "Choice: ";
    }
    
    void handleMove() {
        cout << "Direction (N/S/E/W or diagonal NE/NW/SE/SW): ";
        string dir;
        cin >> dir;
        transform(dir.begin(), dir.end(), dir.begin(), ::toupper);
        
        int dx = 0, dy = 0;
        if (dir == "N") dy = -1;
        else if (dir == "S") dy = 1;
        else if (dir == "E") dx = 1;
        else if (dir == "W") dx = -1;
        else if (dir == "NE") { dx = 1; dy = -1; }
        else if (dir == "NW") { dx = -1; dy = -1; }
        else if (dir == "SE") { dx = 1; dy = 1; }
        else if (dir == "SW") { dx = -1; dy = 1; }
        else {
            cout << "Invalid direction!\n";
            return;
        }
        
        int nx = player.px + dx;
        int ny = player.py + dy;
        
        if (!world.isSolid(nx, ny)) {
            player.px = nx;
            player.py = ny;
            cout << "Moved to (" << nx << ", " << ny << ") — " << world.getRegionName(nx, ny) << "\n";
            checkItems();
        } else {
            cout << "Can't go there!\n";
        }
    }
    
    void checkItems() {
        for (auto& it : world.items) {
            if (it.x == player.px && it.y == player.py && !it.collected) {
                cout << "📍 You found: " << it.emoji << " " << it.name << "!\n";
                cout << "Press 'P' to pick up (or any key to leave): ";
                char c;
                cin >> c;
                if (c == 'p' || c == 'P') {
                    it.collected = true;
                    if (it.type == "torch") {
                        player.hasTorch = true;
                        player.torchOn = true;
                        objectivesDone[0] = true;
                        cout << "🔦 Got the torch! Now I can see in the dark.\n";
                    } else if (it.type == "firstaid") {
                        player.hasAid++;
                        if (!objectivesDone[1]) objectivesDone[1] = true;
                        cout << "🩹 First aid kit acquired!\n";
                    } else if (it.type == "bat") {
                        player.hasBat = true;
                        cout << "🏏 Baseball bat acquired! +25 attack power!\n";
                    } else if (it.type == "key") {
                        player.hasKey = true;
                        cout << "🔑 Safe house key acquired! East Harbor here I come!\n";
                    }
                    cout << "✓ Objective progress updated!\n";
                    showObjectives();
                }
            }
        }
    }
    
    void handleAttack() {
        bool found = false;
        for (int i = 0; i < (int)zombies.size(); i++) {
            int dist = abs(zombies[i].x - player.px) + abs(zombies[i].y - player.py);
            if (dist <= 1) {
                found = true;
                int dmg = player.getAttackPower();
                zombies[i].hp -= dmg;
                cout << "🗡 You attack the zombie for " << dmg << " damage!\n";
                if (zombies[i].hp <= 0) {
                    cout << "💀 Zombie killed!\n";
                    player.kills++;
                    if (!objectivesDone[2]) {
                        objectivesDone[2] = true;
                        cout << "✓ Objective: Kill a zombie — COMPLETE!\n";
                    }
                    zombies.erase(zombies.begin() + i);
                } else {
                    cout << "Zombie HP: " << zombies[i].hp << "/" << zombies[i].maxHp << "\n";
                }
                break;
            }
        }
        if (!found) cout << "No zombie nearby to attack!\n";
    }
    
    void handleInteract() {
        // Check doors
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                int x = player.px + dx;
                int y = player.py + dy;
                if (x >= 0 && x < World::WORLD_W && y >= 0 && y < World::WORLD_H) {
                    if (world.tiles[y][x] == DOOR) {
                        // Check if it's the safe house
                        if (abs(x - 79) < 4 && abs(y - 24) < 4 && !player.hasKey) {
                            cout << "🔒 Locked. I need the safe house key.\n";
                            return;
                        }
                        world.tiles[y][x] = DOOR_OPEN;
                        cout << "🚪 Door opened!\n";
                        if (abs(x - 79) < 4 && abs(y - 24) < 4) {
                            objectivesDone[3] = true;
                            cout << "✓ You reached the safe house!\n";
                            checkWin();
                        }
                        return;
                    }
                }
            }
        }
        cout << "Nothing to interact with here.\n";
    }
    
    void checkWin() {
        if (objectivesDone[0] && objectivesDone[1] && objectivesDone[2] && objectivesDone[3]) {
            state = WON;
        }
    }
    
    void updateZombies() {
        for (auto& z : zombies) {
            z.update(player, world);
            if (player.hp <= 0) {
                state = LOST;
                return;
            }
        }
    }
    
    void run() {
        showIntro();
        
        while (state == PLAYING) {
            turnCount++;
            cout << "\n===== TURN " << turnCount << " =====\n";
            showObjectives();
            displayNearby();
            player.displayStatus();
            showActions();
            
            int choice;
            cin >> choice;
            
            switch (choice) {
                case 1: handleMove(); break;
                case 2: handleAttack(); break;
                case 3:
                    if (player.hasTorch) {
                        player.torchOn = !player.torchOn;
                        cout << "🔦 Torch " << (player.torchOn ? "ON" : "OFF") << "\n";
                    }
                    break;
                case 4: player.heal(40); break;
                case 5: world.displayMap(player.px, player.py); break;
                case 6: world.displayFullMap(player.px, player.py); break;
                case 7: player.displayStatus(); break;
                case 8: handleInteract(); break;
                case 9: cout << "You wait...\n"; break;
                case 0:
                    cout << "Goodbye, Aarav.\n";
                    return;
                default: cout << "Invalid choice!\n";
            }
            
            // Zombies move after player
            if (state == PLAYING) {
                updateZombies();
                if (player.hp <= 0) state = LOST;
            }
            
            checkWin();
        }
        
        if (state == WON) {
            cout << "\n╔══════════════════════════════════════════════╗\n";
            cout << "║   ✓ YOU SURVIVED!                            ║\n";
            cout << "║   You reached the safe house at East Harbor. ║\n";
            cout << "║   But the search for your sister...          ║\n";
            cout << "║   has just begun.                            ║\n";
            cout << "╚══════════════════════════════════════════════╝\n";
            cout << "Zombies killed: " << player.kills << "\n";
            cout << "Turns survived: " << turnCount << "\n";
        } else if (state == LOST) {
            cout << "\n╔══════════════════════════════════════════════╗\n";
            cout << "║   ✗ YOU DIED                                 ║\n";
            cout << "║   The darkness consumed you...               ║\n";
            cout << "╚══════════════════════════════════════════════╝\n";
            cout << "Zombies killed: " << player.kills << "\n";
            cout << "Turns survived: " << turnCount << "\n";
        }
    }
};

// ===== Main =====
int main() {
    Game game;
    game.run();
    return 0;
}
