#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"
#include <string.h>
#include "rand.h"
#include "graphics.h"

/// print a blockData
// returns 1 on successful print to file
// this function is to print the data from a map in a readable format.
// THIS IS NOT FOR SAVING MAP DATA
short block_print_to_file(struct blockData *datBlock, char *fileName){
	
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
	
	if(datBlock == NULL){
		error("block_random_fill() could not fill datBlock. datBlock = NULL.");
		return 0;
	}
	
	
	// write all elevation data
	int j, i;
	for(j=0; j<BLOCK_HEIGHT; j++){
		for(i=0; i<BLOCK_WIDTH; i++){
			fprintf(fp, "%10.10f\t", datBlock->elevation[i][j]);
		}
		// new line
		fprintf(fp, "\n");
	}
	
	// wrote datBlock to file successfully.
	return 1;
}




/// throws random data into blockData
// returns 0 on success
// returns 1 when the datBlock is a NULL pointer.
short block_random_fill(struct blockData *datBlock, float range_low, float range_high){
	
	// check for datBlock pointer being NULL.
	if(datBlock == NULL){
		error("block_random_fill() could not fill datBlock. datBlock = NULL.");
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
			datBlock->elevation[i][j] = ((genrand()%100001)/100000.0)*(range_high-range_low) + range_low;//((rand()%100001)/100000.0)*(range_higher-range_lower)-((range_higher+range_lower)/2);
		}
	}
	
	// generated random data in datBlock successfully.
	return 0;
}




