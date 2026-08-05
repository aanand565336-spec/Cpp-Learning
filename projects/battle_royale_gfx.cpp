/*
 * ============================================
 *   BATTLE ROYALE — GRAPHICS EDITION
 *   Built by Aanand
 * ============================================
 *
 *   Same Battle Royale but with ANSI COLORS,
 *   ASCII MAP, visual bars, and styled UI!
 *
 *   Features:
 *   1. ANSI color codes (red/green/yellow/cyan/magenta)
 *   2. Visual ASCII map showing zone, player & enemies
 *   3. Colored HP/Armor/Zone bars
 *   4. Color-coded kill feed (red = death, yellow = combat)
 *   5. Weapon rarity colors (green=rare, purple=epic, gold=legendary)
 *   6. Box-drawing borders
 *   7. 50-player battle royale gameplay
 *   8. Shrinking zone, loot, weapons, armor
 *
 *   How to compile:
 *   g++ projects/battle_royale_gfx.cpp -o brgfx
 *   ./brgfx
 *
 * ============================================
 */

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cstring>
using namespace std;

// ===== ANSI Colors =====
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BG_RED  "\033[41m"
#define BG_GREEN "\033[42m"
#define BG_YELLOW "\033[43m"
#define BG_BLUE "\033[44m"
#define BG_CYAN "\033[46m"
#define BG_WHITE "\033[47m"

// ===== Constants =====
const int TOTAL_PLAYERS = 50;
const int MAP_SIZE = 100;
const int GRID_SIZE = 20; // ASCII map grid

// ===== Weapon Data =====
struct Weapon {
    string name;
    int damage, range, fireRate, ammo, maxAmmo, rarity;
    string color; // ANSI color
};

const int NUM_WEAPONS = 12;
Weapon weaponDB[NUM_WEAPONS] = {
    {"Pistol",         15, 10, 2, 12, 12, 1, WHITE},
    {"SMG",            20, 15, 4, 25, 25, 2, GREEN},
    {"Shotgun",        35,  5, 1,  6,  6, 2, GREEN},
    {"Assault Rifle",  30, 25, 3, 30, 30, 3, CYAN},
    {"Sniper Rifle",   60, 50, 1,  5,  5, 4, MAGENTA},
    {"LMG",            25, 20, 4, 50, 50, 3, CYAN},
    {"Crossbow",       40, 30, 1,  3,  3, 3, CYAN},
    {"Desert Eagle",   45, 15, 1,  7,  7, 4, MAGENTA},
    {"Minigun",        35, 15, 6, 80, 80, 5, YELLOW},
    {"Plasma Rifle",   50, 35, 3, 20, 20, 5, YELLOW},
    {"Rocket Launcher",80, 40, 1,  3,  3, 5, YELLOW},
    {"Energy Sword",   70,  5, 2, 999,999,5, YELLOW}
};

string rarityNames[] = {"", "Common", "Uncommon", "Rare", "Epic", "Legendary"};
string raritySym[]    = {"", "[C]", "[U]", "[R]", "[E]", "[L]"};

// ===== Player =====
struct Player {
    string name;
    bool isAlive;
    int hp, maxHp, armor, maxArmor;
    int kills, damageDealt, damageTaken;
    int x, y;
    bool inZone;
    int weapon1, weapon2, activeWeapon;
    int heals, bandages;
    bool isBot;
    int aggression;
    int survivalTime;
};

// ===== Zone =====
struct Zone {
    int cx, cy, radius, nextRadius, shrinkTimer, damage;
};

// ===== Utility =====
int dist(int x1, int y1, int x2, int y2) {
    int dx = abs(x1-x2), dy = abs(y1-y2);
    return dx + dy;
}

int rngInt(int mn, int mx) { return mn + (rand() % (mx - mn + 1)); }
bool rollChance(int p) { return (rand() % 100) < p; }

// ===== Colored Bar =====
void drawBar(int cur, int max, int width, string color) {
    int filled = (cur * width) / max;
    if (filled < 0) filled = 0;
    if (filled > width) filled = width;
    cout << color << "[";
    for (int i = 0; i < filled; i++) cout << "█";
    for (int i = filled; i < width; i++) cout << "░";
    cout << "]" << RESET;
}

// ===== Box Border =====
void drawBox(int width) {
    cout << CYAN << "┌";
    for (int i = 0; i < width; i++) cout << "─";
    cout << "┐" << RESET << endl;
}

void drawBoxEnd(int width) {
    cout << CYAN << "└";
    for (int i = 0; i < width; i++) cout << "─";
    cout << "┘" << RESET << endl;
}

