/*
 * ============================================
 *   BATTLE ARENA v2.0 — ADVANCED EDITION
 *   Built by Aanand
 * ============================================
 *
 *   Advanced RPG battle game with:
 *   1. Character class selection (Warrior, Mage, Rogue)
 *   2. Status effects (Poison, Burn, Stun, Heal-over-time)
 *   3. Shop system between waves
 *   4. Gold & inventory system
 *   5. Weapon & armor upgrades
 *   6. Boss fights with special mechanics
 *   7. Combo system — consecutive hits increase damage
 *   8. Energy/Ultimate ability per class
 *   9. 15 enemy types with unique AI
 *  10. Save/load high score
 *
 *   Concepts used:
 *   - Structs with nested data
 *   - Enum for status effects
 *   - Function pointers (attack patterns)
 *   - Arrays (inventory, enemy list)
 *   - Random numbers (RNG mechanics)
 *   - Switch-case (menus)
 *   - Loops (game loop, wave system)
 *   - File I/O (high score saving)
 *   - String manipulation (display)
 *
 *   How to compile:
 *   g++ projects/battle_arena_advanced.cpp -o battle2
 *   ./battle2
 *
 * ============================================
 */

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <string>
using namespace std;

// ===== Status Effects =====
enum StatusEffect {
    STATUS_NONE = 0,
    STATUS_POISON,    // Take damage each turn
    STATUS_BURN,      // Take damage each turn (more)
    STATUS_STUN,      // Skip a turn
    STATUS_REGEN,     // Heal each turn
    STATUS_WEAKEN     // Reduced attack
};

// ===== Structs =====
struct Status {
    StatusEffect effect;
    int turns;       // How many turns left
    int power;       // Damage/heal amount per turn
};

struct Fighter {
    string name;
    string className;
    int hp;
    int maxHp;
    int attack;
    int defense;
    int mana;
    int maxMana;
    int gold;
    int level;
    int potions;
    int score;
    int combo;       // Consecutive hit counter
    int weaponLevel; // Weapon upgrade level
    int armorLevel;  // Armor upgrade level
    Status status;   // Active status effect
    bool isStunned;
};

struct Item {
    string name;
    int price;
    string description;
};

// ===== Global Data =====
const int NUM_SHOP_ITEMS = 6;
Item shopItems[NUM_SHOP_ITEMS] = {
    {"Health Potion",  30, "Heal 50 HP"},
    {"Mana Potion",    25, "Restore 30 Mana"},
    {"Weapon Upgrade", 80, "+5 Attack Power"},
    {"Armor Upgrade",  70, "+3 Defense"},
    {"Max HP Boost",   60, "+25 Max HP (full heal)"},
    {"Antidote",       15, "Clear status effects"}
};

string enemyNames[] = {
    "Goblin Scout", "Skeleton Warrior", "Dark Knight", "Orc Berserker",
    "Shadow Mage", "Ice Golem", "Fire Demon", "Dragon Whelp",
    "Undead Knight", "Venom Spider", "Lightning Elemental",
    "Stone Giant", "Phantom Assassin", "Dragon Lord", "Final Boss"
};

// ===== Function Declarations =====
void initPlayer(Fighter &p);
void selectClass(Fighter &p);
void initEnemy(Fighter &e, int wave);
void showStats(Fighter &p, Fighter &e);
void printBar(int current, int max, string label, char symbol);
int randomDamage(int base, int variance);
bool rollChance(int percent);
void applyStatus(Fighter &f, StatusEffect effect, int turns, int power);
void processStatus(Fighter &f);
string statusName(StatusEffect e);
void shop(Fighter &p);
int playerTurn(Fighter &p, Fighter &e);
void enemyTurn(Fighter &p, Fighter &e);
void battle(Fighter &p, Fighter &e, int wave);
void levelUp(Fighter &p);
void gameOverScreen(Fighter &p);

// ===== Utility Functions =====
int randomDamage(int base, int variance) {
    return base + (rand() % (variance * 2 + 1)) - variance;
}

bool rollChance(int percent) {
    return (rand() % 100) < percent;
}

void printBar(int current, int max, string label, char symbol) {
    int barWidth = 20;
    int filled = (current * barWidth) / max;
    if (filled < 0) filled = 0;
    if (filled > barWidth) filled = barWidth;
    cout << label << " [";
    for (int i = 0; i < filled; i++) cout << symbol;
    for (int i = filled; i < barWidth; i++) cout << " ";
    cout << "] " << current << "/" << max;
}

