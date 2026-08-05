/*
 * ============================================
 *   BATTLE ROYALE — C++ Survival Game
 *   Built by Aanand
 * ============================================
 *
 *   Inspired by Free Fire & BGMI!
 *   50 players drop into a battlefield.
 *   Loot weapons, survive the shrinking zone,
 *   fight enemies, and be the LAST ONE STANDING!
 *
 *   Features:
 *   1. 50-player battle royale (you + 49 AI bots)
 *   2. Shrinking safe zone (take damage outside)
 *   3. Loot system — 10+ weapons, armor, heals
 *   4. Weapon stats (damage, range, fire rate, ammo)
 *   5. Inventory system (carry 2 weapons + items)
 *   6. Kill feed & live player count
 *   7. Rank system — #1 to #50
 *   8. Bot AI — fight, flee, loot, hide
 *   9. Multiple zones — risk vs reward (better loot in danger)
 *  10. Match stats — kills, damage, survival time
 *
 *   How to compile:
 *   g++ projects/battle_royale.cpp -o br
 *   ./br
 *
 * ============================================
 */

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cstring>
using namespace std;

// ===== Constants =====
const int TOTAL_PLAYERS = 50;
const int MAX_INVENTORY = 4;
const int MAP_SIZE = 100;

// ===== Weapon Data =====
struct Weapon {
    string name;
    int damage;
    int range;
    int fireRate;    // shots per turn
    int ammo;
    int maxAmmo;
    int rarity;      // 1=Common, 2=Uncommon, 3=Rare, 4=Epic, 5=Legendary
};

const int NUM_WEAPONS = 12;
Weapon weaponDB[NUM_WEAPONS] = {
    {"Pistol",        15, 10, 2, 12, 12, 1},
    {"SMG",           20, 15, 4, 25, 25, 2},
    {"Shotgun",       35, 5,  1, 6,  6,  2},
    {"Assault Rifle",  30, 25, 3, 30, 30, 3},
    {"Sniper Rifle",  60, 50, 1, 5,  5,  4},
    {"LMG",           25, 20, 4, 50, 50, 3},
    {"Crossbow",      40, 30, 1, 3,  3,  3},
    {"Desert Eagle",  45, 15, 1, 7,  7,  4},
    {"Minigun",       35, 15, 6, 80, 80, 5},
    {"Plasma Rifle",  50, 35, 3, 20, 20, 5},
    {"Rocket Launcher",80, 40, 1, 3,  3, 5},
    {"Energy Sword",  70, 5,  2, 999, 999, 5}
};

string rarityNames[] = {"", "Common", "Uncommon", "Rare", "Epic", "Legendary"};
string raritySymbols[] = {"", "[C]", "[U]", "[R]", "[E]", "[L]"};

// ===== Item Types =====
enum ItemType { ITEM_NONE, ITEM_HEALTH, ITEM_ARMOR, ITEM_AMMO, ITEM_BANDAGE };

struct Item {
    ItemType type;
    string name;
    int value;
    int count;
};

// ===== Player =====
struct Player {
    string name;
    bool isAlive;
    int hp;
    int maxHp;
    int armor;
    int maxArmor;
    int kills;
    int damageDealt;
    int damageTaken;
    int x, y;          // Position on map
    bool inZone;       // Inside safe zone
    int weapon1;       // Index into weaponDB, -1 if none
    int weapon2;
    int activeWeapon;  // 1 or 2
    int heals;         // Health potions
    int bandages;      // Bandages
    bool isBot;
    int aggression;    // AI behavior: 0=passive, 1=balanced, 2=aggressive
    int survivalTime;  // Turns survived
};

// ===== Zone (Safe Circle) =====
struct Zone {
    int centerX, centerY;
    int radius;
    int nextRadius;
    int shrinkTimer;
    int damage;       // Damage per turn outside zone
};

// ===== Function Declarations =====
void initPlayer(Player &p, string name, bool isBot);
void initBots(Player bots[], int count);
void initZone(Zone &z);
void shrinkZone(Zone &z, Player players[]);
void movePlayer(Player &p);
void lootArea(Player &p);
void showInventory(Player &p);
void showMap(Player &p, Zone &z, Player players[], int aliveCount);
void showKillFeed(string killer, string victim, string weapon);
int findNearestEnemy(Player &p, Player players[]);
void botTurn(Player &bot, Player players[], Zone &z, string &killFeed, int &aliveCount);
void playerTurn(Player &p, Player players[], Zone &z, string &killFeed, int &aliveCount);
void attack(Player &attacker, Player &target, string &killFeed, int &aliveCount);
void useHeal(Player &p);
void useBandage(Player &p);
void swapWeapon(Player &p);
void reloadWeapon(Player &p);
string getRarityColor(int rarity);
void gameLoop();
void showResults(Player &p, int rank, int totalPlayers);
void printStat(string label, int value);

