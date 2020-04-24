#include "maze.h"
#include "path.h"
#include<queue>
#include<vector>
#include<list>
#include<tuple>
#include<utility>
#include<iostream>
#include<climits>
#include<sstream>
using namespace std;

path solve_left(Maze& m, int rows, int cols);
path solve_dfs(Maze& m, int rows, int cols);
path solve_bfs(Maze& m, int rows, int cols);
path solve_dijkstra(Maze& m, int rows, int cols);
path solve_tour(Maze& m, int rows, int cols);


int main(int argc, char** argv)
{
    if(argc != 4)
    {
        cerr << "usage:\n"
             << "./maze option rows cols\n"
             << " options:\n"
             << "  -left: always-look-left search\n"
             << "  -dfs:  depth first search (backtracking)\n"
             << "  -bfs:  breadth first search\n"
             << "  -dij:  dijkstra's algorithm\n"
             << "  -tour: all corners tour\n"
             << "  -basic: run dfs, bfs, and dij\n"
             << "  -advanced: run dfs, bfs, dij and tour" << endl;
        return 0;
    }
    string opt(argv[1]);

    int rows, cols;
    stringstream s;
    s << argv[2] << " " << argv[3];
    s >> rows >> cols;

    // construct a new random maze;
    Maze m(rows, cols);

    // print the initial maze out
    cout << "Initial maze" << endl;
    m.print_maze(cout, opt == "-dij" || opt == "-tour");

    if(opt == "-left")
    {
        cout << "\nSolved left" << endl;
        path p = solve_left(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);
    }

    if(opt == "-dfs")
    {
        cout << "\nSolved dfs" << endl;
        path p = solve_dfs(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);
    }


    if(opt == "-bfs")
    {
        cout << "\nSolved bfs" << endl;
        path p = solve_bfs(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);
    }

    if(opt == "-dij")
    {
        cout << "\nSolved dijkstra" << endl;
        path p = solve_dijkstra(m, rows, cols);
        m.print_maze_with_path(cout, p, true, false);
    }

    if(opt == "-tour")
    {
        cout << "\nSolved all courners tour" << endl;
        path p = solve_tour(m, rows, cols);
        m.print_maze_with_path(cout, p, true, true);
    }
    if(opt == "-basic")
    {
        cout << "\nSolved dfs" << endl;
        path p = solve_dfs(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);

        cout << "\nSolved bfs" << endl;
        p = solve_bfs(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);

        cout << "\nSolved dijkstra" << endl;
        p = solve_dijkstra(m, rows, cols);
        m.print_maze_with_path(cout, p, true, false);
    }
    if(opt == "-advanced")
    {
        cout << "\nSolved dfs" << endl;
        path p = solve_dfs(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);

        cout << "\nSolved bfs" << endl;
        p = solve_bfs(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);

        cout << "\nSolved dijkstra" << endl;
        p = solve_dijkstra(m, rows, cols);
        m.print_maze_with_path(cout, p, true, false);

        cout << "\nSolved all courners tour" << endl;
        p = solve_tour(m, rows, cols);
        m.print_maze_with_path(cout, p, true, true);
    }
}

/*
 *  A brute-force maze solving solution - follow the left wall, no matter how long it takes
 *    and allowing doubling-back on yourself.
 */
path solve_left(Maze& m, int rows, int cols)
{
    int forward     = 2;  // Save the direction we're pointing. We don't know at start, but we know we're at 0,0
    int current_row = 0;
    int current_col = 0;
    int next_row    = 0;
    int next_col    = 0;
    list<point> pointlist;
    point       next_point;

    // Push point 0,0 onto the list
    next_point = make_pair(next_row,next_col);
    pointlist.push_back(next_point);

    #ifdef DEBUG
    std::cout << "push point:  row:  " << next_point.first << "  col:  " << next_point.second << std::endl;
    #endif

    // Calculate our path points and push them onto the list
    while(next_row < m.rows()-1 || next_col < m.columns()-1) {
        int relative_left  = (forward+1)%4;
        int relative_back  = (forward+2)%4;
        int relative_right = (forward+3)%4;

        current_row = next_row;
        current_col = next_col;

        if(m.can_go(relative_left,       current_row, current_col)) { // Can we go left?
            forward = relative_left;                                  // Turn to face left
        }
        else if(m.can_go(forward,        current_row, current_col)) {  // Can we go forward?
        }       
        else if(m.can_go(relative_right, current_row, current_col)) {  // Can we go right?
            forward = relative_right;                                  // Turn to face right
        }       
        else {
            forward = relative_back;                                   // Turn around
        }

        if      (forward == UP) {
            next_row = current_row - 1;
        }
        else if (forward == LEFT) { 
            next_col = current_col - 1;
        }
        else if (forward == DOWN) {
            next_row = current_row + 1;
        }
        else if (forward == RIGHT) {
            next_col = current_col + 1;
        }
        else {
            std::cout << "FAIL - bad direction value.";
        }

        next_point = make_pair(next_row,next_col);
        pointlist.push_back(next_point);
        #ifdef DEBUG
        std::cout << "push point:  row:  " << next_point.first << "  col:  " << next_point.second << std::endl;
        #endif
    }

    #ifdef DEBUG
    for (const point & ipoint : pointlist)
    {
    	std::cout << ipoint.first << "/" << ipoint.second << std::endl;
    }
    #endif
    
    return pointlist;
}

path solve_dfs(Maze& m, int rows, int cols)
{
    return list<point>();
}

path solve_bfs(Maze& m, int rows, int cols)
{
    return list<point>();
}

path solve_dijkstra(Maze& m, int rows, int cols)
{
    return list<point>();
}

path solve_tour(Maze& m, int rows, int cols)
{
    return list<point>();
}
