/*
 * ============================================
 *   TIC TAC TOE — C++ Game
 *   Built by Aanand
 * ============================================
 *
 *   Play vs Computer (AI) or 2 Players!
 *
 *   Concepts used:
 *   - 2D Arrays (game board)
 *   - Functions (game logic)
 *   - Loops (game loop, win checking)
 *   - Conditionals (win/draw detection)
 *   - Random numbers (computer moves)
 *   - Input validation
 *
 *   How to compile:
 *   g++ projects/tic_tac_toe.cpp -o ttt
 *   ./ttt
 *
 * ============================================
 */

#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

// ===== Global Game Board (3x3 grid) =====
char board[3][3] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'}
};

int mode;       // 1 = 2 players, 2 = vs computer
char currentMarker;
int currentPlayer;

// ===== Function Declarations =====
void drawBoard();
bool placeMarker(int slot);
int checkWinner();
void switchPlayer();
void computerMove();
void resetBoard();
void playGame();

// ===== Draw the Game Board =====
void drawBoard() {
    cout << "\n";
    cout << "     |     |     " << endl;
    cout << "  " << board[0][0] << "  |  " << board[0][1] << "  |  " << board[0][2] << "  " << endl;
    cout << "_____|_____|_____" << endl;
    cout << "     |     |     " << endl;
    cout << "  " << board[1][0] << "  |  " << board[1][1] << "  |  " << board[1][2] << "  " << endl;
    cout << "_____|_____|_____" << endl;
    cout << "     |     |     " << endl;
    cout << "  " << board[2][0] << "  |  " << board[2][1] << "  |  " << board[2][2] << "  " << endl;
    cout << "     |     |     " << endl;
    cout << "\n";
}

// ===== Place Marker on Board =====
bool placeMarker(int slot) {
    int row = (slot - 1) / 3;
    int col = (slot - 1) % 3;

    // Check if slot is valid and not taken
    if (slot < 1 || slot > 9) {
        cout << "Invalid slot! Choose 1-9." << endl;
        return false;
    }

    if (board[row][col] != ('0' + slot)) {
        cout << "Slot already taken! Try again." << endl;
        return false;
    }

    board[row][col] = currentMarker;
    return true;
}

// ===== Check for Winner =====
// Returns: 1 = player 1 wins, 2 = player 2/computer wins, 0 = no winner yet, -1 = draw
int checkWinner() {
    // Check all rows
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2]) {
            return currentPlayer;
        }
    }

    // Check all columns
    for (int j = 0; j < 3; j++) {
        if (board[0][j] == board[1][j] && board[1][j] == board[2][j]) {
            return currentPlayer;
        }
    }

    // Check diagonals
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
        return currentPlayer;
    }
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
        return currentPlayer;
    }

    // Check for draw (all slots taken)
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] != 'X' && board[i][j] != 'O') {
                return 0;  // Game still going
            }
        }
    }

    return -1;  // Draw
}

// ===== Switch Player Turn =====
void switchPlayer() {
    if (currentMarker == 'X') {
        currentMarker = 'O';
    } else {
        currentMarker = 'X';
    }

    if (mode == 1) {
        currentPlayer = (currentPlayer == 1) ? 2 : 1;
    } else {
        currentPlayer = (currentPlayer == 1) ? 2 : 1;
    }
}