void drawBoxMid(int width) {
    cout << CYAN << "│" << RESET;
    for (int i = 0; i < width; i++) cout << " ";
    cout << CYAN << "│" << RESET << endl;
}

// ===== Init Player =====
void initPlayer(Player &p, string name, bool isBot) {
    p.name = name; p.isAlive = true;
    p.hp = 100; p.maxHp = 100;
    p.armor = 0; p.maxArmor = 100;
    p.kills = 0; p.damageDealt = 0; p.damageTaken = 0;
    p.x = rngInt(1, MAP_SIZE); p.y = rngInt(1, MAP_SIZE);
    p.inZone = true;
    p.weapon1 = -1; p.weapon2 = -1; p.activeWeapon = 1;
    p.heals = 1; p.bandages = 2;
    p.isBot = isBot; p.aggression = isBot ? rngInt(0,2) : 0;
    p.survivalTime = 0;
    if (isBot) {
        p.weapon1 = rngInt(0, NUM_WEAPONS-1);
        if (rollChance(30)) p.weapon2 = rngInt(0, NUM_WEAPONS-1);
        if (rollChance(40)) p.armor = rngInt(10, 50);
        p.heals = rngInt(0,2); p.bandages = rngInt(0,3);
    }
}

void initBots(Player bots[], int count) {
    string names[] = {
        "ProKiller","ShadowFox","NoobMaster","DragonSlayer","SnipeKing",
        "RushB","CampMaster","LootGoblin","HeadshotHQ","ToxicGamer",
        "SilentDeath","GhostRecon","FireBreath","IronWolf","VenomStrike",
        "BlazeKing","NightHawk","StormRider","PhantomX","Reaper",
        "Viper","HunterX","ColdBlood","WarMachine","StrikeForce",
        "DarkAngel","RogueOne","AlphaWolf","BetaTester","GammaRay",
        "DeltaForce","Epsilon","ZetaKing","EtaHunter","ThetaStorm",
        "IotaBlade","KappaPro","LambdaX","MuGhost","NuReaper",
        "XiWolf","Omicron","PiLord","RhoStrike","SigmaBoss",
        "TauKiller","Upsilon","PhiMaster","ChiBlade","PsiStorm"
    };
    for (int i = 0; i < count; i++) initPlayer(bots[i], names[i], true);
}

void initZone(Zone &z) {
    z.cx = MAP_SIZE/2; z.cy = MAP_SIZE/2;
    z.radius = MAP_SIZE * 0.7;
    z.nextRadius = z.radius * 0.65;
    z.shrinkTimer = 5; z.damage = 1;
}

// ===== ASCII Map =====
void drawMap(Player &p, Zone &z, Player players[], int aliveCount) {
    cout << "\n" << CYAN << BOLD << "  ╔══════════ BATTLE MAP ══════════╗" << RESET << endl;

    // Map is GRID_SIZE x GRID_SIZE
    // Scale real coordinates to grid
    int grid[GRID_SIZE][GRID_SIZE];
    memset(grid, 0, sizeof(grid));

    // Mark zone boundary
    for (int gy = 0; gy < GRID_SIZE; gy++) {
        for (int gx = 0; gx < GRID_SIZE; gx++) {
            int realX = (gx + 1) * (MAP_SIZE / GRID_SIZE);
            int realY = (gy + 1) * (MAP_SIZE / GRID_SIZE);
            int d = dist(realX, realY, z.cx, z.cy);
            if (d <= z.radius) grid[gy][gx] = 0; // safe zone
            else grid[gy][gx] = 1; // danger zone
        }
    }

    // Mark enemies on grid
    for (int i = 1; i < TOTAL_PLAYERS; i++) {
        if (!players[i].isAlive) continue;
        int gx = players[i].x / (MAP_SIZE / GRID_SIZE);
        int gy = players[i].y / (MAP_SIZE / GRID_SIZE);
        if (gx >= 0 && gx < GRID_SIZE && gy >= 0 && gy < GRID_SIZE) {
            if (grid[gy][gx] == 0) grid[gy][gx] = 2; // enemy in safe zone
            else grid[gy][gx] = 3; // enemy in danger
        }
    }

    // Mark player position
    int px = p.x / (MAP_SIZE / GRID_SIZE);
    int py = p.y / (MAP_SIZE / GRID_SIZE);
    if (px >= GRID_SIZE) px = GRID_SIZE - 1;
    if (py >= GRID_SIZE) py = GRID_SIZE - 1;
    grid[py][px] = 4; // player

    // Draw map
    cout << "  ";
    for (int gy = 0; gy < GRID_SIZE; gy++) {
        for (int gx = 0; gx < GRID_SIZE; gx++) {
            switch (grid[gy][gx]) {
                case 0: cout << GREEN << "·" << RESET; break;  // safe zone
                case 1: cout << RED << "▓" << RESET; break;    // danger
                case 2: cout << YELLOW << "●" << RESET; break; // enemy safe
                case 3: cout << BG_RED << YELLOW << "●" << RESET; break; // enemy danger
                case 4: cout << BOLD << CYAN << "★" << RESET; break;    // player
            }
        }
        if (gy < GRID_SIZE - 1) cout << endl << "  ";
    }
    cout << endl;

    cout << CYAN << BOLD << "  ╚═════════════════════════════════╝" << RESET << endl;

    // Legend
    cout << "  " << BOLD << CYAN << "★" << RESET << " You  "
         << YELLOW << "●" << RESET << " Enemy  "
         << GREEN << "·" << RESET << " Safe  "
         << RED << "▓" << RESET << " Danger Zone" << endl;
    cout << "  Zone: (" << z.cx << "," << z.cy << ") R:" << z.radius
         << " | Dmg: " << z.damage << "/turn  | Alive: " << aliveCount << endl;
}

