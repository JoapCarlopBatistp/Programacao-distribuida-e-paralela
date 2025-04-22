#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <omp.h>
#include <chrono>

using namespace std;

const int knight_move[8][2]={{1,2}, {2,1}, {2,-1}, {1,-2}, {-1,-2}, {-2,-1}, {-2,1}, {-1,2}};

class tour {
  vector< vector< int > > board;
  int sx, sy, size;

public:
  bool findtour(tour& , int );

  // Constructor
  tour(int s = 5, int startx = 0, int starty = 0)
    :sx(startx), sy(starty), size(s)
  {
    // Get the board to size x size
    board.resize(size);
    for(int i = 0; i < size; ++i)
      board[i].resize(size);

    // Fill the board with -1s
    for(int i = 0; i < size; ++i)
      for(int j = 0; j < size; ++j)
        board[i][j] = -1;

    // Move 0
    board[sx][sy] = 0;

    // Solve the problem
    auto start = chrono::high_resolution_clock::now();
    if(!findtour(*this, 0))
      cout << "No solutions found\n";
    auto end = chrono::high_resolution_clock::now();

    cout << "Tempo total: " 
         << chrono::duration<double>(end - start).count() << "s\n";
  }

  // Copy constructor
  tour(const tour& T): sx(T.sx), sy(T.sy), size(T.size) {
    this->board.resize(size);
    for(int i = 0; i < size; ++i)
      board[i].resize(size);

    // Copy the board
    for(int i = 0; i < size; ++i)
      for(int j = 0; j < size; ++j)
        board[i][j] = T.board[i][j];
  }

  // Function to output class to ostream
  friend std::ostream& operator<<
  (std::ostream& os, const tour& T);
};


std::ostream& operator<<(std::ostream& os, const tour& T) {
  int size = T.size;

  for(int i = 0; i < size; ++i) {
    for(int j = 0; j < size; ++j)
      os <<  setw(2) << T.board[i][j] << " ";
    os << endl;
  }

  return os;
}

// A recursive function to find the knight tour.
bool tour::findtour(tour& T, int imove) {
  if(imove == (size*size - 1)) return true;

  // make a move
  int cx = T.sx;
  int cy = T.sy;
  int cs = T.size;

  bool found_solution = false;
  #pragma omp parallel for shared(found_solution)
  for (int i = 0; i < 8; ++i) {
    if(found_solution) continue;
    int tcx = cx + knight_move[i][0];
    int tcy = cy + knight_move[i][1];
    if (
        // Is this a valid move?
        (tcx >= 0) &&  (tcy >= 0)  &&  (tcx < cs) &&  (tcy < cs) &&
        // Has this place been visited yet?
        (T.board[tcx][tcy] == -1)
        ) {
      tour temp(T);
      temp.board[tcx][tcy] = imove+1;
      temp.sx = tcx;
      temp.sy = tcy;
      if(temp.findtour(temp, imove+1)) {
        #pragma omp critical
        {
          if(!found_solution){
            cout << temp << endl;
            found_solution = true;
            exit(1);
          }

        }
      }
    }
  }
  return found_solution;
}

int main(void) {
  int table_size, start_x, start_y;
  omp_set_num_threads(8); //muda o número de threads, se quiser pode usar direto na compilação
  cin >> table_size >> start_x >> start_y;

  auto start = chrono::high_resolution_clock::now();
  tour T(table_size, start_x, start_y);
  auto end = chrono::high_resolution_clock::now();
  cout << "Tempo total: " << chrono::duration<double>(end - start).count() << "\n";
  return 0;
}

// Adapted with permission from the original implementation by
// prof. Piyush Kumar (Florida State University)
// http://www.compgeom.com/~piyush/
