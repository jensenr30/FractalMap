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
	
	// return a pointer to the camera.
	return cam;
}



/// this will make sure that your camera has valid parameters (1/3 < scale < 3, x within (BLOCK_WIDTH/2 - 1), and y within (BLOCK_HEIGHT/2 - 1)).
// returns 0 on successful check
// returns 1 on null cam pointer
short camera_check(struct cameraData *cam){
	
	if(cam == NULL){
		// report error
		error("camera_check() was sent invalid cam. cam = NULL");
		// and return an error
		return 1;
	}
	
	short check;
	do{
		
		// check is initially set to 0 at the start of each loop.
		// if a camera action was performed, check will be set to 1.
		// this will indicate the loop needs to be evaluated one more time.
		check = 0;
		
		//--------------------------------------------------
		// check to see if x is out of the bounds of the target block
		//--------------------------------------------------
		if(cam->x < 0){
			// pan the camera appropriately
			camera_pan(cam, CAMERA_PAN_LEFT);
			// record that a single check was made.
			check = 1;
		}
		else if(cam->x >= BLOCK_WIDTH){
			// pan the camera appropriately
			camera_pan(cam, CAMERA_PAN_RIGHT);
			// record that a single check was made.
			check = 1;
		}
		
		//--------------------------------------------------
		// check if y is out of bounds of the target block
		//--------------------------------------------------
		if(cam->y < 0){
			// pan the camera appropriately
			camera_pan(cam, CAMERA_PAN_UP);
			// record that a single check was made.
			check = 1;
		}
		else if(cam->y >= BLOCK_HEIGHT){
			// pan the camera appropriately
			camera_pan(cam, CAMERA_PAN_DOWN);
			// record that a single check was made.
			check = 1;
		}
		
		
		//--------------------------------------------------
		// check for scale being too large
		//--------------------------------------------------
		if(cam->scale >= BLOCK_LINEAR_SCALE_FACTOR){
			// zoom out once
			camera_zoom_out(cam);
			// record that a single check was made
			check = 1;
		}
		
		//--------------------------------------------------
		// check for scale being too small
		//--------------------------------------------------
		if(cam->scale <= 1/BLOCK_LINEAR_SCALE_FACTOR){
			// zoom in once
			camera_zoom_in(cam);
			// record that a single check was made.
			check = 1;
		}
		
		
		
	}while(check);	// loop again if a modification was made to the camera
	
	
	return 0;
}





/// this function zooms cam in once.
/// this function is meant to be used by camera_check().
/// It is discouraged to call this function from any place other than camera_check()
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
	// update the scale of the camera now that it is pointing at the child of the previous block
	cam->scale *= BLOCK_LINEAR_SCALE_FACTOR;
	
	// success
	return 0;
}



/// this function zooms camera out once.
/// this function is meant to be used by camera_check().
/// It is discouraged to call this function from any place other than camera_check()
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
	
	// reset scale.
	cam->scale /= BLOCK_LINEAR_SCALE_FACTOR;
	
	// success
	return 0;
}






/// this function pans cam in the specified direction once.
/// this function is meant to be used by camera_check().
/// It is discouraged to call this function from any place other than camera_check()
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
short camera_render(SDL_Renderer *dest, struct cameraData *cam){
	
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
	
	/* OLD PRINTING CODE
	// i and j index into the surface pixel array.
	// iElevation and jElevation index into the blockData's elevation array.
	int i, j, iElevation, jElevation;
	// these store the constant scaling factors used to scale the i and j variables into the iElevation and jElevation variables.
	float iScale = cam->scale*BLOCK_WIDTH /(float)dest->w;
	float jScale = cam->scale*BLOCK_HEIGHT/(float)dest->h;
	
	// calculate the color for every pixel on the destination surface
	for(i=0; i<dest->w; i++){
		for(j=0; j<dest->h; j++){
			// calculate the x and y indexes for the elevation array
			iElevation = (int)(cam->x + i*iScale);
			jElevation = (int)(cam->y + j*jScale);
			// make sure the elevation index is in between
			if(iElevation>=0 && iElevation<BLOCK_WIDTH && jElevation >= 0 && jElevation < BLOCK_HEIGHT){
				set_pixel(dest, i, j, (0xff000000|(int)(cam->target->elevation[iElevation][jElevation]))&(0xff00f000) );
			}
		}
	}
	*/
	
	
	// if the target block has not been rendered, render it now.
	if(cam->target->texture == NULL || cam->target->renderMe)
		block_render(cam->target, dest);
	
	SDL_RenderCopy(dest, cam->target->texture, NULL, NULL);
	
	
	
	// successful print.
	return 0;
}