// ===== Show HUD =====
void showHUD(Player &p, Zone &z, int aliveCount, int turn) {
    cout << "\n" << CYAN << BOLD << "┌─────────────────────────────────────────┐" << RESET << endl;
    cout << CYAN << BOLD << "│" << RESET << "  TURN " << BOLD << turn << RESET
         << "  |  ALIVE: " << BOLD << GREEN << aliveCount << RESET << "/" << TOTAL_PLAYERS
         << "  |  Kills: " << BOLD << p.kills << RESET
         << CYAN << BOLD << "  │" << RESET << endl;
    cout << CYAN << BOLD << "├─────────────────────────────────────────┤" << RESET << endl;

    // HP Bar
    cout << CYAN << BOLD << "│" << RESET << " HP   ";
    string hpColor = (p.hp > 60) ? GREEN : (p.hp > 30) ? YELLOW : RED;
    drawBar(p.hp, p.maxHp, 20, hpColor);
    cout << " " << hpColor << p.hp << RESET << "/100";

    // Armor bar
    cout << "  " << BLUE << "ARM" << RESET;
    drawBar(p.armor, p.maxArmor, 10, BLUE);
    cout << CYAN << BOLD << "  │" << RESET << endl;

    // Weapon info
    cout << CYAN << BOLD << "│" << RESET;
    int wIdx = (p.activeWeapon == 1) ? p.weapon1 : p.weapon2;
    if (wIdx >= 0) {
        Weapon &w = weaponDB[wIdx];
        cout << " " << w.color << BOLD << raritySym[w.rarity] << " " << w.name << RESET;
        cout << " DMG:" << w.damage << " RNG:" << w.range;
        if (w.ammo != 999) cout << " [" << w.ammo << "/" << w.maxAmmo << "]";
        else cout << " [∞]";
    } else {
        cout << " " << RED << "NO WEAPON — LOOT!" << RESET;
    }
    cout << CYAN << BOLD << "       │" << RESET << endl;

    // Zone status
    cout << CYAN << BOLD << "│" << RESET;
    if (p.inZone) cout << "  " << GREEN << "● IN ZONE" << RESET;
    else cout << "  " << BOLD << RED << "⚠ OUTSIDE ZONE — " << z.damage << " dmg/turn!" << RESET;
    cout << "  Potions:" << p.heals << " Bandages:" << p.bandages;
    cout << CYAN << BOLD << "   │" << RESET << endl;

    cout << CYAN << BOLD << "└─────────────────────────────────────────┘" << RESET << endl;
}

