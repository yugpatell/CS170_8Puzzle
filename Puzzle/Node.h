#ifndef __NODE_H__
#define __NODE_H__
#include <string>
#include <vector>

using namespace std;

class Node {

private:
  int h;
  int g;
  int cost;
  int depth;
  string move;
  Node *parent;
  vector<vector<int>> currentState;

public:
  Node(int h, int g, int cost, int depth, string move, Node *parent,
       vector<vector<int>> currentState) {
    this->h = h;
    this->g = g;
    this->cost = cost;
    this->depth = depth;
    this->move = move;
    this->parent = parent;
    this->currentState = currentState;
  }

  int getH() const { return this->h; }
  int getG() const { return this->g; }
  int getCost() const { return this->cost; }
  int getDepth() const { return this->depth; }
  string getMove() const { return this->move; }
  Node *getParent() const { return this->parent; }
  vector<vector<int>> getCurrentState() const { return this->currentState; }
};

#endif //__NODE_H__