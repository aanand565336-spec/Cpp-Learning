/*
 * ============================================
 *   DUEL ARENA — Advanced 2-Player Battle Game
 *   Built by Aanand
 * ============================================
 *
 *   An advanced turn-based RPG battle where TWO players
 *   fight each other on the same device.
 *
 *   Features:
 *   1. 6 Character Classes with unique stats & abilities
 *   2. 4 attack types (Basic, Heavy, Special, Ultimate)
 *   3. Status effects (Poison, Burn, Stun, Shield, Bleed, Weaken, Rage)
 *   4. Combo system — consecutive hits build damage multiplier
 *   5. Resource management (HP + Mana + Stamina)
 *   6. Item shop between rounds
 *   7. Best of 3/5/7 match system
 *   8. Gold economy
 *   9. Critical hits & dodge mechanics
 *  10. Turn order based on speed stat
 *  11. Champion screen with full match stats
 *
 *   How to compile:
 *   g++ projects/duel_arena.cpp -o duel
 *   ./duel
 * ============================================
 */

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
using namespace std;

// ===== Status Effects =====
enum StatusEffect {
    ST_NONE = 0, ST_POISON, ST_BURN, ST_STUN, ST_SHIELD,
    ST_REGEN, ST_BLEED, ST_WEAKEN, ST_RAGE
};

// ===== Character Classes =====
enum ClassType {
    CL_WARRIOR = 1, CL_MAGE, CL_ROGUE, CL_PALADIN, CL_ARCHER, CL_NECROMANCER
};

// ===== Structs =====
struct StatusInfo {
    StatusEffect effect;
    int turns;
    int power;
};

struct Fighter {
    string name;
    ClassType cls;
    string className;
    int hp, maxHp, attack, defense, speed;
    int mana, maxMana, stamina, maxStamina;
    int gold, potions, ethers;
    int combo;
    bool isStunned, hasShield;
    StatusInfo status;
    int damageDealt, damageTaken, critsLanded, specialsUsed;
};

struct ClassInfo {
    string name;
    int hp, atk, def, spd, mana, stamina;
    string specialName, specialDesc, ultName, ultDesc;
};

// ===== Class Data =====
const int NUM_CLASSES = 6;
ClassInfo classData[NUM_CLASSES] = {
    {"Warrior",     130, 22, 10, 5, 30, 50,
     "Berserk Rage",  "Double damage for 2 turns",
     "WHIRLWIND",     "Hit enemy 3 times in one turn"},
    {"Mage",        85,  18, 4,  7, 90, 30,
     "Fireball",      "3x damage + Burn (3 turns)",
     "METEOR STORM",  "5x damage + Burn + Stun"},
    {"Rogue",       100, 20, 6,  9, 50, 45,
     "Shadow Strike", "Always crits + Poison (4 turns)",
     "ASSASSINATE",   "8x dmg, 50% instakill below 30% HP"},
    {"Paladin",     120, 18, 9,  4, 60, 40,
     "Holy Light",    "Heal 40 HP + Shield (2 turns)",
     "DIVINE JUDGMENT","4x dmg + Stun + self-heal 30 HP"},
    {"Archer",      95,  21, 5,  8, 45, 50,
     "Multi-Shot",    "Hit 3 times, each 0.7x damage",
     "RAIN OF ARROWS", "5x dmg + Bleed (3 turns)"},
    {"Necromancer", 90,  19, 5,  6, 80, 35,
     "Life Drain",    "Damage + heal self for half",
     "DEATH CURSE",    "6x dmg + Poison + Weaken (3 turns)"}
};

// ===== Shop Items =====
struct ShopItem { string name; int price; string desc; };
const int NUM_SHOP_ITEMS = 7;
ShopItem shopItems[NUM_SHOP_ITEMS] = {
    {"Health Potion",  25, "Restore 50 HP"},
    {"Mana Potion",    20, "Restore 30 Mana"},
    {"Stamina Potion", 15, "Restore 30 Stamina"},
    {"Attack Boost",   50, "+5 permanent Attack"},
    {"Defense Boost",  45, "+4 permanent Defense"},
    {"Speed Boost",    40, "+2 permanent Speed"},
    {"Antidote",       10, "Clear all status effects"}
};

// ===== Utility =====
int rngDamage(int base, int variance) {
    return base + (rand() % (variance * 2 + 1)) - variance;
}

