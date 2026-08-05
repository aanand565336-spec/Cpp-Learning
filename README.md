# 🚀 Cpp-Learning

> My C++ learning journey — from Hello World to full projects!

<div align="center">

<img src="https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" />
<img src="https://img.shields.io/badge/Status-Active%20Learning-success?style=for-the-badge" />
<img src="https://img.shields.io/badge/Exercises-3+-blue?style=for-the-badge" />
<img src="https://img.shields.io/badge/Projects-4+-blueviolet?style=for-the-badge" />

</div>

---

## 📚 Exercises

| # | File | Topic | Status |
|---|---|---|---|
| 01 | `01_hello_world.cpp` | First C++ program | ✅ |
| 02 | `02_variables.cpp` | Variables & data types | ✅ |
| 03 | `03_user_input.cpp` | User input with `cin` | ✅ |

## 🔥 Projects

| Project | Description | File |
|---|---|---|
| Space Terminal v1.0 | Futuristic space mission control terminal | `projects/space_terminal.cpp` |
| Calculator v1.0 | Full-featured calculator with memory & history | `projects/calculator.cpp` |
| Tic Tac Toe v1.0 | Play vs friend or smart AI computer | `projects/tic_tac_toe.cpp` |
| Battle Arena v1.0 | RPG battle game with waves, levels & combat | `projects/battle_game.cpp` |

---

## 🗡️ Battle Arena v1.0 — Featured Game

A text-based RPG battle game. Fight through waves of enemies, level up, and survive!

### Features
1. *Turn-Based Combat* — Player vs Enemy, take turns attacking
2. *Attack Types* — Light Attack, Heavy Attack, Special Move
3. *Critical Hits* — 20% chance for double damage
4. *Dodge System* — Enemies can miss, you can dodge
5. *Mana System* — Special moves cost mana, regenerates over time
6. *Health Potions* — Heal up when low on HP
7. *Defend* — Reduce incoming damage for one turn
8. *Wave System* — 10 different enemies, each stronger than the last
9. *Level Up* — Every 2 waves, get stronger stats and full heal
10. *Score Tracking* — Earn points per wave, beat your high score

### Enemies
Goblin → Skeleton → Dark Knight → Orc Warrior → Shadow Mage → Ice Golem → Fire Demon → Dragon Lord → Undead King → Final Boss

### Concepts Used
- Structs (player & enemy data structures)
- Functions (modular game logic)
- Random numbers (damage variance, crits, dodges)
- Switch-case (action menu)
- Loops (battle loop, wave system)
- Health/Mana bar display
- Input validation
- Game state management

### How to Run
```bash
g++ projects/battle_game.cpp -o battle
./battle
```

---

## 📈 Learning Roadmap

- [x] Hello World
- [x] Variables & Data Types
- [x] User Input
- [x] Functions
- [x] Loops
- [x] Arrays
- [x] Switch-case
- [x] Math library (sqrt, pow)
- [x] 2D Arrays
- [x] Random numbers
- [x] Game logic & AI
- [x] Structs
- [ ] Data Structures & Algorithms
- [ ] Object-Oriented Programming
- [ ] File I/O
- [ ] Pointers & Memory

---

## 🏃 How to Run

```bash
# Compile any exercise
g++ 01_hello_world.cpp -o hello
./hello

# Compile any project
g++ projects/space_terminal.cpp -o space_terminal
g++ projects/calculator.cpp -o calculator -lm
g++ projects/tic_tac_toe.cpp -o ttt
g++ projects/battle_game.cpp -o battle
```

---

*⭐ Star this repo if you found it helpful!*

*Built with passion by [Aanad](https://github.com/aanand565336-spec)*
