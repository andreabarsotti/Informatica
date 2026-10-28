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

// -- Creating the environment and variables -----------------------------------------------------

constexpr int ROWS = 15;
constexpr int COLS = 4;
constexpr int TARGET_ROW = ROWS;

class Tile {
    public:
        int col; //column of the tile: it ranges from 0 to 3, keys ASDF
        int pos = 0; //default vertical position of the tile
        bool solved = false; //checks if the Tile has been successfully hit or not (if true, the Tile must be destroyed)
};

enum outcomes{ 
    Hit, //the righy key was pressed when a tile was on the target row
    NoInput, //no ASDF key pressed
    Fail //wrong ASDF key while tile on target row, or ASDF key while no tiles on target rows
};

class World { //contains all the tiles present in the game
    private:
        vector<Tile> tiles;

    public:
        void addTiles(const vector<Tile>& newTiles); //adds new tiles to the game
        void drawAll() const; //draws the grid with the tiles
        Tile* findHittableTile(int pressCol); //returns a pointer to the hitted tile, if there was one 
        outcomes judgeInput(int pressCol); //checks the outcome of the input
        bool advanceAllTiles(); //advances the Tiles on the grid: returns wether advancing tiles made the player lose or not
        void removeSolved(); //eliminates succesfully hit tiles
        bool tick(int pressedCol); //makes a whole game tick (handling user inputs, advancing tiles, eliminating solved, ...)
};


// -- Printing the Grid ---------------------------------------------------------------------------- 

vector<vector<char>> makeEmptyGrid() { //creates and returns the empty grid
    return vector<vector<char>>(
        ROWS,
        vector<char>(COLS, '.')
    );
}

void placeTile(vector<vector<char>>& grid, const Tile& tile) { //adds one tile to the grid
    if (tile.pos >= 0 && tile.pos < ROWS &&
        tile.col >= 0 && tile.col < COLS) {
        grid[tile.pos][tile.col] = '#';
    }
}

void PrintGrid (const vector<vector<char>>& grid){ //prints the grid with the tiles in it
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
//each function takes in input how much times the pattern repeats -1
/* TEMPLATE:
vector<Tile> NAME(int j){
    int weight=...;
    vector<Tile> tiles(weight);
    for (int i = 0; i<weight; i++){
        tiles[i].col=...;
        tiles[i].pos = -1-i - weight*j;
    }
    return tiles;
}
*/
vector<Tile> Reggaeton(int j){
    int weight=5;
    int col1 = rand()%4;
    int col2;
    do{
        col2 = rand()%4;
    } while(col2==col1);

    vector<Tile> tiles(weight);
    for (int i = 0; i<weight; i++){
        if (i%2==0) tiles[i].col=col1;
        else tiles[i].col=col2;
        tiles[i].pos = -1-i - weight*j;
        if (i==weight-1) tiles[i].solved=true;
    }
    return tiles;
}

vector<Tile> Paradiddle(int j){
    int weight=8;
    int col1 = rand()%4;
    int col2;
    do{
        col2 = rand()%4;
    } while(col2==col1);

    vector<Tile> tiles(weight);
    for (int i=0; i<weight; i++){
        if (i==0 || i==2 || i==3 || i==5) tiles[i].col = col1;
        else tiles[i].col = col2;
        tiles[i].pos = -1 -i -weight*j;
    }
    return tiles;
}


vector<Tile> Gallop(int j){
    int weight = 12;
    int col1 = rand()%2;
    int col2 = col1+1;
    int col3 = col2+1;

    vector<Tile> tiles(weight);
    for (int i=0; i<weight; i++){
        if (i==0 || i==4 || i==8) tiles[i].col = col1;
        else if(i==1 || i==5 || i==9) tiles[i].col = col2;
        else if(i==2 || i==6 || i==10) tiles[i].col = col3;
        else if(i==3 || i==7 || i==11) tiles[i].solved = true;
        tiles[i].pos = -1 -i -weight*j;
    }
    return tiles;
}

vector<Tile> TripleStrokeRoll(int j){
    int weight=12;
    int col1 = rand()%4;
    int col2;
    do{
        col2 = rand()%4;
    } while(col2==col1);

    vector<Tile> tiles(weight);
    for (int i=0; i<weight; i++){
        if (i<3 || (i>5 && i<9)) tiles[i].col = col1;
        else tiles[i].col = col2;
        tiles[i].pos = -1 -i -weight*j;
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

// -- Handling the input ------------------------------------------------------------

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
        target->solved = true;   //the tile is solved
        return Hit;
    }

    return Fail;
}

// -- Running a tick ----------------------------------------------------------------

bool World::advanceAllTiles() { 
    for (Tile& tile : tiles) {
        if (tile.solved) {
            continue; // solved tiles are waiting to be erased
        }

        tile.pos++;

        if (tile.pos > TARGET_ROW) {
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
    //Handles the player's action
    outcomes result = judgeInput(pressedCol);

    if (result == Fail) {
        cout << endl << "You lost :(" << endl;
        return false; // player lost
    }

    //Moves every active tile
    if (!advanceAllTiles()) {
        cout << endl << "You lost :(" << endl;
        return false; // a tile passed the bottom, so the player lost
    }

    //Deletes tiles that were successfully hit
    removeSolved();

    return true; // game continues
}


// -- Main ----------------------------------------------------------------------------------------

int main(){

    bool keepup = true; //checks wether you lost or can keep going
    bool generable = true; //checks if the computer can generate a new pattern
    int weight; //holds the weight of a pattern generator (how many tiles generate, this is the case for the paradiddle only)
    int tick_track = 0; //checks how many ticks passed from a pattern generation
    int j=0;
    srand(time(NULL));

    //storing all possible pattern generators
    using FuncPtr = vector<Tile> (*)(int );
    const vector<FuncPtr> generators = {
        Reggaeton,
        Paradiddle,
        Gallop,
        TripleStrokeRoll
    };
    const vector<int> weights = {
        5,
        8,
        12,
        12
    };

    initializeInput(); //changes terminal settings

    World world; //creates the variable that stores all the tiles

    while (keepup) {
        if (generable){ //generates a pattern
            generable = false;
            j = rand()%4 + 2;
            int g = rand()%generators.size();
            weight = weights[g];
            const auto gen = generators[g]; //actual generator used
            for (int i=0; i<=j; i++){
                world.addTiles(gen(i));
            }
        }
        tick_track++;
        if (tick_track == (j+1)*weight){ 
            tick_track=0;
            generable=true;
        }
        world.drawAll();

        int key = keyboardInput();

        int pressedCol = -1;
        if (key != -1) {
            pressedCol = keyboardToIndex(tolower(key));
        }

        keepup = world.tick(pressedCol);

        this_thread::sleep_for(chrono::milliseconds(250));
    }  

    restoreInput(); //restore terminal settings 

    return 0;
}