// ===== Computer AI Move =====
void computerMove() {
    cout << "Computer is thinking..." << endl;

    // Try to win first
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] != 'X' && board[i][j] != 'O') {
                char temp = board[i][j];
                board[i][j] = 'O';
                // Check if computer wins
                if (board[i][0] == 'O' && board[i][1] == 'O' && board[i][2] == 'O') {
                    return;
                }
                if (board[0][j] == 'O' && board[1][j] == 'O' && board[2][j] == 'O') {
                    return;
                }
                if (i == j && board[0][0] == 'O' && board[1][1] == 'O' && board[2][2] == 'O') {
                    return;
                }
                if (i + j == 2 && board[0][2] == 'O' && board[1][1] == 'O' && board[2][0] == 'O') {
                    return;
                }
                board[i][j] = temp;
            }
        }
    }

    // Try to block player from winning
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] != 'X' && board[i][j] != 'O') {
                char temp = board[i][j];
                board[i][j] = 'X';
                // Check if player would win
                if (board[i][0] == 'X' && board[i][1] == 'X' && board[i][2] == 'X') {
                    board[i][j] = 'O';
                    return;
                }
                if (board[0][j] == 'X' && board[1][j] == 'X' && board[2][j] == 'X') {
                    board[i][j] = 'O';
                    return;
                }
                if (i == j && board[0][0] == 'X' && board[1][1] == 'X' && board[2][2] == 'X') {
                    board[i][j] = 'O';
                    return;
                }
                if (i + j == 2 && board[0][2] == 'X' && board[1][1] == 'X' && board[2][0] == 'X') {
                    board[i][j] = 'O';
                    return;
                }
                board[i][j] = temp;
            }
        }
    }

    // Take center if available
    if (board[1][1] != 'X' && board[1][1] != 'O') {
        board[1][1] = 'O';
        return;
    }

    // Take a corner if available
    int corners[4][2] = {{0,0}, {0,2}, {2,0}, {2,2}};
    for (int i = 0; i < 4; i++) {
        int r = corners[i][0];
        int c = corners[i][1];
        if (board[r][c] != 'X' && board[r][c] != 'O') {
            board[r][c] = 'O';
            return;
        }
    }

    // Random move (fallback)
    while (true) {
        int slot = (rand() % 9) + 1;
        int row = (slot - 1) / 3;
        int col = (slot - 1) % 3;
        if (board[row][col] != 'X' && board[row][col] != 'O') {
            board[row][col] = 'O';
            return;
        }
    }
}

// ===== Reset Board for New Game =====
void resetBoard() {
    char num = '1';
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = num++;
        }
    }
}

// ===== Main Game Loop =====
void playGame() {
    int slot;
    int winner = 0;
    int turns = 0;

    drawBoard();

    while (winner == 0) {
        if (mode == 2 && currentPlayer == 2) {
            // Computer's turn
            computerMove();
            winner = checkWinner();
            if (winner == 0) {
                drawBoard();
                switchPlayer();
                turns++;
            }
        } else {
            // Human turn
            cout << "Player " << currentPlayer << " (" << currentMarker << ") - enter a slot (1-9): ";
            cin >> slot;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid input! Please enter a number 1-9." << endl;
                continue;
            }

            if (placeMarker(slot)) {
                drawBoard();
                winner = checkWinner();
                if (winner == 0) {
                    switchPlayer();
                    turns++;
                }
            }
        }

        if (winner == -1) {
            cout << "\n==================================" << endl;
            cout << "   IT'S A DRAW! Well played!" << endl;
            cout << "==================================" << endl;
            return;
        }
    }

    if (winner == 1) {
        cout << "\n==================================" << endl;
        cout << "   PLAYER 1 (X) WINS! 🎉" << endl;
        cout << "==================================" << endl;
    } else {
        if (mode == 2) {
            cout << "\n==================================" << endl;
            cout << "   COMPUTER (O) WINS! 🤖" << endl;
            cout << "==================================" << endl;
        } else {
            cout << "\n==================================" << endl;
            cout << "   PLAYER 2 (O) WINS! 🎉" << endl;
            cout << "==================================" << endl;
        }
    }
}

// ===== Main Function =====
int main() {
    srand(time(0));  // Seed random number generator

    cout << "==================================" << endl;
    cout << "   TIC TAC TOE v1.0" << endl;
    cout << "   Built by Aanand" << endl;
    cout << "==================================" << endl;

    while (true) {
        cout << "\n----- GAME MENU -----" << endl;
        cout << "1. 2 Players (Player vs Player)" << endl;
        cout << "2. vs Computer (AI)" << endl;
        cout << "0. Exit" << endl;
        cout << "---------------------" << endl;
        cout << "Choose mode: ";
        cin >> mode;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Try again." << endl;
            continue;
        }

        if (mode == 0) {
            cout << "\nThanks for playing! Keep coding! 🚀" << endl;
            break;
        }

        if (mode != 1 && mode != 2) {
            cout << "Invalid choice! Try again." << endl;
            continue;
        }

        // Reset for new game
        resetBoard();
        currentMarker = 'X';
        currentPlayer = 1;

        playGame();
    }

    return 0;
}