// ===== Utility =====
int distance(int x1, int y1, int x2, int y2) {
    int dx = x1 - x2, dy = y1 - y2;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    return dx + dy;  // Manhattan distance
}

int rngInt(int min, int max) {
    return min + (rand() % (max - min + 1));
}

bool rollChance(int percent) {
    return (rand() % 100) < percent;
}

string getRarityColor(int rarity) {
    return raritySymbols[rarity];
}

// ===== Initialize Player =====
void initPlayer(Player &p, string name, bool isBot) {
    p.name = name;
    p.isAlive = true;
    p.hp = 100;
    p.maxHp = 100;
    p.armor = 0;
    p.maxArmor = 100;
    p.kills = 0;
    p.damageDealt = 0;
    p.damageTaken = 0;
    p.x = rngInt(1, MAP_SIZE);
    p.y = rngInt(1, MAP_SIZE);
    p.inZone = true;
    p.weapon1 = -1;
    p.weapon2 = -1;
    p.activeWeapon = 1;
    p.heals = 1;
    p.bandages = 2;
    p.isBot = isBot;
    p.aggression = isBot ? rngInt(0, 2) : 0;
    p.survivalTime = 0;

    // Bots start with random weapons
    if (isBot) {
        p.weapon1 = rngInt(0, NUM_WEAPONS - 1);
        if (rollChance(30)) p.weapon2 = rngInt(0, NUM_WEAPONS - 1);
        if (rollChance(40)) p.armor = rngInt(10, 50);
        p.heals = rngInt(0, 2);
        p.bandages = rngInt(0, 3);
    }
}

// ===== Initialize Bots =====
void initBots(Player bots[], int count) {
    string botNames[] = {
        "ProKiller", "ShadowFox", "NoobMaster", "DragonSlayer", "SnipeKing",
        "RushB", "CampMaster", "LootGoblin", "HeadshotHQ", "ToxicGamer",
        "SilentDeath", "GhostRecon", "FireBreath", "IronWolf", "VenomStrike",
        "BlazeKing", "NightHawk", "StormRider", "PhantomX", "Reaper",
        "Viper", "HunterX", "ColdBlood", "WarMachine", "StrikeForce",
        "DarkAngel", "RogueOne", "AlphaWolf", "BetaTester", "GammaRay",
        "DeltaForce", "Epsilon", "ZetaKing", "EtaHunter", "ThetaStorm",
        "IotaBlade", "KappaPro", "LambdaX", "MuGhost", "NuReaper",
        "XiWolf", "Omicron", "PiLord", "RhoStrike", "SigmaBoss",
        "TauKiller", "Upsilon", "PhiMaster", "ChiBlade", "PsiStorm"
    };

    for (int i = 0; i < count; i++) {
        initPlayer(bots[i], botNames[i], true);
    }
}

// ===== Initialize Zone =====
void initZone(Zone &z) {
    z.centerX = MAP_SIZE / 2;
    z.centerY = MAP_SIZE / 2;
    z.radius = MAP_SIZE * 0.7;
    z.nextRadius = z.radius * 0.65;
    z.shrinkTimer = 5;  // Shrinks every 5 turns
    z.damage = 1;       // Starts at 1 damage per turn outside
}

// ===== Shrink Zone =====
void shrinkZone(Zone &z, Player players[]) {
    z.radius = z.nextRadius;
    z.nextRadius = z.radius * 0.65;
    if (z.nextRadius < 5) z.nextRadius = 5;
    z.shrinkTimer = 5;
    z.damage += 2;  // Each shrink increases zone damage

    // Move center slightly
    z.centerX += rngInt(-10, 10);
    z.centerY += rngInt(-10, 10);
    if (z.centerX < z.radius) z.centerX = z.radius;
    if (z.centerX > MAP_SIZE - z.radius) z.centerX = MAP_SIZE - z.radius;
    if (z.centerY < z.radius) z.centerY = z.radius;
    if (z.centerY > MAP_SIZE - z.radius) z.centerY = MAP_SIZE - z.radius;

    cout << "\n!!! THE ZONE IS SHRINKING !!!" << endl;
    cout << "Safe zone radius: " << z.radius << " | Damage outside: " << z.damage << "/turn" << endl;

    // Update all players' zone status
    for (int i = 0; i < TOTAL_PLAYERS; i++) {
        if (!players[i].isAlive) continue;
        int d = distance(players[i].x, players[i].y, z.centerX, z.centerY);
        players[i].inZone = (d <= z.radius);
    }
}

