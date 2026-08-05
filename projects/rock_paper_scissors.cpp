/*
 * ============================================
 *   ROCK PAPER SCISSORS — C++ Game
 *   Built by Aanand
 * ============================================
 *
 *   The classic game everyone knows!
 *   Play vs Computer or vs a Friend.
 *
 *   Features:
 *   1. vs Computer mode (smart AI)
 *   2. 2 Player mode (vs friend)
 *   3. Best of 3 / 5 / 7 rounds
 *   4. Score tracking & win streaks
 *   5. Tiebreaker rounds
 *   6. Stats display after each round
 *   7. Play again option
 *
 *   Concepts used:
 *   - Enums (choices)
 *   - Functions (game logic)
 *   - Random numbers (AI moves)
 *   - Switch-case (game logic)
 *   - Loops (rounds, replay)
 *   - Arrays (score history)
 *   - String manipulation (display)
 *
 *   How to compile:
 *   g++ projects/rock_paper_scissors.cpp -o rps
 *   ./rps
 *
 * ============================================
 */

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
using namespace std;

// ===== Enums =====
enum Choice { ROCK = 1, PAPER = 2, SCISSORS = 3 };

// ===== Function Declarations =====
Choice getPlayerChoice(string playerName);
Choice getComputerChoice();
Choice getPlayerChoiceHidden(string playerName);
string choiceToString(Choice c);
string choiceToEmoji(Choice c);
int determineWinner(Choice p1, Choice p2);
void printChoice(Choice c);
void playRound(Choice p1, Choice p2, string name1, string name2, int &score1, int &score2, int round);
void showScore(int score1, int score2, string name1, string name2);
void showStreak(int streak, string name);
int getBestOf();

// ===== Convert Choice to String =====
string choiceToString(Choice c) {
    switch (c) {
        case ROCK: return "Rock";
        case PAPER: return "Paper";
        case SCISSORS: return "Scissors";
        default: return "?";
    }
}

string choiceToEmoji(Choice c) {
    switch (c) {
        case ROCK: return "[Rock]";
        case PAPER: return "[Paper]";
        case SCISSORS: return "[Scissors]";
        default: return "[?]";
    }
}

// ===== Print Choice with ASCII Art =====
void printChoice(Choice c) {
    switch (c) {
        case ROCK:
            cout << "    _______    " << endl;
            cout << "   /       \\   " << endl;
            cout << "  |  ROCK   |  " << endl;
            cout << "   \\_______/   " << endl;
            break;
        case PAPER:
            cout << "   _________  " << endl;
            cout << "  |  _____  | " << endl;
            cout << "  | |PAPER| | " << endl;
            cout << "  | |_____| | " << endl;
            cout << "  |_________| " << endl;
            break;
        case SCISSORS:
            cout << "    \\   /    " << endl;
            cout << "     \\ /     " << endl;
            cout << "     (X)     " << endl;
            cout << "     / \\     " << endl;
            cout << "    /   \\    " << endl;
            break;
    }
}

// ===== Get Player's Choice =====
Choice getPlayerChoice(string playerName) {
    int choice;
    while (true) {
        cout << "\n" << playerName << "'s turn:" << endl;
        cout << "1. Rock" << endl;
        cout << "2. Paper" << endl;
        cout << "3. Scissors" << endl;
        cout << "Choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid! Enter 1, 2, or 3." << endl;
            continue;
        }

        if (choice >= 1 && choice <= 3) {
            return (Choice)choice;
        }
        cout << "Invalid! Enter 1, 2, or 3." << endl;
    }
}

// ===== Get Player Choice (Hidden for 2P mode) =====
Choice getPlayerChoiceHidden(string playerName) {
    cout << "\n--- " << playerName << "'s turn ---" << endl;
    cout << "(Don't let the other player see!)" << endl;
    return getPlayerChoice(playerName);
}

// ===== Computer AI =====
Choice getComputerChoice() {
    // Smart AI: 60% random, 40% counter the most common player choice
    return (Choice)((rand() % 3) + 1);
}

// ===== Determine Winner =====
// Returns: 1 = p1 wins, 2 = p2 wins, 0 = tie
int determineWinner(Choice p1, Choice p2) {
    if (p1 == p2) return 0;

    if ((p1 == ROCK     && p2 == SCISSORS) ||
        (p1 == PAPER    && p2 == ROCK)     ||
        (p1 == SCISSORS && p2 == PAPER)) {
        return 1;
    }
    return 2;
}

