#include "block.h"
#include "camera.h"
#include "graphics.h"
#include "utilities.h"
#include <stdlib.h>



/// this function will move a camera a relative amount (xdiff, ydiff) and then multiply the camera's scale by the scaleMult variable.
/// this function interprets user controls and turns them into camera function calls.
// this function will automatically call camera_check to make sure the camera is valid
// returns 0 on successful move and scale
// returns 1 on null cam pointer
// I think this function is kind of pointless.

short camera_user_input(struct cameraData *cam, int xdiff, int ydiff, float scaleMult){
	
	// if the camera received by the function is invalid, 
	if(cam == NULL){
		// report error
		error("camera_move() was sent invalid cam. cam = NULL");
		// and return and error
		return 1;
	}
	
	// add the relative (x,y) difference offsets to the camera location.
	cam->x+=xdiff;
	cam->y+=ydiff;
	// scale the map accordingly.
	cam->scale*=scaleMult;
	
	// check the camera and adjust (zoom in or out, or even pan if necessary.
	camera_check(cam);
	
	// if all went well, return the success condition.
	return 0;
}

/// this will attempt to allocate memory new camera to look at the given block centered and at a scale of 1.
// returns the pointer to the camera.
// may return NULL.
struct cameraData *camera_create(struct blockData *block){
	
	// attempt to allocate memory for a camera
	struct cameraData *cam = malloc(sizeof(struct cameraData));
	
	// if malloc failed to allocate memory for the camera,
	if(cam == NULL){
		// report the error.
		error("camera_create() could not allocate memory for a new camera. cam = NULL");
		// return NULL camera pointer.
		return NULL;
	}
	// if the block sent to the user is null,
	if(block == NULL){
		// report te error.
		error("camera_create() was sent null block. block = NULL");
		// return NULL camera pointer.
		return NULL;
	}
	
	// set all camera settings to default.
	cam->x = 0;
	cam->y = 0;
	cam->scale = 1.0;
	cam->target = block;
	cam->changed = 1;
	
	// return a pointer to the camera.
	return cam;
}