// ===== Check if player is in zone =====
void checkZone(Player &p, Zone &z) {
    int d = distance(p.x, p.y, z.centerX, z.centerY);
    p.inZone = (d <= z.radius);
    if (!p.inZone && p.isAlive) {
        p.hp -= z.damage;
        cout << "  " << p.name << " took " << z.damage << " zone damage! (HP:" << p.hp << ")" << endl;
        if (p.hp <= 0) {
            p.isAlive = false;
            cout << "  " << p.name << " died to the ZONE!" << endl;
        }
    }
}

// ===== Loot Area =====
void lootArea(Player &p) {
    cout << "\n--- LOOTING ---" << endl;

    // Random loot: 40% weapon, 25% heal, 20% bandage, 15% armor
    int roll = rngInt(1, 100);

    if (roll <= 40) {
        // Found a weapon
        int rarity = rngInt(1, 5);
        // Higher rarity = rarer weapons
        int attempts = 0;
        int wIdx = 0;
        while (attempts < 10) {
            wIdx = rngInt(0, NUM_WEAPONS - 1);
            if (weaponDB[wIdx].rarity <= rarity) break;
            attempts++;
        }

        Weapon &w = weaponDB[wIdx];
        cout << "Found: " << raritySymbols[w.rarity] << " " << w.name
             << " (DMG:" << w.damage << " RNG:" << w.range
             << " FR:" << w.fireRate << ")" << endl;

        if (p.weapon1 == -1) {
            p.weapon1 = wIdx;
            cout << "Equipped as Weapon 1!" << endl;
        } else if (p.weapon2 == -1) {
            p.weapon2 = wIdx;
            cout << "Equipped as Weapon 2!" << endl;
        } else {
            cout << "Both slots full! Replace:" << endl;
            if (p.activeWeapon == 1) {
                cout << "1. Replace " << weaponDB[p.weapon1].name << " (current)" << endl;
                cout << "2. Replace " << weaponDB[p.weapon2].name << endl;
            } else {
                cout << "1. Replace " << weaponDB[p.weapon1].name << endl;
                cout << "2. Replace " << weaponDB[p.weapon2].name << " (current)" << endl;
            }
            cout << "3. Discard new weapon" << endl;
            cout << "Choice: ";
            int ch; cin >> ch;
            if (cin.fail()) { cin.clear(); cin.ignore(10000, '\n'); ch = 3; }
            if (ch == 1) { p.weapon1 = wIdx; cout << "Replaced! " << w.name << " equipped!" << endl; }
            else if (ch == 2) { p.weapon2 = wIdx; cout << "Replaced! " << w.name << " equipped!" << endl; }
            else { cout << "Discarded " << w.name << endl; }
        }
    } else if (roll <= 65) {
        p.heals++;
        cout << "Found: Health Potion! (Total: " << p.heals << ")" << endl;
    } else if (roll <= 85) {
        p.bandages++;
        cout << "Found: Bandage! (Total: " << p.bandages << ")" << endl;
    } else {
        int armorFound = rngInt(15, 40);
        p.armor = min(p.armor + armorFound, p.maxArmor);
        cout << "Found: Armor +" << armorFound << "! (Armor: " << p.armor << "/" << p.maxArmor << ")" << endl;
    }
}

// ===== Show Inventory =====
void showInventory(Player &p) {
    cout << "\n===== INVENTORY =====" << endl;
    cout << "HP: " << p.hp << "/100 | Armor: " << p.armor << "/100" << endl;
    cout << "Kills: " << p.kills << " | Damage: " << p.damageDealt << endl;
    cout << "Position: (" << p.x << "," << p.y << ")" << endl;
    cout << "---------------------" << endl;

    cout << "Weapon 1: ";
    if (p.weapon1 >= 0) {
        Weapon &w = weaponDB[p.weapon1];
        cout << raritySymbols[w.rarity] << " " << w.name
             << " DMG:" << w.damage << " RNG:" << w.range
             << " AMMO:" << w.ammo << "/" << w.maxAmmo;
        if (p.activeWeapon == 1) cout << " [ACTIVE]";
    } else cout << "Empty";
    cout << endl;

    cout << "Weapon 2: ";
    if (p.weapon2 >= 0) {
        Weapon &w = weaponDB[p.weapon2];
        cout << raritySymbols[w.rarity] << " " << w.name
             << " DMG:" << w.damage << " RNG:" << w.range
             << " AMMO:" << w.ammo << "/" << w.maxAmmo;
        if (p.activeWeapon == 2) cout << " [ACTIVE]";
    } else cout << "Empty";
    cout << endl;

    cout << "Items: Health Potions:" << p.heals << " | Bandages:" << p.bandages << endl;
    cout << "=====================" << endl;
}

