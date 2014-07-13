#include "block.h"
#include "camera.h"
#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"
#include <string.h>
#include "rand.h"
#include "graphics.h"
#include "my_math.h"


/// throws random data into blockData
// returns 0 on success
// returns 1 when the block is a NULL pointer.
short block_fill_random(struct blockData *block, float range_low, float range_high){
	
	// check for block pointer being NULL.
	if(block == NULL){
		error("block_fill_random() could not fill block. block = NULL.");
		return 1;
	}
	
	// shiffle around the range values if they are not right.
	if(range_low > range_high){
		float temp = range_high;
		range_high = range_low;
		range_low = temp;
	}
	
	int j, i;
	
	// this will generate data that will statistically average zero.
	// the data will not average zero for each set of numbers however.
	for(i=0; i<BLOCK_WIDTH; i++){
		for(j=0; j<BLOCK_HEIGHT; j++){
			// generate a random number between range_low and range_high.
			block->elevation[i][j] = ((genrand()%100001)/100000.0)*(range_high-range_low) + range_low;//((rand()%100001)/100000.0)*(range_higher-range_lower)-((range_higher+range_lower)/2);
		}
	}
	
	// render the block next time it needs to be printed
	block->renderMe = 1;
	// generated random data in block successfully.
	return 0;
}


/// this is a recursive function that will be called by block_generate_terrain().
// this function will be called with the points [x1,y1] [x1,y2] [x2,y1] and [x2,y2] already set to their proper values.
// this function will essentially interpolate (and add randomness) inner elements in the grid, interpolating off of those original four points.
// this function will input elevation data that looks like this: (V = valid, . = not necessarily any data yet)
	//	V . . . . . . . V
	//	. . . . . . . . .
	//	. . . . . . . . .
	//	. . . . . . . . .
	//	. . . . . . . . .
	//	. . . . . . . . .
	//	. . . . . . . . .
	//	. . . . . . . . .
	//	V . . . . . . . V
// and it will create elevation data that looks like this: (C = created data)
	//	V . C C . C C . V
	//	. . . . . . . . .
	//	C . C C . C C . C
	//	C . C C . C C . C
	//	. . . . . . . . .
	//	C . C C . C C . C
	//	C . C C . C C . C
	//	. . . . . . . . .
	//	V . C C . C C . V
