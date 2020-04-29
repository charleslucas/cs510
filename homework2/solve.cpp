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
#include<iomanip>
#include<algorithm>
using namespace std;

path solve_left(Maze& m, int rows, int cols);
path solve_dfs(Maze& m, int rows, int cols);
path solve_bfs(Maze& m, int rows, int cols);
path solve_bfs_custom(Maze& m, int rows, int cols, point start, point end);
path solve_dijkstra(Maze& m, int rows, int cols);
path solve_dijkstra_custom(Maze& m, int rows, int cols, point start, point end, int &path);
path solve_tour(Maze& m, int rows, int cols);

/**
 * BitmapException denotes an exception from reading in a bitmap.
 */
class SolveException : public std::exception
{
    // the message to print out
    std::string _message;

    // position in the bitmap file (in bytes) where the error occured.
    uint32_t _tree_level;

public:
    SolveException() = delete;

    SolveException(const std::string& message, uint32_t tree_level);
    SolveException(std::string&& message, uint32_t tree_level);

    /**
     * prints out the exception in the form:
     *
     * "Error in solver at position 0xposition :
     * message"
     */
    void print_exception();
};

/**
 * SolveException denotes an exception from a maze solver.
 */
SolveException::SolveException(const std::string& message, uint32_t tree_level) {
    _message  = message;
    _tree_level = tree_level;
}
SolveException::SolveException(std::string&& message, uint32_t tree_level) {
    _message  = message;
    _tree_level = tree_level;
}
void SolveException::print_exception() {
    std::cout << "Solver Exception:  " << _message << " - tree level " << _tree_level << std::endl;
}

struct nodeinfo {
    point parent;
    point next;
    int exits[4];     // The exits from our current point (0=up, 1=left, 2=down, 3=right) - 0 = no exit, 1 = exit, -1 = backtracked from there
};

struct exitinfo {
    point  exits[4];        // The exits from our current point (0=up, 1=left, 2=down, 3=right)
    int    ccost;           // The *cumulative* cost for this node
    int    exit_ccosts[4];  // The *cumulative* costs for each exit (costs to the node in that direction plus all costs to this point)
};

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
        cout << "\nSolving left" << endl;
        path p = solve_left(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);
    }

    if(opt == "-dfs")
    {
        cout << "\nSolving dfs" << endl;
        path p = solve_dfs(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);
    }

    if(opt == "-bfs")
    {
        cout << "\nSolving bfs" << endl;
        path p = solve_bfs(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);
    }

    if(opt == "-dij")
    {
        cout << "\nSolving dijkstra" << endl;
        path p = solve_dijkstra(m, rows, cols);
        m.print_maze_with_path(cout, p, true, false);
    }

    if(opt == "-tour")
    {
        cout << "\nSolving all corners tour" << endl;
        path p = solve_tour(m, rows, cols);
        m.print_maze_with_path(cout, p, true, true);
    }
    if(opt == "-basic")
    {
        cout << "\nSolving dfs" << endl;
        path p = solve_dfs(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);

        cout << "\nSolving bfs" << endl;
        p = solve_bfs(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);

        cout << "\nSolving dijkstra" << endl;
        p = solve_dijkstra(m, rows, cols);
        m.print_maze_with_path(cout, p, true, false);
    }
    if(opt == "-advanced")
    {
        cout << "\nSolving dfs" << endl;
        path p = solve_dfs(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);

        cout << "\nSolving bfs" << endl;
        p = solve_bfs(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);

        cout << "\nSolving dijkstra" << endl;
        p = solve_dijkstra(m, rows, cols);
        m.print_maze_with_path(cout, p, true, false);

        cout << "\nSolving all corners tour" << endl;
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

    // Calculate our path points and push them onto the list
    while(next_row < m.rows()-1 || next_col < m.columns()-1) {
        int relative_left  = (forward+1)%4;
        int relative_back  = (forward+2)%4;
        int relative_right = (forward+3)%4;

        current_row = next_row;
        current_col = next_col;

        if(m.can_go(relative_left,       current_row, current_col)) {  // Can we go left?
            forward = relative_left;                                   // Turn to face left
        }
        else if(m.can_go(forward,        current_row, current_col)) {  // Can we go forward?
        }       
        else if(m.can_go(relative_right, current_row, current_col)) {  // Can we go right?
            forward = relative_right;                                  // Turn to face right
        }       
        else {
            forward = relative_back;                                   // Turn around
        }

        // Choose our next point based on which way we are facing
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
    }

    return pointlist;
}

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
            }
            else {
                backtrack = true;
                next_point = current_nodeinfo.parent;
            }
        }

        // If we have to backtrack, pop our node off the list, set the parent as our next node
        if (backtrack) {
            #ifdef DEBUG
            #endif
            point popped = pointlist.back();
            pointlist.pop_back();
        }
        else {
            // Otherwise, push our current node onto the list and map
            // Store our predicted next point in the current point's nodeinfo
            pointlist.push_back(current_point);
            current_nodeinfo.next = next_point;
            node_map.insert(pair<point,nodeinfo>(current_point, current_nodeinfo));
       }

    }

    return pointlist;
}