// ===== Find Nearest Enemy =====
int findNearestEnemy(Player &p, Player players[]) {
    int nearest = -1;
    int minDist = 999;
    for (int i = 0; i < TOTAL_PLAYERS; i++) {
        if (!players[i].isAlive) continue;
        if (&players[i] == &p) continue;
        if (players[i].isBot && p.isBot) {
            // Bots only target nearby players
        }
        int d = distance(p.x, p.y, players[i].x, players[i].y);
        if (d < minDist) {
            minDist = d;
            nearest = i;
        }
    }
    return nearest;
}

// ===== Attack =====
void attack(Player &attacker, Player &target, string &killFeed, int &aliveCount) {
    if (attacker.activeWeapon == 1 && attacker.weapon1 < 0) {
        if (attacker.weapon2 >= 0) { attacker.activeWeapon = 2; }
        else {
            cout << "  No weapon equipped!" << endl;
            return;
        }
    }
    if (attacker.activeWeapon == 2 && attacker.weapon2 < 0) {
        if (attacker.weapon1 >= 0) { attacker.activeWeapon = 1; }
        else {
            cout << "  No weapon equipped!" << endl;
            return;
        }
    }

    int wIdx = (attacker.activeWeapon == 1) ? attacker.weapon1 : attacker.weapon2;
    Weapon &w = weaponDB[wIdx];
    int dist = distance(attacker.x, attacker.y, target.x, target.y);

    if (dist > w.range) {
        if (!attacker.isBot)
            cout << "  Target too far! (" << dist << "/" << w.range << " range)" << endl;
        return;
    }

    if (w.ammo <= 0 && w.ammo != 999) {
        if (!attacker.isBot)
            cout << "  No ammo! Reload first!" << endl;
        return;
    }

    // Consume ammo
    if (w.ammo != 999) w.ammo--;

    // Calculate damage
    int shots = w.fireRate;
    if (attacker.isBot) shots = min(shots, 2);  // Bots fire fewer shots
    int totalDamage = 0;

    for (int s = 0; s < shots; s++) {
        // Hit chance based on distance
        int hitChance = 90 - (dist * 30 / w.range);
        if (attacker.isBot) hitChance -= 20;  // Bots are less accurate
        if (hitChance < 30) hitChance = 30;

        if (!rollChance(hitChance)) {
            if (!attacker.isBot) cout << "  Shot missed!" << endl;
            continue;
        }

        int dmg = w.damage + rngInt(-3, 3);

        // Headshot chance
        bool headshot = rollChance(15);
        if (headshot) {
            dmg *= 2;
            if (!attacker.isBot) cout << "  HEADSHOT! ";
        }

        // Armor absorbs damage
        if (target.armor > 0) {
            int absorbed = dmg * 0.5;
            if (absorbed > target.armor) absorbed = target.armor;
            target.armor -= absorbed;
            dmg -= absorbed;
        }

        target.hp -= dmg;
        totalDamage += dmg;
        attacker.damageDealt += dmg;
        target.damageTaken += dmg;

        if (!attacker.isBot) {
            cout << "  " << attacker.name << " hit " << target.name
                 << " for " << dmg << " damage!"
                 << " (Target HP: " << (target.hp > 0 ? target.hp : 0) << ")" << endl;
        }

        if (target.hp <= 0) break;
    }

    // Cap damage per attack turn
    if (totalDamage > 80) {
        int overflow = totalDamage - 80;
        target.hp += overflow;
        target.armor += overflow / 2;
        if (target.armor > target.maxArmor) target.armor = target.maxArmor;
        totalDamage = 80;
        attacker.damageDealt -= overflow;
        target.damageTaken -= overflow;
    }

    if (totalDamage > 0 && attacker.isBot && !target.isBot) {
        cout << "  " << attacker.name << " shot you for " << totalDamage << " damage!"
             << " (Your HP: " << (target.hp > 0 ? target.hp : 0) << ")" << endl;
    }

    // Check kill
    if (target.hp <= 0) {
        target.isAlive = false;
        attacker.kills++;
        aliveCount--;
        killFeed = attacker.name + " [" + w.name + "] eliminated " + target.name;

        if (!attacker.isBot || !target.isBot) {
            cout << "\n  *** ELIMINATION! ***" << endl;
            cout << "  " << killFeed << endl;
            if (!attacker.isBot) cout << "  KILL COUNT: " << attacker.kills << " !!!" << endl;
        }
    }
}

