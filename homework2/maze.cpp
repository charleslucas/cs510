#include "maze.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <random>
#include <list>
#include <utility>

using namespace std;

// used for printing underlined characters
const char* us = "\033[4m";
const char* ue = "\033[0m";


/**
 * Constructor for Maze class
 * Sets up a new maze that is a grid of rows and columns
 * The Maze is completely disconnected.
 * each cell has walls around it.
 * 
 * @rows number of rows
 * @cols number of columns
 */
Maze::Maze(int rows, int cols) : _rows(rows), _cols(cols),
                                 _squares(rows, vector<Square>(cols, Square()))
{
    gen_random_maze();
}

/**
 * Generates a random maze using a depth first search.
 */
void Maze::gen_random_maze()
{
    //make a vector of cells we've already seen
    //so we don't get in an infinite loop
    vector<vector<bool>> seen(_rows, vector<bool>(_cols, false));

    // Initialize random
    // We don't need good randomness, we just need it to be different
    // every time we run the program
    random_device r;
    default_random_engine rng(r());
    gen_dfs(seen, 0, 0, rng);

    // delete about 1/10 of the walls
    delete_walls(0.1, r);

    set_heights();
}


/**
 * Sets all squares to a random height
 */
void Maze::set_heights()
{
    for(int r = 0; r < _rows; r++)
    {
        for(int c = 0; c < _cols; c++)
        {
            _squares[r][c].set_height(rand()%10);
        }
    }
}

/**
 * delete some of the walls
 *
 * @param frac the fraction of walls to delete
 * @param r our random number generator.
 */
void Maze::delete_walls(double frac, random_device& rng)
{
    // set up uniform distributions for deleting walls
    uniform_int_distribution<int> ur(1, _rows-2);
    uniform_int_distribution<int> uc(1, _cols-2);
    uniform_int_distribution<int> u(0, 3);

    for(int i = 0; i < _rows*_cols*frac; i++)
    {
        //keep going until we actually delete something
        bool deleted = false;
        while(!deleted)
        {
            int r = ur(rng);
            int c = uc(rng);
            int dir = u(rng);

            // did we actually delete anything?
            deleted = !_squares[r][c].can_go_dir(dir);

            auto [dr,dc] = moveIn(dir);

            _squares[r][c].set_dir(true, dir);
            _squares[r+dr][c+dc].set_dir(true, opposite(dir));
        }
    }
}


/**
 *
 * Generates a random maze using a depth first search.
 * This version runs recursively.
 *
 * @param seen vector of squares we've already visited
 * @param r the current row that we're on
 * @param c the current column that we're on
 *
 */
void Maze::gen_dfs(vector<vector<bool> >& seen, int r, int c, default_random_engine& rng)
{
    seen[r][c] = true;

    //shuffel the directions, so we actually go in a random direction
    vector<int> order = {UP,LEFT,DOWN,RIGHT};
    shuffle(order.begin(), order.end(), rng);

    // for each possible direction check if we can go there
    // we use order to randomly permute the directions.
    for(int i = 0; i < 4; i++)
    {
        auto [dr,dc] = moveIn(order[i]);


        //if we are within the bounds of our maze
        //AND we haven't visited the square above us yet.
        if(r+dr >= 0 && r+dr < _rows &&
           c+dc >= 0 && c+dc < _cols &&
           !seen[r+dr][c+dc])
        {
            //kill the wall between this square and the square above us
            _squares[r][c].set_dir(true, order[i]);
            _squares[r+dr][c+dc].set_dir(true, opposite(order[i]));

            //continue from the square above us.
            gen_dfs(seen, r+dr, c+dc, rng);
        }
    }
}

////////////////////////////////////////////////////////////////////////
//
// print the maze
//
////////////////////////////////////////////////////////////////////////

/**
 * Print out the maze
 *
 * @param weighted print out the heights of the rooms
 */
void Maze::print_maze(ostream& out, bool weighted) const
{
    //print the top boarder of the maze
    out << us;
    for(int i = 0; i < _cols; i++)
    {
        out << "  ";
    }
    out << " " << ue << endl;

    for(int r = 0; r < _rows; r++)
    {
        //print the left boarder of the maze
        out << "|";

        //for each square check if it can go right or down
        //Note: left and up are checked by the squared to the left and above
        //Note2: we don't need to print the bottom or right boarders, because the 
        //last square in a row/column, can never leave the maze
        for(int c = 0; c < _cols; c++)
        {
            if(_squares[r][c].can_go_dir(DOWN))
            {
                if(weighted)
                    out << _squares[r][c].height();
                else
                    out << " ";
            }
            else
            {
                if(weighted)
                    out << us << _squares[r][c].height() << ue;
                else
                    out << us << " " << ue;
            }
            if(_squares[r][c].can_go_dir(RIGHT))
                out << us << " " << ue;
            else
                out << '|';
        }
        out << endl;
    }
}

