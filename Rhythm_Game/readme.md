# Rhythm Game

### Why I'm doing this
I want to do more codes than we do in class, so this is not suggested by the teacher but it's something I wanted to try

### What this game is about
It's basically piano tiles, the game where you have four columns in which piano keys flow and you have to hit them at the right time to keep the melody. 
The only differences are that I will build it in C++ running on the terminal, and it's mainly centered on rythm rather than melodies of known songs (because I don't want to add audio to it, for now)

### How does it work
The basic structure at the moment is this (first idea):
* The game is a grid with 4 columns associated with the keys A,S,D,F: tiles flow down and you have to press the key when the tile touches the bottom line
* Tiles are a struct that has these variables: the column in which it resides, its vertical position (when position is equal to the end of the grid, you arrived at the bottom), if it has been touched or not 
* Periodically the grid gets refreshed, the tiles flow down and I run a check: if I touched a tile at the same tick in which it arrived at the bottom line, I get a point, otherwise no: after three mistakes I lose the game