bool rollChance(int percent) {
    return (rand() % 100) < percent;
}

string statusName(StatusEffect e) {
    switch (e) {
        case ST_POISON: return "POISONED";
        case ST_BURN:   return "BURNING";
        case ST_STUN:   return "STUNNED";
        case ST_SHIELD: return "SHIELDED";
        case ST_REGEN:  return "REGEN";
        case ST_BLEED:  return "BLEEDING";
        case ST_WEAKEN: return "WEAKENED";
        case ST_RAGE:   return "ENRAGED";
        default: return "";
    }
}

string classSpecialName(ClassType cls) { return classData[cls - 1].specialName; }
string classUltName(ClassType cls) { return classData[cls - 1].ultName; }

void applyStatus(Fighter &f, StatusEffect eff, int turns, int power) {
    f.status.effect = eff;
    f.status.turns = turns;
    f.status.power = power;
    if (eff == ST_STUN) f.isStunned = true;
    if (eff == ST_SHIELD) f.hasShield = true;
}

void processStatus(Fighter &f) {
    if (f.status.turns <= 0) return;
    switch (f.status.effect) {
        case ST_POISON:
            f.hp -= f.status.power;
            f.damageTaken += f.status.power;
            cout << "  " << f.name << " takes " << f.status.power << " poison damage!" << endl;
            break;
        case ST_BURN:
            f.hp -= f.status.power;
            f.damageTaken += f.status.power;
            cout << "  " << f.name << " takes " << f.status.power << " burn damage!" << endl;
            break;
        case ST_BLEED:
            f.hp -= f.status.power;
            f.damageTaken += f.status.power;
            cout << "  " << f.name << " bleeds for " << f.status.power << " damage!" << endl;
            break;
        case ST_REGEN:
            f.hp += f.status.power;
            if (f.hp > f.maxHp) f.hp = f.maxHp;
            cout << "  " << f.name << " regenerates " << f.status.power << " HP!" << endl;
            break;
        case ST_SHIELD:
            cout << "  " << f.name << " is shielded!" << endl;
            break;
        case ST_RAGE:
            cout << "  " << f.name << " is enraged! Double damage!" << endl;
            break;
        case ST_WEAKEN:
            cout << "  " << f.name << " is weakened!" << endl;
            break;
        case ST_STUN:
            f.isStunned = true;
            break;
        default: break;
    }
    f.status.turns--;
    if (f.status.turns <= 0) {
        if (f.status.effect != ST_NONE) {
            cout << "  " << f.name << " is no longer " << statusName(f.status.effect) << "!" << endl;
        }
        f.status.effect = ST_NONE;
        f.status.power = 0;
        if (f.status.effect == ST_STUN) f.isStunned = false;
        f.hasShield = false;
    }
}

void printBar(int cur, int max, string label, char sym) {
    int width = 18;
    int filled = (cur * width) / max;
    if (filled < 0) filled = 0;
    if (filled > width) filled = width;
    cout << label << " [";
    for (int i = 0; i < filled; i++) cout << sym;
    for (int i = filled; i < width; i++) cout << " ";
    cout << "] " << cur << "/" << max;
}

// ===== Initialize Fighter =====
void initFighter(Fighter &f, string name, ClassType cls) {
    ClassInfo &ci = classData[cls - 1];
    f.name = name;
    f.cls = cls;
    f.className = ci.name;
    f.maxHp = ci.hp;        f.hp = ci.hp;
    f.attack = ci.atk;      f.defense = ci.def;
    f.speed = ci.spd;
    f.maxMana = ci.mana;     f.mana = ci.mana;
    f.maxStamina = ci.stamina; f.stamina = ci.stamina;
    f.gold = 50;            f.potions = 2;
    f.ethers = 1;           f.combo = 0;
    f.isStunned = false;    f.hasShield = false;
    f.status.effect = ST_NONE;
    f.status.turns = 0;     f.status.power = 0;
    f.damageDealt = 0;      f.damageTaken = 0;
    f.critsLanded = 0;      f.specialsUsed = 0;
}

