// Mini Game Project: Tic Tac Toe
// Console-based game in C++ demonstrating loops, arrays and conditional logic.
//
//   - Arrays      : 3x3 char array for the board, array of all 8 winning lines
//   - Loops       : game loop, replay loop, menu loop, board drawing, win checks
//   - Conditions  : move validation, win / draw / loss detection, computer moves
//
// Modes: Player vs Player, Player vs Computer. Scoreboard is kept across rounds.

#include <iostream>
#include <string>
#include <limits>

using namespace std;

char board[3][3];

// All 8 winning lines (rows, columns, diagonals) as cell numbers 1-9
const int LINES[8][3] = {
    {1, 2, 3}, {4, 5, 6}, {7, 8, 9},   // rows
    {1, 4, 7}, {2, 5, 8}, {3, 6, 9},   // columns
    {1, 5, 9}, {3, 5, 7}               // diagonals
};

struct Score { int xWins = 0, oWins = 0, draws = 0; };

// ---------- board helpers ----------
char &cell(int n) { return board[(n - 1) / 3][(n - 1) % 3]; }
bool isFree(int n) { return cell(n) != 'X' && cell(n) != 'O'; }

void resetBoard() {
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            board[r][c] = char('1' + r * 3 + c);
}

void drawBoard() {
    cout << "\n";
    for (int r = 0; r < 3; ++r) {
        cout << "     " << board[r][0] << " | " << board[r][1] << " | " << board[r][2] << "\n";
        if (r < 2) cout << "    ---+---+---\n";
    }
    cout << "\n";
}

// Returns 'X' or 'O' if that symbol has a full line, otherwise ' '
char winner() {
    for (const auto &line : LINES) {
        char a = cell(line[0]);
        if (a == cell(line[1]) && a == cell(line[2])) return a;
    }
    return ' ';
}

bool boardFull() {
    for (int n = 1; n <= 9; ++n)
        if (isFree(n)) return false;
    return true;
}

// ---------- input ----------
int readInt(const string &prompt) {
    int v;
    while (true) {
        cout << prompt;
        if (cin >> v) { cin.ignore(numeric_limits<streamsize>::max(), '\n'); return v; }
        if (cin.eof()) { cout << "\nInput ended. Goodbye!\n"; exit(0); }
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  Please enter a number.\n";
    }
}

string readName(const string &prompt) {
    string s;
    while (true) {
        cout << prompt;
        if (!getline(cin, s)) exit(0);
        if (!s.empty()) return s;
    }
}

bool askReplay() {
    while (true) {
        cout << "Play again? (y/n): ";
        string ans;
        if (!getline(cin, ans)) exit(0);
        if (ans == "y" || ans == "Y") return true;
        if (ans == "n" || ans == "N") return false;
        cout << "  Please type y or n.\n";
    }
}

int humanMove(const string &name, char sym) {
    while (true) {
        int n = readInt(name + " (" + sym + "), choose a cell 1-9: ");
        if (n < 1 || n > 9)  cout << "  Invalid cell! Choose a number from 1 to 9.\n";
        else if (!isFree(n)) cout << "  Cell " << n << " is already taken! Try another.\n";
        else return n;
    }
}

// ---------- computer player ----------
// Finds a cell that completes a line for 'sym', or 0 if none
int findWinningCell(char sym) {
    for (const auto &line : LINES) {
        int count = 0, empty = 0;
        for (int n : line) {
            if (cell(n) == sym) ++count;
            else if (isFree(n)) empty = n;
        }
        if (count == 2 && empty != 0) return empty;
    }
    return 0;
}

int computerMove(char me, char opp) {
    int n = findWinningCell(me);           // 1. win if possible
    if (n) return n;
    n = findWinningCell(opp);              // 2. block opponent
    if (n) return n;
    if (isFree(5)) return 5;               // 3. take center
    const int corners[4] = {1, 3, 7, 9};   // 4. take a corner
    for (int c : corners) if (isFree(c)) return c;
    for (int s = 2; s <= 8; s += 2) if (isFree(s)) return s;   // 5. take a side
    return 0;
}

// ---------- one round ----------
// Returns 'X', 'O' or 'D' (draw)
char playRound(const string &nameX, const string &nameO, bool vsComputer) {
    resetBoard();
    char turn = 'X';
    int moveNo = 0;
    cout << "\n" << nameX << " is X, " << nameO << " is O. X goes first.";
    drawBoard();

    while (true) {
        int n;
        if (vsComputer && turn == 'O') {
            n = computerMove('O', 'X');
            cout << nameO << " (O) chooses cell " << n << ".\n";
        } else {
            n = humanMove(turn == 'X' ? nameX : nameO, turn);
        }
        cell(n) = turn;
        ++moveNo;
        cout << "Move " << moveNo << ":";
        drawBoard();                        // board shown after every move

        if (winner() == turn) return turn;
        if (boardFull()) return 'D';
        turn = (turn == 'X') ? 'O' : 'X';
    }
}

void showScore(const Score &s, const string &nameX, const string &nameO) {
    cout << "------ SCOREBOARD ------\n"
         << nameX << " (X): " << s.xWins << "\n"
         << nameO << " (O): " << s.oWins << "\n"
         << "Draws: " << s.draws << "\n"
         << "------------------------\n";
}

// ---------- game session with replay ----------
void playSession(bool vsComputer, Score &total) {
    string nameX, nameO;
    if (vsComputer) {
        nameX = readName("Enter your name: ");
        nameO = "Computer";
    } else {
        nameX = readName("Enter Player 1 name (X): ");
        nameO = readName("Enter Player 2 name (O): ");
    }

    Score session;
    int round = 1;
    do {
        cout << "\n========== ROUND " << round++ << " ==========";
        char result = playRound(nameX, nameO, vsComputer);
        if (result == 'D') {
            cout << "It's a DRAW! The board is full.\n";
            ++session.draws; ++total.draws;
        } else if (result == 'X') {
            cout << "*** " << nameX << " (X) WINS! ***\n";
            ++session.xWins; ++total.xWins;
        } else {
            if (vsComputer) cout << "*** Computer (O) WINS! You lose. Better luck next time! ***\n";
            else cout << "*** " << nameO << " (O) WINS! ***\n";
            ++session.oWins; ++total.oWins;
        }
        showScore(session, nameX, nameO);
    } while (askReplay());
}

int main() {
    Score total;
    cout << "=================================\n"
         << "      TIC TAC TOE - MINI GAME\n"
         << "=================================\n"
         << "Board cells are numbered like this:";
    resetBoard();
    drawBoard();

    while (true) {
        cout << "\n----- MAIN MENU -----\n"
             << "1. Player vs Player\n"
             << "2. Player vs Computer\n"
             << "3. Overall Statistics\n"
             << "4. Exit\n";
        int choice = readInt("Enter your choice: ");
        if (choice == 1) playSession(false, total);
        else if (choice == 2) playSession(true, total);
        else if (choice == 3) {
            int games = total.xWins + total.oWins + total.draws;
            cout << "\nGames played : " << games << "\n"
                 << "X wins       : " << total.xWins << "\n"
                 << "O wins       : " << total.oWins << "\n"
                 << "Draws        : " << total.draws << "\n";
        } else if (choice == 4) { cout << "Thanks for playing Tic Tac Toe. Goodbye!\n"; return 0; }
        else cout << "Invalid choice. Please select 1-4.\n";
    }
}