// ===== Use Heal =====
void useHeal(Player &p) {
    if (p.heals <= 0) { cout << "  No health potions!" << endl; return; }
    p.heals--;
    int heal = 50;
    p.hp = min(p.hp + heal, p.maxHp);
    cout << "  Used Health Potion! +" << heal << " HP (HP:" << p.hp << "/100)" << endl;
}

void useBandage(Player &p) {
    if (p.bandages <= 0) { cout << "  No bandages!" << endl; return; }
    p.bandages--;
    int heal = 20;
    p.hp = min(p.hp + heal, p.maxHp);
    cout << "  Used Bandage! +" << heal << " HP (HP:" << p.hp << "/100)" << endl;
}

// ===== Swap Weapon =====
void swapWeapon(Player &p) {
    if (p.activeWeapon == 1 && p.weapon2 >= 0) {
        p.activeWeapon = 2;
        cout << "  Switched to: " << weaponDB[p.weapon2].name << endl;
    } else if (p.activeWeapon == 2 && p.weapon1 >= 0) {
        p.activeWeapon = 1;
        cout << "  Switched to: " << weaponDB[p.weapon1].name << endl;
    } else {
        cout << "  No other weapon!" << endl;
    }
}

// ===== Reload Weapon =====
void reloadWeapon(Player &p) {
    int wIdx = (p.activeWeapon == 1) ? p.weapon1 : p.weapon2;
    if (wIdx < 0) { cout << "  No weapon to reload!" << endl; return; }
    Weapon &w = weaponDB[wIdx];
    if (w.ammo == w.maxAmmo) { cout << "  Already full ammo!" << endl; return; }
    if (w.ammo == 999) { cout << "  Infinite ammo weapon!" << endl; return; }
    w.ammo = w.maxAmmo;
    cout << "  Reloaded " << w.name << "! (" << w.ammo << "/" << w.maxAmmo << ")" << endl;
}

// ===== Show Map =====
void showMap(Player &p, Zone &z, Player players[], int aliveCount) {
    cout << "\n===== MAP (You at " << p.x << "," << p.y << ") =====" << endl;
    cout << "Zone center: (" << z.centerX << "," << z.centerY << ") Radius:" << z.radius << endl;
    cout << "Your distance from center: " << distance(p.x, p.y, z.centerX, z.centerY);
    if (p.inZone) cout << " [IN ZONE]";
    else cout << " [OUTSIDE ZONE! Take " << z.damage << " dmg/turn!]";
    cout << endl;

    // Show nearby enemies
    int nearby = 0;
    for (int i = 0; i < TOTAL_PLAYERS; i++) {
        if (!players[i].isAlive) continue;
        if (&players[i] == &p) continue;
        int d = distance(p.x, p.y, players[i].x, players[i].y);
        if (d <= 25) {
            nearby++;
            if (d <= 5) cout << "  RED ZONE: " << players[i].name << " (" << d << " units)" << endl;
            else if (d <= 15) cout << "  NEAR: " << players[i].name << " (" << d << " units)" << endl;
            else cout << "  DETECTED: " << players[i].name << " (" << d << " units)" << endl;
        }
    }
    if (nearby == 0) cout << "  No enemies nearby. Safe for now." << endl;

    cout << "Alive: " << aliveCount << "/" << TOTAL_PLAYERS << endl;
    cout << "========================" << endl;
}

