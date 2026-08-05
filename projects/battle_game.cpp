/*
 * ============================================
 *   BATTLE ARENA — C++ RPG Battle Game
 *   Built by Aanand
 * ============================================
 *
 *   A text-based battle game where you fight
 *   enemies in an arena. Level up, choose
 *   attacks, use potions, and survive!
 *
 *   Features:
 *   1. Player vs Enemy turn-based combat
 *   2. Multiple attack types (Light, Heavy, Special)
 *   3. Health potions & mana system
 *   4. Enemy AI with different difficulty levels
 *   5. Multiple waves of enemies
 *   6. Level up system — get stronger each win
 *   7. Critical hits & dodge mechanics
 *   8. Score tracking
 *
 *   Concepts used:
 *   - Structs (player & enemy data)
 *   - Functions (game logic)
 *   - Random numbers (damage, crits, dodges)
 *   - Loops (battle loop, wave system)
 *   - Switch-case (action menu)
 *   - Conditionals (game logic)
 *
 *   How to compile:
 *   g++ projects/battle_game.cpp -o battle
 *   ./battle
 *
 * ============================================
 */

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
using namespace std;

// ===== Structs =====
struct Fighter {
    string name;
    int hp;          // Health points
    int maxHp;
    int attack;      // Base attack power
    int defense;     // Damage reduction
    int mana;        // For special attacks
    int maxMana;
    int level;
    int potions;    // Health potions
    int score;
};

// ===== Function Declarations =====
void initPlayer(Fighter &p);
void initEnemy(Fighter &e, int wave);
int playerTurn(Fighter &p, Fighter &e);
void enemyTurn(Fighter &p, Fighter &e);
void showStats(Fighter &p, Fighter &e);
void levelUp(Fighter &p);
int randomDamage(int base, int variance);
bool isCritical();
bool isDodge(int dodgeChance);
void printBar(int current, int max, string label);
void battle(Fighter &p, Fighter &e, int wave);
void gameOverScreen(Fighter &p);

// ===== Utility Functions =====
int randomDamage(int base, int variance) {
    return base + (rand() % (variance + 1)) - (variance / 2);
}

bool isCritical() {
    return (rand() % 100) < 20;  // 20% crit chance
}

bool isDodge(int dodgeChance) {
    return (rand() % 100) < dodgeChance;
}

// ===== Health/Mana Bar Display =====
void printBar(int current, int max, string label) {
    int barWidth = 20;
    int filled = (current * barWidth) / max;
    if (filled < 0) filled = 0;
    if (filled > barWidth) filled = barWidth;

    cout << label << " [";
    for (int i = 0; i < filled; i++) cout << "=";
    for (int i = filled; i < barWidth; i++) cout << " ";
    cout << "] " << current << "/" << max << endl;
}

// ===== Show Both Fighters' Stats =====
void showStats(Fighter &p, Fighter &e) {
    cout << "\n===========================================" << endl;
    cout << "  " << e.name << " (Enemy)" << endl;
    printBar(e.hp, e.maxHp, "  HP  ");
    cout << "  ATK: " << e.attack << "  DEF: " << e.defense << endl;
    cout << "-------------------------------------------" << endl;
    cout << "  " << p.name << " (Lv." << p.level << ")" << endl;
    printBar(p.hp, p.maxHp, "  HP  ");
    printBar(p.mana, p.maxMana, "  MANA");
    cout << "  ATK: " << p.attack << "  DEF: " << p.defense << endl;
    cout << "  Potions: " << p.potions << "  Score: " << p.score << endl;
    cout << "===========================================\n" << endl;
}

// ===== Initialize Player =====
void initPlayer(Fighter &p) {
    p.name = "Hero";
    p.maxHp = 100;
    p.hp = 100;
    p.attack = 20;
    p.defense = 5;
    p.maxMana = 50;
    p.mana = 50;
    p.level = 1;
    p.potions = 3;
    p.score = 0;
}

// ===== Initialize Enemy Based on Wave =====
void initEnemy(Fighter &e, int wave) {
    string enemies[] = {"Goblin", "Skeleton", "Dark Knight", "Orc Warrior",
                        "Shadow Mage", "Ice Golem", "Fire Demon", "Dragon Lord",
                        "Undead King", "Final Boss"};
    e.name = enemies[min(wave - 1, 9)];
    e.maxHp = 60 + (wave * 25);
    e.hp = e.maxHp;
    e.attack = 12 + (wave * 4);
    e.defense = 2 + (wave * 2);
    e.mana = 0;
    e.maxMana = 0;
    e.level = wave;
    e.potions = 0;
    e.score = 0;
}

