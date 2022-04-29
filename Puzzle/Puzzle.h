#ifndef __PUZZLE_H__
#define __PUZZLE_H__

#include "Node.h"
#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>
#include <bits/stdc++.h>
#include <unordered_map>
#include <chrono>


enum searchAlgorithm {
    UNIFORM,
    MISPLACED,
    EUCLIDEAN,
};

using namespace std;

struct CompareCost {
    bool operator()(const Node *lhs, const Node *rhs) const {
        return lhs->getCost() > rhs->getCost();
    }
};


class Puzzle {

private:
    vector<vector<int>> initialState;
    vector<vector<int>> finalState;
    searchAlgorithm search;
    int maxFrontierSize;
    int expandedNodeCount;

public:
    Puzzle(searchAlgorithm search, vector<vector<int>> initialState = {{1, 2, 3},
                                                                       {4, 0, 6},
                                                                       {7, 5, 8}},
           vector<vector<int>> finalState = {{1, 2, 3},
                                             {4, 5, 6},
                                             {7, 8, 0}}) {
        this->search = search;
        this->initialState = initialState;
        this->finalState = finalState;
        this->maxFrontierSize = 0;
        this->expandedNodeCount = 0;
    }

    vector<Node *> possibleOperators(Node *&current) {
        pair<int, int> index = findZeroIndex(current->getCurrentState());
        vector<Node *> newNodes;


        // BLANK field (Zero) is NOT located in the far LEFT column;
        if (index.second != 0) {
            vector<vector<int>> copy = current->getCurrentState();
            swap(copy[index.first][index.second],
                 copy[index.first][index.second - 1]);
            int h = (this->search == UNIFORM) ? 0 : (this->search == MISPLACED) ? calculateMisplacedTitles(copy)
                                                                                : calculateEuclideanDistance(
                            copy);
            newNodes.push_back(
                    new Node(h, current->getG() + 1, h + current->getG() + 1,
                             current->getDepth() + 1, "left", current, copy, generateHash(copy)));
        }

        // BLANK field (Zero) is NOT located in the RIGHT left column;
        if (index.second != 2) {
            vector<vector<int>> copy = current->getCurrentState();
            swap(copy[index.first][index.second],
                 copy[index.first][index.second + 1]);
            int h = (this->search == UNIFORM) ? 0 : (this->search == MISPLACED) ? calculateMisplacedTitles(copy)
                                                                                : calculateEuclideanDistance(copy);
            newNodes.push_back(
                    new Node(h, current->getG() + 1, h + current->getG() + 1,
                             current->getDepth() + 1, "right", current, copy, generateHash(copy)));
        }

        // BLANK field (Zero) is NOT located in the very BOTTOM row;
        if (index.first != 2) {
            vector<vector<int>> copy = current->getCurrentState();
            swap(copy[index.first][index.second],
                 copy[index.first + 1][index.second]);
            int h = (this->search == UNIFORM) ? 0 : (this->search == MISPLACED) ? calculateMisplacedTitles(copy)
                                                                                : calculateEuclideanDistance(copy);
            newNodes.push_back(
                    new Node(h, current->getG() + 1, h + current->getG() + 1,
                             current->getDepth() + 1, "down", current, copy, generateHash(copy)));
        }

        // BLANK field (Zero) is NOT located in the very TOP row;
        if (index.first != 0) {
            vector<vector<int>> copy = current->getCurrentState();
            swap(copy[index.first][index.second],
                 copy[index.first - 1][index.second]);
            int h = (this->search == UNIFORM) ? 0 : (this->search == MISPLACED) ? calculateMisplacedTitles(copy)
                                                                                : calculateEuclideanDistance(copy);
            newNodes.push_back(
                    new Node(h, current->getG() + 1, h + current->getG() + 1,
                             current->getDepth() + 1, "up", current, copy, generateHash(copy)));
        }

        if (!newNodes.empty()) increaseExpandedCount();
        return newNodes;
    }


    void solvePuzzle() {
        Node *root = new Node(0, 0, 0, 0, "", nullptr, getInitialState(), generateHash(getInitialState()));
        unordered_map<string, int> explored, frontierMap;
        auto started = chrono::high_resolution_clock::now();

        /* Resource: https://stackoverflow.com/questions/2439283/how-can-i-create-min-stl-priority-queue */
        priority_queue<Node *, vector<Node *>, CompareCost> frontier;
        frontier.push(root);
        frontierMap[root->getHashKey()] = 1;
        updateMaxFrontierSize(frontier.size());
        while (true) {
            if (frontier.empty()) {
                cerr << "Failure detected!" << endl;
                cout << "The maximum number of nodes in the queue at any one time: " << getMaxFrontierSize() << endl;
                cout << "The number of nodes expanded: " << getExpandedNodesCount() << endl;
                return;
            };
            Node *currNode = getLowestCostNode(frontier);
            if (compareStates(this->getFinalState(), currNode->getCurrentState())) {
                auto done = std::chrono::high_resolution_clock::now();
                auto end_time = std::chrono::duration_cast<std::chrono::milliseconds>(done-started).count();
                printSolution(currNode);
                cout << "Goal!!!" << endl;
                cout << "To solve this problem, the search algorithm expanded a total of " << getExpandedNodesCount()
                     << " numbers of nodes" << endl;
                cout << "The maximum number of nodes in the queue at any one time: " << getMaxFrontierSize() << endl;
                cout << "The depth of the goal node was: " << currNode->getDepth() << endl;
                /* Resource to print time: https://stackoverflow.com/questions/36042637/how-to-calculate-execution-time-in-milliseconds */
                cout << "Time taken by program is: " << fixed << end_time << "ms." << endl;
                return;
            }
            explored[generateHash(currNode->getCurrentState())] = 1;
            vector<Node *> nodes = possibleOperators(currNode);
            for (Node *node: nodes) {
                if (!isInExplored(explored, node) && !isInFrontier(frontierMap, node)) {
                    frontierMap[currNode->getHashKey()] = 1;
                    frontier.push(node);
                } else {
                    delete node;
                }
            }
            updateMaxFrontierSize(max(getMaxFrontierSize(), static_cast<int>(frontier.size())));
        }

    }

