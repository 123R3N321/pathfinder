/**
* first, do everything in a single file.
 * wes works on gaming part
 * Ren works on algo
 * Ren has exclusive modification access to main.cpp
 * wes has everything else
*/

/**
 * General algo design:
 * mimic my old work in python
 * need dynamically allocate nodes this time.
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
//all declarations
struct Node;
float heuristic(const Node* curPos);
enum class Action;

const size_t   xSize = 50,   //size of map, subject to change, also safer to have size_t
            ySize = 30;

const float initX = 0.0f,   //where do we start
            initY = 0.0f;

const float endX = 49.0f,   //where to we end
            endY = 29.0f;
const size_t maxIteration = 9999;   //stop if we spend too long

std::vector<Node*> allNodes;    //this is a global var, all its elements on heap, my implementation makes access faster than RBtree map.

std::vector<Action> actionTree;  //same as above


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


struct Node{   //this is just more readable than pair,
            // btw we calculate heuristic on spot, we can change to init to save calculation time, as a design choice
    float x;
    float y;
    Action parent;  //know what action brought us here.
    float pathCost;
    bool reached;   //if reached, we will not explore it in AllNodes
    float heuristicVal;

    Node(float x=0.0f, float y=0.0f) :
    x(x), y(y), pathCost(0), reached(false){
        heuristicVal = heuristic(this);
    }    //default value 0,0

    //copy constructor
    Node(const Node& other){
        x = other.x;
        y = other.y;
        parent = other.parent;
        pathCost = other.pathCost;
        reached = other.reached;
        heuristicVal = other.heuristicVal;
    }

    Node& operator=(const Node& other){ //assignment
        if (this != &other) { // Check for self-assignment
            x = other.x;
            y = other.y;
            parent = other.parent;
            pathCost = other.pathCost;
            reached = other.reached;
            heuristicVal = other.heuristicVal;
        }
        return *this;
    }



    bool isStart(){
        return initX==x && initY==y;
    }

    bool isEnd(){   //we actually don need this as heuristic will be 0.
        return endX==x && endY==y;
    }
};



struct compFunctor{
    bool operator()(const Node* a, const Node* b) const {
        // Implement your comparison logic here
        // For example, comparing path costs of the nodes
        return a->pathCost+ heuristic(a) > b->pathCost + heuristic(b); // Change this as needed
    }
};


std::priority_queue<Node*, std::vector<Node*>, compFunctor> frontier; //It simply points to those nodes in allNodes, we do not have memory issue
//do NOT attempt to delete from frontier as it will literally be double delete!


/**
 * Tentatively, use float for precision (also honor cs3113)
 *

 * @param curPos      //curent position
 * @return euclidean distance, float
 */
float heuristic(const Node* curPos) {
    return sqrt(pow(endX - curPos->x, 2) + pow(endY - curPos->y, 2));
}


/**
 * The soul of my design optimization
 * Node with coord (a,b) has ind pos [a*ySize + b]
 * where a <= xSize-1, b<= ySize - 1, total ind goes to xSize * ySize - 1 which is good
 * This provides const time access.
 */
void initialize(){  //create all nodes on heap
    for(size_t i=0;i<xSize; ++i){
        for(size_t j=0;j<ySize;++j){
            allNodes.push_back(new Node(i,j));  //at initialization step, each node already knows their heuristic
        }
    }
}





void aStar(std::vector<Node*> map){ //only need the map. we have global start and end point
    frontier.push(map[ySize*initX + initY]);    //the math approach gives constant access.
    size_t curIteration = 0;    //to be compared with maxIteration
    while(frontier.size()>0 && curIteration < maxIteration){
        curIteration ++;

        Node current = *(frontier.top());   //create a local shallow copy, not so costly as it is a single node
        frontier.pop(); //only kicked out from frontier, the node is still in our worldly worldly map lol
        if(current.isEnd()){
            return; //we do not need to do anything
        }
        for ...

    }
}

int main(){
//    foo* foo1 = new foo;
//    foo* foo2 = new foo;
//    foo1->val +=1.0f;

//LOG(foo2.val);

//    std::priority_queue<foo*, std::vector<foo*>, Functor> fooheap;
//    fooheap.push(foo2);
//
//    fooheap.push(foo1);
//    LOG(fooheap.top()->val);
//    delete foo1;
//    delete foo2;

    initialize();   //creates game map with all nodes
    frontier.push(allNodes[0]);
    frontier.push(allNodes[800]);
    frontier.push(allNodes[2]);
    frontier.push(allNodes[47]);
    //          (ind / sizeY, ind % sizeY)
    LOG(allNodes[ySize*initX + initY]->y);

//handle all deletes, no memory leak.
    for(Node* each : allNodes){delete each;}
    return 0;
}