// ===== Loot =====
void lootArea(Player &p) {
    cout << "\n" << YELLOW << BOLD << "  🔍 LOOTING AREA..." << RESET << endl;
    int roll = rngInt(1, 100);

    if (roll <= 40) {
        int rarity = rngInt(1, 5);
        int wIdx = 0, attempts = 0;
        while (attempts < 10) {
            wIdx = rngInt(0, NUM_WEAPONS - 1);
            if (weaponDB[wIdx].rarity <= rarity) break;
            attempts++;
        }
        Weapon &w = weaponDB[wIdx];
        cout << "  " << w.color << BOLD << raritySym[w.rarity] << " " << w.name << RESET
             << " (DMG:" << w.damage << " RNG:" << w.range << " FR:" << w.fireRate << ")" << endl;

        if (p.weapon1 == -1) { p.weapon1 = wIdx; cout << "  " << GREEN << "Equipped Slot 1!" << RESET << endl; }
        else if (p.weapon2 == -1) { p.weapon2 = wIdx; cout << "  " << GREEN << "Equipped Slot 2!" << RESET << endl; }
        else {
            cout << "  Both slots full! Replace which?" << endl;
            cout << "  1. " << weaponDB[p.weapon1].color << weaponDB[p.weapon1].name << RESET;
            cout << "  2. " << weaponDB[p.weapon2].color << weaponDB[p.weapon2].name << RESET;
            cout << "  3. Discard" << endl;
            cout << "  Choice: ";
            int ch; cin >> ch;
            if (cin.fail()) { cin.clear(); cin.ignore(10000,'\n'); ch = 3; }
            if (ch == 1) { p.weapon1 = wIdx; cout << GREEN << "  Replaced!" << RESET << endl; }
            else if (ch == 2) { p.weapon2 = wIdx; cout << GREEN << "  Replaced!" << RESET << endl; }
            else cout << RED << "  Discarded." << RESET << endl;
        }
    } else if (roll <= 65) { p.heals++; cout << "  " << GREEN << "Health Potion! (x" << p.heals << ")" << RESET << endl; }
    else if (roll <= 85) { p.bandages++; cout << "  " << GREEN << "Bandage! (x" << p.bandages << ")" << RESET << endl; }
    else {
        int a = rngInt(15, 40); p.armor = min(p.armor + a, p.maxArmor);
        cout << "  " << BLUE << "Armor +" << a << "! (" << p.armor << "/100)" << RESET << endl;
    }
}

// ===== Find Nearest Enemy =====
int findNearestEnemy(Player &p, Player players[]) {
    int nearest = -1, minD = 999;
    for (int i = 0; i < TOTAL_PLAYERS; i++) {
        if (!players[i].isAlive || &players[i] == &p) continue;
        int d = dist(p.x, p.y, players[i].x, players[i].y);
        if (d < minD) { minD = d; nearest = i; }
    }
    return nearest;
}

// ===== Attack =====
void attack(Player &att, Player &tgt, string &feed, int &alive) {
    int wIdx = (att.activeWeapon == 1) ? att.weapon1 : att.weapon2;
    if (wIdx < 0) {
        if (att.weapon2 >= 0) { att.activeWeapon = 2; wIdx = att.weapon2; }
        else if (att.weapon1 >= 0) { att.activeWeapon = 1; wIdx = att.weapon1; }
        else { if (!att.isBot) cout << RED << "  No weapon!" << RESET << endl; return; }
    }
    Weapon &w = weaponDB[wIdx];
    int d = dist(att.x, att.y, tgt.x, tgt.y);

    if (d > w.range) { if (!att.isBot) cout << YELLOW << "  Too far! (" << d << "/" << w.range << ")" << RESET << endl; return; }
    if (w.ammo <= 0 && w.ammo != 999) { if (!att.isBot) cout << RED << "  No ammo!" << RESET << endl; return; }
    if (w.ammo != 999) w.ammo--;

    int shots = w.fireRate;
    if (att.isBot) shots = min(shots, 2);
    int totalDmg = 0;

    for (int s = 0; s < shots; s++) {
        int hitChance = 90 - (d * 30 / w.range);
        if (att.isBot) hitChance -= 20;
        if (hitChance < 30) hitChance = 30;

        if (!rollChance(hitChance)) {
            if (!att.isBot) cout << DIM << "  Miss!" << RESET << endl;
            continue;
        }

        int dmg = w.damage + rngInt(-3, 3);
        bool headshot = rollChance(15);
        if (headshot) { dmg *= 2; if (!att.isBot) cout << BOLD << RED << "  HEADSHOT! " << RESET; }

        if (tgt.armor > 0) {
            int absorbed = min(dmg * 0.5, (double)tgt.armor);
            tgt.armor -= absorbed; dmg -= absorbed;
        }
        tgt.hp -= dmg; totalDmg += dmg;
        att.damageDealt += dmg; tgt.damageTaken += dmg;

        if (!att.isBot) cout << "  " << CYAN << att.name << RESET << " → "
             << RED << tgt.name << RESET << " -" << dmg << " (HP:" << max(0,tgt.hp) << ")" << endl;
        if (tgt.hp <= 0) break;
    }

    if (totalDmg > 80) { tgt.hp += totalDmg - 80; totalDmg = 80; }
    if (totalDmg > 0 && att.isBot && !tgt.isBot)
        cout << "  " << RED << att.name << " hit you for " << totalDmg << "!" << RESET
             << " (HP:" << max(0,tgt.hp) << ")" << endl;

    if (tgt.hp <= 0) {
        tgt.isAlive = false; att.kills++; alive--;
        feed = att.name + " [" + w.name + "] eliminated " + tgt.name;
        cout << "\n  " << BOLD << RED << "☠ ELIMINATION! " << RESET << endl;
        cout << "  " << BOLD << feed << RESET << endl;
        if (!att.isBot) cout << "  " << BOLD << GREEN << "KILL COUNT: " << att.kills << " !!!" << RESET << endl;
    }
}