    /*
     * Search through each index of currentState and match with finalState if different then increase misplaced titles count by one
     */
    int calculateMisplacedTitles(const vector<vector<int>> &currentState) {
        int count = 0;
        for (int i = 0; i < currentState.size(); i++) {
            for (int j = 0; j < currentState.at(i).size(); j++) {
                if (currentState.at(i).at(j) != 0 &&
                    currentState.at(i).at(j) != this->getFinalState().at(i).at(j))
                    count++;
            }
        }
        return count;
    }

    int calculateEuclideanDistance(const vector<vector<int>> &currentState) {
        int calculateDistance = 0;
        for (int i = 0; i < currentState.size(); i++) {
            for (int j = 0; j < currentState.at(i).size(); j++) {
                if (currentState.at(i).at(j) == 0) continue; // Ignore zero's calcuation
                pair<int, int> location = getLocationInFinalState(currentState.at(i).at(j));
                calculateDistance += (abs(location.first - i) + abs(location.second - j));
            }
        }
        return calculateDistance;
    }

    pair<int, int> getLocationInFinalState(const int &num) {
        for (int i = 0; i < this->getFinalState().size(); i++) {
            for (int j = 0; j < this->getFinalState().at(i).size(); j++) {
                if (this->getFinalState().at(i).at(j) == num) {
                    return make_pair(i, j);
                }
            }
        }
        return make_pair(-1, -1);
    }

    void printSolution(Node *solutionNode) {
        if (solutionNode->getParent() == nullptr) {
            cout << "Expanding state: " << endl;
            printCurrentState(solutionNode->getCurrentState());
            return;
        } else {
            printSolution(solutionNode->getParent());
            cout << "The best state to expand with g(n) = " << solutionNode->getG() << " and h(n) = "
                 << solutionNode->getH() << " by moving blank space " << solutionNode->getMove() << ": "
                 << endl;
            printCurrentState(solutionNode->getCurrentState());
        }
    }

    /*
     * Print's the passed in state's contents in a 2D array format
     */
    void printCurrentState(const vector<vector<int>> &currentState) {
        for (const auto &row: currentState) {
            for (const auto &column: row) {
                cout << column << " ";
            }
            cout << endl;
        }
        cout << endl << endl << endl;
    }

    /*
     * Find's the zero in the current state and return the value as a pair
     * Otherwise return nothing.
     * Resource:
     * https://www.geeksforgeeks.org/returning-multiple-values-from-a-function-using-tuple-and-pair-in-c/
     */
    pair<int, int> findZeroIndex(const vector<vector<int>> &currentState) {
        for (int i = 0; i < currentState.size(); i++) {
            for (int j = 0; j < currentState.at(i).size(); j++) {
                if (currentState.at(i).at(j) == 0)
                    return make_pair(i, j);
            }
        }
        return make_pair(-1, -1);
    }

    Node *getLowestCostNode(priority_queue<Node *, vector<Node *>, CompareCost> &frontier) {
        Node *top = frontier.top();
        frontier.pop();
        return top;
    }

    bool isInExplored(unordered_map<string, int> &explored, Node *&currNode) {
        if (explored[currNode->getHashKey()] == 1) return true;
        return false;
    }

    bool isInFrontier(unordered_map<string, int> &frontierMap, Node *&currNode) {
        if (frontierMap[currNode->getHashKey()] == 1) return true;
        return false;
    }

    /*
     * Compares if both states are equal, utilized when checking if current node is the given solution
     */
    bool compareStates(const vector<vector<int>> &lhs, const vector<vector<int>> &rhs) {

        for (int i = 0; i < lhs.size(); i++) {
            for (int j = 0; j < lhs.at(i).size(); j++) {
                if (lhs.at(i).at(j) != rhs.at(i).at(j)) return false;
            }
        }

        return true;
    }

    string generateHash(const vector<vector<int>> & currentState) {
        string output = "";
        for (int i = 0; i < currentState.size(); i++) {
            for (int j = 0; j < currentState.at(i).size(); j++) {
                output += currentState.at(i).at(j);
            }
        }
        return output;
    }

    vector<vector<int>> getInitialState() { return this->initialState; }

    vector<vector<int>> getFinalState() { return this->finalState; }

    int getMaxFrontierSize() const { return this->maxFrontierSize; }

    void updateMaxFrontierSize(int size) { this->maxFrontierSize = size; }

    int getExpandedNodesCount() const { return this->expandedNodeCount; }

    void increaseExpandedCount() { this->expandedNodeCount += 1; }
};


#endif //__PUZZLE_H__