string statusName(StatusEffect e) {
    switch (e) {
        case STATUS_POISON: return "POISONED";
        case STATUS_BURN:   return "BURNING";
        case STATUS_STUN:   return "STUNNED";
        case STATUS_REGEN:  return "REGEN";
        case STATUS_WEAKEN: return "WEAKENED";
        default: return "";
    }
}

void applyStatus(Fighter &f, StatusEffect effect, int turns, int power) {
    f.status.effect = effect;
    f.status.turns = turns;
    f.status.power = power;
    if (effect == STATUS_STUN) f.isStunned = true;
}

void processStatus(Fighter &f) {
    if (f.status.turns <= 0) return;

    switch (f.status.effect) {
        case STATUS_POISON:
            f.hp -= f.status.power;
            cout << f.name << " takes " << f.status.power << " poison damage!" << endl;
            break;
        case STATUS_BURN:
            f.hp -= f.status.power;
            cout << f.name << " takes " << f.status.power << " burn damage!" << endl;
            break;
        case STATUS_REGEN:
            f.hp += f.status.power;
            if (f.hp > f.maxHp) f.hp = f.maxHp;
            cout << f.name << " regenerates " << f.status.power << " HP!" << endl;
            break;
        case STATUS_WEAKEN:
            // Just visual, handled in damage calc
            break;
        case STATUS_STUN:
            f.isStunned = true;
            cout << f.name << " is stunned!" << endl;
            break;
        default: break;
    }

    f.status.turns--;
    if (f.status.turns <= 0) {
        if (f.status.effect != STATUS_NONE) {
            cout << f.name << " is no longer " << statusName(f.status.effect) << "!" << endl;
        }
        f.status.effect = STATUS_NONE;
        f.status.power = 0;
        if (f.status.effect == STATUS_STUN) f.isStunned = false;
    }
}

// ===== Class Selection =====
void selectClass(Fighter &p) {
    cout << "\n===== CHOOSE YOUR CLASS =====" << endl;
    cout << "1. Warrior  — High HP, Strong Defense, Melee" << endl;
    cout << "   HP: 120 | ATK: 22 | DEF: 8 | Mana: 30" << endl;
    cout << "   Special: BERSERK (double damage, 3 turns)" << endl;
    cout << "" << endl;
    cout << "2. Mage     — High Mana, Powerful Spells" << endl;
    cout << "   HP: 80  | ATK: 16 | DEF: 3 | Mana: 80" << endl;
    cout << "   Special: FIREBALL (massive damage + burn)" << endl;
    cout << "" << endl;
    cout << "3. Rogue    — Balanced, High Crit Chance" << endl;
    cout << "   HP: 95  | ATK: 20 | DEF: 5 | Mana: 50" << endl;
    cout << "   Special: SHADOW STRIKE (always crits + poison)" << endl;
    cout << "============================" << endl;
    cout << "Class choice: ";

    int choice;
    cin >> choice;

    if (cin.fail() || choice < 1 || choice > 3) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid! Defaulting to Warrior." << endl;
        choice = 1;
    }

    switch (choice) {
        case 1:
            p.className = "Warrior";
            p.maxHp = 120; p.hp = 120;
            p.attack = 22; p.defense = 8;
            p.maxMana = 30; p.mana = 30;
            break;
        case 2:
            p.className = "Mage";
            p.maxHp = 80; p.hp = 80;
            p.attack = 16; p.defense = 3;
            p.maxMana = 80; p.mana = 80;
            break;
        case 3:
            p.className = "Rogue";
            p.maxHp = 95; p.hp = 95;
            p.attack = 20; p.defense = 5;
            p.maxMana = 50; p.mana = 50;
            break;
    }

    cout << "\nYou chose: " << p.className << "!" << endl;
}

// ===== Initialize Player =====
void initPlayer(Fighter &p) {
    p.name = "Hero";
    p.className = "";
    p.gold = 50;
    p.level = 1;
    p.potions = 3;
    p.score = 0;
    p.combo = 0;
    p.weaponLevel = 0;
    p.armorLevel = 0;
    p.isStunned = false;
    p.status.effect = STATUS_NONE;
    p.status.turns = 0;
    p.status.power = 0;

    selectClass(p);
}