// ===== Zone Shrink =====
void shrinkZone(Zone &z, Player players[]) {
    z.radius = z.nextRadius;
    z.nextRadius = max(5, (int)(z.radius * 0.65));
    z.shrinkTimer = 5; z.damage += 2;
    z.cx = max(z.radius, min(MAP_SIZE - z.radius, z.cx + rngInt(-10, 10)));
    z.cy = max(z.radius, min(MAP_SIZE - z.radius, z.cy + rngInt(-10, 10)));

    cout << "\n  " << BOLD << BG_RED << WHITE << " ⚠ ZONE SHRINKING! Radius:" << z.radius
         << " Damage:" << z.damage << "/turn " << RESET << endl;

    for (int i = 0; i < TOTAL_PLAYERS; i++) {
        if (!players[i].isAlive) continue;
        players[i].inZone = (dist(players[i].x, players[i].y, z.cx, z.cy) <= z.radius);
    }
}

void checkZone(Player &p, Zone &z) {
    p.inZone = (dist(p.x, p.y, z.cx, z.cy) <= z.radius);
    if (!p.inZone && p.isAlive) {
        p.hp -= z.damage;
        if (!p.isBot) cout << "  " << RED << "Zone damage: -" << z.damage << " (HP:" << max(0,p.hp) << ")" << RESET << endl;
        if (p.hp <= 0) { p.isAlive = false; cout << "  " << BOLD << RED << p.name << " died to the ZONE!" << RESET << endl; }
    }
}

// ===== Bot AI =====
void botTurn(Player &bot, Player players[], Zone &z, string &feed, int &alive) {
    if (!bot.isAlive) return;

    if (!bot.inZone) {
        bot.x += (z.cx > bot.x) ? rngInt(5,15) : -rngInt(5,15);
        bot.y += (z.cy > bot.y) ? rngInt(5,15) : -rngInt(5,15);
        bot.x = max(1, min(MAP_SIZE, bot.x));
        bot.y = max(1, min(MAP_SIZE, bot.y));
    }

    if (!rollChance(50)) { // Bots skip turns
        bot.x = max(1, min(MAP_SIZE, bot.x + rngInt(-5,5)));
        bot.y = max(1, min(MAP_SIZE, bot.y + rngInt(-5,5)));
        return;
    }

    int tgtIdx = findNearestEnemy(bot, players);
    if (tgtIdx < 0) return;
    Player &tgt = players[tgtIdx];
    int d = dist(bot.x, bot.y, tgt.x, tgt.y);
    int wIdx = (bot.activeWeapon == 1) ? bot.weapon1 : bot.weapon2;
    if (wIdx < 0) return;
    Weapon &w = weaponDB[wIdx];

    if (bot.aggression == 2) {
        if (d <= w.range && w.ammo > 0) attack(bot, tgt, feed, alive);
        else {
            bot.x += (tgt.x > bot.x) ? 10 : -10;
            bot.y += (tgt.y > bot.y) ? 10 : -10;
            bot.x = max(1, min(MAP_SIZE, bot.x));
            bot.y = max(1, min(MAP_SIZE, bot.y));
        }
    } else if (bot.aggression == 1) {
        if (bot.hp < 30 && rollChance(50)) {
            bot.x = max(1, min(MAP_SIZE, bot.x + rngInt(-15,15)));
            bot.y = max(1, min(MAP_SIZE, bot.y + rngInt(-15,15)));
            if (bot.heals > 0) { bot.heals--; bot.hp = min(bot.hp+50, bot.maxHp); }
        } else if (d <= w.range && w.ammo > 0) attack(bot, tgt, feed, alive);
        else if (d < 20) {
            bot.x += (tgt.x > bot.x) ? 10 : -10;
            bot.y += (tgt.y > bot.y) ? 10 : -10;
            bot.x = max(1, min(MAP_SIZE, bot.x));
            bot.y = max(1, min(MAP_SIZE, bot.y));
        }
    } else {
        if (d <= w.range && rollChance(50) && w.ammo > 0) attack(bot, tgt, feed, alive);
        else {
            bot.x = max(1, min(MAP_SIZE, bot.x + rngInt(-10,10)));
            bot.y = max(1, min(MAP_SIZE, bot.y + rngInt(-10,10)));
            if (rollChance(20)) {
                if (bot.weapon1 < 0) bot.weapon1 = rngInt(0, NUM_WEAPONS-1);
                if (rollChance(30)) bot.armor = min(bot.armor+20, bot.maxArmor);
                if (rollChance(20)) bot.heals++;
            }
        }
    }

    if (wIdx >= 0 && weaponDB[wIdx].ammo <= 0 && weaponDB[wIdx].ammo != 999) {
        weaponDB[wIdx].ammo = weaponDB[wIdx].maxAmmo;
        if (bot.activeWeapon == 1 && bot.weapon2 >= 0) bot.activeWeapon = 2;
        else if (bot.activeWeapon == 2 && bot.weapon1 >= 0) bot.activeWeapon = 1;
    }
}

