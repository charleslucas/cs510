#ifndef SQUARE_H
#define SQUARE_H

#include<vector>
#include "path.h"

/**
 * Class representing a square.
 * This is only used for the internals of the maze.
 * You can ignore this class.
 */
class Square
{
private:
    // am I allowed to go up down left or right?
    vector<bool> _walls;
    int _height;
    
public:
    //The default square is completely isolated.
    Square()           : _walls(4,false), _height(0) {}
    Square(int height) : _walls(4,false), _height(height) {}
    Square(const Square& s) : _walls(s._walls), _height(s._height) {}

    // Used for setting up the maze.
    // Set's the boarders for the square.
    void set_dir(bool val, int dir)  {_walls[dir]   = val;}
    void set_height(int height)      {_height       = height;}

    // check if you can go in any of these directions.
    bool can_go_dir(int dir) const  {return _walls[dir];}
    int height() const              {return _height;}
};

#endif // SQUARE_H