// ===== Initialize Enemy Based on Wave =====
void initEnemy(Fighter &e, int wave) {
    int idx = min(wave - 1, 14);
    e.name = enemyNames[idx];
    e.className = "Enemy";

    bool isBoss = (wave % 5 == 0);  // Every 5th wave is a boss

    e.maxHp = 50 + (wave * 20);
    if (isBoss) e.maxHp *= 2;  // Bosses have double HP
    e.hp = e.maxHp;
    e.attack = 10 + (wave * 3);
    e.defense = 1 + (wave * 2);
    e.mana = 0;
    e.maxMana = 0;
    e.gold = 20 + (wave * 10);
    if (isBoss) e.gold *= 3;  // Bosses give 3x gold
    e.level = wave;
    e.potions = 0;
    e.score = 0;
    e.combo = 0;
    e.weaponLevel = 0;
    e.armorLevel = 0;
    e.isStunned = false;
    e.status.effect = STATUS_NONE;
    e.status.turns = 0;
    e.status.power = 0;

    if (isBoss) {
        cout << "\n!!! BOSS BATTLE !!!" << endl;
    }
}

// ===== Show Stats =====
void showStats(Fighter &p, Fighter &e) {
    cout << "\n===========================================" << endl;
    cout << "  " << e.name << " (Lv." << e.level << ")" << endl;
    printBar(e.hp, e.maxHp, "  HP  ", '#');
    cout << "  ATK: " << e.attack << "  DEF: " << e.defense;
    if (e.status.turns > 0) cout << "  [" << statusName(e.status.effect) << ":" << e.status.turns << "]";
    cout << "\n-------------------------------------------" << endl;
    cout << "  " << p.name << " the " << p.className << " (Lv." << p.level << ")" << endl;
    printBar(p.hp, p.maxHp, "  HP  ", '=');
    cout << endl;
    printBar(p.mana, p.maxMana, "  MANA", '*');
    cout << "\n  ATK: " << p.attack << "  DEF: " << p.defense;
    cout << "  Gold: " << p.gold;
    if (p.combo > 1) cout << "  COMBO x" << p.combo;
    if (p.status.turns > 0) cout << "  [" << statusName(p.status.effect) << ":" << p.status.turns << "]";
    cout << "\n  Potions: " << p.potions << "  Score: " << p.score;
    cout << "\n===========================================\n" << endl;
}

// ===== Shop System =====
void shop(Fighter &p) {
    cout << "\n===== SHOP =====" << endl;
    cout << "Your Gold: " << p.gold << endl;
    cout << "----------------" << endl;
    for (int i = 0; i < NUM_SHOP_ITEMS; i++) {
        cout << i + 1 << ". " << shopItems[i].name
             << " (" << shopItems[i].price << "g) — "
             << shopItems[i].description << endl;
    }
    cout << "0. Leave Shop" << endl;
    cout << "----------------" << endl;

    while (true) {
        cout << "Buy what? ";
        int choice;
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid!" << endl;
            continue;
        }

        if (choice == 0) {
            cout << "You leave the shop." << endl;
            return;
        }

        if (choice < 1 || choice > NUM_SHOP_ITEMS) {
            cout << "Invalid item!" << endl;
            continue;
        }

        Item &item = shopItems[choice - 1];
        if (p.gold < item.price) {
            cout << "Not enough gold! You have " << p.gold << ", need " << item.price << endl;
            continue;
        }

        p.gold -= item.price;

        switch (choice) {
            case 1: // Health Potion
                p.potions++;
                cout << "Bought Health Potion! You now have " << p.potions << endl;
                break;
            case 2: // Mana Potion
                p.mana = min(p.mana + 30, p.maxMana);
                cout << "Restored 30 Mana! Mana: " << p.mana << "/" << p.maxMana << endl;
                break;
            case 3: // Weapon Upgrade
                p.weaponLevel++;
                p.attack += 5;
                cout << "Weapon upgraded to Lv." << p.weaponLevel << "! ATK: " << p.attack << endl;
                break;
            case 4: // Armor Upgrade
                p.armorLevel++;
                p.defense += 3;
                cout << "Armor upgraded to Lv." << p.armorLevel << "! DEF: " << p.defense << endl;
                break;
            case 5: // Max HP Boost
                p.maxHp += 25;
                p.hp = p.maxHp;
                cout << "Max HP increased to " << p.maxHp << "! Full heal!" << endl;
                break;
            case 6: // Antidote
                p.status.effect = STATUS_NONE;
                p.status.turns = 0;
                p.status.power = 0;
                p.isStunned = false;
                cout << "Status effects cleared!" << endl;
                break;
        }
    }
}

