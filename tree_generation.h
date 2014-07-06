#include "graphics.h"

/// stucture used to hold all the information about a tree
// the paramters used are to randomize the trees
// an example tree
// struct tree myTree;
// myTree.leafColor = 0xff088c05;
// myTree.trunkColor = 0xffc66505;
// myTree.leavesNumbers = 50;
// myTree.leavesSize = 5;
// myTree.trunkHeight = 40;
// myTree.leavesGenerationRadiusX = 20;
// myTree.leavesGenerationRadiusY = 10;
// myTree.leavesDistribution = 5;
// myTree.trunkHeight = 15;
// myTree.trunkIncrement = 3;
// myTree.trunkNegativeLow = -1;
// myTree.trunkNegativeHigh = -10;
// myTree.trunkPositiveLow = 1;
// myTree.trunkPositiveHigh = 10;
struct treeData {
	// these variables are used to define the leaves
	// the amount of leaves that this tree will have
	short leavesNumbers;
	
	// the maximum size of the leaves that will be generated
	short leavesSize;
	
	// the radius of the leaves from the center on the x axis
	short leavesGenerationRadiusX;
	
	// the radius of the leaves from the center on the y axis 
	short leavesGenerationRadiusY;
	
	// the distiribution value of the leaves
	short leavesDistribution;
	
	// these variables are used to define the trunk
	// the hight value of the tree trunk
	short trunkHeight;
	
	// the amount of trunk lines to be generated
	short trunkIncrement;
	
	// the lower bound of the random number generator used for the posative postions.
	short trunkPositiveLow;
	
	// the higher bound of the random number generator used for the posative postions.
	short trunkPositiveHigh;
	
	// the lower bound of the random number generator used for the negative postions.
	short trunkNegativeLow;
	
	// the higher bound of the random number generator used for the negative postions.
	short trunkNegativeHigh;
	
	// the color value of the leaves
	Uint32 leafColor;
	
	// the color value of the trunk
	Uint32 trunkColor;
};

// function prototypes

short generateTree(SDL_Surface *dest, struct treeData *theTree);