// ===== Bot AI Turn =====
void botTurn(Player &bot, Player players[], Zone &z, string &killFeed, int &aliveCount) {
    if (!bot.isAlive) return;

    // Move toward zone center if outside
    if (!bot.inZone) {
        int dx = z.centerX - bot.x;
        int dy = z.centerY - bot.y;
        if (dx > 0) bot.x += rngInt(5, 15);
        else if (dx < 0) bot.x -= rngInt(5, 15);
        if (dy > 0) bot.y += rngInt(5, 15);
        else if (dy < 0) bot.y -= rngInt(5, 15);
        bot.x = max(1, min(MAP_SIZE, bot.x));
        bot.y = max(1, min(MAP_SIZE, bot.y));
    }

    // Find nearest enemy
    int targetIdx = findNearestEnemy(bot, players);
    if (targetIdx < 0) return;

    Player &target = players[targetIdx];
    int dist = distance(bot.x, bot.y, target.x, target.y);

    // Get bot's weapon range
    int wIdx = (bot.activeWeapon == 1) ? bot.weapon1 : bot.weapon2;
    if (wIdx < 0) {
        // No weapon — move to loot
        bot.x += rngInt(-10, 10);
        bot.y += rngInt(-10, 10);
        bot.x = max(1, min(MAP_SIZE, bot.x));
        bot.y = max(1, min(MAP_SIZE, bot.y));
        return;
    }
    Weapon &w = weaponDB[wIdx];

    // AI behavior — bots don't attack every turn
    if (!rollChance(50)) {
        // Bot does nothing this turn (repositioning)
        bot.x += rngInt(-5, 5);
        bot.y += rngInt(-5, 5);
        bot.x = max(1, min(MAP_SIZE, bot.x));
        bot.y = max(1, min(MAP_SIZE, bot.y));
        return;
    }

    if (bot.aggression == 2) {
        // Aggressive: always attack if in range, else move closer
        if (dist <= w.range && w.ammo > 0) {
            attack(bot, target, killFeed, aliveCount);
        } else {
            // Move closer
            int dx = target.x - bot.x;
            int dy = target.y - bot.y;
            int step = min(15, dist);
            if (dx > 0) bot.x += step/2;
            else if (dx < 0) bot.x -= step/2;
            if (dy > 0) bot.y += step/2;
            else if (dy < 0) bot.y -= step/2;
            bot.x = max(1, min(MAP_SIZE, bot.x));
            bot.y = max(1, min(MAP_SIZE, bot.y));
        }
    } else if (bot.aggression == 1) {
        // Balanced: attack if close, flee if low HP
        if (bot.hp < 30 && rollChance(50)) {
            // Flee
            bot.x += rngInt(-15, 15);
            bot.y += rngInt(-15, 15);
            bot.x = max(1, min(MAP_SIZE, bot.x));
            bot.y = max(1, min(MAP_SIZE, bot.y));
            // Heal if available
            if (bot.heals > 0) { bot.heals--; bot.hp = min(bot.hp + 50, bot.maxHp); }
        } else if (dist <= w.range && w.ammo > 0) {
            attack(bot, target, killFeed, aliveCount);
        } else if (dist < 20) {
            // Move closer
            int dx = target.x - bot.x;
            int dy = target.y - bot.y;
            if (dx > 0) bot.x += 10;
            else if (dx < 0) bot.x -= 10;
            if (dy > 0) bot.y += 10;
            else if (dy < 0) bot.y -= 10;
            bot.x = max(1, min(MAP_SIZE, bot.x));
            bot.y = max(1, min(MAP_SIZE, bot.y));
        }
    } else {
        // Passive: only attack if very close, otherwise loot
        if (dist <= w.range && rollChance(50) && w.ammo > 0) {
            attack(bot, target, killFeed, aliveCount);
        } else {
            // Wander and loot
            bot.x += rngInt(-10, 10);
            bot.y += rngInt(-10, 10);
            bot.x = max(1, min(MAP_SIZE, bot.x));
            bot.y = max(1, min(MAP_SIZE, bot.y));
            // Random loot
            if (rollChance(20)) {
                if (bot.weapon1 < 0) bot.weapon1 = rngInt(0, NUM_WEAPONS - 1);
                else if (bot.weapon2 < 0 && rollChance(30)) bot.weapon2 = rngInt(0, NUM_WEAPONS - 1);
                if (rollChance(30)) bot.armor = min(bot.armor + 20, bot.maxArmor);
                if (rollChance(20)) bot.heals++;
            }
        }
    }

    // Reload if out of ammo
    if (wIdx >= 0 && weaponDB[wIdx].ammo <= 0 && weaponDB[wIdx].ammo != 999) {
        weaponDB[wIdx].ammo = weaponDB[wIdx].maxAmmo;
    }

    // Swap weapons if no ammo
    if (wIdx >= 0 && weaponDB[wIdx].ammo <= 0 && weaponDB[wIdx].ammo != 999) {
        if (bot.activeWeapon == 1 && bot.weapon2 >= 0) bot.activeWeapon = 2;
        else if (bot.activeWeapon == 2 && bot.weapon1 >= 0) bot.activeWeapon = 1;
    }
}

