/*
 * ============================================
 *   C++ Calculator — Phase 3 Project
 *   Built by Aanand
 * ============================================
 *
 *   Features:
 *   1. Addition, Subtraction, Multiplication, Division
 *   2. Power (exponent)
 *   3. Square root
 *   4. Modulus (remainder)
 *   5. Factorial
 *   6. Percentage calculator
 *   7. Memory functions (M+, M-, MR, MC)
 *   8. History of last 5 calculations
 *
 *   Concepts used:
 *   - Functions (modular design)
 *   - Switch-case (menu navigation)
 *   - Loops (continuous operation)
 *   - Arrays (history storage)
 *   - Math library (sqrt, pow)
 *
 *   How to compile:
 *   g++ projects/calculator.cpp -o calculator -lm
 *   ./calculator
 *
 * ============================================
 */

#include <iostream>
#include <cmath>
#include <string>
using namespace std;

// ===== Global Variables =====
double memory = 0;
double history[5];
string historyOps[5];
int historyCount = 0;

// ===== Function Declarations =====
double add(double a, double b);
double subtract(double a, double b);
double multiply(double a, double b);
double divide(double a, double b);
double power(double base, double exp);
double squareRoot(double n);
int modOp(int a, int b);
long long factorial(int n);
double percentage(double value, double percent);
void showHistory();
void addToHistory(string op, double result);

// ===== Main Function =====
int main() {
    cout << "==================================" << endl;
    cout << "   C++ CALCULATOR v1.0" << endl;
    cout << "   Built by Aanand" << endl;
    cout << "==================================" << endl;

    int choice;
    double a, b, result;
    int x, y, n;

    while (true) {
        cout << "\n----- MAIN MENU -----" << endl;
        cout << "1.  Addition (+)" << endl;
        cout << "2.  Subtraction (-)" << endl;
        cout << "3.  Multiplication (*)" << endl;
        cout << "4.  Division (/)" << endl;
        cout << "5.  Power (x^y)" << endl;
        cout << "6.  Square Root" << endl;
        cout << "7.  Modulus (%)" << endl;
        cout << "8.  Factorial (!)" << endl;
        cout << "9.  Percentage" << endl;
        cout << "10. Show History" << endl;
        cout << "11. Memory: M+ (add to memory)" << endl;
        cout << "12. Memory: M- (subtract from memory)" << endl;
        cout << "13. Memory: MR (recall)" << endl;
        cout << "14. Memory: MC (clear)" << endl;
        cout << "0.  Exit" << endl;
        cout << "---------------------" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Enter two numbers: ";
                cin >> a >> b;
                result = add(a, b);
                cout << "Result: " << a << " + " << b << " = " << result << endl;
                addToHistory(to_string(a) + " + " + to_string(b), result);
                break;

            case 2:
                cout << "Enter two numbers: ";
                cin >> a >> b;
                result = subtract(a, b);
                cout << "Result: " << a << " - " << b << " = " << result << endl;
                addToHistory(to_string(a) + " - " + to_string(b), result);
                break;

            case 3:
                cout << "Enter two numbers: ";
                cin >> a >> b;
                result = multiply(a, b);
                cout << "Result: " << a << " * " << b << " = " << result << endl;
                addToHistory(to_string(a) + " * " + to_string(b), result);
                break;

            case 4:
                cout << "Enter two numbers: ";
                cin >> a >> b;
                if (b == 0) {
                    cout << "ERROR: Cannot divide by zero!" << endl;
                } else {
                    result = divide(a, b);
                    cout << "Result: " << a << " / " << b << " = " << result << endl;
                    addToHistory(to_string(a) + " / " + to_string(b), result);
                }
                break;

            case 5:
                cout << "Enter base and exponent: ";
                cin >> a >> b;
                result = power(a, b);
                cout << "Result: " << a << "^" << b << " = " << result << endl;
                addToHistory(to_string(a) + "^" + to_string(b), result);
                break;

            case 6:
                cout << "Enter a number: ";
                cin >> a;
                if (a < 0) {
                    cout << "ERROR: Cannot find square root of negative number!" << endl;
                } else {
                    result = squareRoot(a);
                    cout << "Result: sqrt(" << a << ") = " << result << endl;
                    addToHistory("sqrt(" + to_string(a) + ")", result);
                }
                break;

            case 7:
                cout << "Enter two integers: ";
                cin >> x >> y;
                if (y == 0) {
                    cout << "ERROR: Cannot mod by zero!" << endl;
                } else {
                    int modResult = modOp(x, y);
                    cout << "Result: " << x << " % " << y << " = " << modResult << endl;
                    addToHistory(to_string(x) + " % " + to_string(y), modResult);
                }
                break;

            case 8:
                cout << "Enter a non-negative integer: ";
                cin >> n;
                if (n < 0) {
                    cout << "ERROR: Factorial of negative number doesn't exist!" << endl;
                } else if (n > 20) {
                    cout << "ERROR: Too large! Use a number <= 20" << endl;
                } else {
                    long long fact = factorial(n);
                    cout << "Result: " << n << "! = " << fact << endl;
                    addToHistory(to_string(n) + "!", (double)fact);
                }
                break;

            case 9:
                cout << "Enter value and percentage: ";
                cin >> a >> b;
                result = percentage(a, b);
                cout << "Result: " << b << "% of " << a << " = " << result << endl;
                addToHistory(to_string(b) + "% of " + to_string(a), result);
                break;

            case 10:
                showHistory();
                break;

            case 11:
                cout << "Enter value to add to memory: ";
                cin >> a;
                memory += a;
                cout << "Memory updated: M = " << memory << endl;
                break;

            case 12:
                cout << "Enter value to subtract from memory: ";
                cin >> a;
                memory -= a;
                cout << "Memory updated: M = " << memory << endl;
                break;

            case 13:
                cout << "Memory recall: M = " << memory << endl;
                break;

            case 14:
                memory = 0;
                cout << "Memory cleared!" << endl;
                break;

            case 0:
                cout << "\nThanks for using C++ Calculator!" << endl;
                cout << "Keep coding, Aanand!" << endl;
                return 0;

            default:
                cout << "Invalid choice! Please try again." << endl;
        }
    }
    return 0;
}

// ===== Function Definitions =====
double add(double a, double b) { return a + b; }
double subtract(double a, double b) { return a - b; }
double multiply(double a, double b) { return a * b; }
double divide(double a, double b) { return a / b; }
double power(double base, double exp) { return pow(base, exp); }
double squareRoot(double n) { return sqrt(n); }
int modOp(int a, int b) { return a % b; }

long long factorial(int n) {
    if (n <= 1) return 1;
    long long result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

double percentage(double value, double percent) {
    return (value * percent) / 100.0;
}

void addToHistory(string op, double result) {
    if (historyCount >= 5) {
        for (int i = 0; i < 4; i++) {
            history[i] = history[i + 1];
            historyOps[i] = historyOps[i + 1];
        }
        history[4] = result;
        historyOps[4] = op + " = " + to_string(result);
    } else {
        history[historyCount] = result;
        historyOps[historyCount] = op + " = " + to_string(result);
        historyCount++;
    }
}

void showHistory() {
    if (historyCount == 0) {
        cout << "No calculations yet!" << endl;
        return;
    }
    cout << "\n----- LAST 5 CALCULATIONS -----" << endl;
    for (int i = 0; i < historyCount; i++) {
        cout << (i + 1) << ". " << historyOps[i] << endl;
    }
    cout << "-------------------------------" << endl;
}