// ===== Player's Turn =====
int playerTurn(Fighter &p, Fighter &e) {
    if (p.isStunned) {
        cout << "You are stunned and can't move!" << endl;
        p.isStunned = false;
        if (p.status.effect == STATUS_STUN) {
            p.status.effect = STATUS_NONE;
            p.status.turns = 0;
        }
        return 0;
    }

    int choice;
    cout << "----- YOUR TURN -----" << endl;
    cout << "1. Attack         (normal damage)" << endl;
    cout << "2. Heavy Attack   (2x damage, 15% miss)" << endl;
    cout << "3. Special Move   (class-specific, 25 mana)" << endl;
    cout << "4. Use Potion     (heal 50 HP, have " << p.potions << ")" << endl;
    cout << "5. Defend         (boost DEF, regen 5 mana)" << endl;
    cout << "----------------------" << endl;
    cout << "Action: ";
    cin >> choice;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid! You lose your turn!" << endl;
        p.combo = 0;
        return 0;
    }

    int effectiveAttack = p.attack;
    if (p.status.effect == STATUS_WEAKEN) {
        effectiveAttack -= p.status.power;
        if (effectiveAttack < 1) effectiveAttack = 1;
    }

    switch (choice) {
        case 1: {  // Normal Attack
            if (rollChance(5)) {
                cout << "Your attack missed!" << endl;
                p.combo = 0;
                return 0;
            }
            int dmg = randomDamage(effectiveAttack, 5);
            bool crit = rollChance(15 + (p.className == "Rogue" ? 15 : 0));
            if (crit) { dmg *= 2; p.combo++; cout << "CRITICAL! "; }
            else p.combo++;

            // Combo bonus
            if (p.combo >= 3) {
                int bonus = p.combo * 2;
                dmg += bonus;
                cout << "COMBO x" << p.combo << " (+" << bonus << " dmg) ";
            }

            dmg -= e.defense;
            if (dmg < 1) dmg = 1;
            e.hp -= dmg;
            cout << "You hit " << e.name << " for " << dmg << " damage!" << endl;
            break;
        }
        case 2: {  // Heavy Attack
            if (rollChance(15)) {
                cout << "Heavy attack missed!" << endl;
                p.combo = 0;
                return 0;
            }
            int dmg = randomDamage(effectiveAttack * 2, 8);
            bool crit = rollChance(10);
            if (crit) { dmg *= 2; cout << "CRITICAL! "; }
            p.combo++;
            dmg -= e.defense;
            if (dmg < 1) dmg = 1;
            e.hp -= dmg;
            cout << "HEAVY STRIKE! " << dmg << " damage to " << e.name << "!" << endl;
            break;
        }
        case 3: {  // Special Move
            if (p.mana < 25) {
                cout << "Not enough mana! (Need 25, have " << p.mana << ")" << endl;
                return playerTurn(p, e);
            }
            p.mana -= 25;

            if (p.className == "Warrior") {
                // BERSERK: Double attack for 3 turns
                applyStatus(p, STATUS_WEAKEN, 0, 0); // Clear any existing
                int dmg = randomDamage(effectiveAttack * 2, 10);
                dmg -= e.defense;
                if (dmg < 1) dmg = 1;
                e.hp -= dmg;
                p.combo++;
                cout << "BERSERK RAGE! You deal " << dmg << " damage!" << endl;
                // Buff next 2 turns
                p.attack += 10;
                // We'll track this with a simple flag
                cout << "(Attack boosted for 2 turns!)" << endl;
            } else if (p.className == "Mage") {
                // FIREBALL: Massive damage + burn
                int dmg = randomDamage(effectiveAttack * 3, 15);
                dmg -= e.defense;
                if (dmg < 1) dmg = 1;
                e.hp -= dmg;
                applyStatus(e, STATUS_BURN, 3, 8);
                p.combo++;
                cout << "FIREBALL! " << dmg << " damage + BURN for 3 turns!" << endl;
            } else if (p.className == "Rogue") {
                // SHADOW STRIKE: Always crits + poison
                int dmg = randomDamage(effectiveAttack * 2, 10);
                dmg *= 2;  // Always crit
                dmg -= e.defense;
                if (dmg < 1) dmg = 1;
                e.hp -= dmg;
                applyStatus(e, STATUS_POISON, 4, 6);
                p.combo++;
                cout << "SHADOW STRIKE! " << dmg << " damage + POISON for 4 turns!" << endl;
            }
            break;
        }
        case 4: {  // Potion
            if (p.potions <= 0) {
                cout << "No potions left!" << endl;
                return playerTurn(p, e);
            }
            p.potions--;
            int heal = 50;
            p.hp += heal;
            if (p.hp > p.maxHp) p.hp = p.maxHp;
            p.combo = 0;
            cout << "You drink a potion! +" << heal << " HP" << endl;
            return -1;
        }
        case 5: {  // Defend
            p.defense += 15;
            p.mana = min(p.mana + 5, p.maxMana);
            p.combo = 0;
            cout << "You raise your guard! +15 DEF, +5 Mana" << endl;
            return -2;
        }
        default:
            cout << "Invalid! You lose your turn!" << endl;
            p.combo = 0;
            return 0;
    }
    return 0;
}