// ===== Class Selection =====
void selectClass(Fighter &f) {
    cout << "\n===== CHOOSE CLASS FOR " << f.name << " =====" << endl;
    for (int i = 0; i < NUM_CLASSES; i++) {
        ClassInfo &ci = classData[i];
        cout << i + 1 << ". " << ci.name << endl;
        cout << "   HP:" << ci.hp << " ATK:" << ci.atk << " DEF:" << ci.def
             << " SPD:" << ci.spd << " MP:" << ci.mana << " STA:" << ci.stamina << endl;
        cout << "   Special: " << ci.specialName << " — " << ci.specialDesc << endl;
        cout << "   Ultimate: " << ci.ultName << " — " << ci.ultDesc << endl;
        cout << endl;
    }
    cout << "Choice: ";
    int choice;
    cin >> choice;
    if (cin.fail() || choice < 1 || choice > NUM_CLASSES) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid! Defaulting to Warrior." << endl;
        choice = 1;
    }
    initFighter(f, f.name, (ClassType)choice);
    cout << f.name << " chose: " << f.className << "!" << endl;
}

// ===== Show Battle Screen =====
void showBattleScreen(Fighter &p1, Fighter &p2) {
    cout << "\n===========================================" << endl;
    cout << "  " << p1.name << " [" << p1.className << "]" << endl;
    printBar(p1.hp, p1.maxHp, "  HP    ", '=');
    cout << endl;
    printBar(p1.mana, p1.maxMana, "  MANA  ", '*');
    cout << endl;
    printBar(p1.stamina, p1.maxStamina, "  STAM  ", '~');
    cout << "\n  ATK:" << p1.attack << " DEF:" << p1.defense << " SPD:" << p1.speed;
    if (p1.combo >= 2) cout << " COMBO:x" << p1.combo;
    if (p1.status.turns > 0) cout << " [" << statusName(p1.status.effect) << ":" << p1.status.turns << "]";
    cout << "\n-------------------------------------------" << endl;
    cout << "  " << p2.name << " [" << p2.className << "]" << endl;
    printBar(p2.hp, p2.maxHp, "  HP    ", '=');
    cout << endl;
    printBar(p2.mana, p2.maxMana, "  MANA  ", '*');
    cout << endl;
    printBar(p2.stamina, p2.maxStamina, "  STAM  ", '~');
    cout << "\n  ATK:" << p2.attack << " DEF:" << p2.defense << " SPD:" << p2.speed;
    if (p2.combo >= 2) cout << " COMBO:x" << p2.combo;
    if (p2.status.turns > 0) cout << " [" << statusName(p2.status.effect) << ":" << p2.status.turns << "]";
    cout << "\n===========================================\n" << endl;
}

// ===== Shop =====
void shop(Fighter &f) {
    cout << "\n===== SHOP (" << f.name << ") — Gold: " << f.gold << " =====" << endl;
    for (int i = 0; i < NUM_SHOP_ITEMS; i++) {
        cout << i + 1 << ". " << shopItems[i].name
             << " (" << shopItems[i].price << "g) — " << shopItems[i].desc << endl;
    }
    cout << "0. Leave" << endl;
    while (true) {
        cout << f.name << ", buy: ";
        int choice;
        cin >> choice;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid!" << endl;
            continue;
        }
        if (choice == 0) return;
        if (choice < 1 || choice > NUM_SHOP_ITEMS) { cout << "Invalid!" << endl; continue; }
        ShopItem &item = shopItems[choice - 1];
        if (f.gold < item.price) {
            cout << "Not enough gold! Have " << f.gold << ", need " << item.price << endl;
            continue;
        }
        f.gold -= item.price;
        switch (choice) {
            case 1: f.potions++;  cout << "Bought Health Potion! (" << f.potions << ")" << endl; break;
            case 2: f.ethers++;   cout << "Bought Mana Potion! (" << f.ethers << ")" << endl; break;
            case 3: f.stamina = min(f.stamina + 30, f.maxStamina); cout << "+30 Stamina!" << endl; break;
            case 4: f.attack += 5;  cout << "+5 Attack! ATK:" << f.attack << endl; break;
            case 5: f.defense += 4; cout << "+4 Defense! DEF:" << f.defense << endl; break;
            case 6: f.speed += 2;    cout << "+2 Speed! SPD:" << f.speed << endl; break;
            case 7:
                f.status.effect = ST_NONE;
                f.status.turns = 0;
                f.status.power = 0;
                f.isStunned = false;
                f.hasShield = false;
                cout << "Status cleared!" << endl;
                break;
        }
    }
}

