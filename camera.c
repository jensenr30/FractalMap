#include "camera.h"
#include "utilities.h"


/// this function pans cam in the specified direction once.
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
			block_calculate_neighbors(cam->target, BLOCK_NEIGHBOR_UP);
			/// TODO: write pan down
			break;
		case CAMERA_PAN_DOWN:
			// verify that THE neighbor exists. If it doesn't exist, this function will create it.
			block_calculate_neighbors(cam->target, BLOCK_NEIGHBOR_DOWN);
			/// TODO: write pan down
			break;
		case CAMERA_PAN_LEFT:
			// verify that THE neighbor exists. If it doesn't exist, this function will create it.
			block_calculate_neighbors(cam->target, BLOCK_NEIGHBOR_LEFT);
			/// TODO: write pan left
			break;
		case CAMERA_PAN_RIGHT:
			// verify that THE neighbor exists. If it doesn't exist, this function will create it.
			block_calculate_neighbors(cam->target, BLOCK_NEIGHBOR_RIGHT);
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




/// this function zooms cam in once.
// this function will adjust the target, scale, and x, y variables of the camera.
// returns 0 on success
// returns 1 on invalid cameraData pointer
short camera_zoom_in(struct cameraData *cam){
	
	// check for camera pointer being NULL
	if(cam == NULL){
		error("camera_zoom_in() was sent invalid cameraData pointer. cam = NULL");
		return 1;
	}
	
	// TODO: write zoom in function
	
	// success
	return 0;
}



/// this function zooms camera out once
// returns 0 on success
// returns 1 on invalid cameraData pointer
short camera_zoom_out(struct cameraData *cam){
	
	// check for camera pointer being NULL
	if(cam == NULL){
		error("camera_zoom_out() was sent invalid cameraData pointer. cam = NULL");
		return 1;
	}
	
	// TODO: write zoom out code
	
	// success
	return 0;
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
		if(cam->x < -BLOCK_WIDTH_1_2){
			// pan the camera appropriately
			camera_pan(cam, CAMERA_PAN_LEFT);
			// record that a single check was made.
			check = 1;
		}
		else if(cam->x > BLOCK_WIDTH_1_2){
			// pan the camera appropriately
			camera_pan(cam, CAMERA_PAN_RIGHT);
			// record that a single check was made.
			check = 1;
		}
		
		//--------------------------------------------------
		// check if y is out of bounds of the target block
		//--------------------------------------------------
		if(cam->y < -BLOCK_HEIGHT_1_2){
			// pan the camera appropriately
			camera_pan(cam, CAMERA_PAN_UP);
			// record that a single check was made.
			check = 1;
		}
		else if(cam->y > BLOCK_HEIGHT_1_2){
			// pan the camera appropriately
			camera_pan(cam, CAMERA_PAN_DOWN);
			// record that a single check was made.
			check = 1;
		}
		
	}while(check);	// loop again if a modification was made to the camera
	
	
	return 0;
}


/// this function will move a camera a relative amount (xdiff, ydiff).
// this function will automatically call camera_check to make sure the camera is valid
// returns 0 on successful move
// returns 1 on null cam pointer
short camera_move(struct cameraData *cam, int xdiff, int ydiff){
	
	// if the camera received by the function is invalid, 
	if(cam == NULL){
		// report error
		error("camera_move() was sent invalid cam. cam = NULL");
		// and return and error
		return 1;
	}
	
	// TODO: write camera move function
	
	camera_check(cam);
	return 0;
}
