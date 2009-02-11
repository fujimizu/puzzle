#include <iostream>
#include <map>
#include <set>

using namespace std;

const int SIZE_BOARD = 9;
const int SIZE_BLOCK = 3;
const int SPACE = 0;

typedef map<int, set<int> > Candidates;
typedef set<int> Cand;
typedef set<int>::iterator CandIt;

class NumberPlace {
  private:
    int board[SIZE_BOARD][SIZE_BOARD];
  public:
    NumberPlace() { init(); }
    NumberPlace(int[SIZE_BOARD][SIZE_BOARD]);
    ~NumberPlace() {}
    void init();
    bool is_solved();
    void print();
    Candidates candidates();
    bool solve();
    int make_key(int x, int y) { return x * SIZE_BOARD * 10 + y; }
};

NumberPlace::NumberPlace(int board[SIZE_BOARD][SIZE_BOARD]) {
  for (int x = 0; x < SIZE_BOARD; x++) {
    for (int y = 0; y < SIZE_BOARD; y++) {
      this->board[x][y] = board[x][y];
    }
  }
}

void NumberPlace::init() {
  for (int x = 0; x < SIZE_BOARD; x++) {
    for (int y = 0; y < SIZE_BOARD; y++) {
      board[x][y] = SPACE;
    }
  }
}


bool NumberPlace::is_solved() {
  for (int x = 0; x < SIZE_BOARD; x++) {
    for (int y = 0; y < SIZE_BOARD; y++) {
      if (board[x][y] == SPACE) {
        return false;
      }
    }
  }
  return true;
}

void NumberPlace::print() {
  for (int x = 0; x < SIZE_BOARD; x++) {
    for (int y = 0; y < SIZE_BOARD; y++) {
      if (board[x][y] == SPACE) {
        cout << ".";
      } else {
        cout << board[x][y];
      }
      if (y != SIZE_BOARD-1) {
        cout << " ";
      }
    }
    cout << endl;
  }
  cout << endl;
}

Candidates NumberPlace::candidates() {
  Candidates candidates;
  for (int x = 0; x < SIZE_BOARD; x++) {
    for (int y = 0; y < SIZE_BOARD; y++) {
      Cand cand;
      for (int i = 1; i < SIZE_BOARD+1; i++) {
        cand.insert(i);
      }
      int key = make_key(x, y);
      candidates[key] = cand;
    }
  }
  for (int x = 0; x < SIZE_BOARD; x++) {
    for (int y = 0; y < SIZE_BOARD; y++) {
      int key = make_key(x, y);
      if (board[x][y] != SPACE) {
        candidates.erase(key);
        continue;
      } else {
        // 同じ行、列
        for (int i = 0; i < SIZE_BOARD; i++) {
          CandIt it;
          it = candidates[key].find(board[x][i]);
          if (it != candidates[key].end()) {
            candidates[key].erase(it);
          }
          it = candidates[key].find(board[i][y]);
          if (it != candidates[key].end()) {
            candidates[key].erase(it);
          }
        }
      }
      // 同じブロック
      int area_x = x / SIZE_BLOCK;
      int area_y = y / SIZE_BLOCK;
      int start_x = area_x * SIZE_BLOCK;
      int start_y = area_y * SIZE_BLOCK;
      for (int i = start_x; i < start_x + SIZE_BLOCK; i++) {
        for (int j = start_y; j < start_y + SIZE_BLOCK; j++) {
          if (x == i || y == j) continue;
          CandIt it;
          it = candidates[key].find(board[i][j]);
          if (it != candidates[key].end()) {
            candidates[key].erase(it);
          }
        }
      }
    }
  }
  return candidates;
}

bool NumberPlace::solve() {
  //print();

  if (is_solved()) {
    this->print();
    return true;
  }
  Candidates candidates = this->candidates();
  int min_x = -1;
  int min_y = -1;
  int num_cand = SIZE_BOARD + 1;
  for (int x = 0; x < SIZE_BOARD; x++) {
    for (int y = 0; y < SIZE_BOARD; y++) {
      int key = make_key(x, y);
      if (candidates.count(key) == 0) continue;
      int size = candidates[key].size();
      if (size != 0 && size < num_cand) {
        min_x = x;
        min_y = y;
        num_cand = candidates[key].size();
      }
    }
  }
  int key = make_key(min_x, min_y);
  Cand cand = candidates[key];
  CandIt it = cand.begin();
  while (it != cand.end()) {
    int org = board[min_x][min_y];
    board[min_x][min_y] = (int)*it;
    bool result = solve();
    if (result) {
      return true;
    }
    board[min_x][min_y] = org;
    it++;
  }
  return false;
}


int main(int argc, char **argv) {
  int board[SIZE_BOARD][SIZE_BOARD] = {
    {0,0,3,0,0,0,7,0,0},
    {0,2,0,4,0,6,0,8,0},
    {1,0,0,0,5,0,0,0,9},
    {0,4,0,0,0,0,0,1,0},
    {0,0,6,0,0,0,2,0,0},
    {0,1,0,0,0,0,0,3,0},
    {8,0,0,0,1,0,0,0,4},
    {0,6,0,9,0,2,0,5,0},
    {0,0,7,0,0,0,6,0,0}
  };
  NumberPlace numpla(board);
  numpla.print();
  numpla.solve();
  return 0;
}