// ===== Player Action Menu =====
int playerAction(Fighter &attacker, Fighter &defender) {
    if (attacker.isStunned) {
        cout << attacker.name << " is STUNNED and skips this turn!" << endl;
        attacker.isStunned = false;
        if (attacker.status.effect == ST_STUN) {
            attacker.status.effect = ST_NONE;
            attacker.status.turns = 0;
        }
        return -1;
    }
    int choice;
    cout << "----- " << attacker.name << "'s TURN -----" << endl;
    cout << "1. Basic Attack   (dmg based on ATK, free)" << endl;
    cout << "2. Heavy Attack   (1.8x dmg, costs 15 stamina)" << endl;
    cout << "3. Special Move   (" << classSpecialName(attacker.cls) << ", costs 25 mana)" << endl;
    cout << "4. Ultimate       (" << classUltName(attacker.cls) << ", costs 40 mana + 20 stamina)" << endl;
    cout << "5. Use HP Potion  (heal 50 HP, have " << attacker.potions << ")" << endl;
    cout << "6. Use Mana Potion(refill 30 MP, have " << attacker.ethers << ")" << endl;
    cout << "7. Defend         (+15 DEF, +5 mana, +10 stamina, Shield)" << endl;
    cout << "------------------------------------" << endl;
    cout << "Action: ";
    cin >> choice;
    if (cin.fail()) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid! Turn skipped!" << endl;
        attacker.combo = 0;
        return -1;
    }
    if (choice < 1 || choice > 7) {
        cout << "Invalid! Turn skipped!" << endl;
        attacker.combo = 0;
        return -1;
    }
    return choice;
}

// ===== Execute Special Move =====
void executeSpecial(Fighter &attacker, Fighter &defender) {
    int baseAttack = attacker.attack;
    if (attacker.status.effect == ST_RAGE) baseAttack *= 2;

    switch (attacker.cls) {
        case CL_WARRIOR: {
            int dmg = rngDamage(baseAttack * 2, 10);
            if (defender.hasShield) dmg /= 2;
            dmg -= defender.defense;
            if (dmg < 1) dmg = 1;
            defender.hp -= dmg;
            attacker.damageDealt += dmg;
            defender.damageTaken += dmg;
            applyStatus(attacker, ST_RAGE, 2, 0);
            cout << "  BERSERK RAGE! " << attacker.name << " deals " << dmg << " damage + ENRAGED for 2 turns!" << endl;
            break;
        }
        case CL_MAGE: {
            int dmg = rngDamage(baseAttack * 3, 12);
            if (defender.hasShield) dmg /= 2;
            dmg -= defender.defense;
            if (dmg < 1) dmg = 1;
            defender.hp -= dmg;
            attacker.damageDealt += dmg;
            defender.damageTaken += dmg;
            applyStatus(defender, ST_BURN, 3, 8);
            cout << "  FIREBALL! " << dmg << " damage + BURN for 3 turns!" << endl;
            break;
        }
        case CL_ROGUE: {
            int dmg = rngDamage(baseAttack * 2, 8);
            dmg *= 2;
            attacker.critsLanded++;
            if (defender.hasShield) dmg /= 2;
            dmg -= defender.defense;
            if (dmg < 1) dmg = 1;
            defender.hp -= dmg;
            attacker.damageDealt += dmg;
            defender.damageTaken += dmg;
            applyStatus(defender, ST_POISON, 4, 6);
            cout << "  SHADOW STRIKE! " << dmg << " damage (guaranteed crit) + POISON for 4 turns!" << endl;
            break;
        }
        case CL_PALADIN: {
            int heal = 40;
            attacker.hp += heal;
            if (attacker.hp > attacker.maxHp) attacker.hp = attacker.maxHp;
            applyStatus(attacker, ST_SHIELD, 2, 0);
            cout << "  HOLY LIGHT! Heals " << heal << " HP + SHIELD for 2 turns!" << endl;
            break;
        }
        case CL_ARCHER: {
            int totalDmg = 0;
            for (int i = 0; i < 3; i++) {
                int dmg = rngDamage(baseAttack * 0.7, 4);
                if (defender.hasShield) dmg /= 2;
                dmg -= defender.defense / 2;
                if (dmg < 1) dmg = 1;
                defender.hp -= dmg;
                totalDmg += dmg;
            }
            attacker.damageDealt += totalDmg;
            defender.damageTaken += totalDmg;
            cout << "  MULTI-SHOT! 3 arrows for " << totalDmg << " total damage!" << endl;
            break;
        }
        case CL_NECROMANCER: {
            int dmg = rngDamage(baseAttack * 2, 8);
            if (defender.hasShield) dmg /= 2;
            dmg -= defender.defense;
            if (dmg < 1) dmg = 1;
            defender.hp -= dmg;
            attacker.damageDealt += dmg;
            defender.damageTaken += dmg;
            int heal = dmg / 2;
            attacker.hp += heal;
            if (attacker.hp > attacker.maxHp) attacker.hp = attacker.maxHp;
            cout << "  LIFE DRAIN! " << dmg << " damage + heals self for " << heal << " HP!" << endl;
            break;
        }
    }
}