/// this will make sure that your camera has valid parameters (1/3 < scale < 3, x within (BLOCK_WIDTH/2 - 1), and y within (BLOCK_HEIGHT/2 - 1)).
// returns 0 on successful check
// returns 1 on null cam pointer
short camera_check(struct cameraData *cam){
	
	// if the camera pointer is invalid,
	if(cam == NULL){
		// report error
		error("camera_check() was sent invalid cam. cam = NULL");
		// and return an error
		return 1;
	}
	
	// this keeps track of whether or not we want to keep checking
	static Uint32 check;
	// this is where we check to make sure the scale and x and y position of the camera is valid.
	do{
		
		// check is initially set to 0 at the start of each loop.
		// if a camera action was performed, check will be set to 1.
		// this will indicate the loop needs to be evaluated one more time.
		check = 0;
		
		//--------------------------------------------------
		// check to see if x is out of the bounds of the target block
		//--------------------------------------------------
		if(cam->x < -BLOCK_WIDTH_1_2){
			// pan the camera appropriately
			camera_pan(cam, CAMERA_PAN_LEFT);
			// add BLOCK_WIDTH to x
			cam->x += BLOCK_WIDTH;
			// record that a single check was made.
			check++;
		}
		else if(cam->x > BLOCK_WIDTH_1_2){
			// pan the camera appropriately
			camera_pan(cam, CAMERA_PAN_RIGHT);
			// subtract BLOCK_WIDTH from x
			cam->x -= BLOCK_WIDTH;
			// record that a single check was made.
			check++;
		}
		
		//--------------------------------------------------
		// check if y is out of bounds of the target block
		//--------------------------------------------------
		if(cam->y < -BLOCK_HEIGHT_1_2){
			// pan the camera appropriately
			camera_pan(cam, CAMERA_PAN_UP);
			// add BLOCK_HEIGHT to y
			cam->y += BLOCK_HEIGHT;
			// record that a single check was made.
			check++;
		}
		else if(cam->y > BLOCK_HEIGHT_1_2){
			// pan the camera appropriately
			camera_pan(cam, CAMERA_PAN_DOWN);
			// subtract BLOCK_HEIGHT from y
			cam->y -= BLOCK_HEIGHT;
			// record that a single check was made.
			check++;
		}
		
		
		//--------------------------------------------------
		// check for scale being too large
		//--------------------------------------------------
		if(cam->scale > CAMERA_SCALE_MAX){
			// zoom out once
			camera_zoom_out(cam);
			// update scale.
			cam->scale /= BLOCK_LINEAR_SCALE_FACTOR;
			// record that a single check was made
			check++;
		}
		
		//--------------------------------------------------
		// check for scale being too small
		//--------------------------------------------------
		if(cam->scale <= CAMERA_SCALE_MIN){
			// zoom in once
			camera_zoom_in(cam);
			// update scale
			cam->scale *= BLOCK_LINEAR_SCALE_FACTOR;
			// record that a single check was made.
			check++;
		}
		
		// if check was made to be nonzero at any point, that means the camera will need to be checked.
		if(check) cam->changed = 1;
		
	}while(check);	// loop again if a modification was made to the camera
	
	// this block of code is only executed if the camera changed positions.
	// (this will be nonzero if any camera zooming/panning was necessary.
	if(cam->changed){
		// this generates all the neighbors that will be necessary to render images
		// it ensures that the 8 blocks immediately around the target block exist
		block_generate_neighbor(cam->target, BLOCK_NEIGHBOR_ALL);										// UP, DOWN, LEFT, RIGHT
		block_generate_neighbor(cam->target->neighbors[BLOCK_NEIGHBOR_UP], BLOCK_NEIGHBOR_LEFT);		// UP+LEFT
		block_generate_neighbor(cam->target->neighbors[BLOCK_NEIGHBOR_UP], BLOCK_NEIGHBOR_RIGHT);		// UP+RIGHT
		block_generate_neighbor(cam->target->neighbors[BLOCK_NEIGHBOR_DOWN], BLOCK_NEIGHBOR_LEFT);		// DOWN+LEFT
		block_generate_neighbor(cam->target->neighbors[BLOCK_NEIGHBOR_DOWN], BLOCK_NEIGHBOR_RIGHT);		// DOWN+RIGHT
		// this sets all of the renderMe flags to 1 initially
		//cam->target->renderMe = 1;																		// CENTER
		//cam->target->neighbors[BLOCK_NEIGHBOR_UP]->renderMe = 1;										// UP
		//cam->target->neighbors[BLOCK_NEIGHBOR_DOWN]->renderMe = 1;										// DOWN
		//cam->target->neighbors[BLOCK_NEIGHBOR_LEFT]->renderMe = 1;										// LEFT
		//cam->target->neighbors[BLOCK_NEIGHBOR_RIGHT]->renderMe = 1;										// RIGHT
		//cam->target->neighbors[BLOCK_NEIGHBOR_UP]->neighbors[BLOCK_NEIGHBOR_LEFT]->renderMe = 1;		// UP+LEFT
		//cam->target->neighbors[BLOCK_NEIGHBOR_UP]->neighbors[BLOCK_NEIGHBOR_RIGHT]->renderMe = 1;		// UP+RIGHT
		//cam->target->neighbors[BLOCK_NEIGHBOR_DOWN]->neighbors[BLOCK_NEIGHBOR_LEFT]->renderMe = 1;		// DOWN+LEFT
		//cam->target->neighbors[BLOCK_NEIGHBOR_DOWN]->neighbors[BLOCK_NEIGHBOR_RIGHT]->renderMe = 1;		// DOWN+RIGHT
	}
	
	return 0;
}