// ===== Player Turn =====
void playerTurn(Player &p, Player players[], Zone &z, string &feed, int &alive) {
    int tgtIdx = findNearestEnemy(p, players);

    cout << "\n  " << CYAN << "1." << RESET << " Attack  "
         << CYAN << "2." << RESET << " Move  "
         << CYAN << "3." << RESET << " Loot  "
         << CYAN << "4." << RESET << " Heal  "
         << CYAN << "5." << RESET << " Bandage" << endl;
    cout << "  " << CYAN << "6." << RESET << " Swap   "
         << CYAN << "7." << RESET << " Reload "
         << CYAN << "8." << RESET << " Stats  "
         << CYAN << "9." << RESET << " Hide" << endl;

    if (tgtIdx >= 0) {
        int d = dist(p.x, p.y, players[tgtIdx].x, players[tgtIdx].y);
        string dColor = (d <= 10) ? RED : (d <= 25) ? YELLOW : GREEN;
        cout << "  " << dColor << "Nearest: " << players[tgtIdx].name
             << " (" << d << " units)" << RESET << endl;
    }

    cout << "  > ";
    int ch; cin >> ch;
    if (cin.fail()) { cin.clear(); cin.ignore(10000,'\n'); cout << RED << "  Skipped!" << RESET << endl; return; }

    switch (ch) {
        case 1: {
            if (tgtIdx < 0) { cout << YELLOW << "  No enemies!" << RESET << endl; break; }
            int d = dist(p.x, p.y, players[tgtIdx].x, players[tgtIdx].y);
            if (p.weapon1 >= 0 && p.weapon2 >= 0) {
                if (d > weaponDB[p.weapon1].range && d <= weaponDB[p.weapon2].range) p.activeWeapon = 2;
                else if (d > weaponDB[p.weapon2].range && d <= weaponDB[p.weapon1].range) p.activeWeapon = 1;
            }
            int wIdx = (p.activeWeapon == 1) ? p.weapon1 : p.weapon2;
            if (wIdx < 0) { cout << RED << "  No weapon! Loot first!" << RESET << endl; break; }
            cout << "  " << CYAN << "Attacking " << players[tgtIdx].name << " with "
                 << weaponDB[wIdx].color << weaponDB[wIdx].name << RESET << "..." << endl;
            attack(p, players[tgtIdx], feed, alive);
            break;
        }
        case 2: {
            int nx, ny;
            cout << "  New X (1-100): "; cin >> nx;
            cout << "  New Y (1-100): "; cin >> ny;
            if (cin.fail()) { cin.clear(); cin.ignore(10000,'\n'); cout << RED << "  Invalid!" << RESET << endl; break; }
            int moved = dist(p.x, p.y, nx, ny);
            if (moved > 20) { double r = 20.0/moved; nx = p.x+(nx-p.x)*r; ny = p.y+(ny-p.y)*r; cout << YELLOW << "  Max 20 units!" << RESET << endl; }
            p.x = max(1, min(MAP_SIZE, nx)); p.y = max(1, min(MAP_SIZE, ny));
            cout << GREEN << "  Moved to (" << p.x << "," << p.y << ")" << RESET << endl;
            if (rollChance(30)) { cout << GREEN << "  Found loot!" << RESET << endl; lootArea(p); }
            break;
        }
        case 3: lootArea(p); break;
        case 4:
            if (p.heals <= 0) { cout << RED << "  No potions!" << RESET << endl; break; }
            p.heals--; p.hp = min(p.hp+50, p.maxHp);
            cout << GREEN << "  +" << 50 << " HP! (" << p.hp << "/100)" << RESET << endl; break;
        case 5:
            if (p.bandages <= 0) { cout << RED << "  No bandages!" << RESET << endl; break; }
            p.bandages--; p.hp = min(p.hp+20, p.maxHp);
            cout << GREEN << "  +20 HP! (" << p.hp << "/100)" << RESET << endl; break;
        case 6: {
            if (p.activeWeapon == 1 && p.weapon2 >= 0) { p.activeWeapon = 2; cout << GREEN << "  → " << weaponDB[p.weapon2].color << weaponDB[p.weapon2].name << RESET << endl; }
            else if (p.activeWeapon == 2 && p.weapon1 >= 0) { p.activeWeapon = 1; cout << GREEN << "  → " << weaponDB[p.weapon1].color << weaponDB[p.weapon1].name << RESET << endl; }
            else cout << RED << "  No other weapon!" << RESET << endl;
            break;
        }
        case 7: {
            int wIdx = (p.activeWeapon == 1) ? p.weapon1 : p.weapon2;
            if (wIdx < 0) { cout << RED << "  No weapon!" << RESET << endl; break; }
            Weapon &w = weaponDB[wIdx];
            if (w.ammo == w.maxAmmo) { cout << YELLOW << "  Already full!" << RESET << endl; break; }
            if (w.ammo == 999) { cout << YELLOW << "  Infinite ammo!" << RESET << endl; break; }
            w.ammo = w.maxAmmo; cout << GREEN << "  Reloaded! [" << w.ammo << "/" << w.maxAmmo << "]" << RESET << endl; break;
        }
        case 8: {
            cout << "\n  " << CYAN << BOLD << "═══ STATS ═══" << RESET << endl;
            cout << "  Kills: " << BOLD << p.kills << RESET << " | Dmg: " << p.damageDealt << endl;
            cout << "  Pos: (" << p.x << "," << p.y << ")" << endl;
            cout << "  W1: ";
            if (p.weapon1 >= 0) { Weapon &w = weaponDB[p.weapon1]; cout << w.color << raritySym[w.rarity] << " " << w.name << RESET << " [" << w.ammo << "]"; }
            else cout << "Empty";
            cout << endl << "  W2: ";
            if (p.weapon2 >= 0) { Weapon &w = weaponDB[p.weapon2]; cout << w.color << raritySym[w.rarity] << " " << w.name << RESET << " [" << w.ammo << "]"; }
            else cout << "Empty";
            cout << endl;
            break;
        }
        case 9: cout << DIM << "  You hide..." << RESET << endl; break;
        default: cout << RED << "  Skipped!" << RESET << endl;
    }
}