// ===== Execute Ultimate =====
void executeUltimate(Fighter &attacker, Fighter &defender) {
    int baseAttack = attacker.attack;
    cout << "  *** ULTIMATE: " << classUltName(attacker.cls) << " ***" << endl;

    switch (attacker.cls) {
        case CL_WARRIOR: {
            int totalDmg = 0;
            for (int i = 0; i < 3; i++) {
                int dmg = rngDamage(baseAttack, 5);
                if (defender.hasShield) dmg /= 2;
                dmg -= defender.defense;
                if (dmg < 1) dmg = 1;
                defender.hp -= dmg;
                totalDmg += dmg;
                cout << "  Hit " << (i+1) << ": " << dmg << " damage!" << endl;
            }
            attacker.damageDealt += totalDmg;
            defender.damageTaken += totalDmg;
            cout << "  WHIRLWIND total: " << totalDmg << " damage!" << endl;
            break;
        }
        case CL_MAGE: {
            int dmg = rngDamage(baseAttack * 5, 20);
            if (defender.hasShield) dmg /= 2;
            dmg -= defender.defense;
            if (dmg < 1) dmg = 1;
            defender.hp -= dmg;
            attacker.damageDealt += dmg;
            defender.damageTaken += dmg;
            applyStatus(defender, ST_BURN, 3, 10);
            if (rollChance(60)) {
                applyStatus(defender, ST_STUN, 1, 0);
                defender.isStunned = true;
                cout << "  METEOR STORM! " << dmg << " damage + BURN + STUN!" << endl;
            } else {
                cout << "  METEOR STORM! " << dmg << " damage + BURN!" << endl;
            }
            break;
        }
        case CL_ROGUE: {
            if (defender.hp < (defender.maxHp * 0.3) && rollChance(50)) {
                cout << "  ASSASSINATE! INSTANT KILL!" << endl;
                defender.hp = 0;
                defender.damageTaken += defender.maxHp;
            } else {
                int dmg = rngDamage(baseAttack * 4, 15);
                if (defender.hasShield) dmg /= 2;
                dmg -= defender.defense;
                if (dmg < 1) dmg = 1;
                defender.hp -= dmg;
                attacker.damageDealt += dmg;
                defender.damageTaken += dmg;
                cout << "  ASSASSINATE! " << dmg << " damage!" << endl;
            }
            break;
        }
        case CL_PALADIN: {
            int dmg = rngDamage(baseAttack * 4, 15);
            if (defender.hasShield) dmg /= 2;
            dmg -= defender.defense;
            if (dmg < 1) dmg = 1;
            defender.hp -= dmg;
            attacker.damageDealt += dmg;
            defender.damageTaken += dmg;
            int heal = 30;
            attacker.hp += heal;
            if (attacker.hp > attacker.maxHp) attacker.hp = attacker.maxHp;
            if (rollChance(50)) {
                applyStatus(defender, ST_STUN, 1, 0);
                defender.isStunned = true;
                cout << "  DIVINE JUDGMENT! " << dmg << " damage + STUN + heals " << heal << " HP!" << endl;
            } else {
                cout << "  DIVINE JUDGMENT! " << dmg << " damage + heals " << heal << " HP!" << endl;
            }
            break;
        }
        case CL_ARCHER: {
            int dmg = rngDamage(baseAttack * 5, 18);
            if (defender.hasShield) dmg /= 2;
            dmg -= defender.defense;
            if (dmg < 1) dmg = 1;
            defender.hp -= dmg;
            attacker.damageDealt += dmg;
            defender.damageTaken += dmg;
            applyStatus(defender, ST_BLEED, 3, 8);
            cout << "  RAIN OF ARROWS! " << dmg << " damage + BLEED for 3 turns!" << endl;
            break;
        }
        case CL_NECROMANCER: {
            int dmg = rngDamage(baseAttack * 6, 20);
            if (defender.hasShield) dmg /= 2;
            dmg -= defender.defense;
            if (dmg < 1) dmg = 1;
            defender.hp -= dmg;
            attacker.damageDealt += dmg;
            defender.damageTaken += dmg;
            applyStatus(defender, ST_POISON, 3, 8);
            applyStatus(defender, ST_WEAKEN, 3, 10);
            cout << "  DEATH CURSE! " << dmg << " damage + POISON + WEAKEN for 3 turns!" << endl;
            break;
        }
    }
}