/**
 * Print out the maze, as well as the path
 *
 * @param out the stream to write to
 * @param path the path to print out
 * @param weighted print out the heights
 * @param tour are we checking the path or the tour
 */
void Maze::print_maze_with_path(ostream& out, const list<point>& path, bool weighted, bool tour) const
{

    //keep track of what spaces are on the board
    vector<vector<bool>> board(_rows, vector<bool>(_cols, false));
    vector<int> heights;

    int weight = 0;

    // get all of the heights in the path
    if(!path.empty())
    {
        for(auto [r,c] : path)
        {
            board[r][c] = true;
            heights.push_back(_squares[r][c].height());
        }

        // get the total cost of the path
        for(int i = 0; i < heights.size()-1; i++)
        {
            weight += abs(heights[i] - heights[i+1]);
        }
    }

    //print the top boarder of the maze
    out << us;
    for(int i = 0; i < _cols; i++)
    {
        out << "  ";
    }
    out << " " << ue << endl;

    for(int r = 0; r < _rows; r++)
    {
        //print the left boarder of the maze
        out << "|";

        //for each square check if it can go right or down
        //Note: left and up are checked by the squared to the left and above
        //Note2: we don't need to print the bottom or right boarders, because the 
        //last square in a row/column, can never leave the maze
        for(int c = 0; c < _cols; c++)
        {
            // if this square is in the path, print a *
            if(board[r][c])
            {
                if(_squares[r][c].can_go_dir(DOWN))
                    out << "*";
                else 
                    out << us << "*" << ue;
            }
            else
            {
                // either print out the height or a space
                char space = weighted ? _squares[r][c].height() + '0' : ' ';
                if(_squares[r][c].can_go_dir(DOWN))
                {
                    out << space;
                }
                else
                {
                    out << us << space << ue;
                }
            }
            if(_squares[r][c].can_go_dir(RIGHT))
                out << us << " " << ue;
            else
                out << '|';
        }
        out << endl;
    }

    out << "total time: " << weight << endl;

    // check to see if it's a valid path
    if((!tour  && valid_solution(*this,path)) ||
       (tour && valid_tour(*this,path)))
        out << "valid" << endl;
    else
        out << "invalid" << endl;
}


/**
 * Check to see if it's a valid path through the maze.
 *
 * if we start at (0,0)
 * and end at (rows-1, columns-1)
 * and it's a valid path
 */
bool valid_solution(const Maze& m, const list<point>& p)
{
    return !p.empty() &&
           p.front() == make_pair(0,0) &&
           p.back() == make_pair(m.rows()-1, m.columns()-1) &&
           valid_path(m, p);
}

/**
 * Check to see if it's a valid tour.
 *
 * if our path contains (0,0) (0,columns-1) (rows-1,0) (rows-1,columns-1)
 * and we start and end on rows/2 and columns/2
 * and it's a valid path
 */
bool valid_tour(const Maze& m, const list<point>& p)
{
    // if our path contains (0,0) (0,columns-1) (rows-1,0) (rows-1,columns-1)
    // and we start and end on rows/2 and columns/2
    // and it's a valid path
    return !p.empty() &&
           find(p.begin(), p.end(), make_pair(0, 0))             != p.end() &&
           find(p.begin(), p.end(), make_pair(0, m.columns()-1)) != p.end() &&
           find(p.begin(), p.end(), make_pair(m.rows()-1, 0))    != p.end() &&
           find(p.begin(), p.end(), make_pair(m.rows()-1, m.columns()-1)) != p.end() &&
           p.front() == make_pair(m.rows()/2,m.columns()/2) &&
           p.back() == make_pair(m.rows()/2,m.columns()/2) &&
           valid_path(m, p);
}


/**
 * Check to see if its a valid path.
 *
 * A path is valid if
 * its not empty,
 * and every node is adjacent to the previous one,
 * and we can travel between each room.
 */
bool valid_path(const Maze& m, const list<point>& p)
{
    if(p.empty())
    {
        return false;
    }

    auto it = p.begin();
    point last = *it;

    // start at the second room
    it++;

    for(; it != p.end(); it++)
    {
        // can we move in this direction
        int dir = direction(*it, last);
        if(dir == FAIL || !m.can_go(dir, it->first, it->second))
        {
            return false;
        }
        last = *it;
    }
    return true;
}