// ===== Level Up Player After Win =====
void levelUp(Fighter &p) {
    p.level++;
    p.maxHp += 20;
    p.hp = p.maxHp;       // Full heal on level up
    p.attack += 5;
    p.defense += 2;
    p.maxMana += 10;
    p.mana = p.maxMana;   // Full mana refill
    p.potions += 1;       // Get a potion each level

    cout << "\n*** LEVEL UP! You are now Level " << p.level << " ***" << endl;
    cout << "HP: " << p.maxHp << " | ATK: " << p.attack << " | DEF: " << p.defense << endl;
    cout << "Mana: " << p.maxMana << " | Potions: " << p.potions << endl;
    cout << "Full health restored!\n" << endl;
}

// ===== Player's Turn =====
// Returns: 0 = continue, -1 = used potion (skip attack)
int playerTurn(Fighter &p, Fighter &e) {
    int choice;

    cout << "----- YOUR TURN -----" << endl;
    cout << "1. Light Attack  (low dmg, no mana, 10% crit)" << endl;
    cout << "2. Heavy Attack  (high dmg, no mana, 20% miss)" << endl;
    cout << "3. Special Move  (huge dmg, costs 20 mana)" << endl;
    cout << "4. Use Potion    (heal 40 HP, you have " << p.potions << ")" << endl;
    cout << "5. Defend        (reduce damage next turn)" << endl;
    cout << "----------------------" << endl;
    cout << "Action: ";
    cin >> choice;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input! You hesitate and lose your turn!" << endl;
        return 0;
    }

    switch (choice) {
        case 1: {  // Light Attack
            if (isDodge(10)) {
                cout << e.name << " dodged your attack!" << endl;
                return 0;
            }
            int dmg = randomDamage(p.attack, 6);
            if (isCritical()) {
                dmg *= 2;
                cout << "CRITICAL HIT! ";
            }
            dmg -= e.defense;
            if (dmg < 1) dmg = 1;
            e.hp -= dmg;
            cout << "You hit " << e.name << " for " << dmg << " damage!" << endl;
            break;
        }
        case 2: {  // Heavy Attack
            if (isDodge(20)) {
                cout << "Your heavy attack missed!" << endl;
                return 0;
            }
            int dmg = randomDamage(p.attack * 2, 10);
            if (isCritical()) {
                dmg *= 2;
                cout << "CRITICAL HIT! ";
            }
            dmg -= e.defense;
            if (dmg < 1) dmg = 1;
            e.hp -= dmg;
            cout << "HEAVY STRIKE! You deal " << dmg << " damage to " << e.name << "!" << endl;
            break;
        }
        case 3: {  // Special Move
            if (p.mana < 20) {
                cout << "Not enough mana! (Need 20, have " << p.mana << ")" << endl;
                return playerTurn(p, e);  // Re-do turn
            }
            p.mana -= 20;
            int dmg = randomDamage(p.attack * 3, 15);
            dmg -= e.defense;
            if (dmg < 1) dmg = 1;
            e.hp -= dmg;
            cout << "SPECIAL ATTACK! Massive " << dmg << " damage to " << e.name << "!" << endl;
            break;
        }
        case 4: {  // Use Potion
            if (p.potions <= 0) {
                cout << "No potions left!" << endl;
                return playerTurn(p, e);
            }
            p.potions--;
            int heal = 40;
            p.hp += heal;
            if (p.hp > p.maxHp) p.hp = p.maxHp;
            cout << "You drink a potion and heal " << heal << " HP!" << endl;
            return -1;  // Used potion, no attack
        }
        case 5: {  // Defend
            p.defense += 10;  // Temporary boost
            cout << "You raise your shield! Defense boosted for this turn!" << endl;
            // Note: defense boost is temporary — we reset it after enemy turn
            return -2;
        }
        default:
            cout << "Invalid choice! You lose your turn!" << endl;
            return 0;
    }
    return 0;
}