// ===== Execute Attack =====
void executeAttack(Fighter &attacker, Fighter &defender, int action) {
    int baseAttack = attacker.attack;
    if (attacker.status.effect == ST_WEAKEN) {
        baseAttack -= attacker.status.power;
        if (baseAttack < 1) baseAttack = 1;
    }
    if (attacker.status.effect == ST_RAGE) baseAttack *= 2;

    switch (action) {
        case 1: {
            if (rollChance(5)) {
                cout << "  " << attacker.name << "'s attack MISSED!" << endl;
                attacker.combo = 0;
                return;
            }
            int dmg = rngDamage(baseAttack, 5);
            bool crit = rollChance(15);
            if (crit) { dmg *= 2; attacker.critsLanded++; cout << "  CRITICAL HIT! "; }
            attacker.combo++;
            if (attacker.combo >= 3) {
                int bonus = attacker.combo * 3;
                dmg += bonus;
                cout << "COMBO x" << attacker.combo << " (+" << bonus << ") ";
            }
            if (defender.hasShield) { dmg = dmg / 2; cout << "(Shield halves!) "; }
            dmg -= defender.defense;
            if (dmg < 1) dmg = 1;
            defender.hp -= dmg;
            attacker.damageDealt += dmg;
            defender.damageTaken += dmg;
            cout << attacker.name << " hits " << defender.name << " for " << dmg << " damage!" << endl;
            break;
        }
        case 2: {
            if (attacker.stamina < 15) {
                cout << "  Not enough stamina! (" << attacker.stamina << "/15)" << endl;
                attacker.combo = 0;
                return;
            }
            attacker.stamina -= 15;
            if (rollChance(15)) {
                cout << "  " << attacker.name << "'s heavy attack MISSED!" << endl;
                attacker.combo = 0;
                return;
            }
            int dmg = rngDamage(baseAttack * 2 - 3, 8);
            bool crit = rollChance(10);
            if (crit) { dmg *= 2; attacker.critsLanded++; cout << "  CRITICAL! "; }
            attacker.combo++;
            if (defender.hasShield) dmg = dmg / 2;
            dmg -= defender.defense;
            if (dmg < 1) dmg = 1;
            defender.hp -= dmg;
            attacker.damageDealt += dmg;
            defender.damageTaken += dmg;
            cout << "  " << attacker.name << " uses HEAVY STRIKE for " << dmg << " damage!" << endl;
            break;
        }
        case 3: {
            if (attacker.mana < 25) {
                cout << "  Not enough mana! (" << attacker.mana << "/25)" << endl;
                attacker.combo = 0;
                return;
            }
            attacker.mana -= 25;
            attacker.specialsUsed++;
            attacker.combo++;
            executeSpecial(attacker, defender);
            break;
        }
        case 4: {
            if (attacker.mana < 40 || attacker.stamina < 20) {
                cout << "  Not enough resources! (Need 40 MP + 20 STA)" << endl;
                attacker.combo = 0;
                return;
            }
            attacker.mana -= 40;
            attacker.stamina -= 20;
            attacker.specialsUsed++;
            attacker.combo++;
            executeUltimate(attacker, defender);
            break;
        }
        case 5: {
            if (attacker.potions <= 0) {
                cout << "  No HP potions left!" << endl;
                attacker.combo = 0;
                return;
            }
            attacker.potions--;
            int heal = 50;
            attacker.hp += heal;
            if (attacker.hp > attacker.maxHp) attacker.hp = attacker.maxHp;
            attacker.combo = 0;
            cout << "  " << attacker.name << " drinks a potion! +" << heal << " HP!" << endl;
            break;
        }
        case 6: {
            if (attacker.ethers <= 0) {
                cout << "  No mana potions left!" << endl;
                attacker.combo = 0;
                return;
            }
            attacker.ethers--;
            attacker.mana = min(attacker.mana + 30, attacker.maxMana);
            attacker.combo = 0;
            cout << "  " << attacker.name << " drinks an ether! +30 MP!" << endl;
            break;
        }
        case 7: {
            attacker.defense += 15;
            attacker.mana = min(attacker.mana + 5, attacker.maxMana);
            attacker.stamina = min(attacker.stamina + 10, attacker.maxStamina);
            attacker.combo = 0;
            applyStatus(attacker, ST_SHIELD, 1, 0);
            cout << "  " << attacker.name << " raises guard! +15 DEF +5 MP +10 STA + Shield!" << endl;
            break;
        }
    }
}

