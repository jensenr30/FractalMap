#include <SDL2/SDL.h>
//#include "block.h"

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
	// they describe the distance from the origin of the target block (the upper left hand corner: element [0][0]) and the center of the camera).
	// positive x is right, positive y is down.
	// if either x or y are greater than (BLOCK_WIDTH-1), the camera will have to pan right or down respectively.
	// if either x or y are less than 0, the camera will have to pan left or up respectively.
	// the camera is centered on [x][y] of the target block.
	int x, y;
};


//--------------------------------------------------
// function prototypes
//--------------------------------------------------

// this function basically does addition and multiplication.
// this function is really useless.
// the only good thing about it is that it will always perform camera_check() at the end.
// but that can be done in main().
// this function will probably not need to be used as it is just modifying a few variables.
short camera_user_input(struct cameraData *cam, int xdiff, int ydiff, float scaleMult);

// this will initialize a camera too look at the origin with x=0, y=0, scale = 1.
struct cameraData *camera_create(struct blockData *block);

// this makes sure that the cameraData is within bounds of a block/level.
// It will will user camera_pan, camera_zoom_in, and camera_zoom_out to navigate to the fractal block network and update the cameraData.
short camera_check(struct cameraData *cam);


// these are for manipulation of the camera (done by the program)

// this will pan the camera horizontally (by zooming out and zooming back in)
short camera_pan(struct cameraData *cam, short panDir);
// this zooms the camera out
short camera_zoom_in(struct cameraData *cam);
// this zooms the camera in.
short camera_zoom_out(struct cameraData *cam);

// this will render the camera to an SDL_Renderer
short camera_render(SDL_Renderer *dest, struct cameraData *cam);