// ===== Enemy's Turn (AI) =====
void enemyTurn(Fighter &p, Fighter &e) {
    cout << "\n----- ENEMY TURN -----" << endl;

    // Enemy AI: 70% normal attack, 20% strong attack, 10% special
    int roll = rand() % 100;
    int dmg;

    if (roll < 70) {
        // Normal attack
        if (isDodge(15)) {
            cout << "You dodged " << e.name << "'s attack!" << endl;
            return;
        }
        dmg = randomDamage(e.attack, 6);
        if (isCritical()) {
            dmg *= 2;
            cout << e.name << " lands a CRITICAL HIT! ";
        }
        dmg -= p.defense;
        if (dmg < 1) dmg = 1;
        p.hp -= dmg;
        cout << e.name << " attacks you for " << dmg << " damage!" << endl;
    } else if (roll < 90) {
        // Strong attack
        if (isDodge(25)) {
            cout << e.name << "'s strong attack missed!" << endl;
            return;
        }
        dmg = randomDamage(e.attack * 2, 10);
        dmg -= p.defense;
        if (dmg < 1) dmg = 1;
        p.hp -= dmg;
        cout << e.name << " uses a POWERFUL STRIKE for " << dmg << " damage!" << endl;
    } else {
        // Special attack (every few turns)
        dmg = randomDamage(e.attack * 2, 15);
        dmg -= p.defense;
        if (dmg < 1) dmg = 1;
        p.hp -= dmg;
        cout << e.name << " unleashes a SPECIAL ATTACK for " << dmg << " damage!" << endl;
    }

    // Restore 2 mana to player per enemy turn
    p.mana += 2;
    if (p.mana > p.maxMana) p.mana = p.maxMana;
}

// ===== Main Battle Function =====
void battle(Fighter &p, Fighter &e, int wave) {
    cout << "\n\n";
    cout << "===========================================" << endl;
    cout << "   WAVE " << wave << " — " << e.name << " APPEARS!" << endl;
    cout << "===========================================" << endl;

    while (p.hp > 0 && e.hp > 0) {
        showStats(p, e);

        // Player's turn
        int result = playerTurn(p, e);

        // Reset defense boost if player defended
        if (result == -2) {
            // Enemy attacks with player's boosted defense
            enemyTurn(p, e);
            p.defense -= 10;  // Reset defense after enemy turn
        } else {
            // Check if enemy is dead
            if (e.hp <= 0) break;

            // Enemy's turn
            enemyTurn(p, e);
        }

        cout << "\n";
    }
}

// ===== Game Over Screen =====
void gameOverScreen(Fighter &p) {
    cout << "\n\n===========================================" << endl;
    cout << "              GAME OVER" << endl;
    cout << "===========================================" << endl;
    cout << "  You reached Level " << p.level << endl;
    cout << "  Final Score: " << p.score << endl;
    cout << "  Potions used: " << (3 + p.level - 1 - p.potions) << endl;
    cout << "===========================================" << endl;
    cout << "\n  GG! Try again to beat your high score!" << endl;
    cout << "  Keep coding, Aanand! 🎮\n" << endl;
}

// ===== Main Function =====
int main() {
    srand(time(0));

    cout << "===========================================" << endl;
    cout << "   BATTLE ARENA v1.0" << endl;
    cout << "   Built by Aanand" << endl;
    cout << "===========================================" << endl;
    cout << "\nFight through waves of enemies!" << endl;
    cout << "Level up, use potions, and survive!\n" << endl;

    Fighter player, enemy;
    initPlayer(player);

    int wave = 1;
    bool playing = true;

    while (playing) {
        // Create enemy for this wave
        initEnemy(enemy, wave);

        // Fight!
        battle(player, enemy, wave);

        // Check result
        if (player.hp <= 0) {
            gameOverScreen(player);
            playing = false;
        } else if (enemy.hp <= 0) {
            // Player won this wave!
            int waveScore = wave * 100;
            player.score += waveScore;
            cout << "\n*** You defeated " << enemy.name << "! ***" << endl;
            cout << "*** Score +" << waveScore << " | Total: " << player.score << " ***\n" << endl;

            // Level up every 2 waves
            if (wave % 2 == 0) {
                levelUp(player);
            } else {
                // Small heal between waves
                int heal = player.maxHp * 0.25;
                player.hp += heal;
                if (player.hp > player.maxHp) player.hp = player.maxHp;
                player.mana += 10;
                if (player.mana > player.maxMana) player.mana = player.maxMana;
                cout << "You rest briefly. +" << heal << " HP, +10 Mana\n" << endl;
            }

            // Ask to continue
            cout << "Next wave? (1 = Yes, 0 = Quit): ";
            int cont;
            cin >> cont;
            if (cin.fail() || cont == 0) {
                cout << "\nYou retire from the arena as a champion!" << endl;
                cout << "Final Score: " << player.score << " | Level: " << player.level << endl;
                playing = false;
            }

            wave++;
        }
    }

    return 0;
}