// ===== Determine Turn Order =====
int determineTurnOrder(Fighter &p1, Fighter &p2) {
    if (p1.speed > p2.speed) return 1;
    if (p2.speed > p1.speed) return 2;
    return (rand() % 2 == 0) ? 1 : 2;
}

// ===== Process Round =====
void processRound(Fighter &p1, Fighter &p2, int &score1, int &score2, int round) {
    cout << "\n\n===========================================" << endl;
    cout << "           ROUND " << round << " — FIGHT!" << endl;
    cout << "===========================================" << endl;

    p1.combo = 0;    p2.combo = 0;
    p1.isStunned = false; p2.isStunned = false;

    int turnOrder = determineTurnOrder(p1, p2);
    Fighter *first, *second;
    if (turnOrder == 1) { first = &p1; second = &p2; }
    else { first = &p2; second = &p1; }

    cout << first->name << " is faster and goes first!" << endl;

    while (p1.hp > 0 && p2.hp > 0) {
        showBattleScreen(p1, p2);

        if (first->status.turns > 0) processStatus(*first);
        if (first->hp <= 0) break;

        int action1 = playerAction(*first, *second);
        if (action1 > 0) executeAttack(*first, *second, action1);
        if (action1 == 7) { first->defense -= 15; first->hasShield = false; }

        if (second->hp <= 0) break;
        cout << endl;

        if (second->status.turns > 0) processStatus(*second);
        if (second->hp <= 0) break;

        int action2 = playerAction(*second, *first);
        if (action2 > 0) executeAttack(*second, *first, action2);
        if (action2 == 7) { second->defense -= 15; second->hasShield = false; }

        first->mana = min(first->mana + 2, first->maxMana);
        second->mana = min(second->mana + 2, second->maxMana);
        first->stamina = min(first->stamina + 3, first->maxStamina);
        second->stamina = min(second->stamina + 3, second->maxStamina);
        cout << endl;
    }

    cout << "\n===========================================" << endl;
    if (p1.hp <= 0 && p2.hp <= 0) {
        cout << "  DOUBLE KO! It's a DRAW!" << endl;
    } else if (p1.hp <= 0) {
        cout << "  " << p2.name << " WINS ROUND " << round << "!" << endl;
        score2++; p2.gold += 20;
    } else {
        cout << "  " << p1.name << " WINS ROUND " << round << "!" << endl;
        score1++; p1.gold += 20;
    }
    cout << "===========================================" << endl;
    showBattleScreen(p1, p2);

    if (p1.hp <= 0) {
        p1.hp = p1.maxHp; p1.mana = p1.maxMana; p1.stamina = p1.maxStamina;
        p1.status.effect = ST_NONE; p1.status.turns = 0;
        p1.isStunned = false; p1.hasShield = false;
        cout << p1.name << " fully recovers!" << endl;
    }
    if (p2.hp <= 0) {
        p2.hp = p2.maxHp; p2.mana = p2.maxMana; p2.stamina = p2.maxStamina;
        p2.status.effect = ST_NONE; p2.status.turns = 0;
        p2.isStunned = false; p2.hasShield = false;
        cout << p2.name << " fully recovers!" << endl;
    }
}

