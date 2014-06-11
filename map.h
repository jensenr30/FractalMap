#include <SDL2/SDL.h>

/// block definitions

#define BLOCK_CHILDREN				9
#define BLOCK_NEIGHBORS				8
#define BLOCK_LINEAR_SCALE_FACTOR	3.0
#define BLOCK_WIDTH					243
#define BLOCK_HEIGHT				243
#define BLOCK_WIDTH_1_3				81
#define BLOCK_WIDTH_2_3				162

#define BLOCK_DEFAULT_ELEVATION		0.0
#define BLOCK_ORIGIN_LEVEL			0

// block child layout
#define BLOCK_CHILD_TOP_LEFT		0
#define BLOCK_CHILD_TOP_CENTER		1
#define BLOCK_CHILD_TOP_RIGHT		2
#define BLOCK_CHILD_CENTER_LEFT		3
#define BLOCK_CHILD_CENTER_CENTER	4
#define BLOCK_CHILD_CENTER_RIGHT	5
#define BLOCK_CHILD_BOTTOM_LEFT		6
#define BLOCK_CHILD_BOTTOM_CENTER	7
#define BLOCK_CHILD_BOTTOM_RIGHT	8

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
	// if parentView = 8, then the parent of
	char parentView;
	
	// these are pointers to child blocks.
	// these are pointers to other blocks inside of this main block.
	// each block will be split up into BLOCK_CHILDREN smaller ones.
	// these are indexed with 0,1,2,3,4,5,6,7,8.
	struct blockData *children[BLOCK_CHILDREN];
	
	// these are eight pointers to the eight neighbors on the same level.
	// if these are NULL, the neighbor could exist, but it just might not be entered in this blocks neighbor's index (some neighbors are friendly than others :P)
	struct blockData *neighbors[BLOCK_NEIGHBORS];
};


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

struct blockData *block_create_origin();
short block_generate_children(struct blockData *datParent);

short block_collector(struct blockData *source, short operation);
short map_print(SDL_Surface *dest, struct blockData *source);

short block_smooth(struct blockData *source, float smoothFactor);
float block_surrounding_average(struct blockData *source, unsigned int x, unsigned int y);

int block_fill_middle(struct blockData *dat, float inVal, float outVal);
short block_fill_nine_squares(struct blockData *Block, int color);
short block_fill_nine_squares_own_color(struct blockData *Block, int one, int two, int three, int four, int five, int six, int seven, int eight, int nine);

short block_calculate_neighbors(struct blockData *dat);
