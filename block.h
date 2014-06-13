#include <SDL2/SDL.h>

/// block definitions
// someday I should really clean this up and remove all of the WIDTH/HEIGHT specifications and make everything SIZE.
// Because x and y are going to be the same scale for everything anyway.

// block neighbors needs  to be signed because of the "block_neighbor_all" flag
#define BLOCK_NEIGHBORS					4
#define BLOCK_NEIGHBOR_ALL				-1
#define BLOCK_NEIGHBOR_UP				0
#define BLOCK_NEIGHBOR_DOWN				1
#define BLOCK_NEIGHBOR_LEFT				2
#define BLOCK_NEIGHBOR_RIGHT			3

#define BLOCK_LINEAR_SCALE_FACTOR		3.0f
#define BLOCK_LINEAR_SCALE_FACTOR_INV	(1.0/3.0f)
#define BLOCK_WIDTH						243
#define BLOCK_HEIGHT					243
#define BLOCK_WIDTH_1_2					(BLOCK_WIDTH/2)		// integer division
#define BLOCK_HEIGHT_1_2				(BLOCK_HEIGHT/2)	// integer division
#define BLOCK_WIDTH_1_3					81
#define BLOCK_WIDTH_2_3					162
#define BLOCK_HEIGHT_1_3				81
#define BLOCK_HEIGHT_2_3				162

#define BLOCK_DEFAULT_ELEVATION			0.0f
#define BLOCK_ORIGIN_LEVEL				0

// block child layout
#define BLOCK_CHILDREN					9

#define BLOCK_CHILD_TOP_LEFT			0
#define BLOCK_CHILD_TOP_CENTER			1
#define BLOCK_CHILD_TOP_RIGHT			2
#define BLOCK_CHILD_CENTER_LEFT			3
#define BLOCK_CHILD_CENTER_CENTER		4
#define BLOCK_CHILD_CENTER_RIGHT		5
#define BLOCK_CHILD_BOTTOM_LEFT			6
#define BLOCK_CHILD_BOTTOM_CENTER		7
#define BLOCK_CHILD_BOTTOM_RIGHT		8
#define BLOCK_CHILD_INVALID				10

//	0 1 2
//	3 4 5
//	6 7 8


// each block can have 9 children.
// each block can have one parent.
// more children are generated as the player zooms in.
// more parents are generated as the player zooms out.
// the children of a single block enumeration:
// 0 1 2
// 3 4 5
// 6 7 8
// the numbers 0 through 8 correspond to a child of the current block (zooming in).
// the number 9 represents zooming out to see the parent of the current block.
/// this structure contains all the information that one block needs. The map is made up of a fractal arrangement of these blocks.
struct blockData{
	
	signed long long level;
	
	// this is the two dimensional array of elevation values for each block.
	float elevation[BLOCK_WIDTH][BLOCK_HEIGHT];
	
	// this points to the block that this block is inside.
	// if this is NULL, a parent has not been generated yet.
	struct blockData *parent;
	// this is how the parent sees the child.
	// this is a number from 0 to BLOCK_CHILDREN-1.
	// if parentView = 3, then the parent of this block sees this block in its center left position.
	// if parentView = 8, then the parent of this block sees this block in its lower right position.
	char parentView;
	
	// these are pointers to child blocks.
	// these are pointers to other blocks inside of this main block.
	// each block will be split up into BLOCK_CHILDREN smaller ones.
	// these are indexed with 0,1,2,3,4,5,6,7,8.
	struct blockData *children[BLOCK_CHILDREN];
	
	// these are eight pointers to the four neighbors on the same level (up, down, left, and right
	// if these are NULL, the neighbor could exist, but it just might not be entered in this blocks neighbor's index (some neighbors are friendly than others :P)
	struct blockData *neighbors[BLOCK_NEIGHBORS];
};


