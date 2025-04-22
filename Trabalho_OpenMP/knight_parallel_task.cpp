#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <omp.h>
#include <chrono>
#include <atomic>

using namespace std;

const int knight_move[8][2] = {
  {1, 2}, {2, 1}, {2, -1}, {1, -2},
  {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}
};

#define TASK_DEPTH 3

// Flag global para indicar se uma solução já foi encontrada
std::atomic<bool> solution_found(false);

class tour {
  vector<vector<int>> board;
  int sx, sy, size;

public:
  bool findtour(tour& T, int imove, int depth);

  tour(int s = 5, int startx = 0, int starty = 0)
    : sx(startx), sy(starty), size(s)
  {
    board.resize(size, vector<int>(size, -1));
    board[sx][sy] = 0;

    bool found = false;

    auto start = chrono::high_resolution_clock::now();

    #pragma omp parallel
    {
      #pragma omp single
      {
        found = findtour(*this, 0, 0);
      }
    }

    auto end = chrono::high_resolution_clock::now();

    if (!solution_found)
      cout << "No solutions found\n";

    cout << "Tempo total: "
         << chrono::duration<double>(end - start).count() << "s\n";
  }

  tour(const tour& T) : sx(T.sx), sy(T.sy), size(T.size), board(T.board) {}

  friend ostream& operator<<(ostream& os, const tour& T);
};

ostream& operator<<(ostream& os, const tour& T) {
  for (int i = 0; i < T.size; ++i) {
    for (int j = 0; j < T.size; ++j)
      os << setw(2) << T.board[i][j] << " ";
    os << endl;
  }
  return os;
}

bool tour::findtour(tour& T, int imove, int depth) {
  if (solution_found) return false;

  if (imove == (size * size - 1)) {
    #pragma omp critical
    {
      if (!solution_found) {
        cout << T << endl;
        solution_found = true;
      }
    }
    return true;
  }

  int cx = T.sx;
  int cy = T.sy;
  int cs = T.size;

  for (int i = 0; i < 8; ++i) {
    if (solution_found) break;

    int tcx = cx + knight_move[i][0];
    int tcy = cy + knight_move[i][1];

    if (
      tcx >= 0 && tcy >= 0 && tcx < cs && tcy < cs &&
      T.board[tcx][tcy] == -1
    ) {
      tour temp(T);
      temp.board[tcx][tcy] = imove + 1;
      temp.sx = tcx;
      temp.sy = tcy;

      if (depth < TASK_DEPTH) {
        #pragma omp task firstprivate(temp)
        {
          temp.findtour(temp, imove + 1, depth + 1);
        }
      } else {
        temp.findtour(temp, imove + 1, depth + 1);
      }
    }
  }

  #pragma omp taskwait
  return false;
}

int main() {
  int table_size, start_x, start_y;

  cin >> table_size >> start_x >> start_y;

  omp_set_num_threads(8);

  tour T(table_size, start_x, start_y);

  return 0;
}
