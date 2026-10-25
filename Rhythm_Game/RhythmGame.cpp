#include <iostream>
#include <ctime>
#include <vector>
#include <cstdlib>
#include <thread> 
#include <chrono>

using namespace std;

// -- Creating the grid dimensions and the tile struct ---------------------------------------------

constexpr int ROWS = 15;
constexpr int COLS = 4;

struct Tile {
    int col; //column of the tile: it ranges from 1 to 4, keys ASDF
    int pos = 0; //vertical position of the tile
    bool touched = false; //if the Tile has been touched or not
};


// -- Printing the Grid ---------------------------------------------------------------------------- 

vector<vector<char>> makeEmptyGrid() { //creates and returns the empty grid
    return vector<vector<char>>(
        ROWS,
        vector<char>(COLS, '.')
    );
}

void placeTile(vector<vector<char>>& grid, const Tile& tile) { //adds the tile to the grid
    if (tile.pos >= 0 && tile.pos < ROWS &&
        tile.col >= 0 && tile.col < COLS) {
        grid[tile.pos][tile.col] = '#';
    }
}

void PrintGrid (const vector<vector<char>>& grid){ //prints the grid with the tile in it
    cout << "\033[H"; //used to clear the terminal

    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            cout << grid[r][c] << " ";
        }
        cout << '\n';
    }
    cout << endl << endl;
    cout << "A S D F " << endl;
}


// -- Game Logic ---------------------------------------------------------------------------------

bool AdvanceTick (Tile &tiles){ //advances the tiles and checks for buttons pressed
    
    if (tiles.pos!=15 && tiles.touched){ //hit too soon
        cout << "You lost :(" << endl;
        return false;
    }
    tiles.pos++;
    if (tiles.pos==15 && !tiles.touched){ //missed it
        cout << "You lost :(" << endl;
        return false;
    }
    return true;
}


// -- Main ----------------------------------------------------------------------------------------

int main(){
    bool keepup = true; //variabile to check if you lost or you can keep going
    Tile tile1;
    tile1.col = 2;
    while (keepup){
        auto grid = makeEmptyGrid();
        placeTile(grid, tile1);
        PrintGrid(grid);
        keepup = AdvanceTick(tile1);
        this_thread::sleep_for(chrono::milliseconds(220));
    }
    return 0;
}