# 🚀 Cpp-Learning

> My C++ learning journey — from Hello World to full projects!

<div align="center">

<img src="https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" />
<img src="https://img.shields.io/badge/Status-Active%20Learning-success?style=for-the-badge" />
<img src="https://img.shields.io/badge/Exercises-3+-blue?style=for-the-badge" />
<img src="https://img.shields.io/badge/Projects-3+-blueviolet?style=for-the-badge" />

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

---

## 🚀 Space Terminal v1.0

A fully interactive C++ terminal application:

1. *Rocket Launch* — Live countdown with launch sequence
2. *Planet Explorer* — All 8 planets with real distance data
3. *Space Quiz* — Test your space knowledge
4. *Cargo Calculator* — Weight calculator with safety limits

### Concepts Used
- Functions, Arrays, Loops, Switch-case, Thread sleep, ASCII art

---

## 🧮 Calculator v1.0

A full-featured command-line calculator.

### Features
1. Basic Operations — Add, Subtract, Multiply, Divide
2. Advanced Math — Power, Square Root, Modulus, Factorial
3. Percentage Calculator
4. Memory Functions — M+, M-, MR, MC
5. History — Shows last 5 calculations
6. Error Handling — Division by zero, negative sqrt, overflow protection

### How to Run
```bash
g++ projects/calculator.cpp -o calculator -lm
./calculator
```

---

## 🎮 Tic Tac Toe v1.0

Play the classic Tic Tac Toe game — vs a friend or vs a smart AI!

### Features
1. *2 Player Mode* — Play with a friend, take turns
2. *vs Computer* — AI that tries to win, blocks your moves, takes center/corners
3. *Input Validation* — Handles invalid slots and bad input
4. *Win Detection* — Checks rows, columns, and diagonals
5. *Draw Detection* — Knows when the board is full

### Concepts Used
- 2D Arrays (game board)
- Functions (game logic)
- Loops (game loop, win checking)
- Conditionals (win/draw detection)
- Random numbers (computer moves)
- Input validation (cin.fail handling)

### How to Run
```bash
g++ projects/tic_tac_toe.cpp -o ttt
./ttt
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

# Compile the Space Terminal
g++ projects/space_terminal.cpp -o space_terminal
./space_terminal

# Compile the Calculator
g++ projects/calculator.cpp -o calculator -lm
./calculator

# Compile Tic Tac Toe
g++ projects/tic_tac_toe.cpp -o ttt
./ttt
```

---

*⭐ Star this repo if you found it helpful!*

*Built with passion by [Aanad](https://github.com/aanand565336-spec)*
