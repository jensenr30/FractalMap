#include "block.h"

// these are defined to match up with the BLOCK_CHILD directions.
// there is no specific reason for doing this, but I think it may simplify something later.
#define CAMERA_PAN_UP		1
#define CAMERA_PAN_DOWN		7
#define CAMERA_PAN_LEFT		3
#define CAMERA_PAN_RIGHT	5

/// this describes where the user is looking in the fractal block network.
// This holds data that describes what the user wants to see.
// Camera functions will be used to translate what the user wants to see into which blocks the program has to render.
struct cameraData {
	// this is the target block
	struct blockData *target;
	
	// this records the scale the user is looking at RELATIVE TO THE TARGET BLOCK'S LEVEL.
	// scale if scale = 1, then the user is looking at a 1:1 ratio of the target block.
	// if scale is 2, then the user is zoomed out and at a factor of two.
	// if scale is 3 or greater, then the program should shift target to the parent and multiply scale by 1/3.
	// if scale is 1/3 or smaller, then the program should shift target to the appropriate child block and multiply scale by 3.
	float scale;
	
	// these are coordinates RELATIVE to the TARGET BLOCK (on the same level)
	// they describe the distance from the center of the target block the user is looking.
	// positive x is right, positive y is down.
	// if the magnitude of either x or y is greater than (BLOCK_WIDTH/2 + 1) or (BLOCK_HEIGHT/2 + 1) respectively, then the camera will have to pan horizontally, and set target as the appropriate neighbor.
	// the camera is centered on (x,y).
	int x, y;
};


//--------------------------------------------------
// function prototypes
//--------------------------------------------------

short camera_pan(struct cameraData *cam, short panDir);
short camera_zoom_in(struct cameraData *cam);
short camera_zoom_out(struct cameraData *cam);

short camera_check(struct cameraData *cam);
short camera_move(struct cameraData *cam, int xdiff, int ydiff);
