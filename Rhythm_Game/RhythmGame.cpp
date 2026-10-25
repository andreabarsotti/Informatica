#include <iostream>
#include <ctime>
#include <vector>
#include <cstdlib>
#include <thread> 
#include <chrono>
#include <termios.h>
#include <poll.h>
#include <unistd.h>

using namespace std;

// -- Creating the tile struct and setting grid dimensions --------------------------------------------

constexpr int ROWS = 15;
constexpr int COLS = 4;

class Tile {
    public:
        int col; //column of the tile: it ranges from 0 to 3, keys ASDF
        int pos = 0; //default vertical position of the tile
        bool solved = false; //checks if the Tile has been successfully hit or not (if true, the Tile must be destroyed)
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


// -- Generating Patterns ------------------------------------------------------------------------

Tile SingleTile1(){
    Tile tile;
    tile.col = 0;
    tile.pos = -2;
    return tile;
}

Tile SingleTile2(){
    Tile tile;
    tile.col = 3;
    tile.pos = -2;
    return tile;
}

// GAME LOGIC ---------------------------------------------------------------------------------


// -- Getting the input and converting it to an integer --------------------------------------

termios originalTermios; //stores terminal's settings

void initializeInput(){ //function that enables inputs that don't block the game flow, require no Enter and don't show up in the terminal
    tcgetattr(STDIN_FILENO, &originalTermios); //actually storing terminal's settings

    termios raw = originalTermios; //creating a copy of the settings

    raw.c_lflag &= ~(ICANON | ECHO); //setting no need for Enter and no delay

    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

void restoreInput(){ //function that restore the usual terminal input methods
    tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios);
}

int keyboardInput(){ //function that checks if there is a user input

    //defines what to look for in the input
    pollfd fds;
    fds.fd = STDIN_FILENO;
    fds.events = POLLIN;

    //checks if there's an input right now
    int ready = poll(&fds, 1, 0);

    if (ready <= 0) {
        return -1; //no input
    }

    if (!(fds.revents & POLLIN)) {
        return -1; //no input
    }

    //reads the input
    char key;
    ssize_t bytesRead = read(STDIN_FILENO, &key, 1);

    if (bytesRead != 1) {
        return -1; //no input
    }

    return static_cast<unsigned char>(key); //key holds the user input
}

int keyboardToIndex (int inp){ //converts the keyboard input to the column index (a-0, s-1, d-2, f-3)
    if (inp == 'a') return 0;
    else if (inp == 's') return 1;
    else if (inp == 'd') return 2;
    else if (inp == 'f') return 3;
    else return -1;
}

// -- Handling the input ------------------------------------------------------------------

enum outcomes{ //possible outcomes of the user input
    Hit = 0, //the righy key was pressed when a tile was on the target row
    NoInput = 1, //no ASDF key pressed
    Misclick = 2, //wrong ASDF key while tile on target row
    EarlyMiss = 3 //ASDF key while no tiles on target rows
};

outcomes handleInput (Tile &tiles, int &presscol){ //checks if you pressed a button corresponding to the right column when the tile is on the target row
    if (presscol == -1) {
        return NoInput;
    }
    else if (presscol == tiles.col && tiles.pos == ROWS){ 
        return Hit; 
    }
    else if (tiles.pos < ROWS){
        return EarlyMiss;
    }
    else {
        return Misclick;
    }
}

// -- Making the game flow ------------------------------------------------------------

bool AdvanceTick (Tile &tiles, outcomes judge){
    if (judge == Hit){ //hit correctly
        tiles.solved = true;
        return true;
    }
    tiles.pos++;
    if (tiles.pos>ROWS && !tiles.solved){ //missed it
        cout << "You lost :(" << endl;
        return false;
    }
    return true;
}


// -- Main ----------------------------------------------------------------------------------------

int main(){

    // game running variables
    bool keepup = true; //checks wether you lost or can keep going
    int pressedCol = -1; //takes the value of the pressed column (1,2,3,4 or -1 if nothing is pressed)
    int keybInput; // takes the keyboard input
    enum outcomes judgement; //checks what the player did
    int multiple = 0; //first version for multiple tiles: used only for recreating the same tile over and over

    initializeInput(); //changes terminal settings

    Tile tile1 = SingleTile1();

    while (keepup){

        //printing the grid
        auto grid = makeEmptyGrid();
        placeTile(grid, tile1);
        PrintGrid(grid);

        //user input 
        keybInput = keyboardInput();
        if (keybInput != -1){ //there's an input, so let's read it
            keybInput = static_cast<char>(tolower(keybInput));
            pressedCol = keyboardToIndex(keybInput);
        }

        //checks and flow
        judgement = handleInput(tile1, pressedCol);
        if (judgement == Misclick || judgement == EarlyMiss){
            cout << "You lost :(" << endl;
            keepup = false;
        }
        
        keepup = AdvanceTick(tile1, judgement) && keepup; //is true only if advancetick and judgements say so

        if (tile1.solved) tile1.~Tile(); //destroys solved tiles

        //used in the first version of multiple tiles
        multiple++; 
        if (multiple%19==0 && multiple>1){
            tile1.solved = false;
            tile1.pos = 0;
            tile1.col = 0;
        }

        pressedCol = -1; //resets for next tick

        //tick timing
        this_thread::sleep_for(chrono::milliseconds(220));
    }

    restoreInput(); //restore terminal settings 

    return 0;
}