/*
 *  Implement a breadth-first algorithm
 *  Construct a list of just the points that make up the shortest distance to the end - duplicates are not allowed.
*/
path solve_bfs(Maze& m, int rows, int cols)
{
    // Pass-through function to call the bfs routine with the default top-left start and bottom-right end points.
    return(solve_bfs_custom(m, rows, cols, make_pair(0,0), make_pair(m.rows()-1, m.columns()-1)));
}

/*
 *  Implement a breadth-first algorithm
 *  Construct a list of just the points that make up the shortest distance from a given start point to the given
 *  end point - duplicate points in the path are not allowed.
*/
path solve_bfs_custom(Maze& m, int rows, int cols, point start, point end)
{
    list<point> pointlist;  // The resulting point list we will return to the checker

    map <point,exitinfo> exit_map;        // Create a map-based tree structure of each room and it's exits
    map <point,point>    parent_map;      // Create a map-based tree structure of each room and it's parent

    list <point> current_row;             // the nodes in the current row of our tree
    list <point> next_row;                // the nodes in the next row of our tree

    int  tree_level = 0;                  // Current tree level
    bool found_path = 0;                  // Set this when we reach the end square

    point current_point = start;          // first=row, second=col, start at our given coordinate
    exitinfo  current_exitinfo;

    point current_parent;                 // For tracking the parent chain while constructing our final list

    current_row.push_back(current_point); // Insert our first point into the first tree row
    parent_map.insert(pair<point,point>(current_point, make_pair(-1,-1)));  // Use -1,-1 to denote that our root node has no parent

    // Discover the exits for each point in each possible path until we find the end
    while(found_path == false) {

        // Iterate over all the points in the current tree row, find their exits
        for (std::list<point>::iterator it = current_row.begin(); it != current_row.end(); it++) {
            point it_point = *it;
            current_point = make_pair(it_point.first, it_point.second);

            point current_parent = parent_map.at(current_point);

            bool can_go_up    = m.can_go_up(current_point.first, current_point.second);
            bool can_go_left  = m.can_go_left(current_point.first, current_point.second);
            bool can_go_down  = m.can_go_down(current_point.first, current_point.second);
            bool can_go_right = m.can_go_right(current_point.first, current_point.second);
         
            // Determine all the exits from our current point
            if(can_go_up) {                                   // Can we go up?
                point up_point = make_pair(current_point.first-1, current_point.second);
                current_exitinfo.exits[0] = up_point;         // Store the x,y for the point to up

                if ((up_point != current_parent) && (exit_map.count(up_point) == 0)) {
                    next_row.push_back(up_point);             // Insert the exit point into the next tree row
                    parent_map.insert(pair<point,point>(up_point, current_point));  // Insert our current node into the parent tree map
                }
            }
            if(can_go_left) {                                 // Can we go left?
                point left_point = make_pair(current_point.first, current_point.second-1);
                current_exitinfo.exits[1] = left_point;       // Store the x,y for the point to the left

                if ((left_point != current_parent) && (exit_map.count(left_point) == 0)) {
                    next_row.push_back(left_point);           // Insert the exit point into the next tree row
                    parent_map.insert(pair<point,point>(left_point, current_point));  // Insert our current node into the parent tree map
                }
            }
            if(can_go_down) {                                 // Can we go down?
                point down_point = make_pair(current_point.first+1, current_point.second);
                current_exitinfo.exits[2] = down_point;       // Store the x,y for the point to down

                if ((down_point != current_parent) && (exit_map.count(down_point) == 0)) {
                    next_row.push_back(down_point);           // Insert the exit point into the next tree row
                    parent_map.insert(pair<point,point>(down_point, current_point));  // Insert our current node into the parent tree map
                }
            }       
            if(can_go_right) {                                // Can we go right?
                point right_point = make_pair(current_point.first, current_point.second+1);
                current_exitinfo.exits[3] = right_point;      // Store the x,y for the point to the right

                if ((right_point != current_parent) && (exit_map.count(right_point) == 0)) {
                    next_row.push_back(right_point);          // Insert the exit point into the next tree row
                    parent_map.insert(pair<point,point>(right_point, current_point));  // Insert our current node into the parent tree map
                }
            }       

            exit_map.insert(pair<point,exitinfo>(current_point, current_exitinfo));  // Insert our current node into the tree map

            // If we found the end of the maze, exit the while loop after the current row
            // If we happen to find multiple solutions this row, they will all be of equal length
            if (current_point == end) {
                found_path = true;
            }
        }

        current_row = next_row;
        next_row.clear();
        tree_level++;

        // Safety check - we shouldn't have more tree levels than squares in the maze
        if (tree_level >= (m.rows() * m.columns())) {
            std::cout << "Runaway while loop - tree level > max number of maze squares" << std::endl;
            throw(SolveException("Exceeded maximum number of tree levels", tree_level));
        }
    }

    // If we are here, then we have found the end of the maze
    current_point = end; // Start with the point at the end of the maze
    pointlist.push_front(current_point);                 // Add that point to the list
    current_parent = parent_map.at(current_point);       // Get that node's parent

    // Iterate backwards over our solution list until we hit -1/-1, the parent of 0,0
    while (current_parent.first != -1 && current_parent.second != -1) {
        current_point = current_parent;                  // Move backwards through the solution path
        pointlist.push_front(current_point);             // Add each point of the solution to the list
        current_parent = parent_map.at(current_point);   // Get the parent for each node
    }
 
    return pointlist;
}