// ===== Player Turn =====
void playerTurn(Player &p, Player players[], Zone &z, string &killFeed, int &aliveCount) {
    cout << "\n===== YOUR TURN =====" << endl;
    showMap(p, z, players, aliveCount);

    cout << "\nActions:" << endl;
    cout << "1. Attack nearest enemy" << endl;
    cout << "2. Move (enter new coordinates)" << endl;
    cout << "3. Loot this area" << endl;
    cout << "4. Use Health Potion (have " << p.heals << ")" << endl;
    cout << "5. Use Bandage (have " << p.bandages << ")" << endl;
    cout << "6. Swap weapon" << endl;
    cout << "7. Reload weapon" << endl;
    cout << "8. Show inventory" << endl;
    cout << "9. Hide (skip turn, stay quiet)" << endl;
    cout << "--------------------" << endl;
    cout << "Action: ";

    int choice;
    cin >> choice;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid! Turn skipped!" << endl;
        return;
    }

    switch (choice) {
        case 1: {
            int targetIdx = findNearestEnemy(p, players);
            if (targetIdx < 0) {
                cout << "  No enemies found!" << endl;
                return;
            }
            Player &target = players[targetIdx];
            int dist = distance(p.x, p.y, target.x, target.y);

            // Auto-select weapon with best range
            if (p.weapon1 >= 0 && p.weapon2 >= 0) {
                int r1 = weaponDB[p.weapon1].range;
                int r2 = weaponDB[p.weapon2].range;
                if (dist > r1 && dist <= r2) p.activeWeapon = 2;
                else if (dist > r2 && dist <= r1) p.activeWeapon = 1;
            }

            int wIdx = (p.activeWeapon == 1) ? p.weapon1 : p.weapon2;
            if (wIdx < 0) { cout << "  No weapon! Loot first!" << endl; return; }

            cout << "  Attacking " << target.name << " (dist:" << dist
                 << ") with " << weaponDB[wIdx].name << "..." << endl;
            attack(p, target, killFeed, aliveCount);
            break;
        }
        case 2: {
            int newX, newY;
            cout << "  Current pos: (" << p.x << "," << p.y << ")" << endl;
            cout << "  Enter new X (1-100): ";
            cin >> newX;
            cout << "  Enter new Y (1-100): ";
            cin >> newY;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "  Invalid! Staying put." << endl;
                return;
            }
            int maxMove = 20;
            int moved = distance(p.x, p.y, newX, newY);
            if (moved > maxMove) {
                // Move as far as possible
                double ratio = (double)maxMove / moved;
                newX = p.x + (newX - p.x) * ratio;
                newY = p.y + (newY - p.y) * ratio;
                cout << "  Can only move " << maxMove << " units!" << endl;
            }
            p.x = max(1, min(MAP_SIZE, newX));
            p.y = max(1, min(MAP_SIZE, newY));
            cout << "  Moved to (" << p.x << "," << p.y << ")" << endl;

            // Random chance to find loot by moving
            if (rollChance(30)) {
                cout << "  You found something while moving!" << endl;
                lootArea(p);
            }
            break;
        }
        case 3:
            lootArea(p);
            break;
        case 4:
            useHeal(p);
            break;
        case 5:
            useBandage(p);
            break;
        case 6:
            swapWeapon(p);
            break;
        case 7:
            reloadWeapon(p);
            break;
        case 8:
            showInventory(p);
            break;
        case 9:
            cout << "  You hide and wait..." << endl;
            // Small chance to avoid detection
            break;
        default:
            cout << "Invalid! Turn skipped!" << endl;
    }
}

// ===== Show Results =====
void showResults(Player &p, int rank, int totalPlayers) {
    cout << "\n\n===========================================" << endl;
    if (rank == 1) {
        cout << "  *** WINNER WINNER CHICKEN DINNER! ***" << endl;
        cout << "  *** #1 — YOU ARE THE LAST ONE STANDING! ***" << endl;
    } else if (rank <= 5) {
        cout << "  TOP 5! Great match!" << endl;
    } else if (rank <= 10) {
        cout << "  TOP 10! Good game!" << endl;
    } else {
        cout << "  You placed #" << rank << endl;
    }
    cout << "===========================================" << endl;
    cout << "\n----- MATCH STATS -----" << endl;
    cout << "  Rank:       #" << rank << "/" << totalPlayers << endl;
    cout << "  Kills:      " << p.kills << endl;
    cout << "  Damage:     " << p.damageDealt << " dealt" << endl;
    cout << "  Damage Taken:" << p.damageTaken << endl;
    cout << "  Survived:   " << p.survivalTime << " turns" << endl;
    cout << "  Weapon 1:   ";
    if (p.weapon1 >= 0) cout << weaponDB[p.weapon1].name;
    else cout << "None";
    cout << endl;
    cout << "  Weapon 2:   ";
    if (p.weapon2 >= 0) cout << weaponDB[p.weapon2].name;
    else cout << "None";
    cout << endl;
    cout << "===========================================\n" << endl;

    if (rank == 1) cout << "  GG! You survived the Battle Royale! 🏆" << endl;
    else cout << "  GG! Drop in again and try for #1! 🎮" << endl;
}