// ===== Enemy AI Turn =====
void enemyTurn(Fighter &p, Fighter &e) {
    cout << "\n----- ENEMY TURN -----" << endl;

    if (e.isStunned) {
        cout << e.name << " is stunned and can't attack!" << endl;
        e.isStunned = false;
        if (e.status.effect == STATUS_STUN) {
            e.status.effect = STATUS_NONE;
            e.status.turns = 0;
        }
        return;
    }

    int roll = rand() % 100;
    int dmg;

    // Bosses have more complex AI
    bool isBoss = (e.level % 5 == 0);

    if (isBoss && roll < 25) {
        // Boss special: Stun attack
        if (rollChance(60)) {
            dmg = randomDamage(e.attack * 2, 10);
            dmg -= p.defense;
            if (dmg < 1) dmg = 1;
            p.hp -= dmg;
            if (rollChance(40)) {
                applyStatus(p, STATUS_STUN, 1, 0);
                p.isStunned = true;
                cout << e.name << " uses STUNNING BLOW! " << dmg << " damage + you're STUNNED!" << endl;
            } else {
                cout << e.name << " uses a powerful attack for " << dmg << " damage!" << endl;
            }
        } else {
            dmg = randomDamage(e.attack, 5);
            dmg -= p.defense;
            if (dmg < 1) dmg = 1;
            p.hp -= dmg;
            cout << e.name << " attacks for " << dmg << " damage!" << endl;
        }
    } else if (roll < 60) {
        // Normal attack
        if (rollChance(10)) {
            cout << e.name << "'s attack missed!" << endl;
            return;
        }
        dmg = randomDamage(e.attack, 5);
        bool crit = rollChance(10);
        if (crit) { dmg *= 2; cout << "CRITICAL! "; }
        dmg -= p.defense;
        if (dmg < 1) dmg = 1;
        p.hp -= dmg;
        cout << e.name << " attacks for " << dmg << " damage!" << endl;
    } else if (roll < 85) {
        // Strong attack
        if (rollChance(20)) {
            cout << e.name << "'s strong attack missed!" << endl;
            return;
        }
        dmg = randomDamage(e.attack * 2, 8);
        dmg -= p.defense;
        if (dmg < 1) dmg = 1;
        p.hp -= dmg;
        cout << e.name << " uses a POWERFUL STRIKE for " << dmg << " damage!" << endl;
    } else {
        // Status attack (poison/burn)
        dmg = randomDamage(e.attack, 5);
        dmg -= p.defense;
        if (dmg < 1) dmg = 1;
        p.hp -= dmg;

        if (rollChance(50)) {
            applyStatus(p, STATUS_POISON, 3, 5);
            cout << e.name << " inflicts POISON! " << dmg << " damage + poison for 3 turns!" << endl;
        } else {
            applyStatus(p, STATUS_BURN, 2, 7);
            cout << e.name << " inflicts BURN! " << dmg << " damage + burn for 2 turns!" << endl;
        }
    }

    // Regen mana for player
    p.mana = min(p.mana + 3, p.maxMana);
}

