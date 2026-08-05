/*
 * ============================================
 *   BATTLE ROYALE — GRAPHICS EDITION v2.0
 *   Fortnite-style HUD Layout
 *   Built by Aanand
 * ============================================
 *
 *   Features:
 *   1. Game HUD matching battle royale games
 *   2. Top-left: Kill count, alive count
 *   3. Top-right: ASCII minimap
 *   4. Bottom-left: Player info box
 *   5. Bottom-center: Health & Armor bars
 *   6. Bottom-right: Inventory slots
 *   7. Center: Combat feed & messages
 *   8. ANSI colors throughout
 *   9. 50-player battle royale
 *   10. 12 weapons, 5 rarity tiers
 *   11. Shrinking zone, loot, AI bots
 *
 *   Compile: g++ projects/battle_royale_gfx.cpp -o brgfx
 *   Run: ./brgfx
 * ============================================
 */

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <string>
using namespace std;

// ===== ANSI Colors =====
#define R   "\033[0m"
#define B   "\033[1m"
#define DIM "\033[2m"
#define RED "\033[31m"
#define GRN "\033[32m"
#define YLW "\033[33m"
#define BLU "\033[34m"
#define MAG "\033[35m"
#define CYN "\033[36m"
#define WHT "\033[37m"
#define BRED "\033[41m"
#define BGRN "\033[42m"
#define BYLW "\033[43m"
#define BBLU "\033[44m"
#define BCYN "\033[46m"

// ===== Constants =====
const int TOTAL_PLAYERS = 50;
const int MAP_SIZE = 100;
const int GRID = 15; // Minimap grid size

// ===== Weapon Data =====
struct Weapon {
    string name, icon, color;
    int damage, range, fireRate, ammo, maxAmmo, rarity;
};

