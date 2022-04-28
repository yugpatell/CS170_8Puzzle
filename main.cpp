#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include "./Puzzle/Puzzle.h"

using namespace std;


vector<vector<int>> initialState = {{1, 2, 3},
                                    {4, 8, 0},
                                    {7, 6, 5}};

/* Resource: https://www.delftstack.com/howto/cpp/cpp-split-string-by-space/ */

vector<int> parseString(string input) {
    vector<int> row;
    istringstream iss(input);
    int a, b, c;
    iss >> a >> b >> c;

    row.push_back(a);
    row.push_back(b);
    row.push_back(c);
    return row;
}

vector<vector<int>> customPuzzle() {
    vector<vector<int>> newInitialState;
    string input;
    cin.ignore();
    cout << "---------------------------------------------" << endl;
    cout << "Enter your first row, use space between numbers (zero to represent blank): " << endl;
    getline(cin, input);
    newInitialState.push_back(parseString(input));
    cout << "Enter the second row, use space between numbers (zero to represent blank):" << endl;
    getline(cin, input);
    newInitialState.push_back(parseString(input));
    cout << "Enter the third row, use space between numbers (zero to represent blank): " << endl;
    getline(cin, input);
    newInitialState.push_back(parseString(input));
    cout << "---------------------------------------------" << endl;
    return newInitialState;
}

int main() {

    int initialChoice, searchChoice;
    do {
        cout << endl << endl << "Welcome to Yug's (862164042) 8 puzzle solver!" << endl;
        cout << "---------------------------------------------" << endl;
        cout << "1. To use default puzzle" << endl;
        cout << "2. To enter custom puzzle" << endl;
        cout << "---------------------------------------------" << endl << endl;
        cin >> initialChoice;
    } while (initialChoice != 1 && initialChoice != 2);

    if (initialChoice == 2) {
        initialState = customPuzzle();
    }

    do {
        cout << endl << endl << "Enter your choice of algorithm." << endl;
        cout << "---------------------------------------------" << endl;
        cout << "1. Uniform Cost Search." << endl;
        cout << "2. A* with the Misplaced Tile heuristic." << endl;
        cout << "3. A* with the Euclidean Distance heuristic." << endl;
        cout << "---------------------------------------------" << endl << endl;
        cin >> searchChoice;
    } while (searchChoice != 1 && searchChoice != 2 && searchChoice != 3);


    if (searchChoice == 1) {
        Puzzle newPuzzle(UNIFORM, initialState);
        newPuzzle.solvePuzzle();
    } else if (searchChoice == 2) {
        Puzzle newPuzzle(MISPLACED, initialState);
        newPuzzle.solvePuzzle();
    } else {
        Puzzle newPuzzle(EUCLIDEAN, initialState);
        newPuzzle.solvePuzzle();
    }
    return 0;

}