// note that, in general, it need not be a 9x9 input, but a 9x9 input is the smallest input that is valid.
// the size of input area (as specified by x1,y1,x2,y2) must be a grid with width and length equal to a power of three greater than 3 (9, 27, 81, 243, etc...)
// x1, y1, x2, y2 are the corners of the input grid area. the function will operate on the block elevation, but only within the square that x1,y1,x2,y2 describes.
short block_generate_terrain_recursive_call(struct blockData *block, int x1, int y1, int x2, int y2, float slopeMax){
	
	// this function does not check to see if block is valid, because it should have already been checked in block_generate_terrain
	
	// calculate width and height (they should always be the same)
	// width should also always be a power of 3
	int width = x2-x1+1;
	int height = y2-y1+1;
	
	// if the width is less than 9, then generate the last 5 elevations "manually".
	if(width < 9 || height < 9){
		// generate the four around the center based on the four corners
		block->elevation[x1+1][y1] = rand_radius_retry((block->elevation[x1][y1]+block->elevation[x2][y1])/2.0, slopeMax, BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);
		block->elevation[x1+1][y2] = rand_radius_retry((block->elevation[x1][y2]+block->elevation[x2][y2])/2.0, slopeMax, BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);
		block->elevation[x1][y1+1] = rand_radius_retry((block->elevation[x1][y1]+block->elevation[x1][y2])/2.0, slopeMax, BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);
		block->elevation[x2][y1+1] = rand_radius_retry((block->elevation[x2][y1]+block->elevation[x2][y2])/2.0, slopeMax, BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);
		// generate the center based on the four around the center
		block->elevation[x1+1][y1+1] = rand_radius_retry(( block->elevation[x1+1][y1] +  block->elevation[x1+1][y2] + block->elevation[x1][y1+1] + block->elevation[x2][y1+1])/4.0, slopeMax, BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);
		// success;
		return 0;
	}
	
	// otherwise, generating elevation will need to be done the regular way.
	
	// calculate the four mid-values in between the four corners
	int w1 = x1 + width/3;
	int w2 = x1 + (2*width)/3;
	int h1 = y1 + (height)/3;
	int h2 = y1 + (2*height)/3;
	
	// generate the perimeter top and bottom side elevations first (V = valid, C = creating now)
		//	V . C C . C C . V
		//	. . . . . . . . .
		//	. . . . . . . . .
		//	. . . . . . . . .
		//	. . . . . . . . .
		//	. . . . . . . . .
		//	. . . . . . . . .
		//	. . . . . . . . .
		//	V . C C . C C . V
	// top four
	block->elevation[w1-1][y1] = rand_radius_retry((block->elevation[x1][y1]  *(x2-(w1-1)) + block->elevation[x2][y1]*(w1-1-x1))/(x2-x1),   slopeMax*(w1-1), BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);
	block->elevation[w1]  [y1] = rand_radius_retry((block->elevation[w1-1][y1]*(x2-w1)     + block->elevation[x2][y1])          /(x2-w1+1), slopeMax       , BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);
	block->elevation[w2-1][y1] = rand_radius_retry((block->elevation[w1][y1]  *(x2-(w2-1)) + block->elevation[x2][y1]*(w2-1-w1))/(x2-w1),   slopeMax*(w1-1), BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);
	block->elevation[w2]  [y1] = rand_radius_retry((block->elevation[w2-1][y1]*(x2-w2)     + block->elevation[x2][y1])          /(x2-w2+1), slopeMax       , BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);
	// bottom four
	block->elevation[w1-1][y2] = rand_radius_retry((block->elevation[x1][y2]  *(x2-(w1-1)) + block->elevation[x2][y2]*(w1-1-x1))/(x2-x1),   slopeMax*(w1-1), BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);
	block->elevation[w1]  [y2] = rand_radius_retry((block->elevation[w1-1][y2]*(x2-w1)     + block->elevation[x2][y2])          /(x2-w1+1), slopeMax       , BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);
	block->elevation[w2-1][y2] = rand_radius_retry((block->elevation[w1][y2]  *(x2-(w2-1)) + block->elevation[x2][y2]*(w2-1-w1))/(x2-w1),   slopeMax*(w1-1), BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);
	block->elevation[w2]  [y2] = rand_radius_retry((block->elevation[w2-1][y2]*(x2-w2)     + block->elevation[x2][y2])          /(x2-w2+1), slopeMax       , BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);
	
	// generate the perimeter top and bottom side elevations first (V = valid, C = creating now)
		//	V . . . . . . . V
		//	. . . . . . . . .
		//	C . . . . . . . C
		//	C . . . . . . . C
		//	. . . . . . . . .
		//	C . . . . . . . C
		//	C . . . . . . . C
		//	. . . . . . . . .
		//	V . . . . . . . V
	// left four
	block->elevation[x1][h1-1] = rand_radius_retry((block->elevation[x1][y1]  *(y2-(h1-1)) + block->elevation[x1][y2]*(h1-1-y1))/(y2-y1),   slopeMax*(h1-1), BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);
	block->elevation[x1][h1]   = rand_radius_retry((block->elevation[x1][h1-1]*(y2-h1)     + block->elevation[x1][y2])          /(y2-h1+1), slopeMax       , BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);
	block->elevation[x1][h2-1] = rand_radius_retry((block->elevation[x1][h1]  *(y2-(h2-1)) + block->elevation[x1][y2]*(h2-1-h1))/(y2-h1),   slopeMax*(h1-1), BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);
	block->elevation[x1][h2]   = rand_radius_retry((block->elevation[x1][h2-1]*(y2-h2)     + block->elevation[x1][y2])          /(y2-h2+1), slopeMax       , BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);
	// right four
	block->elevation[x2][h1-1] = rand_radius_retry((block->elevation[x2][y1]  *(y2-(h1-1)) + block->elevation[x2][y2]*(h1-1-y1))/(y2-y1),   slopeMax*(h1-1), BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);
	block->elevation[x2][h1]   = rand_radius_retry((block->elevation[x2][h1-1]*(y2-h1)     + block->elevation[x2][y2])          /(y2-h1+1), slopeMax       , BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);
	block->elevation[x2][h2-1] = rand_radius_retry((block->elevation[x2][h1]  *(y2-(h2-1)) + block->elevation[x2][y2]*(h2-1-h1))/(y2-h1),   slopeMax*(h1-1), BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);
	block->elevation[x2][h2]   = rand_radius_retry((block->elevation[x2][h2-1]*(y2-h2)     + block->elevation[x2][y2])          /(y2-h2+1), slopeMax       , BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);
	
	
	// calculate the middle 16 points based on the 16 edge points
	
	block->elevation[w1-1][h1-1] = rand_radius_retry(weighted_ave_4_recip_f(block->elevation[w1-1][y1],block->elevation[w1-1][y2],block->elevation[x1]  [h1-1],block->elevation[x2][h1-1],h1-1-y1,y2-(h1-1),w1-1-x1,x2-(w1-1)), slopeMax*(h1-1-y1), BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);			
	block->elevation[w1]  [h1-1] = rand_radius_retry(weighted_ave_4_recip_f(block->elevation[w1]  [y1],block->elevation[w1]  [y2],block->elevation[w1-1][h1-1],block->elevation[x2][h1-1],h1-1-y1,y2-(h1-1),1      ,x2-w1    ), slopeMax          , BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);			
	block->elevation[w2-1][h1-1] = rand_radius_retry(weighted_ave_4_recip_f(block->elevation[w2-1][y1],block->elevation[w2-1][y2],block->elevation[w1]  [h1-1],block->elevation[x2][h1-1],h1-1-y1,y2-(h1-1),w2-1-w1,x2-(w2-1)), slopeMax*(h1-1-y1), BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);			
	block->elevation[w2]  [h1-1] = rand_radius_retry(weighted_ave_4_recip_f(block->elevation[w2]  [y1],block->elevation[w2]  [y2],block->elevation[w2-1][h1-1],block->elevation[x2][h1-1],h1-1-y1,y2-(h1-1),1      ,x2-w2    ), slopeMax          , BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);			
	
	block->elevation[w1-1][h1] = rand_radius_retry(weighted_ave_4_recip_f(block->elevation[w1-1][h1-1],block->elevation[w1-1][y2],block->elevation[x1]  [h1]  ,block->elevation[x2][h1]  ,1      ,y2-h1    ,w1-1-x1,x2-(w1-1)), slopeMax          , BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);			
	block->elevation[w1]  [h1] = rand_radius_retry(weighted_ave_4_recip_f(block->elevation[w1]  [h1-1],block->elevation[w1]  [y2],block->elevation[w1-1][h1]  ,block->elevation[x2][h1]  ,1      ,y2-h1    ,1      ,x2-w1    ), slopeMax          , BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);			
	block->elevation[w2-1][h1] = rand_radius_retry(weighted_ave_4_recip_f(block->elevation[w2-1][h1-1],block->elevation[w2-1][y2],block->elevation[w1]  [h1]  ,block->elevation[x2][h1]  ,1      ,y2-h1    ,w2-1-w1,x2-(w2-1)), slopeMax          , BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);			
	block->elevation[w2]  [h1] = rand_radius_retry(weighted_ave_4_recip_f(block->elevation[w2]  [h1-1],block->elevation[w2]  [y2],block->elevation[w2-1][h1]  ,block->elevation[x2][h1]  ,1      ,y2-h1    ,1      ,x2-w2    ), slopeMax          , BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);			
	
	block->elevation[w1-1][h2-1] = rand_radius_retry(weighted_ave_4_recip_f(block->elevation[w1-1][h1],block->elevation[w1-1][y2],block->elevation[x1]  [h2-1],block->elevation[x2][h2-1],h2-1-h1,y2-(h2-1),w1-1-x1,x2-(w1-1)), slopeMax*(h2-1-h1), BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);			
	block->elevation[w1]  [h2-1] = rand_radius_retry(weighted_ave_4_recip_f(block->elevation[w1]  [h1],block->elevation[w1]  [y2],block->elevation[w1-1][h2-1],block->elevation[x2][h2-1],h2-1-h1,y2-(h2-1),1      ,x2-w1    ), slopeMax          , BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);			
	block->elevation[w2-1][h2-1] = rand_radius_retry(weighted_ave_4_recip_f(block->elevation[w2-1][h1],block->elevation[w2-1][y2],block->elevation[w1]  [h2-1],block->elevation[x2][h2-1],h2-1-h1,y2-(h2-1),w2-1-w1,x2-(w2-1)), slopeMax*(h2-1-h1), BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);			
	block->elevation[w2]  [h2-1] = rand_radius_retry(weighted_ave_4_recip_f(block->elevation[w2]  [h1],block->elevation[w2]  [y2],block->elevation[w2-1][h2-1],block->elevation[x2][h2-1],h2-1-h1,y2-(h2-1),1      ,x2-w2    ), slopeMax          , BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);			
	
	block->elevation[w1-1][h2] = rand_radius_retry(weighted_ave_4_recip_f(block->elevation[w1-1][h2-1],block->elevation[w1-1][y2],block->elevation[x1]  [h2]  ,block->elevation[x2][h2]  ,1      ,y2-h2    ,w1-1-x1,x2-(w1-1)), slopeMax          , BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);			
	block->elevation[w1]  [h2] = rand_radius_retry(weighted_ave_4_recip_f(block->elevation[w1]  [h2-1],block->elevation[w1]  [y2],block->elevation[w1-1][h2]  ,block->elevation[x2][h2]  ,1      ,y2-h2    ,1      ,x2-w1    ), slopeMax          , BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);			
	block->elevation[w2-1][h2] = rand_radius_retry(weighted_ave_4_recip_f(block->elevation[w2-1][h2-1],block->elevation[w2-1][y2],block->elevation[w1]  [h2]  ,block->elevation[x2][h2]  ,1      ,y2-h2    ,w2-1-w1,x2-(w2-1)), slopeMax          , BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);			
	block->elevation[w2]  [h2] = rand_radius_retry(weighted_ave_4_recip_f(block->elevation[w2]  [h2-1],block->elevation[w2]  [y2],block->elevation[w2-1][h2]  ,block->elevation[x2][h2]  ,1      ,y2-h2    ,1      ,x2-w2    ), slopeMax          , BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);			
	
	
	// recursive calls to this function
	block_generate_terrain_recursive_call(block, x1, y1, w1-1, h1-1, slopeMax);	// TOP LEFT
	block_generate_terrain_recursive_call(block, w1, y1, w2-1, h1-1, slopeMax);	// TOP CENTER
	block_generate_terrain_recursive_call(block, w2, y1, x2,   h1-1, slopeMax);	// TOP RIGHT
	block_generate_terrain_recursive_call(block, x1, h1, w1-1, h2-1, slopeMax);	// CENTER LEFT
	block_generate_terrain_recursive_call(block, w1, h1, w2-1, h2-1, slopeMax);	// CENTER CENTER
	block_generate_terrain_recursive_call(block, w2, h1, x2,   h2-1, slopeMax);	// CENTER RIGHT
	block_generate_terrain_recursive_call(block, x1, h2, w1-1, y2,   slopeMax);	// BOTTOM LEFT
	block_generate_terrain_recursive_call(block, w1, h2, w2-1, y2,   slopeMax);	// BOTTOM CENTER
	block_generate_terrain_recursive_call(block, w2, h2, x2,   y2,   slopeMax);	// BOTTOM RIGHT
	
	
	
	// this function was painful to write... seriously grueling without the aid of any for loops...
	
	// success
	return 0;
}

