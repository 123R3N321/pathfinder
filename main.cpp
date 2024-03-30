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

/**
 * areas of design choice/ future optimization
 *  .introduce weight for astar
 *  .calculate heuristic for each coord
 *  .implement a datastructure for 8-elem-heap. (this one is prolly the most important)
 */
#include <iostream>
#include<vector>
#include<string>
#include<cmath>
#include<limits>    //may not need it
#include<queue>

#define LOG(argument) std::cout << argument << '\n' //convenient debugg

/*
 * design decision: (0,0) at bottom left
 */

const size_t   xSize = 50,   //size of map, subject to change, also safer to have size_t
            ySize = 30;

const float initX = 0.0f,   //where do we start
            initY = 0.0f;

const float endX = 49.0f,   //where to we end
            endY = 29.0f;

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

//below is the ONLY thing living on heap
std::vector<Action>* stateTree;    //this vector, living on heap, holds all possible actions for pathFinder
                                //it must be on heap as the tree could be huge


struct coord{   //this is just more readable than pair,
            // btw we calculate heuristic on spot, we can change to init to save calculation time, as a design choice
    float x;
    float y;
    Action parent;  //know what action brought us here.

    coord(float x=0.0f, float y=0.0f) : x(x), y(y){}    //default value 0,0

    coord& operator=(const coord& other){
        if (this != &other) { // Check for self-assignment
            x = other.x;
            y = other.y;
            parent = other.parent;
        }
        return *this;
    }
};

coord start(initX, initY);  //start and ending point in coord form, uninitialized parent action.
coord end(endX, endY);



/**
 * Tentatively, use float for precision (also honor cs3113)
 *

 * @param pos1      //coordinate 1
 * @param pos2      //coordinate 2
 * @return euclidean distance, float
 */
float heuristic(coord pos1, coord pos2) {
    return sqrt(pow(pos2.x - pos1.x, 2) + pow(pos2.y - pos2.y, 2));
}




/**
 * This is our player class.
 */
class pathFinder{   //this is our pathfinder
public: //yeah I public everything

    static float pathCost;  //static makes it accessible without class created, to bypass cpp restrictions
    static coord curPos;   //current position

    /**
     * a generic name because we indeed only use it as a simple functor for min heap support
     */
    struct Functor {

        float value; //

        Functor() {
            value = pathCost + heuristic(curPos, end);
        }

        // Overload comparison operator for min heap
        bool operator>(const Functor& other) const {
            // Define comparison logic here
            return value > other.value;
        }
    };

    std::vector<Action> path;   //this is the completed, optimized path
    std::priority_queue<Action, std::vector<Action>, std::greater<Functor>> actions;    //we use customized greater op.

    //constructor, default spawn at (0,0), can be anywhere, assume user will not pass in nonsense position
    pathFinder(){   //more readable not in init.lst
        curPos = start;
        pathCost = 0;   //initialize to be zero
        //at this point there is no action before starting point
    }

    void reset(){   //reset everything. all global params serve our pathFinder
        curPos = start;
        pathCost = 0;
        stateTree->clear(); //we need to re-calculate stateTree
        path.clear();   //ofc we start from scratch
    }

    void astar(){   //start and end globally defined. we do not take parameters
                    //maybe add weight as global param in the future.

    }

};




/**
 * a vector on heap that holds all possible states, it does not belong to path finder (pathfinder lives on stack)
 */
void initialize(){

    stateTree = new std::vector<Action>();

}