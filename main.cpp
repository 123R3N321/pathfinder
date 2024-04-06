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
#include <fstream>

#define LOG(argument) std::cout << argument << '\n' //convenient debugg
float infinity = std::numeric_limits<float>::infinity();
float criticalCritirion = 999.0f;

/*
 * design decision: (0,0) at bottom left
 */
//all declarations
struct Node;
float heuristic(const Node* curPos);
enum class Action;

const size_t   xSize = 50,   //size of map, subject to change, also safer to have size_t
            ySize = 30;

float initX = 0.0f,   //where do we start
            initY = 0.0f;

float endX = 3.0f,   //where to we end
            endY = 3.0f;
const size_t maxIteration = 9999;   //stop if we spend too long

std::vector<Node*> allNodes;    //this is a global var, all its elements on heap, my implementation makes access faster than RBtree map.

std::vector<Action> actionTree;  //same as above

float cardinalCost = 1.0f,
        diagonalCost = 1.4142f;

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


enum class blockType{
    nop,
    path,
    start,
    end,
    wall
};
struct Node{   //this is just more readable than pair,
            // btw we calculate heuristic on spot, we can change to init to save calculation time, as a design choice
    float x;
    float y;
    float typeCost; //this is a cost dependent on the type: wall? road? difficult terrain?
    float cost; //essentially the type of the node, a wall or a tile
    Node* parent;  //know what action brought us here.
    float pathCost;
    bool reached;   //if reached, we will not explore it in AllNodes
    float heuristicVal;
    blockType block = blockType::nop; 

    Node(float x=0.0f, float y=0.0f, float typeCost = 0.0f) :
    x(x), y(y), pathCost(0), typeCost(typeCost), reached(false){
        heuristicVal = heuristic(this);
        parent = nullptr;
    }    //default value 0,0

    //copy constructor
    Node(const Node& other){
        x = other.x;
        y = other.y;
        typeCost = other.typeCost;
        cost = other.cost;
        parent = other.parent;
        pathCost = other.pathCost;
        reached = other.reached;
        heuristicVal = other.heuristicVal;
        block = other.block;
    }

