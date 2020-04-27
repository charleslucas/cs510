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
#include<map>
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
 *  Construct a list of all the points in order that we traverse through them - duplicates are fine.
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

        if(     m.can_go_up(current_row, current_col)) {
            cout << "UP    at point " << current_row << "/" << current_col << std::endl;
        }
        if(     m.can_go_left(current_row, current_col)) {
            cout << "LEFT  at point " << current_row << "/" << current_col << std::endl;
        }
        if(     m.can_go_down(current_row, current_col)) {
            cout << "DOWN  at point " << current_row << "/" << current_col << std::endl;
        }
        if(     m.can_go_right(current_row, current_col)) {
            cout << "RIGHT at point " << current_row << "/" << current_col << std::endl;
        }


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

struct nodeinfo {
    point parent;
    point next;
    int exits[4];  // The exits from our current point (0=up, 1=left, 2=down, 3=right) - 0 = no exit, 1 = exit, -1 = backtracked from there
};

/*
 *  Implement a depth-first (backtracking) algorithm
 *  Find any solution to the maze - duplicate points are not allowed.
 */
path solve_dfs(Maze& m, int rows, int cols)
{
    list<point> pointlist;  // The list of points to return to the solution checker

    map <point,nodeinfo> node_map;        // Create a map-based list of each room, parent/child and exits

    point       previous_point;
    point       current_point;
    point       next_point;

    bool        end_found;

    // Set our root node as 0,0
    next_point = make_pair(0,0);       // Row, Column
    current_point = make_pair(-1,-1);  // So our first previous_point will be -1,-1

    // Calculate our path points and push them onto the list
    while(current_point.first < m.rows()-1 || current_point.second < m.columns()-1) {
        bool can_go_up    = false;
        bool can_go_left  = false;
        bool can_go_down  = false;
        bool can_go_right = false;
        bool backtrack    = false;

        nodeinfo    current_nodeinfo;

        previous_point = current_point; // May be the parent, or the node we just backtracked from
        current_point = next_point;     // Move focus to the point selected by the previous loop iteration
        
        // If the current node already exists in the map, then we know we are backtracking
        if (node_map.count(current_point)) {
            // Restore the nodeinfo for this node from the map
            current_nodeinfo = node_map.at(current_point);

            #ifdef DEBUG
            std::cout << "current_point (backtrack) = " << current_point.first << "/" << current_point.second << std::endl;
            std::cout << "  previous point " << previous_point.first << "/" << previous_point.second << std::endl;
            std::cout << "  parent point "   << current_nodeinfo.parent.first << "/" << current_nodeinfo.parent.second << std::endl;
            #endif

            node_map.erase(current_point);  // Remove our current point from the map, we will edit the nodeinfo and put it back later

            // Figure out which direction we backtracked from, set that direction to -1 (Row/Column = Y/X)
            if      (previous_point.first == current_point.first-1 && previous_point.second == current_point.second  ) {
                current_nodeinfo.exits[UP]    = -1;
            }
            else if (previous_point.first == current_point.first   && previous_point.second == current_point.second-1) {
                current_nodeinfo.exits[LEFT]  = -1;
            }
            else if (previous_point.first == current_point.first+1 && previous_point.second == current_point.second  ) {
                current_nodeinfo.exits[DOWN]  = -1;
            }
            else if (previous_point.first == current_point.first   && previous_point.second == current_point.second+1) {
                current_nodeinfo.exits[RIGHT] = -1;
            }

            #ifdef DEBUG
            std::cout << "  UP: " << current_nodeinfo.exits[UP] <<"  LEFT: " << current_nodeinfo.exits[LEFT] << "  DOWN: " << current_nodeinfo.exits[DOWN] << "  RIGHT: " << current_nodeinfo.exits[RIGHT] << std::endl;
            #endif
        }
        else {
            // Store the last node as our current parent, except if we're at root then store -1,-1
            if (current_point.first == 0 && current_point.second == 0) {
                current_nodeinfo.parent = make_pair(-1,-1);  // Row, Column (Y,X)
            }
            else {
                current_nodeinfo.parent = previous_point;
            }

            can_go_up    = m.can_go_up   (current_point.first, current_point.second);
            can_go_left  = m.can_go_left (current_point.first, current_point.second);
            can_go_down  = m.can_go_down (current_point.first, current_point.second);
            can_go_right = m.can_go_right(current_point.first, current_point.second);

            #ifdef DEBUG
            std::cout << "current_point (forward) = " << current_point.first << "/" << current_point.second << std::endl;
            std::cout << "  previous point " << previous_point.first << "/" << previous_point.second << std::endl;
            std::cout << "  parent point "   << current_nodeinfo.parent.first << "/" << current_nodeinfo.parent.second << std::endl;
            std::cout << "  can_go_up: "     << can_go_up <<"  can_go_left: " << can_go_left << "  can_go_down: " << can_go_down << "  can_go_right: " << can_go_right << std::endl;
            #endif

            if (can_go_up) {
                point up_from_here = make_pair(current_point.first-1, current_point.second);
                // Check if we came from UP, or if we have seen that point before.  If so, mark it with a -1
                if (((previous_point.first == up_from_here.first) && (previous_point.second == up_from_here.second)) ||
                    (node_map.count(up_from_here) == 1 )) {
                    current_nodeinfo.exits[UP] = -1;
                }
                else {
                    current_nodeinfo.exits[UP] = 1;
                }
            }
            else {
                current_nodeinfo.exits[UP] = 0;
            }
    
            if (can_go_left) {
                point left_from_here = make_pair(current_point.first, current_point.second-1);
                // Check if we came from the LEFT, or if we have seen that point before.  If so, mark it with a -1
                if (((previous_point.first == left_from_here.first) && (previous_point.second == left_from_here.second)) ||
                    (node_map.count(left_from_here) == 1 )) {
                    current_nodeinfo.exits[LEFT] = -1;
                }
                else {
                    current_nodeinfo.exits[LEFT] = 1;
                }
            }
            else {
                current_nodeinfo.exits[LEFT] = 0;
            }
    
            if (can_go_down) {
                point down_from_here = make_pair(current_point.first+1, current_point.second);
                // Check if we came from DOWN, or if we have seen that point before.  If so, mark it with a -1
                if (((previous_point.first == down_from_here.first) && (previous_point.second == down_from_here.second)) ||
                    (node_map.count(down_from_here) == 1 )) {
                    current_nodeinfo.exits[DOWN] = -1;
                }
                else {
                    current_nodeinfo.exits[DOWN] = 1;
                }
            }
            else {
                current_nodeinfo.exits[DOWN] = 0;
            }
    
            if (can_go_right) {
                point right_from_here = make_pair(current_point.first, current_point.second+1);
                // Check if we came from the RIGHT, or if we have seen that point before.  If so, mark it with a -1
                if (((previous_point.first == right_from_here.first) && (previous_point.second == right_from_here.second)) ||
                    (node_map.count(right_from_here) == 1 )) {
                    current_nodeinfo.exits[RIGHT] = -1;
                }
                else {
                    current_nodeinfo.exits[RIGHT] = 1;
                }
            }
            else {
                current_nodeinfo.exits[RIGHT] = 0;
            }

            #ifdef DEBUG
            std::cout << "  UP: " << current_nodeinfo.exits[UP] <<"  LEFT: " << current_nodeinfo.exits[LEFT] << "  DOWN: " << current_nodeinfo.exits[DOWN] << "  RIGHT: " << current_nodeinfo.exits[RIGHT] << std::endl;
            #endif
        }

        // Iterate over the valid exits and choose the next one in order
        // If there aren't any valid exits left, backtrack
        if      (current_nodeinfo.exits[DOWN]  == 1) {
            next_point = make_pair(current_point.first + 1, current_point.second    );
        }
        else if (current_nodeinfo.exits[LEFT]  == 1) { 
            next_point = make_pair(current_point.first    , current_point.second - 1);
        }
        else if (current_nodeinfo.exits[UP]    == 1) {
            next_point = make_pair(current_point.first - 1, current_point.second    );
        }
        else if (current_nodeinfo.exits[RIGHT] == 1) {
            next_point = make_pair(current_point.first    , current_point.second + 1);
        }
        else {
            // If there are no valid exits then either we are at the end or we need to backtrack
            if (current_point.first == m.rows()-1 && current_point.second == m.columns()-1) {
                next_point = make_pair(-1,-1);
                #ifdef DEBUG
                std::cout << "Found the End! " << next_point.first << "/" << next_point.second << std::endl;
                #endif
            }
            else {
                backtrack = true;
                next_point = current_nodeinfo.parent;
                #ifdef DEBUG
                std::cout << "Backtracking to " << current_nodeinfo.parent.first << "/" << current_nodeinfo.parent.second << std::endl;
                #endif
            }
        }
        #ifdef DEBUG
        std::cout << "  next point "   << next_point.first << "/" << next_point.second << std::endl;
        #endif

        // If we have to backtrack, pop our node off the list, set the parent as our next node
        if (backtrack) {
            #ifdef DEBUG
            #endif
            point popped = pointlist.back();
            pointlist.pop_back();
            #ifdef DEBUG
            std::cout << "popped point:  row:  " << popped.first << "  col:  " << popped.second << std::endl;
            #endif
        }
        else {
            // Otherwise, push our current node onto the list and map
            // Store our predicted next point in the current point's nodeinfo
            #ifdef DEBUG
            std::cout << "pushing point:  row:  " << current_point.first << "  col:  " << current_point.second << std::endl;
            #endif
            pointlist.push_back(current_point);
            current_nodeinfo.next = next_point;
            node_map.insert(pair<point,nodeinfo>(current_point, current_nodeinfo));
       }

    }

    #ifdef DEBUG
    // Iterate over our list and print all the points
  	std::cout << std::endl << "Final list:" << std::endl;
    for (const point & ipoint : pointlist)
    {
    	std::cout << ipoint.first << "/" << ipoint.second << std::endl;
    }
    #endif

    return pointlist;
}