const int NUM_WEAPONS = 12;
Weapon weaponDB[NUM_WEAPONS] = {
    {"Pistol",         "🔫", WHT,  15, 10, 2, 12, 12, 1},
    {"SMG",            "💨", GRN,  20, 15, 4, 25, 25, 2},
    {"Shotgun",        "💥", GRN,  35,  5, 1,  6,  6, 2},
    {"Assault Rifle",  "🎯", CYN,  30, 25, 3, 30, 30, 3},
    {"Sniper Rifle",   "🔭", MAG,  60, 50, 1,  5,  5, 4},
    {"LMG",            "⚖️", CYN,  25, 20, 4, 50, 50, 3},
    {"Crossbow",       "🏹", CYN,  40, 30, 1,  3,  3, 3},
    {"Desert Eagle",   "🗡️", MAG,  45, 15, 1,  7,  7, 4},
    {"Minigun",        "⚡", YLW,  35, 15, 6, 80, 80, 5},
    {"Plasma Rifle",   "🌟", YLW,  50, 35, 3, 20, 20, 5},
    {"Rocket Launcher","🚀", YLW,  80, 40, 1,  3,  3, 5},
    {"Energy Sword",   "⚔️", YLW,  70,  5, 2, 999,999,5}
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
struct Zone { int cx, cy, radius, nextRadius, shrinkTimer, damage; };

// ===== Utility =====
int dist(int x1, int y1, int x2, int y2) {
    return abs(x1-x2) + abs(y1-y2);
}
int rngInt(int mn, int mx) { return mn + (rand() % (mx - mn + 1)); }
bool rollChance(int p) { return (rand() % 100) < p; }

// ===== Init =====
void initPlayer(Player &p, string name, bool isBot) {
    p.name = name; p.isAlive = true;
    p.hp = 100; p.maxHp = 100; p.armor = 0; p.maxArmor = 100;
    p.kills = 0; p.damageDealt = 0; p.damageTaken = 0;
    p.x = rngInt(1, MAP_SIZE); p.y = rngInt(1, MAP_SIZE);
    p.inZone = true; p.weapon1 = -1; p.weapon2 = -1; p.activeWeapon = 1;
    p.heals = 1; p.bandages = 2;
    p.isBot = isBot; p.aggression = isBot ? rngInt(0,2) : 0; p.survivalTime = 0;
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
    z.nextRadius = max(5, (int)(z.radius * 0.65));
    z.shrinkTimer = 5; z.damage = 1;
}

// ===== HUD: Top Bar (Kills + Alive) =====
void drawTopBar(Player &p, int aliveCount) {
    cout << CYN << B << "┌" << "─────────────────" << "┐" << R;
    cout << "          ";
    cout << CYN << B << "┌" << "──── MINIMAP ────" << "┐" << R << endl;

    cout << CYN << B << "│" << R;
    cout << " " << RED << B << "☠ KILLS: " << p.kills << R << "     " << CYN << B << "│" << R;
    cout << "          ";
    cout << CYN << B << "│" << R;
    // We'll draw minimap separately
    cout << "                  " << CYN << B << "│" << R << endl;

    cout << CYN << B << "│" << R;
    cout << " " << GRN << B << "● ALIVE: " << aliveCount << "/" << TOTAL_PLAYERS << R;
    if (aliveCount < 10) cout << "  ";
    else if (aliveCount < 100) cout << " ";
    cout << CYN << B << "│" << R;
    cout << "          ";
    cout << CYN << B << "│" << R;
    cout << "                  " << CYN << B << "│" << R << endl;

    cout << CYN << B << "│" << R;
    cout << " " << YLW << B << "⏱ TURN: " << p.survivalTime + 1 << R;
    if (p.survivalTime < 9) cout << "      ";
    else if (p.survivalTime < 99) cout << "     ";
    else cout << "    ";
    cout << CYN << B << "│" << R;
    cout << "          ";
    cout << CYN << B << "│" << R;
    cout << "                  " << CYN << B << "│" << R << endl;

    cout << CYN << B << "└" << "─────────────────" << "┘" << R;
    cout << "          ";
    cout << CYN << B << "└" << "──────────────────" << "┘" << R << endl;
}

// ===== Minimap =====
void drawMinimap(Player &p, Zone &z, Player players[]) {
    int grid[GRID][GRID];
    memset(grid, 0, sizeof(grid));

    // Zone boundaries
    for (int gy = 0; gy < GRID; gy++) {
        for (int gx = 0; gx < GRID; gx++) {
            int rx = (gx+1) * (MAP_SIZE/GRID);
            int ry = (gy+1) * (MAP_SIZE/GRID);
            if (dist(rx, ry, z.cx, z.cy) > z.radius) grid[gy][gx] = 1;
        }
    }

    // Enemies
    for (int i = 1; i < TOTAL_PLAYERS; i++) {
        if (!players[i].isAlive) continue;
        int gx = players[i].x / (MAP_SIZE/GRID);
        int gy = players[i].y / (MAP_SIZE/GRID);
        if (gx >= 0 && gx < GRID && gy >= 0 && gy < GRID) {
            if (grid[gy][gx] == 0) grid[gy][gx] = 2;
        }
    }

    // Player
    int px = p.x / (MAP_SIZE/GRID);
    int py = p.y / (MAP_SIZE/GRID);
    if (px >= GRID) px = GRID-1;
    if (py >= GRID) py = GRID-1;
    grid[py][px] = 4;

    cout << "                     ";
    for (int gy = 0; gy < GRID; gy++) {
        if (gy > 0) cout << endl << "                     ";
        for (int gx = 0; gx < GRID; gx++) {
            switch (grid[gy][gx]) {
                case 0: cout << DIM << GRN << "." << R; break;
                case 1: cout << DIM << RED << ":" << R; break;
                case 2: cout << YLW << "●" << R; break;
                case 4: cout << B << CYN << "★" << R; break;
            }
        }
    }
    cout << endl;
}

// ===== Bottom HUD: Player + Bars + Inventory =====
void drawBottomHUD(Player &p, Zone &z) {
    cout << "\n";
    cout << CYN << B << "┌──────────────────────────────────────────────────────────────┐" << R << endl;

    // Row 1: Player info (left) | Health bar (center) | Inventory (right)
    cout << CYN << B << "│" << R;

    // Player name box (bottom-left)
    cout << " " << B << CYN << "┌─────────┐" << R << " ";
    // Health bar (center)
    int wIdx = (p.activeWeapon == 1) ? p.weapon1 : p.weapon2;
    string hpColor = (p.hp > 60) ? GRN : (p.hp > 30) ? YLW : RED;
    cout << "HP ";
    int hpFill = (p.hp * 20) / 100;
    if (hpFill < 0) hpFill = 0;
    cout << hpColor << B;
    for (int i = 0; i < hpFill; i++) cout << "█";
    for (int i = hpFill; i < 20; i++) cout << "░";
    cout << R << " " << hpColor << p.hp << R << "/100";
    // Inventory (right)
    cout << "   " << CYN << "INV:" << R;
    if (p.weapon1 >= 0) {
        Weapon &w = weaponDB[p.weapon1];
        cout << " " << w.color << B << "[" << (p.activeWeapon==1?">":" ") << "]" << raritySym[w.rarity] << R;
    } else cout << " " << RED << "[ ]" << R;
    if (p.weapon2 >= 0) {
        Weapon &w = weaponDB[p.weapon2];
        cout << " " << w.color << B << "[" << (p.activeWeapon==2?">":" ") << "]" << raritySym[w.rarity] << R;
    } else cout << " " << RED << "[ ]" << R;
    cout << " " << CYN << B << "│" << R << endl;

    // Row 2
    cout << CYN << B << "│" << R;
    cout << " " << B << CYN << "│" << R << B << CYN << p.name.substr(0, 7) << R << B << CYN << " │" << R << " ";
    // Armor bar
    cout << "AR ";
    int arFill = (p.armor * 10) / 100;
    if (arFill < 0) arFill = 0;
    cout << BLU << B;
    for (int i = 0; i < arFill; i++) cout << "█";
    for (int i = arFill; i < 10; i++) cout << "░";
    cout << R << " " << BLU << p.armor << R << "/100";
    // Items (right)
    cout << "   " << CYN << "ITM:" << R;
    cout << " " << GRN << " potions:" << p.heals << R;
    cout << " " << GRN << "band:" << p.bandages << R;
    cout << "  " << CYN << B << "│" << R << endl;

    // Row 3
    cout << CYN << B << "│" << R;
    cout << " " << B << CYN << "└─────────┘" << R << " ";
    // Zone status
    if (p.inZone) cout << " " << GRN << B << "● IN SAFE ZONE" << R;
    else cout << " " << B << RED << "⚠ OUTSIDE ZONE! -" << z.damage << "/turn" << R;
    // Weapon details (right)
    cout << "        ";
    if (wIdx >= 0) {
        Weapon &w = weaponDB[wIdx];
        cout << w.color << B << w.name << R << " DMG:" << w.damage << " RNG:" << w.range;
        if (w.ammo != 999) cout << " [" << w.ammo << "]";
        else cout << " [∞]";
    } else cout << RED << "NO WEAPON!" << R;
    cout << " " << CYN << B << "│" << R << endl;

    cout << CYN << B << "└──────────────────────────────────────────────────────────────┘" << R << endl;
}

// ===== Combat Feed =====
void drawCombatFeed(string &feed) {
    if (!feed.empty()) {
        cout << "\n  " << B << RED << "☠ " << feed << R << "\n" << endl;
        feed = "";
    }
}

// ===== Full Game Screen =====
void drawScreen(Player &p, Zone &z, Player players[], int aliveCount, int turn, string &feed) {
    // Clear screen effect
    cout << "\n\n";

    // Top bar: kills (left) + minimap (right)
    drawTopBar(p, aliveCount);

    // Minimap below top-right
    cout << "\n                     ";
    drawMinimap(p, z, players);

    // Zone info line
    cout << "\n  " << CYN << "Zone: (" << z.cx << "," << z.cy << ") R:" << z.radius
         << " | Dmg: " << z.damage << "/turn | Next shrink: " << max(0, z.shrinkTimer) << " turns" << R << endl;

    // Combat feed
    drawCombatFeed(feed);

    // Actions menu (center)
    int tgtIdx = -1, minD = 999;
    for (int i = 0; i < TOTAL_PLAYERS; i++) {
        if (!players[i].isAlive || &players[i] == &p) continue;
        int d = dist(p.x, p.y, players[i].x, players[i].y);
        if (d < minD) { minD = d; tgtIdx = i; }
    }

    cout << "\n  ";
    if (tgtIdx >= 0) {
        string dColor = (minD <= 10) ? RED : (minD <= 25) ? YLW : GRN;
        cout << dColor << B << "Nearest Enemy: " << players[tgtIdx].name
             << " (" << minD << " units)" << R;
    } else cout << GRN << "No enemies nearby!" << R;
    cout << "\n\n";

    cout << "  " << CYN << "1." << R << " Attack   "
         << CYN << "2." << R << " Move   "
         << CYN << "3." << R << " Loot   "
         << CYN << "4." << R << " Heal   "
         << CYN << "5." << R << " Bandage" << endl;
    cout << "  " << CYN << "6." << R << " Swap Wpn "
         << CYN << "7." << R << " Reload "
         << CYN << "8." << R << " Stats  "
         << CYN << "9." << R << " Hide" << endl;

    // Bottom HUD
    drawBottomHUD(p, z);

    cout << "\n  > ";
}

// ===== Loot =====
void lootArea(Player &p) {
    cout << "\n  " << YLW << B << " LOOTING..." << R << endl;
    int roll = rngInt(1, 100);
    if (roll <= 40) {
        int rarity = rngInt(1, 5);
        int wIdx = 0, attempts = 0;
        while (attempts < 10) {
            wIdx = rngInt(0, NUM_WEAPONS-1);
            if (weaponDB[wIdx].rarity <= rarity) break;
            attempts++;
        }
        Weapon &w = weaponDB[wIdx];
        cout << "  " << w.color << B << raritySym[w.rarity] << " " << w.name << R
             << " (DMG:" << w.damage << " RNG:" << w.range << " FR:" << w.fireRate << ")" << endl;
        if (p.weapon1 == -1) { p.weapon1 = wIdx; cout << "  " << GRN << "Equipped Slot 1!" << R << endl; }
        else if (p.weapon2 == -1) { p.weapon2 = wIdx; cout << "  " << GRN << "Equipped Slot 2!" << R << endl; }
        else {
            cout << "  Both slots full! Replace?" << endl;
            cout << "  1. " << weaponDB[p.weapon1].color << weaponDB[p.weapon1].name << R;
            cout << "  2. " << weaponDB[p.weapon2].color << weaponDB[p.weapon2].name << R;
            cout << "  3. Discard" << endl << "  > ";
            int ch; cin >> ch;
            if (cin.fail()) { cin.clear(); cin.ignore(10000,'\n'); ch = 3; }
            if (ch == 1) { p.weapon1 = wIdx; cout << GRN << "  Replaced!" << R << endl; }
            else if (ch == 2) { p.weapon2 = wIdx; cout << GRN << "  Replaced!" << R << endl; }
            else cout << RED << "  Discarded." << R << endl;
        }
    } else if (roll <= 65) { p.heals++; cout << "  " << GRN << "Health Potion! (x" << p.heals << ")" << R << endl; }
    else if (roll <= 85) { p.bandages++; cout << "  " << GRN << "Bandage! (x" << p.bandages << ")" << R << endl; }
    else { int a = rngInt(15, 40); p.armor = min(p.armor + a, p.maxArmor); cout << "  " << BLU << "Armor +" << a << "! (" << p.armor << "/100)" << R << endl; }
}

// ===== Attack =====
void attack(Player &att, Player &tgt, string &feed, int &alive) {
    int wIdx = (att.activeWeapon == 1) ? att.weapon1 : att.weapon2;
    if (wIdx < 0) {
        if (att.weapon2 >= 0) { att.activeWeapon = 2; wIdx = att.weapon2; }
        else if (att.weapon1 >= 0) { att.activeWeapon = 1; wIdx = att.weapon1; }
        else { if (!att.isBot) cout << RED << "  No weapon!" << R << endl; return; }
    }
    Weapon &w = weaponDB[wIdx];
    int d = dist(att.x, att.y, tgt.x, tgt.y);
    if (d > w.range) { if (!att.isBot) cout << YLW << "  Too far! (" << d << "/" << w.range << ")" << R << endl; return; }
    if (w.ammo <= 0 && w.ammo != 999) { if (!att.isBot) cout << RED << "  No ammo! Reload!" << R << endl; return; }
    if (w.ammo != 999) w.ammo--;

    int shots = w.fireRate;
    if (att.isBot) shots = min(shots, 2);
    int totalDmg = 0;

    for (int s = 0; s < shots; s++) {
        int hitChance = 90 - (d * 30 / w.range);
        if (att.isBot) hitChance -= 20;
        if (hitChance < 30) hitChance = 30;
        if (!rollChance(hitChance)) {
            if (!att.isBot) cout << DIM << "  Miss!" << R << endl;
            continue;
        }
        int dmg = w.damage + rngInt(-3, 3);
        bool headshot = rollChance(15);
        if (headshot) { dmg *= 2; if (!att.isBot) cout << B << RED << "  HEADSHOT! " << R; }
        if (tgt.armor > 0) { int absorbed = min(dmg * 0.5, (double)tgt.armor); tgt.armor -= absorbed; dmg -= absorbed; }
        tgt.hp -= dmg; totalDmg += dmg; att.damageDealt += dmg; tgt.damageTaken += dmg;
        if (!att.isBot) cout << "  " << CYN << att.name << R << " -> " << RED << tgt.name << R
             << " -" << dmg << " (HP:" << max(0,tgt.hp) << ")" << endl;
        if (tgt.hp <= 0) break;
    }

    if (totalDmg > 80) { tgt.hp += totalDmg - 80; totalDmg = 80; }
    if (totalDmg > 0 && att.isBot && !tgt.isBot)
        cout << "  " << RED << att.name << " hit you -" << totalDmg << "!" << R << " (HP:" << max(0,tgt.hp) << ")" << endl;

    if (tgt.hp <= 0) {
        tgt.isAlive = false; att.kills++; alive--;
        feed = att.name + " [" + w.name + "] eliminated " + tgt.name;
        cout << "\n  " << B << RED << "☠ ELIMINATION! " << R << B << feed << R << endl;
        if (!att.isBot) cout << "  " << B << GRN << "KILL #" << att.kills << " !!!" << R << endl;
    }
}

// ===== Zone =====
void shrinkZone(Zone &z, Player players[]) {
    z.radius = z.nextRadius;
    z.nextRadius = max(5, (int)(z.radius * 0.65));
    z.shrinkTimer = 5; z.damage += 2;
    z.cx = max(z.radius, min(MAP_SIZE - z.radius, z.cx + rngInt(-10, 10)));
    z.cy = max(z.radius, min(MAP_SIZE - z.radius, z.cy + rngInt(-10, 10)));
    cout << "\n  " << B << BRED << WHT << " ⚠ ZONE SHRINKING! R:" << z.radius << " Dmg:" << z.damage << "/turn " << R << endl;
    for (int i = 0; i < TOTAL_PLAYERS; i++) {
        if (!players[i].isAlive) continue;
        players[i].inZone = (dist(players[i].x, players[i].y, z.cx, z.cy) <= z.radius);
    }
}

void checkZone(Player &p, Zone &z) {
    p.inZone = (dist(p.x, p.y, z.cx, z.cy) <= z.radius);
    if (!p.inZone && p.isAlive) {
        p.hp -= z.damage;
        if (!p.isBot) cout << "  " << RED << "Zone dmg: -" << z.damage << " (HP:" << max(0,p.hp) << ")" << R << endl;
        if (p.hp <= 0) { p.isAlive = false; cout << "  " << B << RED << p.name << " died to the ZONE!" << R << endl; }
    }
}

// ===== Bot AI =====
void botTurn(Player &bot, Player players[], Zone &z, string &feed, int &alive) {
    if (!bot.isAlive) return;
    if (!bot.inZone) {
        bot.x += (z.cx > bot.x) ? rngInt(5,15) : -rngInt(5,15);
        bot.y += (z.cy > bot.y) ? rngInt(5,15) : -rngInt(5,15);
        bot.x = max(1, min(MAP_SIZE, bot.x)); bot.y = max(1, min(MAP_SIZE, bot.y));
    }
    if (!rollChance(50)) {
        bot.x = max(1, min(MAP_SIZE, bot.x + rngInt(-5,5)));
        bot.y = max(1, min(MAP_SIZE, bot.y + rngInt(-5,5)));
        return;
    }
    int tgtIdx = -1, minD = 999;
    for (int i = 0; i < TOTAL_PLAYERS; i++) {
        if (!players[i].isAlive || &players[i] == &bot) continue;
        int d = dist(bot.x, bot.y, players[i].x, players[i].y);
        if (d < minD) { minD = d; tgtIdx = i; }
    }
    if (tgtIdx < 0) return;
    Player &tgt = players[tgtIdx];
    int d = minD;
    int wIdx = (bot.activeWeapon == 1) ? bot.weapon1 : bot.weapon2;
    if (wIdx < 0) return;
    Weapon &w = weaponDB[wIdx];

    if (bot.aggression == 2) {
        if (d <= w.range && w.ammo > 0) attack(bot, tgt, feed, alive);
        else {
            bot.x += (tgt.x > bot.x) ? 10 : -10;
            bot.y += (tgt.y > bot.y) ? 10 : -10;
            bot.x = max(1, min(MAP_SIZE, bot.x)); bot.y = max(1, min(MAP_SIZE, bot.y));
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
            bot.x = max(1, min(MAP_SIZE, bot.x)); bot.y = max(1, min(MAP_SIZE, bot.y));
        }
    } else {
        if (d <= w.range && rollChance(50) && w.ammo > 0) attack(bot, tgt, feed, alive);
        else {
            bot.x = max(1, min(MAP_SIZE, bot.x + rngInt(-10,10)));
            bot.y = max(1, min(MAP_SIZE, bot.y + rngInt(-10,10)));
            if (rollChance(20)) {
                if (bot.weapon1 < 0) bot.weapon1 = rngInt(0, NUM_WEAPONS-1);
                if (rollChance(30)) bot.armor = min(bot.armor+20, bot.maxArmor);
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
    int tgtIdx = -1, minD = 999;
    for (int i = 0; i < TOTAL_PLAYERS; i++) {
        if (!players[i].isAlive || &players[i] == &p) continue;
        int d = dist(p.x, p.y, players[i].x, players[i].y);
        if (d < minD) { minD = d; tgtIdx = i; }
    }

    int ch; cin >> ch;
    if (cin.fail()) { cin.clear(); cin.ignore(10000,'\n'); cout << RED << "  Skipped!" << R << endl; return; }

    switch (ch) {
        case 1: {
            if (tgtIdx < 0) { cout << YLW << "  No enemies!" << R << endl; break; }
            if (p.weapon1 >= 0 && p.weapon2 >= 0) {
                int d = dist(p.x, p.y, players[tgtIdx].x, players[tgtIdx].y);
                if (d > weaponDB[p.weapon1].range && d <= weaponDB[p.weapon2].range) p.activeWeapon = 2;
                else if (d > weaponDB[p.weapon2].range && d <= weaponDB[p.weapon1].range) p.activeWeapon = 1;
            }
            int wIdx = (p.activeWeapon == 1) ? p.weapon1 : p.weapon2;
            if (wIdx < 0) { cout << RED << "  No weapon! Loot first!" << R << endl; break; }
            cout << CYN << "  Attacking " << players[tgtIdx].name << " with "
                 << weaponDB[wIdx].color << weaponDB[wIdx].name << R << "..." << endl;
            attack(p, players[tgtIdx], feed, alive); break;
        }
        case 2: {
            int nx, ny;
            cout << "  X (1-100): "; cin >> nx;
            cout << "  Y (1-100): "; cin >> ny;
            if (cin.fail()) { cin.clear(); cin.ignore(10000,'\n'); cout << RED << "  Invalid!" << R << endl; break; }
            int moved = dist(p.x, p.y, nx, ny);
            if (moved > 20) { double r = 20.0/moved; nx = p.x+(nx-p.x)*r; ny = p.y+(ny-p.y)*r; cout << YLW << "  Max 20 units!" << R << endl; }
            p.x = max(1, min(MAP_SIZE, nx)); p.y = max(1, min(MAP_SIZE, ny));
            cout << GRN << "  Moved to (" << p.x << "," << p.y << ")" << R << endl;
            if (rollChance(30)) { cout << GRN << "  Found loot!" << R << endl; lootArea(p); }
            break;
        }
        case 3: lootArea(p); break;
        case 4:
            if (p.heals <= 0) { cout << RED << "  No potions!" << R << endl; break; }
            p.heals--; p.hp = min(p.hp+50, p.maxHp);
            cout << GRN << "  +50 HP! (" << p.hp << "/100)" << R << endl; break;
        case 5:
            if (p.bandages <= 0) { cout << RED << "  No bandages!" << R << endl; break; }
            p.bandages--; p.hp = min(p.hp+20, p.maxHp);
            cout << GRN << "  +20 HP! (" << p.hp << "/100)" << R << endl; break;
        case 6: {
            if (p.activeWeapon == 1 && p.weapon2 >= 0) { p.activeWeapon = 2; cout << GRN << "  -> " << weaponDB[p.weapon2].color << weaponDB[p.weapon2].name << R << endl; }
            else if (p.activeWeapon == 2 && p.weapon1 >= 0) { p.activeWeapon = 1; cout << GRN << "  -> " << weaponDB[p.weapon1].color << weaponDB[p.weapon1].name << R << endl; }
            else cout << RED << "  No other weapon!" << R << endl;
            break;
        }
        case 7: {
            int wIdx = (p.activeWeapon == 1) ? p.weapon1 : p.weapon2;
            if (wIdx < 0) { cout << RED << "  No weapon!" << R << endl; break; }
            Weapon &w = weaponDB[wIdx];
            if (w.ammo == w.maxAmmo) { cout << YLW << "  Already full!" << R << endl; break; }
            if (w.ammo == 999) { cout << YLW << "  Infinite ammo!" << R << endl; break; }
            w.ammo = w.maxAmmo; cout << GRN << "  Reloaded! [" << w.ammo << "]" << R << endl; break;
        }
        case 8: {
            cout << "\n  " << CYN << B << "=== STATS ===" << R << endl;
            cout << "  Kills: " << B << p.kills << R << " | Dmg: " << p.damageDealt << endl;
            cout << "  Pos: (" << p.x << "," << p.y << ")" << endl;
            cout << "  W1: ";
            if (p.weapon1 >= 0) { Weapon &w = weaponDB[p.weapon1]; cout << w.color << raritySym[w.rarity] << " " << w.name << R << " [" << w.ammo << "]"; }
            else cout << "Empty";
            cout << endl << "  W2: ";
            if (p.weapon2 >= 0) { Weapon &w = weaponDB[p.weapon2]; cout << w.color << raritySym[w.rarity] << " " << w.name << R << " [" << w.ammo << "]"; }
            else cout << "Empty";
            cout << endl; break;
        }
        case 9: cout << DIM << "  You hide..." << R << endl; break;
        default: cout << RED << "  Skipped!" << R << endl;
    }
}

// ===== Results =====
void showResults(Player &p, int rank) {
    cout << "\n\n";
    if (rank == 1) {
        cout << B << BGRN << WHT << "  ★★★ WINNER WINNER CHICKEN DINNER! ★★★  " << R << endl;
        cout << B << GRN << "  #1 — LAST ONE STANDING!" << R << endl;
    } else if (rank <= 5) {
        cout << B << YLW << "  ★ TOP 5! Great match! ★" << R << endl;
    } else if (rank <= 10) {
        cout << B << CYN << "  Top 10! Good game!" << R << endl;
    } else {
        cout << B << RED << "  You placed #" << rank << R << endl;
    }

    cout << "\n  " << CYN << B << "╔════════ MATCH STATS ════════╗" << R << endl;
    cout << "  " << CYN << B << "║" << R << " Rank:     " << B << "#" << rank << "/" << TOTAL_PLAYERS << R << "          " << CYN << B << "║" << R << endl;
    cout << "  " << CYN << B << "║" << R << " Kills:    " << B << p.kills << R << "               " << CYN << B << "║" << R << endl;
    cout << "  " << CYN << B << "║" << R << " Damage:   " << p.damageDealt << " dealt          " << CYN << B << "║" << R << endl;
    cout << "  " << CYN << B << "║" << R << " Taken:    " << p.damageTaken << "               " << CYN << B << "║" << R << endl;
    cout << "  " << CYN << B << "║" << R << " Survived: " << p.survivalTime << " turns          " << CYN << B << "║" << R << endl;
    cout << "  " << CYN << B << "╚══════════════════════════════╝" << R << endl;

    if (rank == 1) cout << "\n  " << B << GRN << "🎮 You won the Battle Royale! 🏆" << R << endl;
    else cout << "\n  " << CYN << "🎮 Drop in again! 🏆" << R << endl;
}

// ===== Game Loop =====
void gameLoop() {
    string name;
    cout << "\n  " << CYN << "Enter name: " << R;
    cin >> name;

    Player players[TOTAL_PLAYERS];
    initPlayer(players[0], name, false);
    initBots(players + 1, TOTAL_PLAYERS - 1);
    Zone zone; initZone(zone);

    int alive = TOTAL_PLAYERS;
    string feed = "";
    int turn = 1;

    cout << "\n  " << B << CYN << "╔══════════════════════════════════════╗" << R << endl;
    cout << "  " << B << CYN << "║   BATTLE ROYALE — GRAPHICS v2.0     ║" << R << endl;
    cout << "  " << B << CYN << "║      50 PLAYERS. 1 WINNER.          ║" << R << endl;
    cout << "  " << B << CYN << "╚══════════════════════════════════════╝" << R << endl;

    cout << "\n  " << GRN << "Landed at (" << players[0].x << "," << players[0].y << ")" << R << endl;
    lootArea(players[0]);
    if (players[0].weapon1 < 0) { players[0].weapon1 = 0; cout << "  " << WHT << "Got a pistol!" << R << endl; }

    while (players[0].isAlive && alive > 1) {
        drawScreen(players[0], zone, players, alive, turn, feed);

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
    cout << B << CYN << "\n  ╔══════════════════════════════════════╗" << R << endl;
    cout << B << CYN << "  ║   BATTLE ROYALE — GRAPHICS v2.0     ║" << R << endl;
    cout << B << CYN << "  ║        Fortnite-style HUD            ║" << R << endl;
    cout << B << CYN << "  ║         Built by Aanand              ║" << R << endl;
    cout << B << CYN << "  ╚══════════════════════════════════════╝" << R << endl;
    cout << "\n  " << DIM << "50 players. ANSI colors. ASCII minimap. Last one wins." << R << endl;
    cout << "  " << DIM << "Inspired by Free Fire & BGMI!\n" << R << endl;

    while (true) {
        gameLoop();
        cout << "\n  Play again? (1=Yes, 0=No): ";
        int a; cin >> a;
        if (cin.fail() || a == 0) { cout << "\n  " << CYN << "Thanks for playing!" << R << endl; break; }
    }
    return 0;
}