/// this will generate terrain for a new block.
// this should be used when you are creating a new block
// this will generate terrain inside the block that you send it.
// if the block has parents, it will resemble its parents.
// if the block has children (by definition, if a block is being created, it will only have a single child in the middle) then that child will be put in the center 1/9th area of the parent
// slope max is the maximum slope in the per unit 
// return 0 on success
// return 1 on failure due to NULL block pointer.
short block_generate_terrain(struct blockData *block, float slopeMax){
	
	// make sure block is valid
	if(block == NULL){
		error("block_generate_terrain() received block = NULL.");
		return 1;
	}
	
	// set the entire block to the BLOCK_INVALID_ELEVATION value.
	/* TODO: UNCOMMENT THIS. I am commenting this temporarily to see the effects of this 
	int i,j;
	for(i=0; i<BLOCK_WIDTH; i++){
		for(j=0; j<BLOCK_HEIGHT; j++){
			block->elevation[i][j] = BLOCK_ELEVATION_INVALID;
		}
	}
	*/
	
	// check to see if we can pull data from the neighbors (in order to keep the block height continuous over block boundaries).
	if(block->neighbors[BLOCK_NEIGHBOR_UP] != NULL){
		/// TODO: import neighbor data
	}
	if(block->neighbors[BLOCK_NEIGHBOR_DOWN] != NULL){
		/// TODO: import neighbor data
	}
	if(block->neighbors[BLOCK_NEIGHBOR_LEFT] != NULL){
		/// TODO: import neighbor data
	}
	if(block->neighbors[BLOCK_NEIGHBOR_RIGHT] != NULL){
		/// TODO: import neighbor data
	}
	
	// generate the first four random points
	if(block->elevation[0][0]							!= BLOCK_ELEVATION_INVALID){
		block->elevation[0][0] = rand_range_f(BLOCK_ELEVATION_BOUND_LOWER,BLOCK_ELEVATION_BOUND_UPPER);
	}
	if(block->elevation[0][BLOCK_HEIGHT-1]				!= BLOCK_ELEVATION_INVALID){
		block->elevation[0][BLOCK_HEIGHT-1] = rand_range_f(BLOCK_ELEVATION_BOUND_LOWER,BLOCK_ELEVATION_BOUND_UPPER);
	}
	if(block->elevation[BLOCK_WIDTH-1][0]				!= BLOCK_ELEVATION_INVALID){
		block->elevation[BLOCK_WIDTH-1][0] = rand_range_f(BLOCK_ELEVATION_BOUND_LOWER,BLOCK_ELEVATION_BOUND_UPPER);
	}
	if(block->elevation[BLOCK_WIDTH-1][BLOCK_HEIGHT-1]	!= BLOCK_ELEVATION_INVALID){
		block->elevation[BLOCK_WIDTH-1][BLOCK_HEIGHT-1] = rand_range_f(BLOCK_ELEVATION_BOUND_LOWER,BLOCK_ELEVATION_BOUND_UPPER);
	}
	
	// this recursive function generates the terrain for the entire block.
	block_generate_terrain_recursive_call(block, 0, 0, BLOCK_WIDTH-1, BLOCK_HEIGHT-1, slopeMax);
	
	// don't forget to update this block's texture the next time a frame is rendered.
	block->renderMe = 1;
	
	// success
	return 0;
}





/// print a blockData
// returns 1 on successful print to file
// this function is to print the data from a map in a readable format.
// THIS IS NOT FOR SAVING MAP DATA
short block_print_to_file(struct blockData *block, char *fileName){
	
	// open a text file to write to and append (appending should never really happen though).
	FILE *fp = fopen(fileName, "a");
	
	if(fp == NULL){
		// create an error message to write to file.
		char message[512];
		strcpy(message, "block_print_to_file() could not open file \"");
		strcat(message, fileName);
		strcat(message, "\"");
		error(message);
		// return that the map_block_print_to_file() didn't get the job done.
		return 0;
	}
	
	if(block == NULL){
		error("block_fill_random() could not fill block. block = NULL.");
		return 0;
	}
	
	
	// write all elevation data
	int j, i;
	for(j=0; j<BLOCK_HEIGHT; j++){
		for(i=0; i<BLOCK_WIDTH; i++){
			fprintf(fp, "%10.10f\t", block->elevation[i][j]);
		}
		// new line
		fprintf(fp, "\n");
	}
	
	// wrote block to file successfully.
	return 1;
}