// ===== Champion Screen =====
void championScreen(Fighter &p1, Fighter &p2, int score1, int score2, string name1, string name2) {
    cout << "\n\n===========================================" << endl;
    cout << "          MATCH COMPLETE!" << endl;
    cout << "===========================================" << endl;
    cout << "  " << name1 << ": " << score1 << "  —  " << name2 << ": " << score2 << endl;
    if (score1 > score2) cout << "\n  " << name1 << " IS THE CHAMPION!" << endl;
    else if (score2 > score1) cout << "\n  " << name2 << " IS THE CHAMPION!" << endl;
    else cout << "\n  IT'S A DRAW!" << endl;

    cout << "\n----- MATCH STATS -----" << endl;
    cout << name1 << " (" << p1.className << "):" << endl;
    cout << "  Damage Dealt: " << p1.damageDealt << " | Taken: " << p1.damageTaken << endl;
    cout << "  Crits: " << p1.critsLanded << " | Specials: " << p1.specialsUsed << endl;
    cout << name2 << " (" << p2.className << "):" << endl;
    cout << "  Damage Dealt: " << p2.damageDealt << " | Taken: " << p2.damageTaken << endl;
    cout << "  Crits: " << p2.critsLanded << " | Specials: " << p2.specialsUsed << endl;
    cout << "===========================================\n" << endl;
}

// ===== Get Match Length =====
int getMatchLength() {
    int choice;
    while (true) {
        cout << "\nMatch length:" << endl;
        cout << "1. Best of 3" << endl;
        cout << "2. Best of 5" << endl;
        cout << "3. Best of 7" << endl;
        cout << "Choice: ";
        cin >> choice;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid!" << endl;
            continue;
        }
        switch (choice) {
            case 1: return 3;
            case 2: return 5;
            case 3: return 7;
            default: cout << "Invalid!" << endl;
        }
    }
}

// ===== Main =====
int main() {
    srand(time(0));

    cout << "===========================================" << endl;
    cout << "   DUEL ARENA — Advanced 2-Player Battle" << endl;
    cout << "   Built by Aanand" << endl;
    cout << "===========================================" << endl;
    cout << "\nTwo players fight in turn-based RPG combat!" << endl;
    cout << "6 classes, special moves, ultimates, items!\n" << endl;

    while (true) {
        string name1, name2;
        cout << "\nEnter Player 1 name: ";
        cin >> name1;
        cout << "Enter Player 2 name: ";
        cin >> name2;

        int matchLen = getMatchLength();
        int winThreshold = (matchLen / 2) + 1;

        Fighter p1, p2;
        p1.name = name1;
        selectClass(p1);
        cout << "\n\n(Switch so other player can't see!)\n" << endl;
        p2.name = name2;
        selectClass(p2);

        cout << "\n===========================================" << endl;
        cout << "  " << name1 << " [" << p1.className << "]" << endl;
        cout << "        VS" << endl;
        cout << "  " << name2 << " [" << p2.className << "]" << endl;
        cout << "  Best of " << matchLen << " (First to " << winThreshold << ")" << endl;
        cout << "===========================================" << endl;

        int score1 = 0, score2 = 0;
        int round = 1;

        while (score1 < winThreshold && score2 < winThreshold) {
            processRound(p1, p2, score1, score2, round);
            round++;

            if (score1 < winThreshold && score2 < winThreshold) {
                cout << "\n--- SHOP PHASE ---" << endl;
                cout << name1 << " Gold: " << p1.gold << " | " << name2 << " Gold: " << p2.gold << endl;
                cout << name1 << ", visit shop? (1=Yes, 0=No): ";
                int s1; cin >> s1;
                if (!cin.fail() && s1 == 1) shop(p1);
                cout << name2 << ", visit shop? (1=Yes, 0=No): ";
                int s2; cin >> s2;
                if (!cin.fail() && s2 == 1) shop(p2);
            }
        }

        championScreen(p1, p2, score1, score2, name1, name2);

        cout << "Play again? (1=Yes, 0=No): ";
        int again;
        cin >> again;
        if (cin.fail() || again == 0) {
            cout << "\nThanks for playing DUEL ARENA! 🎮" << endl;
            break;
        }
    }

    return 0;
}