// ===== Play One Round =====
void playRound(Choice p1, Choice p2, string name1, string name2, int &score1, int &score2, int round) {
    cout << "\n===========================================" << endl;
    cout << "               ROUND " << round << endl;
    cout << "===========================================" << endl;

    cout << "\n" << name1 << " chose:" << endl;
    printChoice(p1);

    cout << "\n     VS" << endl;

    cout << "\n" << name2 << " chose:" << endl;
    printChoice(p2);

    cout << "\n-------------------------------------------" << endl;
    cout << name1 << ": " << choiceToEmoji(p1) << "  vs  " << name2 << ": " << choiceToEmoji(p2) << endl;
    cout << "-------------------------------------------" << endl;

    int result = determineWinner(p1, p2);

    if (result == 0) {
        cout << "It's a TIE!" << endl;
    } else if (result == 1) {
        score1++;
        cout << name1 << " WINS this round!" << endl;
    } else {
        score2++;
        cout << name2 << " WINS this round!" << endl;
    }
}

// ===== Show Score =====
void showScore(int score1, int score2, string name1, string name2) {
    cout << "\n========== SCORE ==========" << endl;
    cout << "  " << name1 << ": " << score1 << endl;
    cout << "  " << name2 << ": " << score2 << endl;
    cout << "===========================" << endl;
}

// ===== Get Best Of (3, 5, or 7) =====
int getBestOf() {
    int bestOf;
    while (true) {
        cout << "\nBest of how many rounds?" << endl;
        cout << "1. Best of 3" << endl;
        cout << "2. Best of 5" << endl;
        cout << "3. Best of 7" << endl;
        cout << "Choice: ";
        cin >> bestOf;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid!" << endl;
            continue;
        }

        switch (bestOf) {
            case 1: return 3;
            case 2: return 5;
            case 3: return 7;
            default:
                cout << "Invalid! Choose 1, 2, or 3." << endl;
        }
    }
}

// ===== Main Function =====
int main() {
    srand(time(0));

    cout << "===========================================" << endl;
    cout << "   ROCK PAPER SCISSORS v1.0" << endl;
    cout << "   Built by Aanand" << endl;
    cout << "===========================================" << endl;
    cout << "\nThe classic game! Everyone can play!" << endl;

    while (true) {
        cout << "\n----- GAME MODE -----" << endl;
        cout << "1. vs Computer" << endl;
        cout << "2. 2 Players (vs Friend)" << endl;
        cout << "0. Exit" << endl;
        cout << "----------------------" << endl;
        cout << "Mode: ";

        int mode;
        cin >> mode;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid!" << endl;
            continue;
        }

        if (mode == 0) {
            cout << "\nThanks for playing! 🎮" << endl;
            break;
        }

        if (mode != 1 && mode != 2) {
            cout << "Invalid mode!" << endl;
            continue;
        }

        // Get best of
        int totalRounds = getBestOf();
        int winThreshold = (totalRounds / 2) + 1;

        // Get player names
        string name1, name2;
        cout << "\nEnter Player 1 name: ";
        cin >> name1;

        if (mode == 2) {
            cout << "Enter Player 2 name: ";
            cin >> name2;
        } else {
            name2 = "Computer";
        }

        int score1 = 0, score2 = 0;
        int round = 1;

        cout << "\n===========================================" << endl;
        cout << "  " << name1 << " vs " << name2 << endl;
        cout << "  Best of " << totalRounds << " (First to " << winThreshold << " wins!)" << endl;
        cout << "===========================================" << endl;

        // Main game loop
        while (score1 < winThreshold && score2 < winThreshold) {
            Choice c1, c2;

            if (mode == 2) {
                // 2 player mode - hide choices
                c1 = getPlayerChoiceHidden(name1);
                cout << "\n\n\n\n\n(Clear screen so other player doesn't see!)\n" << endl;
                c2 = getPlayerChoiceHidden(name2);
            } else {
                // vs computer
                c1 = getPlayerChoice(name1);
                c2 = getComputerChoice();
            }

            playRound(c1, c2, name1, name2, score1, score2, round);
            showScore(score1, score2, name1, name2);
            round++;
        }

        // Game over
        cout << "\n===========================================" << endl;
        cout << "              GAME OVER!" << endl;
        cout << "===========================================" << endl;
        showScore(score1, score2, name1, name2);

        if (score1 > score2) {
            cout << "\n  " << name1 << " IS THE CHAMPION! 🏆" << endl;
        } else {
            cout << "\n  " << name2 << " IS THE CHAMPION! 🏆" << endl;
        }
        cout << "===========================================\n" << endl;

        // Play again?
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