/// this will render a block and store a pointer to the rendered texture in the blockData itself.
// this is useful when you don't want to render a block every time you print the screen.
// this can be used to only re-render the block when a change in the block elevation occurs.
// returns 0 on success
// returns 1 on invalid block
// returns 2 if blockRenderer is NULL
short block_render(struct blockData *block, SDL_Renderer *blockRenderer){
	
	// quit and report error if you were given a bad block.
	if(block == NULL){
		error("block_render() was sent NULL block.");
		return 1;
	}
	// quit and report error if you were given a bad block.
	if(blockRenderer == NULL){
		error("block_render() was sent invalid blockRenderer. blockRenderer = NULL");
		return 2;
	}
	
	// this is a pointer to where surface data is stored.
	// the data will stay here (we can use the same surface data over and over again to render any block, because the surface is only needed temporarily.
	static SDL_Surface *blockSurface;
	static int firstTimeThrough = 1;
	if(firstTimeThrough) {
		blockSurface = create_surface(BLOCK_WIDTH, BLOCK_HEIGHT);
		firstTimeThrough = 0;
	}
	
	// if the block texture is valid, destroy it. This frees up the memory that was used to render the last texture. We no longer need it.
	if(block->texture != NULL) SDL_DestroyTexture(block->texture);
	
	int i, j;
	for(i=0; i<BLOCK_WIDTH; i++){
		for(j=0; j<BLOCK_HEIGHT; j++){
			set_pixel(blockSurface, i, j, color_mix_weighted_f(BLOCK_ELEVATION_COLOR_LOWER, BLOCK_ELEVATION_COLOR_UPPER, 1 - block->elevation[i][j], block->elevation[i][j]) );
		}
	}
	
	// copy surface to texture
	block->texture = SDL_CreateTextureFromSurface(blockRenderer, blockSurface);
	
	// automatically reset the renderMe flag to 0 as the block as just been rendered.
	// removing this one line of code will completely destroy the frame rate of the game.
	block->renderMe = 0;
	
	// success!
	return 0;
}




/// this function will print the given block and "childLevels" number of levels of its children under it.
// this is a recursive function.
// dest is the destination SDL_Surface that this will print to
// focus is the "top" block. "childLevels" of its children will be printed.
// highlight is the "selected" block or the "current" block.
// through the recursiveness, only the valid children will be printed.
short block_print_network_hierarchy(SDL_Surface *dest, struct blockData *focus, struct blockData *highlight, unsigned int childLevelsOrig, unsigned int childLevels, int x, int y, int size, Uint32 colorTop, Uint32 colorBot, Uint32 colorHighlight){
	
	//static SDL_Rect highLightRect;
	
	if(dest == NULL) return 1;
	if(focus == NULL) return 2;
	// draw the focus block
	draw_rect(dest, x, y, size, size, 1, 0xff000000, color_mix_weighted(colorTop, colorBot, childLevels, childLevelsOrig-childLevels), 1);
	
	
	// quit if you have printed all of the necessary children.
	if(childLevels <= 0) return 0;
	
	int c;
	
	// print 9 more of this current block's children (if they exist)
	for(c=0; c<BLOCK_CHILDREN; c++){
		block_print_network_hierarchy(dest, focus->children[c], highlight, childLevelsOrig, childLevels-1, x + (c%((int)(BLOCK_LINEAR_SCALE_FACTOR)))*size/BLOCK_LINEAR_SCALE_FACTOR, y + (c/((int)(BLOCK_LINEAR_SCALE_FACTOR)))*size/BLOCK_LINEAR_SCALE_FACTOR, size/BLOCK_LINEAR_SCALE_FACTOR, colorTop, colorBot, colorHighlight);									
	}
	
	if(focus == highlight) draw_rect(dest, x, y, size, size, 1, colorHighlight, 0x00000000, 0);
	
	
	return 0;
}



/// this function will smooth out a block (just the block it will not smooth with respect to its adjacent blocks).
// smoothFactor is from 0 to 1. it describes how much averaging the function will perform.
// smoothFactor = 1 => the smoothing will replace each element with the average of those around it.
// smoothFactor = 0.5 => the smoothing will replace each elevation with the average of itself and the aaverage of those around it.
short block_smooth(struct blockData *block, float smoothFactor){
	int i, j;
	for(i=0; i<BLOCK_WIDTH; i++){
		for(j=0; j<BLOCK_HEIGHT; j++){
			block->elevation[i][j] = (1.0-smoothFactor)*block->elevation[i][j] + smoothFactor*block_surrounding_average(block, i, j);
		}
	}
	
	// render the block next time it needs to be printed
	block->renderMe = 1;
	// success
	return 0;
}



// this calculates the average of the elevation values around some index (x,y) into the block data.
// returns average in floating point number form
// returns a floating point value of 0.0 if there is any problem computing the error.
float block_surrounding_average(struct blockData *block, unsigned int x, unsigned int y){
	
	if(block == NULL){
		error("block_surrounding_average() could not open block. block = NULL.");
		return 0.0;
	}
	
	float average = 0.0;
	int averageCount = 0;
	
	// add diagonal elevations to average
	if(x>0 && y>0)							{average += block->elevation[x-1][y-1];	averageCount++;}
	if(x>0 && y<BLOCK_HEIGHT-1)				{average += block->elevation[x-1][y+1];	averageCount++;}
	if(x<BLOCK_WIDTH-1 && y>0)				{average += block->elevation[x+1][y-1];	averageCount++;}
	if(x<BLOCK_WIDTH-1 && y<BLOCK_HEIGHT-1)	{average += 
	block->elevation[x+1][y+1];	averageCount++;}
	
	// add adjacent elevations
	if(x>0)					{average += block->elevation[x-1][y];	averageCount++; }
	if(x<BLOCK_WIDTH-1)		{average += 
	block->elevation[x+1][y];	averageCount++; }
	if(y>0)					{average += block->elevation[x][y-1];	averageCount++; }
	if(y<BLOCK_HEIGHT-1)	{average += block->elevation[x][y+1];	averageCount++; }
	
	if(averageCount < 1){
		error_d("block_surrounding_average() had invalid averageCount! averageCount =", averageCount);
		return 0.0;
	}
	
	// render the block next time it needs to be printed
	block->renderMe = 1;
	// if everything went well, return the average of the surrounding elevations
	return average/((float)averageCount);
}





/// this function will fill up the middle 9th of the block (from 1/3 to 2/3 of the block in both width and height)
// the outer ring of 8/9ths of the will be filled with outVal
// the inner 9th is filled with inVal.
// returns 1 if block was NULL.
int block_fill_middle(struct blockData *block, float inVal, float outVal){
	
	if(block == NULL){
		error("block_fill_middle() was sent NULL blockData pointer. block = NULL");
		return 1;
	}
	
	int i, j;
	// index through every row and column
	for(i=0; i<BLOCK_WIDTH; i++){
		for(j=0; j<BLOCK_HEIGHT; j++){
			
			// if the value is in the inner 9th,
			if(i>=BLOCK_WIDTH_1_3 && i<BLOCK_WIDTH_2_3 && j>=BLOCK_WIDTH_1_3 && j<BLOCK_WIDTH_2_3){
				// fill it with inVal
				block->elevation[i][j] = inVal;
			}
			// if  it is in the outer 9th, 
			else{
				// fill with outVal
				block->elevation[i][j] = outVal;
			}
			
		}
	}
	
	// render the block next time it needs to be printed
	block->renderMe = 1;
	// success
	return 0;
}



