#include <getopt.h>
#include <iostream>
#include <string>
#include <chrono>
#include <fstream>
#include <vector>
#include <iomanip>
#include <numeric>

#define WIN_THRESHOLD 1000000000
#define MAX_RUNS      100

using namespace std;

void printHelp();
bool fileExists(const char*);
void run();

unsigned long long player1Time = 0;
unsigned long long player2Time = 0;
unsigned int runs = 0;

string player1;
string player2;
string winner;

bool noWinner = false;

int main(int argc, char** argv) {
    const char* const short_opts = "a:b:h";
    const option long_opts[] = {
            {"player1", required_argument, nullptr, 'a'},
            {"player2", required_argument, nullptr, 'b'},
            {"help", no_argument, nullptr, 'h'},
            {nullptr, no_argument, nullptr, 0}
    };

    // Print help if no arguments are provided
    if (argc == 0) {
        printHelp();
        exit(1);
    }

    // Make sure that logs don't already exists from a previous race
    system("rm -f ./player1.log");
    system("rm -f ./player2.log");

    while (true) {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

        if (-1 == opt) {
            break;
        }

        switch (opt) {
            case 'a':
                player1 = string(optarg);

                if (!fileExists(player1.c_str())) {
                    cout << "Player1 does not exist: " << player1 << endl;
                } else {
                    cout << "Player1: " << player1 << endl;
                }

                break;
            case 'b':
                player2 = string(optarg);

                if (!fileExists(player2.c_str())) {
                    cout << "Player2 does not exist: " << player2 << endl;
                } else {
                    cout << "Player2: " << player2 << endl;
                }

                break;
            case 'h':
            case '?':
            default:
                printHelp();
                break;
        }
    }

    // Only run if we have two valid files
    if (fileExists(player1.c_str()) && fileExists(player2.c_str())) {
        cout << "Running..." << endl;
        run();

        cout << "After " << runs << " runs ";

        if (noWinner) {
            cout << "we have a tie. How anti-climactic..." << endl;
        } else {
            cout << winner << " is victorious! WINNER WINNER CHICKEN DINNER!!!!" << endl;
        }

    } else {
        cout << "Not running because we don't have two players" << endl;
    }

    return 0;
}

void printHelp() {
    cout <<
         "--player1 <filename>:   Path to any executable program\n"
         "--player2 <filename>:   Path to any executable program\n"
         "--help:                 Show help\n";
}

bool fileExists(const char* filename) {
    std::ifstream file(filename);

    if(!file.is_open()) {
        return false;
    }

    return true;
}

bool done() {
    unsigned long long diff;

    if (runs > 0) {
        cout << "Run " << runs << ": ";
    }

    if (player1Time > player2Time) {
        diff = player1Time - player2Time;

        if (diff >= WIN_THRESHOLD) {
            winner = "Player 2";
            return true;
        } else {
            cout << "Player 2 is in the lead by " << diff << " nanoseconds" << endl;
        }
    } else if (player2Time > player1Time) {
        diff = player2Time - player1Time;

        if (diff >= WIN_THRESHOLD) {
            winner = "Player 1";
            return true;
        } else {
            cout << "Player 1 is in the lead by " << diff << " nanoseconds" << endl;
        }
    } else {
        cout << endl;
    }

    if (runs >= MAX_RUNS) {
        noWinner = true;
        return true;
    }

    return false;
}

void run() {
    cout << "Player1 log: player1.log" << endl;
    player1 += " >> ./player1.log 2>&1";

    cout << "Player2 log: player2.log" << endl;
    player2 += " >> ./player2.log 2>&1";

    while (!done()) {
        runs++;

        // Capture start time
        auto t0 = std::chrono::high_resolution_clock::now();
        auto nanosec0 = t0.time_since_epoch();

        system(player1.c_str());

        // Capture stop time
        auto t1 = std::chrono::high_resolution_clock::now();
        auto nanosec1 = t1.time_since_epoch();

        player1Time += (nanosec1.count() - nanosec0.count());

        // -----------------------------------------------------------

        // Capture start time
        auto t2 = std::chrono::high_resolution_clock::now();
        auto nanosec2 = t2.time_since_epoch();

        system(player2.c_str());

        // Capture stop time
        auto t3 = std::chrono::high_resolution_clock::now();
        auto nanosec3 = t3.time_since_epoch();

        player2Time += (nanosec3.count() - nanosec2.count());
    }
}