/// this function zooms cam in once.
/// this function is meant to be used by camera_check().
/// It is discouraged to call this function from any place other than camera_check()
/// this will automatically generation children of the target block if they do not yet exist
/// this will NOT affect the scale variable in the camera structure.
// this function will adjust the target, scale, and x, y variables of the camera.
// returns 0 on success
// returns 1 on invalid cameraData pointer
// returns 2 when xcenter is out of bounds and too far left.
// returns 3 when xcenter is out of bounds and too far right.
// returns 4 when ycenter is out of bounds and too far up.
// returns 5 when ycenter is out of bounds and too far down.
short camera_zoom_in(struct cameraData *cam){
	
	// check for camera pointer being NULL
	if(cam == NULL){
		error("camera_zoom_in() was sent invalid cameraData pointer. cam = NULL");
		return 1;
	}
	
	/// TODO: write zoom in function.
	// you will need to figure out which child you need to zoom to (this should be extremely easy)
	// and you will need to figure out the (x,y) coordinate transformations between the current and the particular child.
	
	
	
	/// quick and dirty way to zoom in
	
	// verify that the children exist. If they don't already, this function will make them.
	block_generate_children(cam->target);
	// calculate which child to zoom in to.
	// this calculates where the center of the camera is on the target block
	int xcenter = cam->x + (BLOCK_WIDTH/2)*cam->scale + 0.5;
	int ycenter = cam->y + (BLOCK_HEIGHT/2)*cam->scale + 0.5;
	
	// test error conditions
	if(xcenter < 0){
		error_d("camera_zoom_in() calculated that xcenter is out of bounds. xcenter too far left. xcenter =", xcenter);
		return 2;
	}
	else if(xcenter >= BLOCK_WIDTH){
		error_d("camera_zoom_in() calculated that xcenter is out of bounds. xcenter too far right. xcenter =", xcenter);
		return 3;
	}
	if(ycenter < 0){
		error_d("camera_zoom_in() calculated that ycenter is out of bounds. ycenter too far up. ycenter =", ycenter);
		return 4;
	}
	else if(ycenter >= BLOCK_HEIGHT){
		error_d("camera_zoom_in() calculated that ycenter is out of bounds. ycenter too far down. ycenter =", ycenter);
		return 5;
	}
	
	// update target to proper child
	cam->target = cam->target->children[xcenter/BLOCK_WIDTH_1_3 + 3*(ycenter/BLOCK_WIDTH_1_3)];
	
	// success
	return 0;
}



/// this function zooms camera out once.
/// this function is meant to be used by camera_check().
/// It is discouraged to call this function from any place other than camera_check()
/// this will automatically generation the parent of the target block if it does not yet exist
// returns 0 on success
// returns 1 on invalid cameraData pointer
short camera_zoom_out(struct cameraData *cam){
	
	// check for camera pointer being NULL
	if(cam == NULL){
		error("camera_zoom_out() was sent invalid cameraData pointer. cam = NULL");
		return 1;
	}
	
	
	
	/// TODO: write function to calculate the (x,y) coordinate transformations between the current block and the parent.
	// this will probably require some math on pen and paper.
	
	
	
	/// this is a temporary (visually choppy) solution for testing:
	
	// this will verify that a parent has been added.
	block_generate_parent(cam->target);
	
	// move to the parent
	cam->target = cam->target->parent;
	
	// success
	return 0;
}






/// this function pans cam in the specified direction once.
/// this function is meant to be used by camera_check().
/// It is discouraged to call this function from any place other than camera_check()
/// this will automatically generate neighbors of the target block if they do not yet exist.
// returns 0 on success
// returns 1 on invalid cameraData pointer
// returns 2 on invalid pan direction
short camera_pan(struct cameraData *cam, short panDir){
	
	// check for camera pointer being NULL
	if(cam == NULL){
		error("camera_pan() was sent invalid cameraData pointer. cam = NULL");
		return 1;
	}
	
	
	switch(panDir){
		case CAMERA_PAN_UP:
			// verify that the neighbor exists. If it doesn't exist, this function will create it.
			block_generate_neighbor(cam->target, BLOCK_NEIGHBOR_UP);
			// move the neighbor
			cam->target = (cam->target)->neighbors[BLOCK_NEIGHBOR_UP];
			break;
		case CAMERA_PAN_DOWN:
			// verify that the neighbor exists. If it doesn't exist, this function will create it.
			block_generate_neighbor(cam->target, BLOCK_NEIGHBOR_DOWN);
			// move the neighbor
			cam->target = (cam->target)->neighbors[BLOCK_NEIGHBOR_DOWN];
			break;
		case CAMERA_PAN_LEFT:
			// verify that the neighbor exists. If it doesn't exist, this function will create it.
			block_generate_neighbor(cam->target, BLOCK_NEIGHBOR_LEFT);
			// move the neighbor
			cam->target = (cam->target)->neighbors[BLOCK_NEIGHBOR_LEFT];
			break;
		case CAMERA_PAN_RIGHT:
			// verify that the neighbor exists. If it doesn't exist, this function will create it.
			block_generate_neighbor(cam->target, BLOCK_NEIGHBOR_RIGHT);
			// move the neighbor
			cam->target = (cam->target)->neighbors[BLOCK_NEIGHBOR_RIGHT];
			break;
		default:
			// report error
			error_d("camera_pan() sent invalid panDir. panDir =", panDir);
			return 2;
			break;
	}
	
	// success
	return 0;
}