/// This function makes a grid of 9 squares each with a different, but similar color to the color input
// block is a structure to the blockData struct used for printing
// color is the starting value for the color
// return 1 is block is NULL
// return 0 on success
short block_fill_nine_squares(struct blockData *block, int color) {
	// if the blockData stucture is null, send an error
	if(block == NULL){
		error("block_fill_nine_squares() was sent NULL blockData pointer. blockData = NULL");
		return 1;
	}
	// variables for indexing through the arrays
	// z variable is the color starting value which is then multiplied so that it changes
	int i = 0, j = 0, z = 100;
	// index through every row and column
	for(i = 0; i < BLOCK_WIDTH; i++) {
		for(j = 0; j < BLOCK_HEIGHT; j++) {
			// if the j variable is below 81 (square one, two, and threee) then print a color
			if(j < BLOCK_WIDTH_1_3)
				block->elevation[i][j] = color;
			// if the j variable is above 81 and below 162 (square four, five, and six) then print a color
			else if(j < BLOCK_WIDTH_2_3	&& j >= BLOCK_WIDTH_1_3)
				block->elevation[i][j] = color*2;
			// if the j variable is above 162 (square four, five, and six) then print a color
			else if(j >= BLOCK_WIDTH_2_3)
				block->elevation[i][j] = color*3;
		}
		// if the mod of i is 81 (1/3 of the screen) then change the variable and multiply it by two
		if(i%81 == 0 && i != 0)
			color*=2;
	}
	// render the block next time it needs to be printed
	block->renderMe = 1;
	// success
	return 0;
}



/// This function fills in a certain square with a color that are specified
// accepts the blockData structure so that it knows were to print too
// accepts nine different color arguments
// the arguments are mapped to the grid like so:
// 1 2 3
// 4 5 6
// 7 8 9
// note: prints black if it function didnt fucntion properly
// return 1 is block is NULL
// return 0 on success
short block_fill_nine_squares_own_color(struct blockData *block, int one, int two, int three, int four, int five, int six, int seven, int eight, int nine) {
	// if the blockData stucture is null, send an error
	if(block == NULL){
		error("block_fill_nine_squares() was sent NULL blockData pointer. blockData = NULL");
		return 1;
	}
	// variables for indexing through the arrays
	// column variable is for keeping track of which block it is in. it starts at the first column
	int i = 0, j = 0, column = 1;
	// index through every row and column
	for(i = 0; i < BLOCK_WIDTH; i++) {
		for(j = 0; j < BLOCK_HEIGHT; j++) {
			// if the j variable is below 81 (square one, two, and threee) then print a color
			if(j < BLOCK_WIDTH_1_3) {
				//switch between the different blocks in the row
				switch(column) {
					//first block
					case 1:
						//print color from argument provided
						block->elevation[i][j] = one;
						break;
					//second block
					case 2:
						//print color from argument provided
						block->elevation[i][j] = two;
						break;
					//third block
					case 3:
						//print color from argument provided
						block->elevation[i][j] = three;
						break;
					//debuging
					default:
						// What? how did you get here?
						// if it gets here, prints black
						block->elevation[i][j] = 0;
						break;
				}
			}
			// if the j variable is above 81 and below 162 (square four, five, and six) then print a color
			else if(j < BLOCK_WIDTH_2_3	&& j >= BLOCK_WIDTH_1_3) {
				//switch between the different blocks in the row
				switch(column) {
					// fourth block
					case 1:
						//print color from argument provided
						block->elevation[i][j] = four;
						break;
					// fifth block
					case 2:
						//print color from argument provided
						block->elevation[i][j] = five;
						break;
					// sixth block
					case 3:
						//print color from argument provided
						block->elevation[i][j] = six;
						break;
					//debuging
					default:
						// What? how did you get here?
						// if it gets here, prints black
						block->elevation[i][j] = 0;
						break;
				}
				
			}
			// if the j variable is above 162 (square seven, eight, and nine) then print the color
			else if(j >= BLOCK_WIDTH_2_3) {
				//switch between the different blocks in the row
				switch(column) {
					// seventh block
					case 1:
						//print color from argument provided
						block->elevation[i][j] = seven;
						break;
					// eighth block
					case 2:
						//print color from argument provided
						block->elevation[i][j] = eight;
						break;
					// ninth block
					case 3:
						//print color from argument provided
						block->elevation[i][j] = nine;
						break;
					//debuging
					default:
						// What? how did you get here?
						// if it gets here, prints black
						block->elevation[i][j] = 0;
						break;
				}
			}
		}
		// if the mod of i is 81 (1/3 of the screen)
		// and not equal to 0 since 0%81 is 0 which makes the first squares in the first block the wrong color
		if(i%81 == 0 && i != 0) {
			// then update the row so that it goes to the next one
			column++;
			// if the row is above 3, then reset it back to one
			if(column > 3)
				column = 1;
		}
	}
	
	// render the block next time it needs to be printed
	block->renderMe = 1;
	// success
	return 0;
}



/// this function will fill up the left half of the blockData with elevationLeft, and the right half of the blockData with elevationRight 
short block_fill_half_vert(struct blockData *block, float elevationLeft, float elevationRight){
	
	if(block == NULL){
		error("block_fill_half_vert() was sent NULL block pointer.");
		return 1;
	}
	
	int i,j;
	for(i=0; i<BLOCK_WIDTH; i++){
		for(j=0; j<BLOCK_HEIGHT; j++){
			if(i <BLOCK_WIDTH/2){
				block->elevation[i][j] = elevationLeft;
			}
			else{
				block->elevation[i][j] = elevationRight;
			}
		}
	}
	
	// render the block next time it needs to be printed
	block->renderMe = 1;
	return 0;
}





/// this function will create a new origin block in memory and add that origin block to the block list using block_collector.
// this function will...
	// set all elevation data to 0.
	// set all child pointers to NULL.
	// set parent pointer to NULL.
	// set parentView to BLOCK_CHILD_CENTER_CENTER (the parent block of the origin will be centered on the origin).
		// as a side note, the parent of the parent of the origin will be centered on the origin. And so on, and so forth, until the end of fractal space.
	// set all neighbors to NULL.
	// set level to 0 (origin level)
// returns a pointer to the origin on success
// returns NULL when allocation of memory fails
struct blockData *block_generate_origin(){
	
	// attempt to allocate memory for a new origin block
	struct blockData *newOrigin = malloc(sizeof(struct blockData));
	// if allocation failed, return NULL.
	if(newOrigin == NULL){
		error("block_generate_origin() was sent NULL newOrigin pointer.");
		return NULL;
	}
	
	// set all children to NULL;
	int c;
	for(c=0; c<BLOCK_CHILDREN; c++){
		newOrigin->children[c] = NULL;
	}
	
	// set all neighbors to NULL.
	for(c=0; c<BLOCK_NEIGHBORS; c++){
		newOrigin->neighbors[c] = NULL;
	}
	
