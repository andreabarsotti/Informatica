#include <iostream>
#include <ctime>
#include <vector>
#include <cstdlib>
#include <thread> 
#include <chrono>

using namespace std;

struct Tile {
    int col; //column of the tile: it ranges from 1 to 4, keys ASDF
    int pos = 10; //vertical position of the tile
    bool touched = false; //if the Tile has been touched or not
};

bool AdvanceTick (Tile &tiles){ //function used to make the game progress
    tiles.pos--;
    cout << tiles.pos << endl;
    if ((tiles.pos==0 && !tiles.touched) || (tiles.pos!=0 && tiles.touched)){
        cout << "You lost :(" << endl;
        return false;
    }
    else {
        return true;
    }
}

int main(){
    bool keepup = true; //variabile to check if you lost or you can keep going
    Tile tile1;
    tile1.col = 1;
    while (keepup){
        keepup = AdvanceTick(tile1);
        this_thread::sleep_for(chrono::milliseconds(80));
    }
    return 0;
}