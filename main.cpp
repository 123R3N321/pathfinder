/**
* first, do everything in a single file.
 * wes works on gaming part
 * Ren works on algo
 * Ren has exclusive modification access to main.cpp
 * wes has everything else
*/

/**
* General approach: big probelm: we cannot get pass a "corner block"
 * create 2 stages: calculate the path (i want it to be visible)
 *                 and take the path (optimal) (ofc also visualized)
 *
 *
 * In path probing stage we need to be able to be corner-blocked.
*/
#include <iostream>
#include<vector>
#include<string>
#include<cmath>
#include<limits>

#define LOG(argument) std::cout << argument << '\n' //convenient debugg

/*
 * design decision: (0,0) at bottom left
 */

const int xSize = 50;   //size of map, subject to change
const int ySize = 30;

enum class Action { //more readable
    north,
    south,
    west,
    east,
    northwest,
    northeast,
    southwest,
    southeast
};


std::vector<Action>* stateTree;    //this vector, living on heap, holds all possible actions for pathFinder
                                //it must be on heap as the tree could be huge


struct coord{   //this is just more readable than pair
    float x;
    float y;
};




/**
 * Tentatively, use float for precision (also honor cs3113)
 *
 * @param x1 //xcoord of point1
 * @param y1 //ycoord of point1
 * @param x2 //of point2
 * @param y2
 * @return euclidean distance
 */
float heuristic(coord pos1, coord pos2) {
    return sqrt(pow(pos2.x - pos1.x, 2) + pow(pos2.y - pos2.y, 2));
}

/**
 * This is our player class.
 */
class pathFinder{   //this is our pathfinder
public: //yeah I public everything
    std::vector<Action> path;   //this is the completed, optimized path

};



/**
 * a vector on heap that holds all possible states, it does not belong to path finder (pathfinder lives on stack)
 */
void initialize(){

    stateTree = new std::vector<Action>();

}