	// set parent to NULL;
	newOrigin->parent = NULL;
	// set the texture to NULL
	newOrigin->texture = NULL;
	// render the new origin the next time through the graphics functions.
	newOrigin->renderMe = 1;
	
	// set the level to the default level.
	newOrigin->level = BLOCK_ORIGIN_LEVEL;
	
	// set parentView to BLOCK_CHILD_CENTER_CENTER.
	newOrigin->parentView = BLOCK_CHILD_CENTER_CENTER;
	
	// randomize the origin
	block_fill_random(newOrigin, BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);
	
	
	
	
	// add origin to the block index.
	block_collector(newOrigin, bc_collect);
	
	return newOrigin;
}



/// this function will generate a parent when given a pointer to a block that is to be taken as the child.
	/// NOTE this function is also very handy to use as a way to VALIDATE that a parent exists.
	/// (if the parent does exist, the function returns immediately. if the parent does not exist, the function will generate it)
	// when any parent is generated, all of its children are automatically generated as well.
	// the reason for this is the following: Every block will either have all of its children or none of its children.
	// this should simplify things considerably in the future.
// returns 0 on success
// returns 1 on NULL centerChild pointer.
// returns 2 when the parent already exists.
// returns 3 if the 
short block_generate_parent(struct blockData *centerChild){
	
	// check to see if a NULL pointer was passed.
	if(centerChild == NULL){
		error("block_generate_parent() was sent NULL pointer. centerChild = NULL.");
		return 1;
	}
	
	// check to see if the function is being asked to re-generate parent.
	if(centerChild->parent != NULL){
		// this function is used frequently as a parent validator (see comments at the top of the function). don't write an error to file.
		//error("block_generate_parent() was asked to regenerate parent.");
		return 2;
	}
	// otherwise, the parent needs to be generated
	else{
		
		// try to allocate memory for the parent block
		centerChild->parent = malloc(sizeof(struct blockData));
		
		// if the parent was not allocated properly, log an error and return 2
		if(centerChild->parent == NULL){
			error("block_generate_parent() cannot allocate data for a parent. centerChild->parent = NULL");
			return 3;
		}
		
		// because the parent was successfully added to memory, we will add it to the block collector's list.
		block_collector(centerChild->parent, bc_collect);
		
		// set elevation to default
		/*
		int i, j;
		for(i=0; i<BLOCK_WIDTH; i++){
			for(j=0; j<BLOCK_HEIGHT; j++){
				(centerChild->parent)->elevation[i][j] = BLOCK_DEFAULT_ELEVATION;
			}
		}
		*/
		block_fill_random(centerChild->parent, BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);
		
		// make all of the children NULL
		int c;
		for(c=0; c<BLOCK_CHILDREN; c++){
				(centerChild->parent)->children[c] = NULL;
		}
		// make the middle child of the parent point to the centerChild pointer
		(centerChild->parent)->children[BLOCK_CHILD_CENTER_CENTER] = centerChild;
		
		// create any children that have not been generated already.
		block_generate_children(centerChild->parent);
		
		// the level of the parent is one above the level of the child.
		(centerChild->parent)->level = centerChild->level + 1;
		
		// the parent has not been rendered yet.
		centerChild->parent->texture = NULL;
		// render the parent next time through the graphics functions.
		centerChild->parent->renderMe = 1;
		
		// this sets the parent of this block to NULL.
		(centerChild->parent)->parent = NULL;
		// because of how block generation is performed, when generating a parent, both the child AND the parent AND the parent's parent AND the parent's parent's parent (etc...) will be concentric.
		// so parentView for all NEW parents and the children that are generating those new parents will be BLOCK_CHILD_CENTER_CENTER.
		// This property of the block network is explained in some detail in block.h under "RYAN'S BLOCK NETWORK GENERATION PROTOCOL"
		(centerChild->parent)->parentView = BLOCK_CHILD_CENTER_CENTER;
		centerChild->parentView = BLOCK_CHILD_CENTER_CENTER;
		
	}
	
	// successfully generated a parent and verified all children exist or have been created.
	return 0;
}



/// creates all three children for the passed blockData, parent
// this function will allocate memory for all 9 children at once.
// returns 0 on success 
// returns 1 for a NULL parent pointer.
// returns 2+child for the first child that cannot be allocated in memory
short block_generate_children(struct blockData *datParent){
	
	if(datParent == NULL){
		error("block_generate_children() was sent NULL datParent pointer. datParent = NULL");
		return 1;
	}
	
	int c;	// this is the child of the parent
	int cc;	// this is the child of the child of the parent
	// allocate space for 9 children
	for(c=0; c<BLOCK_CHILDREN; c++){
		
		// only try to generate a child if the child doesn't already exist.
		if(datParent->children[c] == NULL){
			
			// attempt to allocate memory for the child block.
			datParent->children[c] = (struct blockData *) malloc(sizeof(struct blockData));
			
			// check to make sure child block was allocated incorrectly.
			if(datParent->children[c] == NULL){
				
				error_d("block_generate_children() could not allocate memory for children. malloc() returned NULL. child =",c);
				return 2 + c;
				
			}
			// if the child block was generated correctly,
			else{
				
				// check the child block that was just generated into the block list.
				// this ensures that we will be able to clean up all the allocated blocks when the program closes.
				block_collector(datParent->children[c], bc_collect);
				
				// this records the the parent blocks address
				(datParent->children[c])->parent = datParent;
				// record (in the child block) what child it is with respect to its parent.
				// Is it child_0? child_4 or child_5? This will record that data.
				(datParent->children[c])->parentView = c;
				// this sets all pointers to children for the current child to NULL.
				for(cc=0; cc<BLOCK_CHILDREN; cc++){
					(datParent->children[c])->children[cc] = NULL;
				}
				// the child has not been rendered yet
				(datParent->children[c])->texture = NULL;
				// render the child next time through the graphics functions.
				(datParent->children[c])->renderMe = 1;
				// the level of the child is the level of the parent minus 1.
				(datParent->children[c])->level = datParent->level - 1;
				
				// this is the child's default elevation data
				block_fill_random(datParent->children[c], BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER);
				
				
				
			}
		}
	}
	// successfully generated children
	return 0;
}



