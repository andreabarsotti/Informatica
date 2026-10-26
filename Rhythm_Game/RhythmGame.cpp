#include <iostream>
#include <ctime>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <thread> 
#include <chrono>
#include <termios.h>
#include <poll.h>
#include <cctype>
#include <unistd.h>

using namespace std;

// -- Creating the tile class and setting grid dimensions --------------------------------------------

constexpr int ROWS = 15;
constexpr int COLS = 4;
constexpr int TARGET_ROW = ROWS - 1;

class Tile {
    public:
        int col; //column of the tile: it ranges from 0 to 3, keys ASDF
        int pos = 0; //default vertical position of the tile
        bool solved = false; //checks if the Tile has been successfully hit or not (if true, the Tile must be destroyed)
};

enum outcomes{ //possible outcomes of the user input
    Hit, //the righy key was pressed when a tile was on the target row
    NoInput, //no ASDF key pressed
    Misclick, //wrong ASDF key while tile on target row
    EarlyMiss //ASDF key while no tiles on target rows
};

class World { //contains all the tiles present in the game
    private:
        vector<Tile> tiles;

    public:
        void addTiles(const vector<Tile>& newTiles);
        //void updateAll();
        void drawAll() const;
        //bool allSolved() const;
        Tile* findHittableTile(int pressCol);
        outcomes judgeInput(int pressCol);
        bool advanceAllTiles();
        void removeSolved();
        bool tick(int pressedCol);
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

void World::drawAll() const{
    auto grid = makeEmptyGrid();
    for (const Tile& tile : tiles){
        placeTile(grid, tile);
    }
    PrintGrid(grid);
}

// -- Generating Patterns ------------------------------------------------------------------------

Tile SingleTile(){
    Tile tile;
    tile.col = 0;
    tile.pos = -2;
    return tile;
}

vector<Tile> TwoTiles(){
    vector<Tile> tiles(2);
    for (int i = 0; i<2; i++){
        tiles[i].col=i;
        tiles[i].pos = -1-i;
    }
    return tiles;
}

// GAME LOGIC ---------------------------------------------------------------------------------

// -- Inserting patterns into the World class --------------------------------------------------
void World::addTiles(const vector<Tile>& newTiles) {
    for (const Tile& t : newTiles) {
        tiles.push_back(t);
    }
}

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

// -- Making the game flow ------------------------------------------------------------


Tile* World::findHittableTile(int pressCol) {
    for (Tile& tile : tiles) {
        if (!tile.solved &&
            tile.col == pressCol &&
            tile.pos == TARGET_ROW) {
            return &tile;
        }
    }

    return nullptr;
}

outcomes World::judgeInput(int pressCol){
    if (pressCol == -1) {
        return NoInput;
    }

    Tile* target = findHittableTile(pressCol);

    if (target != nullptr) {
        target->solved = true;   // mark it now; remove it later
        return Hit;
    }

    // A key was pressed, but no hittable tile was in that column.
    return Misclick;
}

bool World::advanceAllTiles() {
    for (Tile& tile : tiles) {
        if (tile.solved) {
            continue; // solved tiles are waiting to be erased
        }

        tile.pos++;

        if (tile.pos >= ROWS) {
            return false; // a tile passed the bottom: player lost
        }
    }

    return true;
}

void World::removeSolved(){
    erase_if(tiles, [](const Tile& t) {
        return t.solved;
    });
}

bool World::tick(int pressedCol) {
    // 1. Handle the player's action
    outcomes result = judgeInput(pressedCol);

    if (result == Misclick || result == EarlyMiss) {
        return false; // player lost
    }

    // 2. Move every active tile
    if (!advanceAllTiles()) {
        return false; // a tile reached/passed the bottom
    }

    // 3. Delete tiles that were successfully hit
    removeSolved();

    return true; // game continues
}


// -- Main ----------------------------------------------------------------------------------------

int main(){

    // game running variables
    bool keepup = true; //checks wether you lost or can keep going

    initializeInput(); //changes terminal settings

    World world; //creates the variable that stores all the tiles
    world.addTiles(TwoTiles());

    while (keepup) {
    world.drawAll();

    int key = keyboardInput();

    int pressedCol = -1;
    if (key != -1) {
        pressedCol = keyboardToIndex(tolower(key));
    }

    keepup = world.tick(pressedCol);

    this_thread::sleep_for(chrono::milliseconds(220));
}

    restoreInput(); //restore terminal settings 

    return 0;
}