/*
	  /==================================================\
	 /|==================================================|\
	 ||==================================================||
	<|||==  RYAN'S BLOCK NETWORK GENERATION PROTOCOL  ==|||>
	 ||==================================================||
	 \|==================================================|/
	  \==================================================/

This protocol describes how the PROGRAM will navigate the fractal map.
The protocol is strict, but robust.
The user interface is going to be smooth and forgiving.
the USER INTERFACE and the PROGRAM'S BLOCK NETWORK PROTOCOL are completely different.
The user will NOT have to navigate the world in the same manor as the program does when generating blocks, and zooming in/out.

//--------------------------------------------------
// Origin Block
//--------------------------------------------------
The world will always start with an origin block.
The origin block is on level 0.
Everything else will be generated with addresses in memory relative to that block.
In other words, any block in a world is connected to origin through a series of parents, or children, or a combination of both.

//--------------------------------------------------
// Generating Child Blocks
//--------------------------------------------------
When the program wants to navigate to the a block's children, it must generate children for that block if they do not yet exist.
When generating children, all 9 will be generated at the same time to ensure that a block either:
	A. has all of its children or 
	B. has none of its children.
All 9 pointers to the 9 children are stored in the parent block.
Each child has its parent point point "up" to its parent at the moment of generation.

//--------------------------------------------------
// Parent Blocks
//--------------------------------------------------
When generating a parent block, the "initiating child" that does not yet have a parent will request a parent be generated.
The program will check to make sure the parent does NOT exist before generating a parent.
Once the parent is generated, it will immidiately generate all of its children.
So, in effect, when a block tries to generate a parent, it will automatically generate eight "sibling" blocks (the eight blocks around it on the same level).
FOR EVERY PARENT THAT IS GENERATED, IT WILL BE CONCENTRIC WITH THE ORIGIN.
The above fact is crucial to understand. It is one of the most important and useful facts about this system.


//--------------------------------------------------
// FAQ
//--------------------------------------------------
Q.	If you can only generate parents and chilrend (zooming in and out of the fractal network), how can you even generate a neighbor?
	How can the user ever pan the view horizontally on the same level?
A.	If the user wants to pan around horizontally on the same level, the program will zoom out, and then zoom back in to get to the location the user wants to go.
	This may seem convoluted, but it makes the map network much more robust, expandable, and less prone to errors when programming it.

*/


#define BLOCK_LINK_SIZE 256
/// this is a linked list of arrays of pointers to blockData structures.
// this structure is here simply to facilitate garbage collection.
// there is a function that collects pointers to all of the blocks generated.
// when this program is terminated, every block on that list will be erased and the memory is freed.
struct blockLink{
	// a pointer to the previous blockLink in the list
	struct blockLink *prev;
	struct blockData *blocks[BLOCK_LINK_SIZE];
	struct blockLink *next;
};


short block_print_to_file(struct blockData *datBlock, char *fileName);
short block_random_fill(struct blockData *datBlock, float range_lower, float range_higher);


/// these are definitions for the block_collector program.
// the function will either be collecting a list of all map blocks being generated (as they are generated)
// or the function will de-allocate (free) all of the data it has collected so far.
#define bc_collect		0
#define bc_clean_up		1
short block_collector(struct blockData *source, short operation);

struct blockData *block_generate_origin();
short block_generate_children(struct blockData *datParent);
short block_generate_parent(struct blockData *oneChild);


short map_print(SDL_Surface *dest, struct blockData *source);


short block_smooth(struct blockData *source, float smoothFactor);
float block_surrounding_average(struct blockData *source, unsigned int x, unsigned int y);


int block_fill_middle(struct blockData *dat, float inVal, float outVal);
short block_fill_nine_squares(struct blockData *Block, int color);
short block_fill_nine_squares_own_color(struct blockData *Block, int one, int two, int three, int four, int five, int six, int seven, int eight, int nine);


short block_calculate_neighbor(struct blockData *dat, short neighbor);
