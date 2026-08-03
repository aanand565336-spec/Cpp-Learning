/*
 * ============================================
 *   SPACE TERMINAL v1.0 — Futuristic C++ Project
 *   A futuristic space mission control terminal
 *   Learn: functions, loops, if-else, switch, arrays
 * ============================================
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
using namespace std;

// Function declarations
void printBanner();
void printLine(int n);
void typewriterEffect(string text);
void menuSystem();
void launchSequence();
void planetExplorer();
void spaceQuiz();
void cargoCalculator();

// Main
int main() {
    system("clear");  // Clear screen for clean look

    printBanner();
    typewriterEffect("[SYSTEM] Initializing SPACE TERMINAL v1.0...");
    typewriterEffect("[SYSTEM] Welcome aboard, Commander Aanad!");
    cout << endl;

    menuSystem();

    return 0;
}

// Futuristic ASCII banner
void printBanner() {
    printLine(55);
    cout << R"(
   _____            __  __ _____   ____    _   _  _____  ____
  / ____|          |  \/  |  __ \ / __ \  | | | |/ ____|/ __ \
 | (___   ___  ___ | \  / | |__) | |  | | | | | | (___ | |  | |
  \___ \ / _ \/ __|| |\/| |  ___/| |  | | | | | |\___ \| |  | |
  ____) |  __/ (__ | |  | | |    | |__| | | |_| |____) | |__| |
 |_____/ \___|\___||_|  |_|_|     \___\_\  \___/|_____/ \___\_\
)" << endl;
    printLine(55);
    cout << "         [ FUTURISTIC SPACE MISSION CONTROL ]" << endl;
    printLine(55);
    cout << endl;
}

// Print a line of dashes
void printLine(int n) {
    for (int i = 0; i < n; i++) {
        cout << "=";
    }
    cout << endl;
}

// Typewriter text effect
void typewriterEffect(string text) {
    for (int i = 0; i < text.length(); i++) {
        cout << text[i] << flush;
        this_thread::sleep_for(chrono::milliseconds(30));
    }
    cout << endl;
}

// Main menu system
void menuSystem() {
    int choice;
    bool running = true;

    while (running) {
        cout << "\n+--- MAIN MENU -------------------------+" << endl;
        cout << "| 1. 🚀 Launch Rocket Sequence          |" << endl;
        cout << "| 2. 🪐 Explore Planets                 |" << endl;
        cout << "| 3. 🧠 Space Knowledge Quiz           |" << endl;
        cout << "| 4. 📦 Cargo Weight Calculator         |" << endl;
        cout << "| 5. 🚪 Exit Terminal                   |" << endl;
        cout << "+---------------------------------------+" << endl;
        cout << "Enter choice [1-5]: ";
        cin >> choice;

        switch (choice) {
            case 1:
                launchSequence();
                break;
            case 2:
                planetExplorer();
                break;
            case 3:
                spaceQuiz();
                break;
            case 4:
                cargoCalculator();
                break;
            case 5:
                typewriterEffect("[SYSTEM] Shutting down terminal...");
                typewriterEffect("[SYSTEM] Goodbye, Commander Aanad! 🚀");
                running = false;
                break;
            default:
                cout << "[ERROR] Invalid choice. Try again." << endl;
        }
    }
}

// Rocket launch sequence with countdown
void launchSequence() {
    cout << "\n[MISSION] Preparing for launch..." << endl;
    cout << "[MISSION] Fuel: 100% | Systems: GO | Weather: CLEAR" << endl;
    cout << "\nStarting countdown:" << endl;

    for (int i = 10; i >= 1; i--) {
        cout << "\r  T-minus: " << i << "   " << flush;
        this_thread::sleep_for(chrono::milliseconds(800));
    }

    cout << "\n\n";
    typewriterEffect("  🚀 LIFTOFF! Rocket is ascending!");
    typewriterEffect("  [TELEMETRY] Altitude: 10,000m and climbing...");
    typewriterEffect("  [TELEMETRY] Speed: 7.8 km/s (orbital velocity reached)");
    typewriterEffect("  [MISSION] Rocket successfully in orbit! ✅");
    cout << endl;
}

// Planet explorer with data
void planetExplorer() {
    string planets[] = {"Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune"};
    int distances[] = {57, 108, 149, 227, 778, 1427, 2871, 4497};  // million km from sun
    int choice;

    cout << "\n[SCANNER] Available planets to explore:" << endl;
    for (int i = 0; i < 8; i++) {
        cout << "  " << (i+1) << ". " << planets[i] << " (" << distances[i] << "M km from Sun)" << endl;
    }

    cout << "\nSelect planet [1-8]: ";
    cin >> choice;

    if (choice >= 1 && choice <= 8) {
        string planet = planets[choice - 1];
        int dist = distances[choice - 1];

        cout << "\n+--- PLANET DATA -----------------------+" << endl;
        cout << "  Name: " << planet << endl;
        cout << "  Distance from Sun: " << dist << " million km" << endl;

        if (dist < 200) {
            cout << "  Temperature: EXTREME HOT 🔥" << endl;
        } else if (dist < 1000) {
            cout << "  Temperature: MODERATE 🌡️" << endl;
        } else {
            cout << "  Temperature: EXTREME COLD 🧊" << endl;
        }

        cout << "+---------------------------------------+" << endl;
        typewriterEffect("[SCANNER] Scan complete for " + planet);
    } else {
        cout << "[ERROR] Invalid planet selection." << endl;
    }
}

// Space quiz game
void spaceQuiz() {
    int score = 0;
    string answer;

    cout << "\n+--- SPACE QUIZ ------------------------+" << endl;
    cout << "  Test your space knowledge!" << endl;
    cout << "+---------------------------------------+" << endl;

    // Question 1
    cout << "\nQ1: What planet is known as the Red Planet?" << endl;
    cout << "Answer: ";
    cin >> answer;
    if (answer == "Mars" || answer == "mars") {
        cout << "  ✅ Correct!" << endl;
        score++;
    } else {
        cout << "  ❌ Wrong! Answer was Mars." << endl;
    }

    // Question 2
    cout << "\nQ2: What is the largest planet in our solar system?" << endl;
    cout << "Answer: ";
    cin >> answer;
    if (answer == "Jupiter" || answer == "jupiter") {
        cout << "  ✅ Correct!" << endl;
        score++;
    } else {
        cout << "  ❌ Wrong! Answer was Jupiter." << endl;
    }

    // Question 3
    cout << "\nQ3: How many planets are in our solar system?" << endl;
    cout << "Answer: ";
    cin >> answer;
    if (answer == "8" || answer == "eight") {
        cout << "  ✅ Correct!" << endl;
        score++;
    } else {
        cout << "  ❌ Wrong! Answer was 8." << endl;
    }

    // Results
    cout << "\n+--- QUIZ RESULTS ----------------------+" << endl;
    cout << "  Score: " << score << "/3" << endl;
    if (score == 3) {
        cout << "  🏆 PERFECT! You are a Space Expert!" << endl;
    } else if (score >= 2) {
        cout << "  ⭐ Great job! Keep learning!" << endl;
    } else {
        cout << "  📚 Keep studying space, Commander!" << endl;
    }
    cout << "+---------------------------------------+" << endl;
}

// Cargo weight calculator
void cargoCalculator() {
    int crates;
    double weightPerCrate, total;

    cout << "\n[CARGO] Space Station Cargo Calculator" << endl;
    cout << "Enter number of cargo crates: ";
    cin >> crates;
    cout << "Enter weight per crate (kg): ";
    cin >> weightPerCrate;

    total = crates * weightPerCrate;

    cout << "\n+--- CARGO MANIFEST --------------------+" << endl;
    cout << "  Crates: " << crates << endl;
    cout << "  Per crate: " << weightPerCrate << " kg" << endl;
    cout << "  Total weight: " << total << " kg" << endl;

    if (total > 50000) {
        cout << "  ⚠️  WARNING: Exceeds 50,000 kg limit!" << endl;
        cout << "  Suggestion: Reduce cargo or use heavy-lift rocket." << endl;
    } else {
        cout << "  ✅ Within weight limits. Safe to launch." << endl;
    }
    cout << "+---------------------------------------+" << endl;
}
