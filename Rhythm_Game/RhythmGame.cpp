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

/*I need functions that do this: 
get an input from keyboard in a way that doesn't require enter and doesn't block the game
convert the input to lowercase, so it becomes case-insensitive*/

int keyboardToIndex (char inp){ //converts the keyboard input to the column index (a-1, s-2, d-3, f-4)
    if (inp == 'a') return 1;
    else if (inp == 's') return 2;
    else if (inp == 'd') return 3;
    else if (inp == 'f') return 4;
    else return -1;
}

void handleInput (Tile &tiles, int &presscol){//checks if you pressed a button corresponding to a tile
    if (presscol == tiles.col){ 
        tiles.touched = true; 
    }
    else {
        tiles.touched = false;
    }
    presscol = -1;
}

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
    int pressedCol = -1; //takes the value of the pressed column (1,2,3,4 or -1 if nothing is pressed)
    Tile tile1;
    tile1.col = 2;
    char keybInput; //takes the keyboard input, I still haven't used it
    while (keepup){

        //printing the grid
        auto grid = makeEmptyGrid();
        placeTile(grid, tile1);
        PrintGrid(grid);

        //user input 
        pressedCol = keyboardToIndex(keybInput);
        handleInput(tile1, pressedCol);

        //checks
        keepup = AdvanceTick(tile1);

        //tick timing
        this_thread::sleep_for(chrono::milliseconds(220));
    }
    return 0;
}