// ===== Game Loop =====
void gameLoop() {
    string playerName;
    cout << "\nEnter your name: ";
    cin >> playerName;

    // Create all players
    Player players[TOTAL_PLAYERS];
    initPlayer(players[0], playerName, false);
    initBots(players + 1, TOTAL_PLAYERS - 1);

    Zone zone;
    initZone(zone);

    int aliveCount = TOTAL_PLAYERS;
    string killFeed = "";
    int turn = 1;

    cout << "\n===========================================" << endl;
    cout << "  BATTLE ROYALE — 50 PLAYERS" << endl;
    cout << "  Drop in. Loot up. Survive." << endl;
    cout << "===========================================\n" << endl;

    // Initial loot for player
    cout << "You landed at (" << players[0].x << "," << players[0].y << ")" << endl;
    cout << "Searching for loot..." << endl;
    lootArea(players[0]);
    if (players[0].weapon1 < 0) {
        // Guarantee a starting weapon
        players[0].weapon1 = 0; // Pistol
        cout << "(Found a pistol to start!)" << endl;
    }

    while (players[0].isAlive && aliveCount > 1) {
        cout << "\n\n===========================================" << endl;
        cout << "  TURN " << turn << " | ALIVE: " << aliveCount << "/" << TOTAL_PLAYERS << endl;
        if (!killFeed.empty()) {
            cout << "  LAST KILL: " << killFeed << endl;
            killFeed = "";
        }
        cout << "===========================================" << endl;

        // Zone shrink check
        zone.shrinkTimer--;
        if (zone.shrinkTimer <= 0) {
            shrinkZone(zone, players);
        }

        // Update zone status for all players
        for (int i = 0; i < TOTAL_PLAYERS; i++) {
            if (!players[i].isAlive) continue;
            checkZone(players[i], zone);
        }

        aliveCount = 0;
        for (int i = 0; i < TOTAL_PLAYERS; i++) {
            if (players[i].isAlive) aliveCount++;
        }

        if (aliveCount <= 1) break;
        if (!players[0].isAlive) break;

        // Player's turn
        playerTurn(players[0], players, zone, killFeed, aliveCount);

        // Bots take turns
        for (int i = 1; i < TOTAL_PLAYERS; i++) {
            if (!players[i].isAlive) continue;
            botTurn(players[i], players, zone, killFeed, aliveCount);
        }

        // Update alive count
        aliveCount = 0;
        for (int i = 0; i < TOTAL_PLAYERS; i++) {
            if (players[i].isAlive) { aliveCount++; players[i].survivalTime++; }
        }

        // Random bot vs bot kills
        if (rollChance(60) && aliveCount > 2) {
            int attempts = 0;
            while (attempts < 20) {
                int killer = rngInt(1, TOTAL_PLAYERS - 1);
                int victim = rngInt(1, TOTAL_PLAYERS - 1);
                if (killer != victim && players[killer].isAlive && players[victim].isAlive) {
                    players[victim].isAlive = false;
                    players[killer].kills++;
                    aliveCount--;
                    int wIdx = (players[killer].activeWeapon == 1) ? players[killer].weapon1 : players[killer].weapon2;
                    string wName = (wIdx >= 0) ? weaponDB[wIdx].name : "Unknown";
                    killFeed = players[killer].name + " [" + wName + "] eliminated " + players[victim].name;
                    cout << "  KILL FEED: " << killFeed << endl;
                    break;
                }
                attempts++;
            }
        }

        turn++;
    }

    // Calculate rank
    int rank = 1;
    for (int i = 0; i < TOTAL_PLAYERS; i++) {
        if (players[i].isAlive && &players[i] != &players[0]) {
            // Other player survived — they ranked higher if alive
        }
    }

    // Count how many are still alive (if player died, count was at death)
    if (players[0].isAlive) {
        rank = 1; // Won!
    } else {
        // Count alive players = players who outlived you
        int alive = 0;
        for (int i = 0; i < TOTAL_PLAYERS; i++) {
            if (players[i].isAlive) alive++;
        }
        rank = alive + 1;
    }

    showResults(players[0], rank, TOTAL_PLAYERS);
}

// ===== Main =====
int main() {
    srand(time(0));

    cout << "===========================================" << endl;
    cout << "   BATTLE ROYALE — C++ Survival Game" << endl;
    cout << "   Built by Aanand" << endl;
    cout << "===========================================" << endl;
    cout << "\n50 players. Shrinking zone. Last one wins." << endl;
    cout << "Inspired by Free Fire & BGMI!\n" << endl;

    while (true) {
        gameLoop();

        cout << "Play again? (1=Yes, 0=No): ";
        int again;
        cin >> again;
        if (cin.fail() || again == 0) {
            cout << "\nThanks for playing! 🎮" << endl;
            break;
        }
    }

    return 0;
}