/// this will render the "cam" cameraData to the "dest" renderer.
// returns 0 on successful rendering.
// returns 1 if the dest pointer is NULL.
// returns 2 if the cam pointer is NULL.
// returns 3 if cam->target is NULL.
short camera_render(SDL_Renderer *dest, struct cameraData *cam, int width, int height){
	
	// check to see if dest is invalid
	if(dest == NULL){
		//report the error
		error("camera_render() was sent an invalid renderer. dest = NULL");
		return 1;
	}
	// check to see if cam is invalid
	if(cam == NULL){
		//report the error
		error("camera_render() was sent an invalid cam value. cam = NULL");
		return 2;
	}
	
	if(cam->target == NULL){
		error("camera_render() was sent a valid camera with an invalid target blockData pointer. cam->target = NULL");
		return 3;
	}
	
	
	camera_check(cam);
	//------------------------------------------------------------
	// update all textures that need updating
	//------------------------------------------------------------
	// TARGET (CENTER)
	if(cam->target->renderMe || cam->target->texture == NULL) block_render(cam->target, dest);
	// NEIGHBORS
	if(cam->target->neighbors[BLOCK_NEIGHBOR_UP]->renderMe    || cam->target->neighbors[BLOCK_NEIGHBOR_UP]->texture    == NULL) block_render(cam->target->neighbors[BLOCK_NEIGHBOR_UP],    dest);
	if(cam->target->neighbors[BLOCK_NEIGHBOR_DOWN]->renderMe  || cam->target->neighbors[BLOCK_NEIGHBOR_DOWN]->texture  == NULL) block_render(cam->target->neighbors[BLOCK_NEIGHBOR_DOWN],  dest);
	if(cam->target->neighbors[BLOCK_NEIGHBOR_LEFT]->renderMe  || cam->target->neighbors[BLOCK_NEIGHBOR_LEFT]->texture  == NULL) block_render(cam->target->neighbors[BLOCK_NEIGHBOR_LEFT],  dest);
	if(cam->target->neighbors[BLOCK_NEIGHBOR_RIGHT]->renderMe || cam->target->neighbors[BLOCK_NEIGHBOR_RIGHT]->texture == NULL) block_render(cam->target->neighbors[BLOCK_NEIGHBOR_RIGHT], dest);
	// NEIGHBOR'S NEIGHBORS
	if(cam->target->neighbors[BLOCK_NEIGHBOR_UP]->neighbors[BLOCK_NEIGHBOR_LEFT]->renderMe    || cam->target->neighbors[BLOCK_NEIGHBOR_UP]->neighbors[BLOCK_NEIGHBOR_LEFT]->texture    == NULL) block_render(cam->target->neighbors[BLOCK_NEIGHBOR_UP]->neighbors[BLOCK_NEIGHBOR_LEFT],    dest);
	if(cam->target->neighbors[BLOCK_NEIGHBOR_UP]->neighbors[BLOCK_NEIGHBOR_RIGHT]->renderMe    || cam->target->neighbors[BLOCK_NEIGHBOR_UP]->neighbors[BLOCK_NEIGHBOR_RIGHT]->texture    == NULL) block_render(cam->target->neighbors[BLOCK_NEIGHBOR_UP]->neighbors[BLOCK_NEIGHBOR_RIGHT],    dest);
	if(cam->target->neighbors[BLOCK_NEIGHBOR_DOWN]->neighbors[BLOCK_NEIGHBOR_LEFT]->renderMe  || cam->target->neighbors[BLOCK_NEIGHBOR_DOWN]->neighbors[BLOCK_NEIGHBOR_LEFT]->texture  == NULL) block_render(cam->target->neighbors[BLOCK_NEIGHBOR_DOWN]->neighbors[BLOCK_NEIGHBOR_LEFT],  dest);
	if(cam->target->neighbors[BLOCK_NEIGHBOR_DOWN]->neighbors[BLOCK_NEIGHBOR_RIGHT]->renderMe  || cam->target->neighbors[BLOCK_NEIGHBOR_DOWN]->neighbors[BLOCK_NEIGHBOR_RIGHT]->texture  == NULL) block_render(cam->target->neighbors[BLOCK_NEIGHBOR_DOWN]->neighbors[BLOCK_NEIGHBOR_RIGHT],  dest);
	
	
	// create a rectangle the specifies the location on the window that the texture will be printed to
	SDL_Rect dstrect;
	// these values are the same for every texture
	dstrect.w = (width)/(cam->scale);
	dstrect.h = (height)/(cam->scale);
	
	//------------------------------------------------------------
	// render target
	//------------------------------------------------------------
	dstrect.x = -cam->x*width/BLOCK_WIDTH;
	dstrect.y = -cam->y*height/BLOCK_HEIGHT;
	// copy texture to screen.
	SDL_RenderCopy(dest, cam->target->texture, NULL, &dstrect);
	//------------------------------------------------------------
	// render neighbor UP
	//------------------------------------------------------------
	// set x y offsets
	dstrect.x = -cam->x*width/BLOCK_WIDTH;
	dstrect.y = -cam->y*height/BLOCK_HEIGHT - dstrect.h;
	// copy texture to screen.
	SDL_RenderCopy(dest, cam->target->neighbors[BLOCK_NEIGHBOR_UP]->texture, NULL, &dstrect);
	//------------------------------------------------------------
	// render neighbor DOWN
	//------------------------------------------------------------
	// set x y offsets
	dstrect.x = -cam->x*width/BLOCK_WIDTH;
	dstrect.y = -cam->y*height/BLOCK_HEIGHT + dstrect.h;
	// copy texture to screen.
	SDL_RenderCopy(dest, cam->target->neighbors[BLOCK_NEIGHBOR_DOWN]->texture, NULL, &dstrect);
	//------------------------------------------------------------
	// render neighbor LEFT
	//------------------------------------------------------------
	// set x y offsets
	dstrect.x = -cam->x*width/BLOCK_WIDTH - dstrect.w;
	dstrect.y = -cam->y*height/BLOCK_HEIGHT;
	// copy texture to screen.
	SDL_RenderCopy(dest, cam->target->neighbors[BLOCK_NEIGHBOR_LEFT]->texture, NULL, &dstrect);
	//------------------------------------------------------------
	// render neighbor RIGHT
	//------------------------------------------------------------
	// set x y offsets
	dstrect.x = -cam->x*width/BLOCK_WIDTH + dstrect.w;
	dstrect.y = -cam->y*height/BLOCK_HEIGHT;
	// copy texture to screen.
	SDL_RenderCopy(dest, cam->target->neighbors[BLOCK_NEIGHBOR_RIGHT]->texture, NULL, &dstrect);
	//------------------------------------------------------------
	// render neighbor UP+LEFT
	//------------------------------------------------------------
	// set x y offsets
	dstrect.x = -cam->x*width/BLOCK_WIDTH - dstrect.w;
	dstrect.y = -cam->y*height/BLOCK_HEIGHT - dstrect.h;
	// copy texture to screen.
	SDL_RenderCopy(dest, cam->target->neighbors[BLOCK_NEIGHBOR_UP]->neighbors[BLOCK_NEIGHBOR_LEFT]->texture, NULL, &dstrect);
	//------------------------------------------------------------
	// render neighbor UP+RIGHT
	//------------------------------------------------------------
	// set x y offsets
	dstrect.x = -cam->x*width/BLOCK_WIDTH + dstrect.w;
	dstrect.y = -cam->y*height/BLOCK_HEIGHT - dstrect.h;
	// copy texture to screen.
	SDL_RenderCopy(dest, cam->target->neighbors[BLOCK_NEIGHBOR_UP]->neighbors[BLOCK_NEIGHBOR_RIGHT]->texture, NULL, &dstrect);
	//------------------------------------------------------------
	// render neighbor DOWN+LEFT
	//------------------------------------------------------------
	// set x y offsets
	dstrect.x = -cam->x*width/BLOCK_WIDTH - dstrect.w;
	dstrect.y = -cam->y*height/BLOCK_HEIGHT + dstrect.h;
	// copy texture to screen.
	SDL_RenderCopy(dest, cam->target->neighbors[BLOCK_NEIGHBOR_DOWN]->neighbors[BLOCK_NEIGHBOR_LEFT]->texture, NULL, &dstrect);
	//------------------------------------------------------------
	// render neighbor DOWN+RIGHT
	//------------------------------------------------------------
	// set x y offsets
	dstrect.x = -cam->x*width/BLOCK_WIDTH + dstrect.w;
	dstrect.y = -cam->y*height/BLOCK_HEIGHT + dstrect.h;
	// copy texture to screen.
	SDL_RenderCopy(dest, cam->target->neighbors[BLOCK_NEIGHBOR_DOWN]->neighbors[BLOCK_NEIGHBOR_RIGHT]->texture, NULL, &dstrect);
	
	// successful print.
	return 0;
}



