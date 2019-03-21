#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>
#include <chrono>
using namespace std;
// User defined variable for the visible array size, padding of +2 is added for
// a buffer around the used array
int boardSize;
// Used to calculate remaining mines
int flagCount;
//Max size for board enterable by the user
const int maxSize = 20;
// Constant used for header spacing, changes how many possible digits can be
// added without spacing issues
const int spacing = 3;
// Set to true to enable cheat/debug mode, prints proximities
bool cheatTest = false;
// Number used for mine generation, chance of mine being added is 1 in
// (mineChance + 1)
const int mineChance = 5;
// Declares board of 0s and 1s for if mine is present
vector<vector<int> > mineBoard;
// Declares board of proximity values to mines
vector<vector<int> > playBoard;
// Declares display board, starts as squares
vector<vector<string> > blankBoard;
// Store values for indexes where continuous zeros are located, used in
// recurUncover
vector<int> rows;
vector<int> columns;
// Prints blankBoard values
void printBoard() {
  // Adds offset for number and header
  cout << "\n  ";
  // Adds spacing depending on spacing const used for variable header offset
  // later
  for (int i = 0; i < spacing; i++) cout << " ";
  // Prints top header, if two digits then space is removed to make room
  for (int x = 0; x < boardSize; x++) {
    for (int y = 0; y < 1 - (int(x / 10.0)); y++) cout << " ";
    cout << x;
  }
  cout << "\n";
  // Prints side header for rows
  for (int z = 1; z < boardSize + 1; z++) {
    // Prints spaces depending on spacing const for single digits, decreases per
    // extra digit
    for (int y = 0; y < spacing - (int)((z - 1) / 10.0); y++) cout << " ";
    std::cout << z - 1 << ". ";
    for (int q = 1; q < boardSize + 1; q++) {
      std::cout << blankBoard[z][q] << " ";
    }
    std::cout << "\n";
  }
}
// Recursive function for findingcontiguous 0s on playBoard
void recurUncover(int z, int q) {
  bool alreadyHere = false;
  // Ensures that passed location hasn't already been processed
  for (int r = 0; r < rows.size(); r++) {
    if (rows[r] == z) {
      if (columns[r] == q) {
        alreadyHere = true;
        // cout << "here";
      }
    }
  }
  if (!alreadyHere) {
    // Adds loc to rows and columns
    rows.push_back(z);
    columns.push_back(q);
    // Searches 4 surrounding squares for more 0s, calls function if so,
    // expanding outward
    if (playBoard[z][q + 1] == 0) recurUncover(z, q + 1);
    if (playBoard[z][q - 1] == 0) recurUncover(z, q - 1);
    if (playBoard[z + 1][q] == 0) recurUncover(z + 1, q);
    if (playBoard[z - 1][q] == 0) recurUncover(z - 1, q);
    // if (playBoard[z+1][q+1] ==0 && blankBoard[z+1][q+1] != "□")
    // recurUncover(z+1,q+1);
    // if (playBoard[z-1][q+1] ==0 && blankBoard[z-1][q+1] != "□")
    // recurUncover(z-1,q+1);
    // if (playBoard[z+1][q-1] ==0 && blankBoard[z+1][q-1] != "□")
    // recurUncover(z+1,q-1);
    // if (playBoard[z-1][q-1] ==0 && blankBoard[z-1][q-1] != "□")
    // recurUncover(z-1,q-1);
  }
}
int main() {
  cout << "Welcome to Minesweeper!";
  // Enters main game loop
  while (true) {
    string userBoardSize;
    cout << "\nEnter board size 1-20 (5 or 10 recommended): ";
    cin >> userBoardSize;
    //Enables applebees cheat, shows prompt again
    if (userBoardSize == "applebees") {
      cheatTest = true;
      cout << "\nCongratulations, cheat activated.";
      cout << "\nEnter size: ";
      cin >> userBoardSize;
    }
    boardSize = stoi(userBoardSize,nullptr,10);
    //Ensures valid board size is entered
    while (boardSize <= 0 || boardSize > maxSize) {
      cout << "\nInvalid size. Reenter: ";
      cin >> boardSize;
    }
    // Sets both arrays for all 3 boards to given boardSize + 2, to create an
    // outer buffer area
    playBoard.resize(boardSize + 2, vector<int>(boardSize + 2));
    mineBoard.resize(boardSize + 2, vector<int>(boardSize + 2));
    blankBoard.resize(boardSize + 2, vector<string>(boardSize + 2));
    // Variable to calculate number of mines for win condition
    int numMines = 0;
    // Throws a random seed into random number generator, to ensure different
    // gameplay each run
    srand(time(NULL));
    // Traverses every index in both inner and outer array
    for (int x = 0; x < boardSize + 2; x++) {
      for (int y = 0; y < boardSize + 2; y++) {
        // Fills mineBoard and playBoard with intial 0s
        mineBoard[x][y] = 0;
        playBoard[x][y] = 0;
        // Fills left and right buffer area with -1s to avoid confusion during
        // recursive search
        if (x == 0 || x == boardSize + 1) playBoard[x][y] = -1;
        // Fills top and bottom buffer to still avoid confusion
        if (y == 0 || y == boardSize + 1) playBoard[x][y] = -1;
        // Fills blankBoard with initial blank squares
        blankBoard[x][y] = "■";
      }
    }
    int random = 0;
    for (int z = 1; z < boardSize + 1; z++) {
      for (int q = 1; q < boardSize + 1; q++) {
        // Generates mines randomly per spot, chance of mine generation is 1 in
        // (mineChance+1)
        random = (rand() % mineChance);
        if (random == 0) {
          // Sets mineBoard to 1 for active mine
          mineBoard[z][q] = 1;
          // Sets playBoard to 9 for mine
          playBoard[z][q] = 9;
          // Increase numMines for win condition calculation
          numMines++;
        }
      }
    }
    // For proximity calculations put into playBoard
    // Traverses visible array without buffer
    for (int z = 1; z < boardSize + 1; z++) {
      for (int q = 1; q < boardSize + 1; q++) {
        int aroundCount = 0;
        // If not mine, continue
        if (mineBoard[z][q] == 0) {
          aroundCount = 0;
          // Checks 8 surrounding squares for mines, tallies total
          if (mineBoard[z][q + 1] == 1) aroundCount++;
          if (mineBoard[z][q - 1] == 1) aroundCount++;
          if (mineBoard[z + 1][q] == 1) aroundCount++;
          if (mineBoard[z - 1][q] == 1) aroundCount++;
          if (mineBoard[z + 1][q + 1] == 1) aroundCount++;
          if (mineBoard[z - 1][q + 1] == 1) aroundCount++;
          if (mineBoard[z + 1][q - 1] == 1) aroundCount++;
          if (mineBoard[z - 1][q - 1] == 1) aroundCount++;
          // Sets playBoard at loc for proximity count
          playBoard[z][q] = aroundCount;
        }
      }
    }
    // Debug mode, prints unobscured proximities
    if (cheatTest) {
      for (int z = 1; z < boardSize + 1; z++) {
        for (int q = 1; q < boardSize + 1; q++) {
          std::cout << playBoard[z][q];
        }
        std::cout << "\n";
      }
    }
    //Prints number of unused flags, for helping determine mines left
    cout << "\nFlags left: " << numMines - flagCount << "\n";
    printBoard();
    //Takes initial time value, for calculating time taken
    auto begin = chrono::high_resolution_clock::now();
    while (true) {
      //Variables to hold user specified row and column
      int row;
      int column;
      //For lose condition
      bool lose = false;
      cout << "Enter column: ";
      cin >> column;
      //Ensures valid column entry
      while (column <0 || column > boardSize -1) {
        cout << "\nInvalid. Reenter: ";
        cin >> column;
      }
      cout << "Enter row: ";
      cin >> row;
      //Ensures valid row entry
      while (row < 0 || row > boardSize -1) {
        cout << "\nInvalid. Reenter: ";
        cin >> row;
      }
      cout << "(un)Flag or dig? f/d: ";
      char flag;
      cin >> flag;
      flag = tolower(flag);
      //Ensures valid response to f/d prompt
      while (flag != 'f' && flag != 'd') {
        cout << "\nInvalid. Reener: ";
        cin >> flag;
      } 
      if (tolower(flag) == 'd') {
        //Ensures that dug block hasn't been flagged to avoid accidental dig
        if (blankBoard[row + 1][column + 1] != "F") {
          //Lose if dug block was mine
          if (playBoard[row + 1][column + 1] == 9) {
            blankBoard[row + 1][column + 1] = "*";
            lose = true;
          } else if ((playBoard[row + 1][column + 1]) == 0) {
            //If unsurrounded block dug, recursive function is ran
            blankBoard[row + 1][column + 1] = "□";
            recurUncover(row + 1, column + 1);
            //Sets 4 surrounding blocks to proximity values, so that the border will be filled with actual proximities
            for (int y = 0; y < rows.size(); y++) {
              blankBoard[rows[y]][columns[y]] =
                  to_string(playBoard[rows[y]][columns[y]]);
              blankBoard[rows[y]][columns[y] + 1] =
                  to_string(playBoard[rows[y]][columns[y] + 1]);
              blankBoard[rows[y]][columns[y] - 1] =
                  to_string(playBoard[rows[y]][columns[y] - 1]);

              blankBoard[rows[y] + 1][columns[y]] =
                  to_string(playBoard[rows[y] + 1][columns[y]]);
              blankBoard[rows[y] - 1][columns[y]] =
                  to_string(playBoard[rows[y] - 1][columns[y]]);
            }
            //Resets catalogued 0s back, after previous step overwrite them in display array
            for (int y = 0; y < rows.size(); y++)
              blankBoard[rows[y]][columns[y]] = "□";
          } else
          //If not empty or mine, sets to proximity
            blankBoard[row + 1][column + 1] =
                to_string(playBoard[row + 1][column + 1]);
        }
        if (lose) {
          //If lost, prints board to see mine, lose message, and exits inner game loop
          printBoard();
          cout << "\nYou lose!\n";
          break;
        }
      } else {
        //Toggles flags when F inputted
        if (blankBoard[row + 1][column + 1] == "F") {
          blankBoard[row + 1][column + 1] = "■";
        } else
          blankBoard[row + 1][column + 1] = "F";
      }
      //Number of undug blocks, used for win condition
      int numBlank = 0;
      flagCount = 0;
      for (int z = 1; z < boardSize + 1; z++) {
        for (int q = 1; q < boardSize + 1; q++) {
          //Tallies undug blocks for win condition
          if (blankBoard[z][q] == "■" || blankBoard[z][q] == "F") numBlank++;
          //Counts flags, used for "Flags used: " printout
          if (blankBoard[z][q] == "F") flagCount++;
        }
      }
      //If it reaches this point without hitting a mind, player has to have won
      if (numBlank == numMines) {
        printBoard();
        std::cout << "\n";
        cout << "\n YOU WIN!!!!!";
        //Takes end time value, finds difference, converts to seconds
        auto end = chrono::high_resolution_clock::now();
        auto dur = end - begin;
        auto ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
        double seconds = ms / 1000.0;
        cout << " It took you " << seconds << " seconds!";
        break;
      }
      //Prints number of unused flags, for helping determine mines left
      cout << "\nFlags left: " << numMines - flagCount << "\n";
      printBoard();
    }
    //Ensures memory occupied by vectors is freed, to avoid crashes on further runs
    rows.clear();
    columns.clear();
    mineBoard.clear();
    playBoard.clear();
    blankBoard.clear();
    char cont;
    cout << "\n Go again? y/n: ";
    cin >> cont;
    cont = tolower(cont);
    //Stays in loop if user response was either y or Y
    if (cont != 'y') break;
  }
}