/// this function will calculate neighbor block(s) of the "dat" block sent to the function.
// this function IS ONLY GUARANTEED TO WORK FOR A 3x3 BLOCK CHILDREN LAYOUT.
// returns 0 on success.
// returns -1 if the neighbor is invalid
// returns 1 on NULL dat block pointer.
// returns 2 when it cannot allocate memory for the first stepLink.
// returns 3 when it cannot allocate memory for the second, third, fourth, (etc...) stepLink in the list.
// returns 4 if, when counting down, stepLink->prev is null BEFORE ascend = 0.
short block_generate_neighbor(struct blockData *dat, short neighbor){
	
	if(dat == NULL){
		error("block_generate_neighbor() was sent NULL blockData pointer. dat = NULL");
		return 1;
	}
	
	// a special case is defined for when the function is told to evaluate all four neighbors.
	if(neighbor == BLOCK_NEIGHBOR_ALL){
		// this catches the return values from each of the four neighbor evaluations.
		short retVal = 0;
		
		// generate all four neighbors
		retVal += block_generate_neighbor(dat, BLOCK_NEIGHBOR_UP);
		retVal += block_generate_neighbor(dat, BLOCK_NEIGHBOR_DOWN);
		retVal += block_generate_neighbor(dat, BLOCK_NEIGHBOR_LEFT);
		retVal += block_generate_neighbor(dat, BLOCK_NEIGHBOR_RIGHT);
		
		// check to see if any of them returned any error conditions.
		// ideally, they should all be 0's.
		if(retVal){
			error_d("block_generate_neighbor() was told to generate all four neighbors and one or more of them returned a nonzero value. retVal =",retVal);
		}
		
		// return the sum of the return values from the four generations
		return retVal;
	}
	
	//--------------------------------------------------
	// begin the main part of the code.
	//--------------------------------------------------
	// most of the code is the same for generating neighbors in the up, down, left, and right directions (relative to "dat" block).
	// there are just occasional if statements that handle minor numerical differences. 
	// For instance, one direction may require a -3 be added to all numbers.
	// Another direction may require that all numbers must be reduced by their modulus by three.
	
	// this is a pointer than we will use to navigate the block matrix.
	// it will be used as a sort of probe.
	// it starts out pointing to the original block that is sent.
	struct blockData *probe = dat;
	// this keeps track of how many ascensions from child to parent were made (and therefore, how many should be made from parent back down to child)
	// this tells you what level you are currently on relative to the original block (dat)
	int ascend = 0;
	
	// allocate a chunk of block steps.
	struct blockStep *stepLink = malloc(sizeof(struct blockStep));
	// verify that stepLink was allocated properly.
	if(stepLink == NULL){
		error("block_generate_neighbor() could not allocate memory for stepLink. stepLink = NULL");
		return 2;
	}
	// this indicates that there is no stepLink before this one. This is the beginning of the linked list of blockSteps.
	stepLink->prev = NULL;
	
	// this variable is used to calculate what the last step was. (this is a number that indicates the relationship between the current block (probe) and the previous probe (one of current probe's children)
	char lastStep;
	
	
	
	
	// as long as this is 1, the loop keeps zooming out.
	int keepZoomingOut = 1;
	while(keepZoomingOut){
		// record the child's relation to the parent so that we can know how to get back down to it when we reach the highest necessary parent.
		lastStep = stepLink->steps[ascend%BLOCK_STEP_SIZE] = probe->parentView;
		// make sure the parent is generated first
		block_generate_parent(probe);
		// move up a level to the next parent
		probe = probe->parent;
		
		// increment the counter
		ascend++;
		
		// if we just filled up the current stepLink, then we need to allocate memory for the next
		if(ascend%BLOCK_STEP_SIZE == 0 && ascend != 0){
			// allocate more memory to hold more steps
			stepLink->next = malloc(sizeof(struct blockStep));
			// check if the memory allocated incorrectly.
			if(stepLink->next == NULL){
				// if the memory did not allocate right, report an error.
				error_d("block_generate_neighbor() could not allocate memory for new stepLink->next. stepLink = NULL. ascend =",ascend);
				// and return an error.
				return 3;
			}
			// otherwise, we will assume the memory was allocated correctly.
			// record that the previous stepLink (from the next stepLink's point of view) is just the current stepLink.
			(stepLink->next)->prev = stepLink;
			// switch to the next stepLink
			stepLink = stepLink->next;
		}
		
		// check to see if the last step taken will allow us to zoom into a block that is above target.
		// if we can start zooming back in now, exit the loop
		// this handles all four directions
		switch(neighbor){
		case BLOCK_NEIGHBOR_UP:
			if(lastStep  >= 3) keepZoomingOut = 0;
			break;
		case BLOCK_NEIGHBOR_DOWN:
			if(lastStep  <  6) keepZoomingOut = 0;
			break;
		case BLOCK_NEIGHBOR_LEFT:
			if(lastStep%3 > 0) keepZoomingOut = 0;
			break;
		case BLOCK_NEIGHBOR_RIGHT:
			if(lastStep%3 < 2) keepZoomingOut = 0;
			break;
		default:
			error("block_generate_neighbor() neighbor was something other than expected... neighbor =");
			return -1;	// something seriously fucked up has just happened
			break;
		}
	}
	
	// we are now at a block that has a parent that contains the neighbor of the original block
	// we may now zoom to the neighbor of the original block.
	
	// decrement ascend once at the beginning (to get back to the last step that was taken)
	ascend--;
	// check to see if you need to step down to the previous stepLink
	if(ascend%BLOCK_STEP_SIZE == BLOCK_STEP_SIZE-1){
		// check to make sure the previous stepLink is valid.
		// this should never EVER be
		if(stepLink->prev != NULL){
			// This shouldn't happen because ascend should run to 0, and then exit before any stepLink->prev can be NULL. Nevertheless, it has somehow happened. 
			error_d("block_generate_neighbor(): somehow has a NULL stepLink->prev. THIS SHOULD NEVER HAPPEN. ascend =",ascend);
			return 4;
		}
		// if the previous stepLink is valid (as it should be) then switch to it.
		stepLink = stepLink->prev;
		// free the "next" step link (the one that we just came from).
		// This is like closing the door on the way out of a building.
		// If you enter a house, then the bathroom, you close the bathroom door on your way out of the bathroom, then you close the house door on your way out of the house.
		// everything is reverse on the way out.
		if(stepLink->next != NULL) free(stepLink->next);
	}
	// zoom in once from the highest-level parent to child of that highest-level parent.
	// the first step of descending is a special case. all the steps after this one are the same.
	// this handles all four directions
	switch(neighbor){
	case BLOCK_NEIGHBOR_UP:
		probe = probe->children[stepLink->steps[ascend%BLOCK_STEP_SIZE]-3];
		break;
	case BLOCK_NEIGHBOR_DOWN:
		probe = probe->children[stepLink->steps[ascend%BLOCK_STEP_SIZE]+3];
		break;
	case BLOCK_NEIGHBOR_LEFT:
		probe = probe->children[stepLink->steps[ascend%BLOCK_STEP_SIZE]-1];
		break;
	case BLOCK_NEIGHBOR_RIGHT:
		probe = probe->children[stepLink->steps[ascend%BLOCK_STEP_SIZE]+1];
		break;
	default:
		error("block_generate_neighbor(): neighbor was something other than expected... neighbor =");
		return -1;	// something seriously fucked up has just happened
		break;
	}
	
	// go all the way back down to the upward neighbor of the original block.
	for(ascend--;ascend>=0; ascend--){
		
		// check to see if you need to switch to the previous stepLink
		if(ascend%BLOCK_STEP_SIZE == BLOCK_STEP_SIZE-1){
			// check to make sure the previous stepLink is valid.
			// this should never EVER be
			if(stepLink->prev != NULL){
				// This shouldn't happen because ascend should run to 0, and then exit before any stepLink->prev can be NULL. Nevertheless, it has somehow happened. 
				error_d("block_generate_neighbor() somehow has a NULL stepLink->prev. THIS SHOULD NEVER HAPPEN. ascend =",ascend);
				return 4;
			}
			// if the previous stepLink is valid (as it should be) then switch to it.
			stepLink = stepLink->prev;
			// free the "next" step link (the one that we just came from).
			// This is like closing the door on the way out of a building.
			// If you enter a house, then the bathroom, you close the bathroom door on your way out of the bathroom, then you close the house door on your way out of the house.
			// everything is reverse on the way out.
			if(stepLink->next != NULL) free(stepLink->next);
		}
		
		// make sure all children of the current probe block exist
		block_generate_children(probe);
		// then move to the right child
		switch(neighbor){
		case BLOCK_NEIGHBOR_UP:
			probe = probe->children[stepLink->steps[ascend] + 6];
			break;
		case BLOCK_NEIGHBOR_DOWN:
			probe = probe->children[stepLink->steps[ascend] - 6];
			break;
		case BLOCK_NEIGHBOR_LEFT:
			probe = probe->children[stepLink->steps[ascend] + 2];
			break;
		case BLOCK_NEIGHBOR_RIGHT:
			probe = probe->children[stepLink->steps[ascend] - 2];
			break;
		default:
			error("block_generate_neighbor(): neighbor was something other than expected... neighbor =");
			return -1;	// something seriously fucked up has just happened
			break;
		}
	}
	
	// store the pointer to the right block in the neighbor pointer array of the block that we initially wanted to know the upwards neighbor of.
	switch(neighbor){
	case BLOCK_NEIGHBOR_UP:
		dat->neighbors[BLOCK_NEIGHBOR_UP] = probe;
		break;
	case BLOCK_NEIGHBOR_DOWN:
		dat->neighbors[BLOCK_NEIGHBOR_DOWN] = probe;
		break;
	case BLOCK_NEIGHBOR_LEFT:
		dat->neighbors[BLOCK_NEIGHBOR_LEFT] = probe;
		break;
	case BLOCK_NEIGHBOR_RIGHT:
		dat->neighbors[BLOCK_NEIGHBOR_RIGHT] = probe;
		break;
	default:
		error("block_generate_neighbor(): neighbor was something other than expected... neighbor =");
		return -1;	// something seriously fucked up has just happened
	}
	// our work here is done. whew!
	
	
	
	// successfully generated/verified all neighbors.
	return 0;
}





