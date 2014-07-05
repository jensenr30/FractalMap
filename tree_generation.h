#include "graphics.h"

/// stucture used to hold all the information about a tree
// the paramters used are to randomize the trees
struct tree {
	// the amount of leaves that this tree will have
	short leavesNumbers;
	
	// the size of the leaves
	short leavesSize;
	
	// the radius of the leaves from the center on the x axis
	short leavesGenerationRadiusX;
	
	// the radius of the leaves from the center on the y axis 
	short leavesGenerationRadiusY;
	
	// the distiribution value of the leaves
	short leavesDistribution;
	
	// the hight value of the tree trunk
	short trunkHeight;
	
	// the width of the tree trunk
	short trunkWidth;
	
	// the color value of the leaves
	Uint32 leafColor;
	
	// the color value of the trunk
	Uint32 trunkColor;
};