// ===== Results =====
void showResults(Player &p, int rank) {
    cout << "\n\n";
    if (rank == 1) {
        cout << BOLD << BG_GREEN << WHITE << "  ★★★ WINNER WINNER CHICKEN DINNER! ★★★  " << RESET << endl;
        cout << BOLD << GREEN << "  #1 — LAST ONE STANDING!" << RESET << endl;
    } else if (rank <= 5) {
        cout << BOLD << YELLOW << "  ★ TOP 5! Great match! ★" << RESET << endl;
    } else if (rank <= 10) {
        cout << BOLD << CYAN << "  Top 10! Good game!" << RESET << endl;
    } else {
        cout << BOLD << RED << "  You placed #" << rank << RESET << endl;
    }

    cout << "\n  " << CYAN << BOLD << "╔════════ MATCH STATS ════════╗" << RESET << endl;
    cout << "  " << CYAN << BOLD << "║" << RESET << " Rank:     " << BOLD << "#" << rank << "/" << TOTAL_PLAYERS << RESET << "          " << CYAN << BOLD << "║" << RESET << endl;
    cout << "  " << CYAN << BOLD << "║" << RESET << " Kills:    " << BOLD << p.kills << RESET << "               " << CYAN << BOLD << "║" << RESET << endl;
    cout << "  " << CYAN << BOLD << "║" << RESET << " Damage:   " << p.damageDealt << " dealt          " << CYAN << BOLD << "║" << RESET << endl;
    cout << "  " << CYAN << BOLD << "║" << RESET << " Taken:    " << p.damageTaken << "               " << CYAN << BOLD << "║" << RESET << endl;
    cout << "  " << CYAN << BOLD << "║" << RESET << " Survived: " << p.survivalTime << " turns          " << CYAN << BOLD << "║" << RESET << endl;
    cout << "  " << CYAN << BOLD << "╚══════════════════════════════╝" << RESET << endl;

    if (rank == 1) cout << "\n  " << BOLD << GREEN << "🎮 GG! You won the Battle Royale! 🏆" << RESET << endl;
    else cout << "\n  " << CYAN << "🎮 Drop in again! 🏆" << RESET << endl;
}

