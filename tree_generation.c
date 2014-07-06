#include "tree_generation.h"
#include "block.h"

/// this function will generate a random tree
// dest is the surface that the tree will be applied too
// theTree is the tree structure that will cantain all the information about the tree
short generateTree(SDL_Surface *dest, struct treeData *theTree) {
	// used in for loops
	int i, j;
	// place holder varibles for the random numbers generated
	float randNumberX = 0, randNumberY = 0, leafSize = 0, trunkPositive = 0, trunkNegative = 0;
	
	// generates the truck of the tree
	// tells how many lines will be generated and the height of the tree
	for(i = 0; i <= theTree->trunkHeight; i += theTree->trunkIncrement) {
		// generates a random number used for the lines in the positive direction.
		trunkPositive = rand_get_check(theTree->trunkNegativeLow, theTree->trunkPositiveHigh);
		// generates a random number used for the lines in the negative direction.
		trunkNegative = rand_get_check(theTree->trunkNegativeLow, theTree->trunkNegativeHigh);
		// draws the line in the positive direction.
		draw_line(dest, (dest->w/2.0), (dest->w/2.0)+theTree->trunkHeight+i+trunkPositive, (dest->w/2.0)+trunkPositive, (dest->w/2.0)+trunkPositive, 1, theTree->trunkColor);
		// draws the line in the negative direction.
		draw_line(dest, (dest->w/2.0), (dest->w/2.0)+theTree->trunkHeight+i-trunkNegative, (dest->w/2.0)+trunkNegative, (dest->w/2.0)-trunkNegative, 1, theTree->trunkColor);
	}
	
	// generates the leaves of the tree
	// tells how many leaves will be generates
	for(i = 0; i < theTree->leavesNumbers; i++) {
		// tells how big the distribution is
		for(j = 0; j < theTree->leavesDistribution; j++) {
			// generates the random number used for the X position.
			randNumberX += rand_get_check(-theTree->leavesGenerationRadiusX, theTree->leavesGenerationRadiusX);
			// generates the random number used for the Y position.
			randNumberY += rand_get_check(-theTree->leavesGenerationRadiusY, theTree->leavesGenerationRadiusY);
			// generates the random number used for the leaves size.
			leafSize += rand_get_check(1, theTree->leavesSize);
		}
		// divides by the distribution to get the proper random value.
		randNumberX /= theTree->leavesDistribution;
		randNumberY /= theTree->leavesDistribution;
		leafSize /= theTree->leavesDistribution;
		
		// draws the randomly generated circles to the surface.
		draw_circle(dest, (dest->h/2.0) + randNumberX, (dest->w/2.0) + randNumberY, leafSize, theTree->leafColor);
	}
}


struct treeData *randomizeTreeSpecies(char* name) {
		// make a blank tree structure
		struct treeData *myTree = malloc(sizeof(struct treeData));
		
		if(myTree == NULL) {
			error("randomizeTreeSpecies() was sent NULL treeData pointer.");
			return NULL;
		}
		// gives the name of the tree
		// used as a paramter since random names are unimplemented
		myTree->name = name;
		
		float treeSpecies = rand_get_check(0xff000000, 0xffffffff);
		
		myTree->leafColor = rand_get_check(1, treeSpecies) | 0xff000000;
		myTree->trunkColor = rand_get_check(1, treeSpecies) | 0xff000000;
		
		
		myTree->leavesNumbers = 150;
		myTree->leavesSize = 3;
		myTree->leavesGenerationRadiusX = 20;
		myTree->leavesGenerationRadiusY = 10;
		myTree->leavesDistribution = 5;
		myTree->trunkHeight = 15;
		myTree->trunkIncrement = 5;
		myTree->trunkNegativeLow = -1;
		myTree->trunkNegativeHigh = -10;
		myTree->trunkPositiveLow = 1;
		myTree->trunkPositiveHigh = 10;
		
}


short makeTree() {
	
	
}