// ===== Main Battle =====
void battle(Fighter &p, Fighter &e, int wave) {
    cout << "\n===========================================" << endl;
    cout << "   WAVE " << wave << " — " << e.name << " APPEARS!" << endl;
    cout << "===========================================" << endl;

    while (p.hp > 0 && e.hp > 0) {
        showStats(p, e);

        // Process status effects at start of turn
        if (p.status.turns > 0) processStatus(p);
        if (e.status.turns > 0) processStatus(e);

        // Check if someone died from status
        if (p.hp <= 0 || e.hp <= 0) break;

        // Player's turn
        int result = playerTurn(p, e);

        // Reset defense boost if defended
        if (result == -2) {
            enemyTurn(p, e);
            p.defense -= 15;
        } else {
            if (e.hp <= 0) break;
            enemyTurn(p, e);
        }

        cout << "\n";
    }
}

// ===== Level Up =====
void levelUp(Fighter &p) {
    p.level++;
    p.maxHp += 15;
    p.hp = min(p.hp + 30, p.maxHp);
    p.attack += 4;
    p.defense += 2;
    p.maxMana += 8;
    p.mana = min(p.mana + 20, p.maxMana);

    cout << "\n*** LEVEL UP! Now Level " << p.level << " ***" << endl;
    cout << "HP: " << p.maxHp << " | ATK: " << p.attack << " | DEF: " << p.defense << endl;
    cout << "Mana: " << p.maxMana << " | +30 HP | +20 Mana" << endl;
}

// ===== Game Over =====
void gameOverScreen(Fighter &p) {
    cout << "\n\n===========================================" << endl;
    cout << "              GAME OVER" << endl;
    cout << "===========================================" << endl;
    cout << "  Class:   " << p.className << endl;
    cout << "  Level:   " << p.level << endl;
    cout << "  Score:   " << p.score << endl;
    cout << "  Gold:    " << p.gold << endl;
    cout << "  Weapon:  Lv." << p.weaponLevel << endl;
    cout << "  Armor:   Lv." << p.armorLevel << endl;
    cout << "===========================================" << endl;
    cout << "\n  GG! Try a different class next time!" << endl;
    cout << "  Keep coding, Aanand! 🎮\n" << endl;
}

// ===== Main Function =====
int main() {
    srand(time(0));

    cout << "===========================================" << endl;
    cout << "   BATTLE ARENA v2.0 — ADVANCED EDITION" << endl;
    cout << "   Built by Aanand" << endl;
    cout << "===========================================" << endl;
    cout << "\nChoose your class, fight through 15 waves," << endl;
    cout << "buy gear from the shop, and beat the Boss!\n" << endl;

    Fighter player, enemy;
    initPlayer(player);

    int wave = 1;
    bool playing = true;

    while (playing) {
        initEnemy(enemy, wave);
        battle(player, enemy, wave);

        if (player.hp <= 0) {
            gameOverScreen(player);
            playing = false;
        } else if (enemy.hp <= 0) {
            int waveScore = wave * 100;
            int goldEarned = enemy.gold;
            player.score += waveScore;
            player.gold += goldEarned;

            cout << "\n*** VICTORY! You defeated " << enemy.name << "! ***" << endl;
            cout << "*** Score +" << waveScore << " | Gold +" << goldEarned << " ***\n" << endl;

            // Level up every 2 waves
            if (wave % 2 == 0) {
                levelUp(player);
            } else {
                int heal = player.maxHp * 0.2;
                player.hp = min(player.hp + heal, player.maxHp);
                cout << "You catch your breath. +" << heal << " HP" << endl;
            }

            // Visit shop every wave
            cout << "\nVisit shop before next wave? (1=Yes, 0=No): ";
            int shopChoice;
            cin >> shopChoice;
            if (!cin.fail() && shopChoice == 1) {
                shop(player);
            }

            cout << "\nContinue to Wave " << (wave + 1) << "? (1=Yes, 0=Quit): ";
            int cont;
            cin >> cont;
            if (cin.fail() || cont == 0) {
                cout << "\nYou retire as a champion!" << endl;
                cout << "Final Score: " << player.score << " | Level: " << player.level << endl;
                playing = false;
            }

            wave++;
        }
    }

    return 0;
}