// ===== Game Loop =====
void gameLoop() {
    string name;
    cout << "\n  " << CYAN << "Enter name: " << RESET;
    cin >> name;

    Player players[TOTAL_PLAYERS];
    initPlayer(players[0], name, false);
    initBots(players + 1, TOTAL_PLAYERS - 1);

    Zone zone;
    initZone(zone);

    int alive = TOTAL_PLAYERS;
    string feed = "";
    int turn = 1;

    cout << "\n  " << BOLD << CYAN << "╔══════════════════════════════════╗" << RESET << endl;
    cout << "  " << BOLD << CYAN << "║   BATTLE ROYALE — GRAPHICS ED.   ║" << RESET << endl;
    cout << "  " << BOLD << CYAN << "║      50 PLAYERS. 1 WINNER.       ║" << RESET << endl;
    cout << "  " << BOLD << CYAN << "╚══════════════════════════════════╝" << RESET << endl;

    cout << "\n  " << GREEN << "Landed at (" << players[0].x << "," << players[0].y << ")" << RESET << endl;
    lootArea(players[0]);
    if (players[0].weapon1 < 0) { players[0].weapon1 = 0; cout << "  " << WHITE << "Got a pistol!" << RESET << endl; }

    while (players[0].isAlive && alive > 1) {
        showHUD(players[0], zone, alive, turn);
        drawMap(players[0], zone, players, alive);

        if (!feed.empty()) { cout << "  " << BOLD << YELLOW << "☠ " << feed << RESET << endl; feed = ""; }

        zone.shrinkTimer--;
        if (zone.shrinkTimer <= 0) shrinkZone(zone, players);

        for (int i = 0; i < TOTAL_PLAYERS; i++) {
            if (!players[i].isAlive) continue;
            checkZone(players[i], zone);
        }

        alive = 0;
        for (int i = 0; i < TOTAL_PLAYERS; i++) if (players[i].isAlive) alive++;
        if (alive <= 1 || !players[0].isAlive) break;

        playerTurn(players[0], players, zone, feed, alive);

        for (int i = 1; i < TOTAL_PLAYERS; i++) {
            if (!players[i].isAlive) continue;
            botTurn(players[i], players, zone, feed, alive);
        }

        alive = 0;
        for (int i = 0; i < TOTAL_PLAYERS; i++) if (players[i].isAlive) { alive++; players[i].survivalTime++; }

        // Random bot kills
        if (rollChance(60) && alive > 2) {
            for (int a = 0; a < 20; a++) {
                int k = rngInt(1, TOTAL_PLAYERS-1), v = rngInt(1, TOTAL_PLAYERS-1);
                if (k != v && players[k].isAlive && players[v].isAlive) {
                    players[v].isAlive = false; players[k].kills++; alive--;
                    feed = players[k].name + " eliminated " + players[v].name;
                    break;
                }
            }
        }
        turn++;
    }

    int rank = players[0].isAlive ? 1 : 0;
    if (rank == 0) {
        int a = 0;
        for (int i = 0; i < TOTAL_PLAYERS; i++) if (players[i].isAlive) a++;
        rank = a + 1;
    }
    showResults(players[0], rank);
}

// ===== Main =====
int main() {
    srand(time(0));

    cout << BOLD << CYAN << "\n  ╔══════════════════════════════════════╗" << RESET << endl;
    cout << BOLD << CYAN << "  ║   BATTLE ROYALE — GRAPHICS EDITION   ║" << RESET << endl;
    cout << BOLD << CYAN << "  ║         Built by Aanand              ║" << RESET << endl;
    cout << BOLD << CYAN << "  ╚══════════════════════════════════════╝" << RESET << endl;
    cout << "\n  " << DIM << "50 players. Colors. ASCII map. Last one wins." << RESET << endl;
    cout << "  " << DIM << "Inspired by Free Fire & BGMI!\n" << RESET << endl;

    while (true) {
        gameLoop();
        cout << "\n  Play again? (1=Yes, 0=No): ";
        int a; cin >> a;
        if (cin.fail() || a == 0) { cout << "\n  " << CYAN << "Thanks for playing! 🎮" << RESET << endl; break; }
    }
    return 0;
}