/*
 *  Implement a breadth-first algorithm weighted by cost
 *  Construct a list of just the points that make up the lowest-cost distance to the end - duplicates are not allowed.
*/
path solve_dijkstra(Maze& m, int rows, int cols)
{
    int path_cost;

    // Pass-through function to call the bfs routine with the default top-left start and bottom-right end points.
    return(solve_dijkstra_custom(m, rows, cols, make_pair(0,0), make_pair(m.rows()-1, m.columns()-1), path_cost));
}

/*
 *  Implement a breadth-first algorithm weighted by cost
 *  Construct a list of just the points that make up the lowest-cost distance from a given start point to the given
 *  end point - duplicate points in the path are not allowed.
*/
path solve_dijkstra_custom(Maze& m, int rows, int cols, point start, point end, int& path_cost)
{
    list<point> pointlist;                // The resulting point list we will return to the checker

    map <point,exitinfo> exit_map;        // Create a map-based tree structure of each room and it's exits
    map <point,point>    parent_map;      // Create a map-based tree structure of each room and it's parent

    list <point> current_frontier;        // the nodes in the current frontier of our tree
    list <point> next_frontier;           // the nodes in the next row of our tree

    int  frontier_level = 0;              // How many frontier levels we have iterated through
    bool found_path = false;              // Set this when we reach the end square

    point current_point = start;          // first=row, second=col
    exitinfo  current_exitinfo;

    point current_parent;                 // For tracking the parent chain while constructing our final list

    current_frontier.push_back(current_point); // Insert our first point into the first frontier
    parent_map.insert(pair<point,point>(current_point, make_pair(-1,-1)));  // Use -1,-1 to denote that our root node has no parent

    // Discover the lowest-cost exits for each point in each possible path until we find the end of the maze
    while(found_path == false && current_frontier.size() != 0) {
        int    lowest_ccost = 0;  // The lowest cumulative movement cost we've found so far
        int it_lowest_ccost = 9 * m.rows() * m.columns();  // Lowest cumulative movement cost we've found for each iteration (set to the max possible cost)

        // Iterate over the current frontier row, fill out all the data for new nodes and figure out the lowest ccost for this frontier
        for (std::list<point>::iterator it = current_frontier.begin(); it != current_frontier.end(); it++) {
            exitinfo current_exitinfo;
            exitinfo parent_exitinfo;

            point it_point = *it;
            current_point = make_pair(it_point.first, it_point.second);
            
            // Get the known info for the current point's parent (unless we're the root node)
            if (current_point == start) {
                current_parent = make_pair(-1,-1);
            }
            else {
                current_parent = parent_map.at(current_point);
                parent_exitinfo = exit_map.at(current_parent);
            }

            //  If this point already exists in our map
            if (exit_map.count(current_point) > 0) {
                //#ifdef DEBUG
                //std::cout << "  Current_point " << current_point.first << "/" << current_point.second << " already exists" << std::endl;
                //#endif

                current_exitinfo = exit_map.at(current_point);          // Get our known exit info for the current point

                // Iterate over our known valid exits for this node to find the lowest cumulative cost
                for (int i = 0; i < 4; i++) {
                    if (current_exitinfo.exits[i].first > -1 && current_exitinfo.exits[i].second > -1 && 
                        it_lowest_ccost > current_exitinfo.exit_ccosts[i]) {
                            it_lowest_ccost = current_exitinfo.exit_ccosts[i];
                        }
                }
            }
            else {  // Create a new node

                //#ifdef DEBUG
                //std::cout << "  Current_point " << current_point.first << "/" << current_point.second << " doesn't exist - creating" << std::endl;
                //#endif

                // Determine our current node's base cumulative cost (unless we're at root, then set it to 0)
                if (current_point == start) {
                    current_exitinfo.ccost = 0;
                }
                else {
                    // If our parent is UP from us, our cumulative cost is the parent's cost plus the DOWN cost from there.
                    if ((current_parent.first == current_point.first-1) && (current_parent.second == current_point.second)) {
                        current_exitinfo.ccost = parent_exitinfo.ccost + m.cost(current_parent.first, current_parent.second, DOWN);
                    }
                    // If our parent is LEFT from us, our cumulative cost is the parent's cost plus the RIGHT cost from there.
                    else if ((current_parent.first == current_point.first) && (current_parent.second == current_point.second-1)) {
                        current_exitinfo.ccost = parent_exitinfo.ccost + m.cost(current_parent.first, current_parent.second, RIGHT);
                    }
                    // If our parent is DOWN from us, our cumulative cost is the parent's cost plus the UP cost from there.
                    else if ((current_parent.first == current_point.first+1) && (current_parent.second == current_point.second)) {
                        current_exitinfo.ccost = parent_exitinfo.ccost + m.cost(current_parent.first, current_parent.second, UP);
                    }
                    // If our parent is RIGHT from us, our cumulative cost is the parent's cost plus the LEFT cost from there.
                    else if ((current_parent.first == current_point.first) && (current_parent.second == current_point.second+1)) {
                        current_exitinfo.ccost = parent_exitinfo.ccost + m.cost(current_parent.first, current_parent.second, LEFT);
                    }
                    else {
                        std::cout << "Error - didn't locate a parent node" << std::endl;
                    }
                }

                bool can_go_up    = m.can_go_up(current_point.first, current_point.second);
                bool can_go_left  = m.can_go_left(current_point.first, current_point.second);
                bool can_go_down  = m.can_go_down(current_point.first, current_point.second);
                bool can_go_right = m.can_go_right(current_point.first, current_point.second);

                point    up_point = make_pair(current_point.first-1, current_point.second);
                point  left_point = make_pair(current_point.first, current_point.second-1);
                point  down_point = make_pair(current_point.first+1, current_point.second);
                point right_point = make_pair(current_point.first, current_point.second+1);

                // Determine cost for all the exits from our current point that aren't our parent
                if (can_go_up && (up_point != current_parent)) {
                    int ccost = m.cost(current_point.first, current_point.second, UP) + current_exitinfo.ccost;
                    if (ccost < it_lowest_ccost) it_lowest_ccost = ccost;
                    current_exitinfo.exits[UP] = up_point;
                    current_exitinfo.exit_ccosts[UP] = ccost;
                }
                else
                {   // Mark that exit invalid
                    current_exitinfo.exits[UP] = make_pair(-1, -1);
                    current_exitinfo.exit_ccosts[UP] = -1;
                }
                if (can_go_left && (left_point != current_parent)) {
                    int ccost = m.cost(current_point.first, current_point.second, LEFT) + current_exitinfo.ccost;
                    if (ccost < it_lowest_ccost) it_lowest_ccost = ccost;
                    current_exitinfo.exits[LEFT] = left_point;
                    current_exitinfo.exit_ccosts[LEFT] = ccost;
                }
                else
                {   // Mark that exit invalid
                    current_exitinfo.exits[LEFT] = make_pair(-1, -1);
                    current_exitinfo.exit_ccosts[LEFT] = -1;
                }
                if (can_go_down && (down_point != current_parent)) {
                    int ccost = m.cost(current_point.first, current_point.second, DOWN) + current_exitinfo.ccost;
                    if (ccost < it_lowest_ccost) it_lowest_ccost = ccost;
                    current_exitinfo.exits[DOWN] = down_point;
                    current_exitinfo.exit_ccosts[DOWN] = ccost;
                }
                else
                {   // Mark that exit invalid
                    current_exitinfo.exits[DOWN] = make_pair(-1, -1);
                    current_exitinfo.exit_ccosts[DOWN] = -1;
                }
                if (can_go_right && (right_point != current_parent)) {
                    int ccost = m.cost(current_point.first, current_point.second, RIGHT) + current_exitinfo.ccost;
                    if (ccost < it_lowest_ccost) it_lowest_ccost = ccost;
                    current_exitinfo.exits[RIGHT] = right_point;
                    current_exitinfo.exit_ccosts[RIGHT] = ccost;
                }
                else
                {   // Mark that exit invalid
                    current_exitinfo.exits[RIGHT] = make_pair(-1, -1);
                    current_exitinfo.exit_ccosts[RIGHT] = -1;
                }

                exit_map.insert(pair<point,exitinfo>(current_point, current_exitinfo));  // Insert our new node into the tree map
                parent_map.insert(pair<point,point> (current_point, current_parent));    // and register it's parent

            }
        }

        // Iterate over the current frontier row again, construct our next frontier based on the lowest cost exits from this one
        // All these nodes should exist in the exit map now
        for (std::list<point>::iterator it = current_frontier.begin(); it != current_frontier.end(); it++) {
            point it_point = *it;
            current_point = make_pair(it_point.first, it_point.second);
            current_exitinfo = exit_map.at(current_point);          // Get our known exit info for the current point
            
            bool can_go_up    = m.can_go_up(current_point.first, current_point.second);
            bool can_go_left  = m.can_go_left(current_point.first, current_point.second);
            bool can_go_down  = m.can_go_down(current_point.first, current_point.second);
            bool can_go_right = m.can_go_right(current_point.first, current_point.second);

            // If one or more exits are equal to our lowest cumulative cost, add them to the next frontier
            if((current_exitinfo.exits[UP].first > -1) && (current_exitinfo.exits[UP].second > -1)) {
                const int up_cost = current_exitinfo.ccost + m.cost(current_point.first, current_point.second, UP);
                if (up_cost == it_lowest_ccost) {
                    const point up_point = make_pair(current_point.first-1, current_point.second);

                    // Try to register this node as the new node's parent - use the return value to check if we've seen it before
                    std::pair<std::map<point,point>::iterator,bool> ret;
                    ret = parent_map.insert(make_pair(up_point, current_point));

                    // If we've registered this node already, skip and mark the exit as invalid
                    if (ret.second == false) {
                        next_frontier.push_back(up_point);                       // Put the node at that exit in our next frontier
                        current_exitinfo.exits[UP] = make_pair(-1,-1);           // Mark this exit invalid now
                    }
                }
            }
            if((current_exitinfo.exits[LEFT].first > -1) && (current_exitinfo.exits[LEFT].second > -1)) {
                const int left_cost = current_exitinfo.ccost + m.cost(current_point.first, current_point.second, LEFT);
                if (left_cost == it_lowest_ccost) {
                    const point left_point = make_pair(current_point.first, current_point.second-1);

                    // Try to register this node as the new node's parent - use the return value to check if we've seen it before
                    std::pair<std::map<point,point>::iterator,bool> ret;
                    ret = parent_map.insert(make_pair(left_point, current_point));

                    // If we've registered this node already, skip and mark the exit as invalid
                    if (ret.second == false) {
                        next_frontier.push_back(left_point);                     // Put the node at that exit in our next frontier
                        current_exitinfo.exits[LEFT] = make_pair(-1,-1);         // Mark this exit invalid now
                    }
                }
            }
            if((current_exitinfo.exits[DOWN].first > -1) && (current_exitinfo.exits[DOWN].second > -1)) {
                const int down_cost = current_exitinfo.ccost + m.cost(current_point.first, current_point.second, DOWN);
                if (down_cost == it_lowest_ccost) {
                    const point down_point = make_pair(current_point.first+1, current_point.second);
                    // Try to register this node as the new node's parent - use the return value to check if we've seen it before
                    std::pair<std::map<point,point>::iterator,bool> ret;
                    ret = parent_map.insert(make_pair(down_point, current_point));

                    // If we've registered this node already, skip and mark the exit as invalid
                    if (ret.second == false) {
                        next_frontier.push_back(down_point);                     // Put the node at that exit in our next frontier
                        current_exitinfo.exits[DOWN] = make_pair(-1,-1);         // Mark this exit invalid now
                    }
                }
            }
            if((current_exitinfo.exits[RIGHT].first > -1) && (current_exitinfo.exits[RIGHT].second > -1)) {
                const int right_cost = current_exitinfo.ccost + m.cost(current_point.first, current_point.second, RIGHT);
                if (right_cost == it_lowest_ccost) {
                    const point right_point = make_pair(current_point.first, current_point.second+1);

                    // Try to register this node as the new node's parent - use the return value to check if we've seen it before
                    std::pair<std::map<point,point>::iterator,bool> ret;
                    ret = parent_map.insert(make_pair(right_point, current_point));

                    // If we've registered this node already, skip and mark the exit as invalid
                    if (ret.second == false) {
                        next_frontier.push_back(right_point);                     // Put the node at that exit in our next frontier
                        current_exitinfo.exits[RIGHT] = make_pair(-1,-1);         // Mark this exit invalid now
                    }
                }
            }

            exit_map.erase(current_point);  // Remove our current point's entry from the map so we can replace it with updated exitinfo
            exit_map.insert(pair<point,exitinfo>(current_point, current_exitinfo));

            // Re-evaluate our current node to see if it has any valid exits left - if so, add it to the next frontier as well
            int valid_exits = 0;
            for (int i = 0; i < 4; i++) {
                if (current_exitinfo.exits[i].first != -1 && current_exitinfo.exits[i].second != -1) {
                    valid_exits = 1;
                }
            }
            if (valid_exits) {
                next_frontier.push_back(current_point);
            }
            
            // If we found the end of the maze, exit the while loop after the current frontier
            // If we happen to find multiple solutions this row, they will all be of equal cost
            // Store our current lowest cost to return with the final path
            if (current_point == end) {
                found_path = true;
                path_cost = it_lowest_ccost;
            }
        }

        // While loop safety check
        if (frontier_level > (5 * m.rows() * m.columns())) {
            std::cout << "Runaway while loop - frontier level == " << frontier_level << std::endl;
            throw(SolveException("Runaway While Loop - Exceeded maximum number of frontier levels", frontier_level));
        }

        current_frontier = next_frontier;
        current_frontier.sort();
        current_frontier.unique();  // Take care of a corner case where we add the same square twice from different sides
        next_frontier.clear();
        frontier_level++;
    }

    // If we are here, then we have found the end of the maze
    current_point = end;                                 // Start with the point at the end of the maze
    pointlist.push_front(current_point);                 // Add that point to the list
    current_parent = parent_map.at(current_point);       // Get that node's parent

    // Iterate backwards over our solution list until we hit -1/-1, the parent of 0,0
    while (current_parent.first != -1 && current_parent.second != -1) {
        current_point = current_parent;                  // Move backwards through the solution path
        pointlist.push_front(current_point);             // Add each point of the solution to the list
        current_parent = parent_map.at(current_point);   // Get the parent for each node
    }
 
    return pointlist;
}