struct exitinfo {
    point exits[4];  // The exits from our current point (0=up, 1=left, 2=down, 3=right)
};

/*
 *  Implement a breadth-first algorithm
 *  Construct a list of just the points that make up the shortest distance to the end - duplicates are not allowed.
*/
path solve_bfs(Maze& m, int rows, int cols)
{
    list<point> pointlist;  // The resulting point list we will return to the checker

    map <point,exitinfo> exit_map;        // Create a map-based tree structure of each room and it's exits
    map <point,point>    parent_map;      // Create a map-based tree structure of each room and it's parent

    list <point> current_row;             // the nodes in the current row of our tree
    list <point> next_row;                // the nodes in the next row of our tree

    int  tree_level = 0;                  // Current tree level
    bool found_path = 0;                  // Set this when we reach the end square

    point current_point = make_pair(0,0);       // first=row, second=col, always start at 0,0
    exitinfo  current_exitinfo;

    current_row.push_back(current_point);    // Insert our first point into the first tree row
    parent_map.insert(pair<point,point>(current_point, make_pair(-1,-1)));  // Use -1,-1 to denote that our root node has no parent

    // Discover the exits for each point in each possible path until we find the end
    while(found_path == false) {

        std::cout << std::endl << "Discovering maze tree level " << tree_level << std::endl;

        // Iterate over all the points in the current tree row, find their exits
        for (std::list<point>::iterator it = current_row.begin(); it != current_row.end(); it++) {
            point it_point = *it;
            current_point = make_pair(it_point.first, it_point.second);
            std::cout << "current_point = " << current_point.first << "/" << current_point.second << std::endl;

            point current_parent = parent_map.at(current_point);
            cout << "  parent point " << current_parent.first << "/" << current_parent.second << std::endl;

            bool can_go_up    = m.can_go_up(current_point.first, current_point.second);
            bool can_go_left  = m.can_go_left(current_point.first, current_point.second);
            bool can_go_down  = m.can_go_down(current_point.first, current_point.second);
            bool can_go_right = m.can_go_right(current_point.first, current_point.second);
            cout << "  UP: " << can_go_up <<"  LEFT: " << can_go_left << "  DOWN: " << can_go_down << "  RIGHT: " << can_go_right << std::endl;
         
            // Determine all the exits from our current point
            if(can_go_up) {                           // Can we go up?
                cout << "UP    at point " << current_point.first << "/" << current_point.second << std::endl;
                point up_point = make_pair(current_point.first-1, current_point.second);
                current_exitinfo.exits[0] = up_point;     // Store the x,y for the point to up
                if ((up_point != current_parent) && (exit_map.count(up_point) == 0)) {
                    cout << "  up     point " << up_point.first << "/" << up_point.second << std::endl;
                    next_row.push_back(up_point);             // Insert the exit point into the next tree row
                    parent_map.insert(pair<point,point>(up_point, current_point));  // Insert our current node into the parent tree map
                }
            }
            if(can_go_left) {                         // Can we go left?
                cout << "LEFT  at point " << current_point.first << "/" << current_point.second << std::endl;
                point left_point = make_pair(current_point.first, current_point.second-1);
                current_exitinfo.exits[1] = left_point;   // Store the x,y for the point to the left
                if ((left_point != current_parent) && (exit_map.count(left_point) == 0)) {
                    cout << "  left   point " << left_point.first << "/" << left_point.second << std::endl;
                    next_row.push_back(left_point);           // Insert the exit point into the next tree row
                    parent_map.insert(pair<point,point>(left_point, current_point));  // Insert our current node into the parent tree map
                }
            }
            if(can_go_down) {                         // Can we go down?
                cout << "DOWN  at point " << current_point.first << "/" << current_point.second << std::endl;
                point down_point = make_pair(current_point.first+1, current_point.second);
                current_exitinfo.exits[2] = down_point;   // Store the x,y for the point to down
                if ((down_point != current_parent) && (exit_map.count(down_point) == 0)) {
                    cout << "  down   point " << down_point.first << "/" << down_point.second << std::endl;
                    next_row.push_back(down_point);           // Insert the exit point into the next tree row
                    parent_map.insert(pair<point,point>(down_point, current_point));  // Insert our current node into the parent tree map
                }
            }       
            if(can_go_right) {                         // Can we go right?
                cout << "RIGHT at point " << current_point.first << "/" << current_point.second << std::endl;
                point right_point = make_pair(current_point.first, current_point.second+1);
                current_exitinfo.exits[3] = right_point;  // Store the x,y for the point to the right
                if ((right_point != current_parent) && (exit_map.count(right_point) == 0)) {
                    cout << "  right  point " << right_point.first << "/" << right_point.second << std::endl;
                    next_row.push_back(right_point);          // Insert the exit point into the next tree row
                    parent_map.insert(pair<point,point>(right_point, current_point));  // Insert our current node into the parent tree map
                }
            }       

            exit_map.insert(pair<point,exitinfo>(current_point, current_exitinfo));  // Insert our current node into the tree map

            // If we found the end of the maze, exit the while loop
            if (current_point.first == m.rows()-1 && current_point.second == m.columns()-1) {
                found_path = true;
                std::cout << "Discovered the end point!" << std::endl;
            }
        }

        // Copy the contents of next_row over current_row
        std::cout << "current_list1:  ";
        for (std::list<point>::iterator it = current_row.begin(); it != current_row.end(); it++) {
            point p = *it;
            std::cout << p.first << "/" << p.second << ",";
        }
        std::cout << std::endl;
        current_row = next_row;
        next_row.clear();
        std::cout << "current_list2:  ";
        for (std::list<point>::iterator it = current_row.begin(); it != current_row.end(); it++) {
            point p = *it;
            std::cout << p.first << "/" << p.second << ",";
        }
        std::cout << std::endl;

        tree_level++;

    }

    // Iterate over our current set of exit points and see if we have seen them before

    // Push start point onto the list
    //pointlist.push_back(current_point);

//    //#ifdef DEBUG
//    std::cout << "push point:  row:  " << next_point.first << "  col:  " << next_point.second << std::endl;
//    //#endif
//
//    // Calculate our path points and push them onto the list
//    while(next_row < m.rows()-1 || next_col < m.columns()-1) {
//        int relative_left  = (forward+1)%4;
//        int relative_back  = (forward+2)%4;
//        int relative_right = (forward+3)%4;
//
//        current_row = next_row;
//        current_col = next_col;
//
//        if(m.can_go(relative_left,       current_row, current_col)) {  // Can we go left?
//            forward = relative_left;                                   // Turn to face left
//            directionlist.push_back(forward);                          // Push relative left onto the direction stack
//        }
//        else if(m.can_go(forward,        current_row, current_col)) {  // Can we go forward?
//            directionlist.push_back(forward);                          // Push forward onto the direction stack
//        }       
//        else if(m.can_go(relative_right, current_row, current_col)) {  // Can we go right?
//            forward = relative_right;                                  // Turn to face right
//            directionlist.push_back(forward);                          // Push relative right onto the direction stack
//        }       
//        else {
//            //forward = relative_back;                                   // Turn around
//
//            // Pop the last entry off both stacks
//            directionlist.pop_back();
//            pointlist.pop_back();
//            
//            // Recover our new current location
//            next_row = pointlist.back().first;
//            next_col = pointlist.back().second;
//
//            break;  // Exit this loop iteration, go check the next direction from where we are pointing
//        }
//
//        // Translate back from relative to cardinal directions
//        if      (forward == UP) {
//            next_row = current_row - 1;
//        }
//        else if (forward == LEFT) { 
//            next_col = current_col - 1;
//        }
//        else if (forward == DOWN) {
//            next_row = current_row + 1;
//        }
//        else if (forward == RIGHT) {
//            next_col = current_col + 1;
//        }
//        else {
//            std::cout << "FAIL - bad direction value.";
//        }
//
//        next_point = make_pair(next_row,next_col);
//        pointlist.push_back(next_point);
//        //#ifdef DEBUG
//        std::cout << "push point:  row:  " << next_point.first << "  col:  " << next_point.second << std::endl;
//        //#endif
//    }
//
//    #ifdef DEBUG
//    for (const point & ipoint : pointlist)
//    {
//    	std::cout << ipoint.first << "/" << ipoint.second << std::endl;
//    }
//    #endif
    
    return pointlist;
}

path solve_dijkstra(Maze& m, int rows, int cols)
{
    return list<point>();
}

path solve_tour(Maze& m, int rows, int cols)
{
    return list<point>();
}
