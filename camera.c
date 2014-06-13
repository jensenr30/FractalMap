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
		// check for scale being too large
		//--------------------------------------------------
		if(cam->scale >= BLOCK_LINEAR_SCALE_FACTOR){
			// zoom out once
			camera_zoom_out(cam);
			// record that a single check was made
			check = 1;
		}
		// check for scale being too small
		if(cam->scale <= BLOCK_LINEAR_SCALE_FACTOR_INV){
			// zoom in once
			camera_zoom_in(cam);
			// record that a single check was made.
			check = 1;
		}
		
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
		
	}while(check);	// loop again if a modification was made to the camera
	
	
	return 0;
}





/// this function zooms cam in once.
/// this function is meant to be used by camera_check().
/// It is discouraged to call this function from any place other than camera_check()
// this function will adjust the target, scale, and x, y variables of the camera.
// returns 0 on success
// returns 1 on invalid cameraData pointer
short camera_zoom_in(struct cameraData *cam){
	
	// check for camera pointer being NULL
	if(cam == NULL){
		error("camera_zoom_in() was sent invalid cameraData pointer. cam = NULL");
		return 1;
	}
	
	/// TODO: write zoom in function.
	// you will need to figure out which child you need to zoom to (this should be extremely easy)
	// and you will need to figure out the (x,y) coordinate transformations between the current and the particular child.
	
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
	block_random_fill(cam->target, 0,0xffffff);
	// reset scale.
	cam->scale *= 1/BLOCK_LINEAR_SCALE_FACTOR;
	
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
			// verify that THE neighbor exists. If it doesn't exist, this function will create it.
			block_calculate_neighbor(cam->target, BLOCK_NEIGHBOR_UP);
			/// TODO: write pan down
			break;
		case CAMERA_PAN_DOWN:
			// verify that THE neighbor exists. If it doesn't exist, this function will create it.
			block_calculate_neighbor(cam->target, BLOCK_NEIGHBOR_DOWN);
			/// TODO: write pan down
			break;
		case CAMERA_PAN_LEFT:
			// verify that THE neighbor exists. If it doesn't exist, this function will create it.
			block_calculate_neighbor(cam->target, BLOCK_NEIGHBOR_LEFT);
			/// TODO: write pan left
			break;
		case CAMERA_PAN_RIGHT:
			// verify that THE neighbor exists. If it doesn't exist, this function will create it.
			block_calculate_neighbor(cam->target, BLOCK_NEIGHBOR_RIGHT);
			/// TODO: write pan right
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





/// this will print the "cam" cameraData to the "dest" surface.
// returns 0 on successful print to surface
// returns 1 if the dest pointer is NULL
// returns 2 if the cam pointer is NULL
short camera_print(SDL_Surface *dest, struct cameraData *cam){
	
	// check to see if dest is invalid
	if(dest == NULL){
		//report the error
		error("print_camera_view() was sent an invalid dest value. dest = NULL");
		return 1;
	}
	// check to see if cam is invalid
	if(cam == NULL){
		//report the error
		error("print_camera_view() was sent an invalid cam value. cam = NULL");
		return 2;
	}
	
	
	int i, istart, iend, j, jstart, jend;
	
	// if the camera is looking on the left side of the block,
	if(cam->x < 0){
		istart = -cam->x;
		iend = BLOCK_WIDTH-1;
	}
	// otherwise, if the camera is centered or on the right side of the block,
	else{
		istart = 0;
		iend = BLOCK_WIDTH-1 - cam->x;
	}
	// if the camera is looking at the upper half of the block,
	if(cam->y < 0){
		jstart = -cam->y;
		jend = BLOCK_HEIGHT-1;
	}
	// otherwise, if the camera is centered or on the upper side of the block,
	else{
		jstart = 0;
		jend = BLOCK_HEIGHT - cam->y;
	}
	
	for(i=istart; i<iend; i++){
		for(j=jstart; j<jend; j++){
			set_pixel( dest, (int)((i-istart)*cam->scale + 0.5), (int)((j-jstart)*cam->scale + 0.5), 0xff000000|(int)(cam->target->elevation[i][j]));
		}
	}
	
	
	// successful print.
	return 0;
}