// A structure to handle an array of corner IDs
struct corner_set {
    int corners[4];
};

// The comparison function to be used with the priority queue
// Should sort the lowest second value to the top
struct solution_cmp {
    bool operator()(const pair<corner_set,int> &a, const pair<corner_set,int> &b) {
        return a.second > b.second;
    };
};

path solve_tour(Maze& m, int rows, int cols)
{
    point dungeons[5];                          // Our array of dungeon points we need to visit
    int   cost_matrix[5][5];                    // Our matrix of dungeon-to-dungeon sub-path costs
    int   perm_elements[4] = {1, 2, 3, 4};      // To calculate permutations

    priority_queue<pair<corner_set, int>, vector<pair<corner_set,int>>, solution_cmp> solution_queue;
    pair<point,point> dungeon_path_array[5];    // Our final array of shortest-cost dungeon paths to calculate
    path lowest_cost_path;                      // Our final list of path points to return

    // Make an array of all the "dungeons" we need to visit
    dungeons[0] = make_pair(m.rows()/2, m.columns()/2);  // Our start/end point in the center of the maze
    dungeons[1] = make_pair(0,          0);              // Top-Left Corner
    dungeons[2] = make_pair(0,          m.columns()-1);  // Top-Right Corner
    dungeons[3] = make_pair(m.rows()-1, m.columns()-1);  // Bottom-Right Corner
    dungeons[4] = make_pair(m.rows()-1, 0);              // Bottom-Left Corner

    // Create a matrix of the shortest path cost from each dungeon to each other dungeon
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            path p;             // A list of points
            int path_cost;      // The travel cost of the path

            // Call the bfs/djikstra routine with the matrix of end points.
            //p = solve_bfs_custom(m, rows, cols, dungeons[j], dungeons[i]);
            p = solve_dijkstra_custom(m, rows, cols, dungeons[j], dungeons[i], path_cost);

            // I'm making the assumption here that we care about the maze weights, and not just the distance
            //  as the assignment appears to say, since we were given a weighted maze.
            // If we don't actually care about the maze weights, then the cost of the paths is the length of the path.
            #ifdef TOUR_BFS
                cost_matrix[j][i] = p.size();
            #else
                cost_matrix[j][i] = path_cost;
            #endif
        }
    }

    #ifdef DEBUG
    std::cout << endl;
    std::cout << setw(4) << " " << setw(4) << "0" << setw(4) << "1" << setw(4) << "2" << setw(4) << "3" << setw(4) << "4" << std::endl << std::endl;
    for (int j = 0; j < 5; j++) {
        std::cout << setw(4) << j << setw(4) << cost_matrix[j][0] << setw(4) << cost_matrix[j][1] << setw(4) << cost_matrix[j][2] << setw(4) << cost_matrix[j][3] << setw(4) << cost_matrix[j][4] << std::endl;
    }
    std::cout << endl;
    #endif

    // Iterate over all permutations of our four corners, then find the cost for the set of paths
    //   start -> corner[0] -> corner[1] -> corner[2] -> corner[3] -> start
    do {
        corner_set corner_order;
        corner_order.corners[0] = perm_elements[0];
        corner_order.corners[1] = perm_elements[1];
        corner_order.corners[2] = perm_elements[2];
        corner_order.corners[3] = perm_elements[3];

        // Calculate our final solution cost, adding up the cost for all six legs of the path
        int solution_cost = cost_matrix[0]               [perm_elements[0]] +
                            cost_matrix[perm_elements[0]][perm_elements[1]] +
                            cost_matrix[perm_elements[1]][perm_elements[2]] +
                            cost_matrix[perm_elements[2]][perm_elements[3]] +
                            cost_matrix[perm_elements[3]][0];

        solution_queue.push(make_pair(corner_order, solution_cost));
        #ifdef DEBUG
        std::cout << "Solution Distance: " << solution_cost << " from " << perm_elements[0] << ' ' << perm_elements[1] << ' ' << perm_elements[2] << ' ' << perm_elements[3] << std::endl;
        #endif
    } while (std::next_permutation(perm_elements,perm_elements+4) );

    pair<corner_set, int> lowest_cost_solution = solution_queue.top();

    #ifdef DEBUG
    std::cout << std::endl << "Lowest Solution Cost:  " << lowest_cost_solution.second << " from " << lowest_cost_solution.first.corners[0] << " -> " << lowest_cost_solution.first.corners[1] << " -> " << lowest_cost_solution.first.corners[2] << " -> " << lowest_cost_solution.first.corners[3] << std::endl << std::endl;
    #endif

    dungeon_path_array[0] = make_pair(                                    dungeons[0], dungeons[lowest_cost_solution.first.corners[0]]);
    dungeon_path_array[1] = make_pair(dungeons[lowest_cost_solution.first.corners[0]], dungeons[lowest_cost_solution.first.corners[1]]);
    dungeon_path_array[2] = make_pair(dungeons[lowest_cost_solution.first.corners[1]], dungeons[lowest_cost_solution.first.corners[2]]);
    dungeon_path_array[3] = make_pair(dungeons[lowest_cost_solution.first.corners[2]], dungeons[lowest_cost_solution.first.corners[3]]);
    dungeon_path_array[4] = make_pair(dungeons[lowest_cost_solution.first.corners[3]], dungeons[0]                                    );

    // Run all six legs of the path and concatenate them together
    for (int i = 0; i < 5; ++i) {
        int path_cost;

        #ifdef DEBUG
        std::cout << "Running path " << dungeon_path_array[i].first.first << "/" << dungeon_path_array[i].first.second << " to " << dungeon_path_array[i].second.first << "/" << dungeon_path_array[i].second.second << std::endl; 
        #endif

        // As above, select if we want bfs of dijkstra for path fitness
        #ifdef TOUR_BFS
            path p = solve_bfs_custom(m, rows, cols, dungeon_path_array[i].first, dungeon_path_array[i].second);
        #else
            path p = solve_dijkstra_custom(m, rows, cols, dungeon_path_array[i].first, dungeon_path_array[i].second, path_cost);
        #endif

        // Remove the first element of every list list after the first so we don't have duplicates
        if (i > 0) {
            p.pop_front();
        }
        lowest_cost_path.splice(lowest_cost_path.end(), p);
    }
     
    #ifdef DEBUG
    // Iterate over our final point list and print all the points before returning
  	std::cout << std::endl << "Final point list:" << std::endl;
    for (const point & ipoint : lowest_cost_path)
    {
    	std::cout << ipoint.first << "/" << ipoint.second << std::endl;
    }
    #endif

    return lowest_cost_path;
}