/// this creates a list of all of the map blocks that have been created during program run time.
/// this function will record every new map block that is generated.
/// before the program closes, this function will need to be called to clean up all of these blocks.
// if operation = bc_collect, then it will collect.
// returns 0 on success.
// returns 1 if sent a null block pointer.
// returns 2 if invalid operation was sent.
// returns 3 if the function could not create the first link in the list.
// returns 4 if it could not create a link in the list OTHER than the first.
// returns 5 if it was asked to bc_clean_up but there was nothing to clean up.
// returns 6 if it was asked to clean up and it reached a null previous pointer before blockCount reached 0.
short block_collector(struct blockData *source, short operation){
	
	// this tells us how many blocks are allocated in memory
	static long long int blockCount = 0;
	// this points to the 
	static struct blockLink *currentLink = NULL;
	// used to temporarily store the link pointer
	struct blockLink *tempLink = NULL;
	
	//check to see what the program was called to do.
	// if the operation is bc_collect
	if(operation == bc_collect){
		
		// check for source pointer being NULL.
		if(source == NULL){
			error("block_collector() could not collect source. source = NULL.");
			return 1;
		}
		// if source is valid,
		else{
			
			// check if you need to create a new link in the list.
			if(blockCount%BLOCK_LINK_SIZE == 0){
				
				// if this is the first block added to the list or if the currentLink pointer is invalid,
				if(blockCount == 0 || currentLink == NULL){
					// create the first link
					currentLink = malloc(sizeof(struct blockLink));
					
					// check for pointer being NULL.
					if(currentLink == NULL){
						error("block_collector() could not create first link in the list. currentLink = NULL.");
						return 3;
					}
					
					// make sure the previous pointer points to NULL.
					currentLink->prev = NULL;
				}
				// otherwise, if this is not the first block,
				else{
					// add a new link in the list
					currentLink->next = malloc(sizeof(struct blockLink));
					
					// check for pointer being NULL.
					if(currentLink->next == NULL){
						error_d("block_collector() could not create link in the list. currentLink-> = NULL. blockCount = ", blockCount);
						return 4;
					}
					
					// make sure the next link points back at the current link.
					(currentLink->next)->prev = currentLink;
					// ascend to the next link in the list.
					currentLink = currentLink->next;
				}
				
			}
			
			// add the block to the list
			currentLink->blocks[blockCount%BLOCK_LINK_SIZE] = source;
			// increment the block counter.
			blockCount++;
			
		}
		
	}
	else if(operation == bc_clean_up){
		
		// check to see if either no blocks have been written or the currentLink was lost.
		if(currentLink == NULL || blockCount == 0){
			// set both variables to the default state
			currentLink = NULL;
			blockCount = 0;
			// report an error
			error("block_collector() asked to clean up. Nothing to clean up. This is not necessarily an error. It could be an error or just a warning.");
			// and return an error
			return 5;
		}
		
		gamelog_d("block_collector() cleaning up blocks. blockCount =", blockCount);
		
		// decrement to the last valid block
		blockCount--;
		
		// loop through all blocks in all links of the list.
		// free all blocks and free all of the links in the list.
		for(; blockCount >= 0; blockCount--){
			
			// if the current block is valid,
			if(currentLink->blocks[blockCount%BLOCK_LINK_SIZE] != NULL){
				
				// if the just got to the end of a link,
				if(blockCount%BLOCK_LINK_SIZE == BLOCK_LINK_SIZE - 1){
					
					// if the previous link is invalid,
					if(currentLink->prev == NULL){
						error_d("block_collector() has NULL previous link pointer before the end of the list. for() loop terminated with un-erased blocks: blockCount =", blockCount);
						blockCount = 0;
						currentLink = NULL;
						return 6;
					}
					
					// temporarily store the pointer to the currentLink
					tempLink = currentLink;
					// shift to the previous link in the list.
					currentLink = currentLink->prev;
					// erase free the memory tempLink points to
					if(tempLink != NULL) free(tempLink);
				}
				// erase the memory that holds the block.
				free(currentLink->blocks[blockCount%BLOCK_LINK_SIZE]);
			}
		}
		
		// free current link. now everything in the list has been erased (both blocks and links).
		if(currentLink != NULL) free(currentLink);
		
	}
	// check for invalid operation
	else{
		error_d("block_collector() received invalid operation. operation =", operation);
		return 2;
	}
	
	// all went smoothly. not errors or problems.
	return 0;
}