    Node& operator=(const Node& other){ //assignment
        if (this != &other) { // Check for self-assignment
            x = other.x;
            y = other.y;
            typeCost = other.typeCost;
            cost = other.cost;
            parent = other.parent;
            pathCost = other.pathCost;
            reached = other.reached;
            heuristicVal = other.heuristicVal;
            block = other.block;
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
void initialize(){  //create all nodes on heap, they all start with zero cost
    for(size_t i=0;i<xSize; ++i){
        for(size_t j=0;j<ySize;++j){
            allNodes.push_back(new Node(i,j));  //at initialization step, each node already knows their heuristic
        }
    }
}

float actionCost(Action action){
    switch(action){
        case Action::north:
        case Action::east:
        case Action::south:
        case Action::west:
            return cardinalCost;
            break;

        case Action::northeast:
        case Action::southeast:
        case Action::southwest:
        case Action::northwest:
            return diagonalCost; //using the diagonal cost given by
            break;

    }
}


Node* getNodeFrom(Node* node, Action action){
    Node* ptr = nullptr;    //later we return this ptr with updated value
    int refVal = ySize * (node->x) + (node->y);
    switch (action){
        case Action::north:
            ptr = allNodes[refVal + 1];  //because of the way we initialize the map, we do algebra like this.
            break;

        case Action::northeast:
            ptr = allNodes[refVal + ySize + 1];
            break;

        case Action::east:
            ptr = allNodes[refVal + ySize];
            break;

        case Action::southeast:
            ptr = allNodes[refVal + ySize - 1];
            break;

        case Action::south:
            ptr = allNodes[refVal - 1];
            break;

        case Action::southwest:
            ptr = allNodes[refVal - ySize - 1];
            break;

        case Action::west:
            ptr = allNodes[refVal -ySize];
            break;

        case Action::northwest:
            ptr = allNodes[refVal - ySize + 1];
            break;
    }
    ptr->cost = ptr->typeCost + actionCost(action);    //so we don need to add one by one in the future
    return ptr;
}

void setNode(size_t x, size_t y, blockType block, float typeCost=infinity){ //modify a node (in particular its cost) inside allNodes
    if(allNodes.size()<=ySize * x + y){ //most likely forgor to initilize()
        LOG("the node exceeded the map."); return;
    }
    allNodes[ySize * x + y]->typeCost = typeCost;
    allNodes[ySize * x + y]->block = block;
}

//returns the node pointer based on given x,y coordinate

Node* getNode(size_t x, size_t y){
    return allNodes[ySize * x + y];
}

/**
 *  first, check all reachable nodes and put in a vector
 *  at this point we assume diagona not blocked yet
 *  second, check each of these reachable ones, do labeling for reached and update parental info
 *
 * @return a vector with pointers to the neighboring nodes
 */
std::vector<Node*> probe(Node* node){   //math correct
    bool mapEdgeNorth = node->y<(ySize-1);
    bool mapEdgeEast = node->x<(xSize-1);
    bool mapEdgeSouth = node->y>0;
    bool mapEdgeWest = node->x>0;

    bool north = false;
    bool east = false;
    bool south = false;
    bool west = false;

    std::vector<Node*> lst;
    //check all 4 basic adjacent candidates
    if(mapEdgeNorth){
        if(!getNodeFrom(node, Action::north)->reached){
            lst.push_back(getNodeFrom(node, Action::north));
        }
        if((getNodeFrom(node,Action::north)->cost)<criticalCritirion){
            north = true;
        }
    }
    if(mapEdgeEast){
        if(!getNodeFrom(node, Action::east)->reached){
            lst.push_back(getNodeFrom(node, Action::east));
        }
        if((getNodeFrom(node,Action::east)->cost)<criticalCritirion){
            east = true;
        }
    }
    if(mapEdgeSouth){
        if(!getNodeFrom(node, Action::south)->reached){
            lst.push_back(getNodeFrom(node, Action::south));
        }
        if((getNodeFrom(node,Action::south)->cost)<criticalCritirion){
            south = true;
        }
    }
    if(mapEdgeWest){
        if(!getNodeFrom(node, Action::west)->reached){
            lst.push_back(getNodeFrom(node, Action::west));
        }
        if((getNodeFrom(node,Action::west)->cost)<criticalCritirion){
            west = true;
        }
    }

    if( (mapEdgeNorth) && (mapEdgeEast) && (north || east)){ //northeast
        if(!getNodeFrom(node, Action::northeast)->reached){
            lst.push_back(getNodeFrom(node, Action::northeast));
        }
    }
    if( (mapEdgeSouth) && (node->x<(xSize-1)) && (south || east)){
        if(!getNodeFrom(node, Action::southeast)->reached){
            lst.push_back(getNodeFrom(node, Action::southeast));
        }
    }
    if( (node->y>0) && (mapEdgeWest) && (south || west)){
        if(!getNodeFrom(node, Action::southwest)->reached){
            lst.push_back(getNodeFrom(node, Action::southwest));
        }
    }
    if( (node->y<(ySize-1)) && (node->x>0) && (north || west)){
        if(!getNodeFrom(node, Action::northwest)->reached){
            lst.push_back(getNodeFrom(node, Action::northwest));
        }
    }

    for(Node* child : lst){
        if( ! child->reached){
            child->parent = node;    //first time seeing this child, adopt by parent
            child->pathCost = node->pathCost + child->cost;   //first time reaching it, update pathCost
        }else{  //already reached, potentially choose a better parent
            if(child->pathCost < node->pathCost + child->cost){
                child->parent = node;   //found a better parent
                child->pathCost = node->pathCost + child->cost; //update this better cost
                child->reached = false; //fake that we did not have a parent before so that we get a better deal
            }
        }
    }
    return lst;
}



void aStar(std::vector<Node*> map){ //only need the map. we have global start and end point
    map[ySize*initX + initY]->reached = true;   //prolly not necessary
    frontier.push(map[ySize*initX + initY]);    //the math approach gives constant access.
    size_t curIteration = 0;    //to be compared with maxIteration
    while(frontier.size()>0 && curIteration < maxIteration){
        curIteration ++;

        Node* current = frontier.top();   //create a local shallow copy of a pointer, very cheap
        frontier.pop(); //only kicked out from frontier, the node is still in our worldly worldly map lol
        if(current->pathCost > criticalCritirion){
            LOG("Bad Map");
            break;
        }

        if(current->isEnd()){
            return; //we do not need to do anything
        }
        std::vector<Node*>cand = probe(current);
        for (Node* child : cand){
            if (! child->reached){
                child->parent = current;
                child->reached = true;
                frontier.push(child);
            }
        }
    }
}

std::string matchInstruction(Node* cur, Node* prev){
    std::string msg;

//    if(prev->x == cur->x && prev->y == cur->y-1){msg="up";}
    if(prev->y == cur->y && prev->x == cur->x-1){msg="right";}else
    if(prev->x == cur->x && prev->y == cur->y+1){msg="down";}else
    if(prev->y == cur->y && prev->x == cur->x+1){msg="left";}else
    if(prev->y == cur->y-1 && prev->x == cur->x-1){msg="top-right";}else
    if(prev->y == cur->y+1 && prev->x == cur->x-1){msg="bottom-right";}else
    if(prev->y == cur->y+1 && prev->x == cur->x+1){msg="bottom-left";}else
    if(prev->y == cur->y-1 && prev->x == cur->x+1){msg="top-left";}else{msg = "up";}

    return msg;
}

void PrintInstructions(Node* iter){ //recursive implementation to get instruction
    if(iter->isStart()){
        return;
    }
    PrintInstructions(iter->parent);
    LOG(matchInstruction(iter,iter->parent));

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

    std::ifstream file; 

    file.open("Input2.txt");
    if(file){
        //std::cout << "hello";
    }else{
        std::cout <<"fail";
    }
    
    file >> initX >> initY >> endX >> endY; //gets init xy and end xy positions
    setNode(initX,initY,blockType::start); // sets the blocktype for both
    setNode(endX, endY,  blockType::end);
    int number;
    int x = 0;
    int y = 29;
    //loops reads through the entire file and changes the node type to wall if it is
    while(file >> number){
        if(x == 50){ x = 0; y--; std::cout << std::endl;}
        if(number == 1){setNode(x,y,blockType::wall) ;}
        x++;
        std::cout << number << " ";
    }
    file.close();
    std::cout << std::endl;

   

    aStar(allNodes);    //assume frontier is clean
    std::ofstream outdata;
    outdata.open("output1.txt");

    /**
     * actual path test, failed
     */
    std::string cost;

    Node* backtrack = allNodes[ySize * endX + endY];
        //this is end node
    std::vector<Node*> pathNodes;
    backtrack = backtrack->parent; //skips end node
        while(! backtrack->isStart()){
        // LOG(backtrack->x<<" "<<backtrack->y);
        setNode(backtrack->x, backtrack->y,blockType::path);
        pathNodes.push_back(backtrack);
        backtrack = backtrack->parent;
    }
    
    for(size_t i = pathNodes.size()-1; i != -1; --i){
        Node* node= pathNodes[i];
        std::string action = matchInstruction(node, node->parent);
        if(action == "right"){outdata << 0 << " ";}
        else if(action == "top-right"){outdata << 1 << " ";}
        else if(action == "up"){outdata << 2 << " ";}
        else if(action == "top-left"){outdata << 3 << " ";}
        else if(action == "left"){outdata << 4 << " ";}
        else if(action == "bottom-left"){outdata << 5 << " ";}
        else if(action == "bottom"){outdata << 6 << " ";}
        else if(action == "bottom-right"){outdata << 7<< " ";}
    }
    outdata << std::endl; 
    for(size_t i = 0; i != pathNodes.size();++i){
        Node* node= pathNodes[i];
        outdata << node->heuristicVal+ node->pathCost << " ";
    }
    outdata << std::endl; 

    // LOG(backtrack->x<<" "<<backtrack->y);
    int count = 0;
    
    for(size_t i=ySize-1;i != -1; --i){
        for(size_t j=0;j<xSize;++j){
            Node* node = getNode(j,i);
            if(node->block == blockType::nop){
                number = 0;
            }
            else if(node->block == blockType::path){
                number = 4;
            }
            else if(node->block == blockType::wall){
                number = 1;
            }
            else if(node->block == blockType::start){
                number = 2;
            }
            else if(node->block == blockType::end){
                number = 5;
            }
            outdata << number << " ";
            std::cout << number << " ";
        }
        std::cout << std::endl;
        outdata << std::endl;
    }
    outdata.close();
       
            
        
        
    
//for(Node* each : allNodes){
//    if(each->reached){
//        LOG(each->x<<" "<<each->y);
//    }
//}


//    frontier.push(allNodes[0]);
//    frontier.push(allNodes[1]);
//    frontier.push(allNodes[2]);
//
//    frontier.push(allNodes[31]);
//    frontier.push(allNodes[3]);
//    frontier.push(allNodes[3]);
//
//    frontier.push(allNodes[3]);
//    frontier.push(allNodes[3]);
//    frontier.push(allNodes[3]);
//LOG(frontier.top()->x<<" "<<frontier.top()->y);

    //              (ind / sizeY, ind % sizeY)

/**
 * probe test, checked
 */

//    Node* startCopy = allNodes[31];//this shld be coord (1,1), checked
//    for(auto each : probe(startCopy)){
//        LOG(each->x <<  " " << each->y );
//    }

/**
 * linAlg test, checked
 */
//    LOG(probe(startCopy).size());
//    Node*northCopy = getNodeFrom(startCopy,Action::north);  //(1,2) checked.
//    Node*eastCopy = getNodeFrom(startCopy,Action::east);  //(2,1) checked
//    Node*southCopy = getNodeFrom(startCopy,Action::south);  //(1,0)
//    Node*westCopy = getNodeFrom(startCopy,Action::west);  //(0,1) checked
//
//    Node*northeastCopy = getNodeFrom(startCopy,Action::northeast);  //(2,2) checked
//    Node*southeastCopy = getNodeFrom(startCopy,Action::southeast);  //(2,0) checked
//    Node*southwestCopy = getNodeFrom(startCopy,Action::southwest);  //(0,0) checked
//    Node*northwestCopy = getNodeFrom(startCopy,Action::northwest);  //(0,2) checked
//    LOG(northwestCopy->y);


//Node*looper = allNodes[0];
//for(int i=0; i<33;i++){
////    LOG(looper->x);
//    LOG(allNodes[i]->heuristicVal);
//    LOG("");
////    looper = looper->parent;
//}



//handle all deletes, no memory leak.
    for(Node* each : allNodes){delete each;}
    return 0;
}