// returns 0 on success
// returns -1 when datBlock == NULL
short block_create_children(struct blockData *source){
	
	// check for source pointer being NULL.
	if(source == NULL){
		error("block_create_children() could not create children. source = NULL.");
		return -1;
	}
	
	int child;
	for(child=0; child<MAP_BLOCK_CHILDREN; child++){
		
		// allocate space for the child in memory.
		source->children[child] = ((struct blockData *)malloc(sizeof(struct blockData)));
		
		// check for NULL pointer (make sure the child was not stillborn)
		if(source->children[child] == NULL){
			error("block_create_children() could not create child. malloc returned NULL pointer. Out of memory?");
			return child;
		}
		// if the child pointer was created correctly
		else{
			// make the child point to its parent.
			(source->children[child])->parent = source;
			// record the block in the block collection index.
			block_collector(source->children[child], bc_collect);
		}
	}
	// generated children successfully.
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
	
	static long long int blockCount = 0;
	static struct blockLink *currentLink = NULL;
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
		
		// decrement to the last valid block
		blockCount--;
		
		// loop through all blocks in all links of the list.
		// free all blocks and free all of the links in the list.
		for(; blockCount >= 0; blockCount--){
			
			// if the current block is valid,
			if(currentLink->blocks[blockCount] != NULL){
				
				// if the just got to the end of a link,
				if(blockCount%BLOCK_LINK_SIZE == BLOCK_LINK_SIZE - 1){
					
					// if the previous link is invalid,
					if(currentLink->prev == NULL){
						error_d("block_collector() has NULL previous link pointer before the end of the list. for() loop terminated with un-erased blocks: blockCount = ", blockCount);
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
		error_d("block_collector() received invalid operation. operation = ", operation);
		return 2;
	}
	
	// all went smoothly. not errors or problems.
	return 0;
}




/// this will print an image of a mapblock to a surface BLOCK_WIDTH x BLOCK_HEIGHT pixels
// returns 0 on success
// returns 1 on NULL dest surface
// returns 2 on NULL datBlock
short map_print(SDL_Surface *dest, struct blockData *source){
	
	if(dest == NULL){
		error("block_collector() could not print to dest. dest = NULL.");
		return 1;
	}
	if(source == NULL){
		error("block_collector() could not print source. source = NULL.");
		return 2;
	}
	
	int i, j;
	for(i=0; i<BLOCK_WIDTH; i++){
		for(j=0; j<BLOCK_HEIGHT; j++){
			set_pixel(dest, i, j, 0xff000000|((int)(source->elevation[i][j])));
		}
	}
	
	return 0;
}


/// this function will smooth out a block (just the block it will not smooth with respect to its adjacent blocks).
// smoothFactor is from 0 to 1. it describes how much averaging the function will perform.
// smoothFactor = 1 => the smoothing will replace each element with the average of those around it.
// smoothFactor = 0.5 => the smoothing will replace each elevation with the average of itself and the aaverage of those around it.
short block_smooth(struct blockData *source, float smoothFactor){
	int i, j;
	for(i=0; i<BLOCK_WIDTH; i++){
		for(j=0; j<BLOCK_HEIGHT; j++){
			source->elevation[i][j] = (1.0-smoothFactor)*source->elevation[i][j] + smoothFactor*block_surrounding_average(source, i, j);
		}
	}
	return 0;
}


// this calculates the average of the elevation values around some index (x,y) into the block data.
// returns average in floating point number form
// returns a floating point value of 0.0 if there is any problem computing the error.
float block_surrounding_average(struct blockData *source, unsigned int x, unsigned int y){
	
	if(source == NULL){
		error("block_surrounding_average() could not open source. source = NULL.");
		return 0.0;
	}
	
	float average = 0.0;
	int averageCount = 0;
	
	// add diagonal elevations to average
	if(x>0 && y>0)							{average += source->elevation[x-1][y-1];	averageCount++;}
	if(x>0 && y<BLOCK_HEIGHT-1)				{average += source->elevation[x-1][y+1];	averageCount++;}
	if(x<BLOCK_WIDTH-1 && y>0)				{average += source->elevation[x+1][y-1];	averageCount++;}
	if(x<BLOCK_WIDTH-1 && y<BLOCK_HEIGHT-1)	{average += source->elevation[x+1][y+1];	averageCount++;}
	
	// add adjacent elevations
	if(x>0)					{average += source->elevation[x-1][y];	averageCount++; }
	if(x<BLOCK_WIDTH-1)		{average += source->elevation[x+1][y];	averageCount++; }
	if(y>0)					{average += source->elevation[x][y-1];	averageCount++; }
	if(y<BLOCK_HEIGHT-1)	{average += source->elevation[x][y+1];	averageCount++; }
	
	if(averageCount < 1){
		error_d("block_surrounding_average() had invalid averageCount! averageCount = ", averageCount);
		return 0.0;
	}
	
	// if everything went well, return the average of the surrounding elevations
	return average/((float)averageCount);
}


/// this function will fill up the middle 9th of the block (from 1/3 to 2/3 of the block in both width and height)
// the outer ring of 8/9ths of the will be filled with outVal
// the inner 9th is filled with inVal.
// returns 1 if dat was NULL.
int block_fill_middle(struct blockData *dat, float inVal, float outVal){
	
	if(dat == NULL){
		error("block_fill_middle() was sent NULL blockData pointer. dat = NULL");
		return 1;
	}
	
	int i, j;
	// index through every row and column
	for(i=0; i<BLOCK_WIDTH; i++){
		for(j=0; j<BLOCK_HEIGHT; j++){
			
			// if the value is in the inner 9th,
			if(i>=BLOCK_WIDTH_1_3 && i<BLOCK_WIDTH_2_3 && j>=BLOCK_WIDTH_1_3 && j<BLOCK_WIDTH_2_3){
				// fill it with inVal
				dat->elevation[i][j] = inVal;
			}
			// if  it is in the outer 9th, 
			else{
				// fill with outVal
				dat->elevation[i][j] = outVal;
			}
			
		}
	}
	// success
	return 0;
}


/// This function makes a grid of 9 squares each with a different, but similar color to the color input
// Block is a structure to the blockData struct used for printing
// color is the starting value for the color
// return 1 is Block is NULL
// return 0 on success
short block_fill_nine_squares(struct blockData *Block, int color) {
	// if the blockData stucture is null, send an error
	if(Block == NULL){
		error("block_fill_nine_squares() was sent NULL blockData pointer. blockData = NULL");
		return 1;
	}
	// variables for indexing through the arrays
	// z variable is the color starting value which is then multiplied so that it changes
	int i = 0, j = 0, z = 100;
	// index through every row and column
	for(i = 0; i < BLOCK_WIDTH; i++) {
		for(j = 0; j < BLOCK_HEIGHT; j++) {
			// if the j variable is below 81 (square one, four, and seven) then print a color
			if(j < BLOCK_WIDTH_1_3)
				Block->elevation[i][j] = color;
			// if the j variable is above 81 and below 162 (square two, five, and eight) then print a color
			else if(j < BLOCK_WIDTH_2_3	&& j >= BLOCK_WIDTH_1_3)
				Block->elevation[i][j] = color*2;
			// if the j variable is above 162 (square three, six, and nine) then print the color
			else if(j >= BLOCK_WIDTH_2_3)
				Block->elevation[i][j] = color*3;
		}
		// if the mod of i is 81 (1/3 of the screen) then change the variable and multiply it by two
		if(i%81 == 0 && i != 0)
			color*=2;
	}
	// success
	return 0;
}
