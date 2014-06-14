#include "block.h"
#include "camera.h"
#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"
#include <string.h>
#include "rand.h"
#include "graphics.h"


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





/// this function will generate a parent when given a pointer to a block that is to be taken as the child.
	// when any parent is generated, all of its children are automatically generated as well.
	// the reason for this is the following: Every block will either have all of its children or none of its children.
	// this should simplify things considerably in the future.
// returns 0 on success
// returns 1 on NULL oneChild pointer.
// returns 2 when the parent already exists.
// returns 3 if the 
short block_generate_parent(struct blockData *oneChild){
	
	// check to see if a NULL pointer was passed.
	if(oneChild == NULL){
		error("block_generate_parent() was sent NULL pointer. oneChild = NULL.");
		return 1;
	}
	
	// check to see if the function is being asked to re-generate parent.
	if(oneChild->parent != NULL){
		error("block_generate_parent() was asked to regenerate parent. Child verification will still occur.");
		block_generate_children((oneChild->parent));
		return 2;
	}
	// otherwise, the parent needs to be generated
	else{
		
		// try to allocate memory for the parent block
		oneChild->parent = malloc(sizeof(struct blockData));
		
		// if the parent was not allocated properly, log an error and return 2
		if(oneChild->parent == NULL){
			error("block_generate_parent() cannot allocate data for a parent. oneChild->parent = NULL");
			return 3;
		}
		
		// set elevation to default
		int i, j;
		for(i=0; i<BLOCK_WIDTH; i++){
			for(j=0; j<BLOCK_HEIGHT; j++){
				(oneChild->parent)->elevation[i][j] = BLOCK_DEFAULT_ELEVATION;
			}
		}
		// the level of the parent is one above the level of the child.
		(oneChild->parent)->level = oneChild->level + 1;
		
		// this sets the parent of this block to NULL.
		(oneChild->parent)->parent = NULL;
		// because of how block generation is performed, when generating a parent, both the child AND the parent AND the parent's parent AND the parent's parent's parent (etc...) will be concentric.
		// so parentView for all NEW parents and the children that are generating those new parents will be BLOCK_CHILD_CENTER_CENTER.
		// This property of the block network is explained in some detail in block.h under "RYAN'S BLOCK NETWORK GENERATION PROTOCOL"
		(oneChild->parent)->parentView = BLOCK_CHILD_CENTER_CENTER;
		oneChild->parentView = BLOCK_CHILD_CENTER_CENTER;
	}
	
	// successfully generated a parent and verified all children exist or have been created.
	return 0;
}




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



short block_print_network_hierarchy(SDL_Surface *dest, struct blockData *focus, float zoom){
	
	
	
	
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
		error_d("block_surrounding_average() had invalid averageCount! averageCount =", averageCount);
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
			// if the j variable is below 81 (square one, two, and threee) then print a color
			if(j < BLOCK_WIDTH_1_3)
				Block->elevation[i][j] = color;
			// if the j variable is above 81 and below 162 (square four, five, and six) then print a color
			else if(j < BLOCK_WIDTH_2_3	&& j >= BLOCK_WIDTH_1_3)
				Block->elevation[i][j] = color*2;
			// if the j variable is above 162 (square four, five, and six) then print a color
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



/// This function fills in a certain square with a color that are specified
// accepts the blockData structure so that it knows were to print too
// accepts nine different color arguments
// the arguments are mapped to the grid like so:
// 1 2 3
// 4 5 6
// 7 8 9
// note: prints black if it function didnt fucntion properly
// return 1 is Block is NULL
// return 0 on success
short block_fill_nine_squares_own_color(struct blockData *Block, int one, int two, int three, int four, int five, int six, int seven, int eight, int nine) {
	// if the blockData stucture is null, send an error
	if(Block == NULL){
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
						Block->elevation[i][j] = one;
						break;
					//second block
					case 2:
						//print color from argument provided
						Block->elevation[i][j] = two;
						break;
					//third block
					case 3:
						//print color from argument provided
						Block->elevation[i][j] = three;
						break;
					//debuging
					default:
						// What? how did you get here?
						// if it gets here, prints black
						Block->elevation[i][j] = 0;
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
						Block->elevation[i][j] = four;
						break;
					// fifth block
					case 2:
						//print color from argument provided
						Block->elevation[i][j] = five;
						break;
					// sixth block
					case 3:
						//print color from argument provided
						Block->elevation[i][j] = six;
						break;
					//debuging
					default:
						// What? how did you get here?
						// if it gets here, prints black
						Block->elevation[i][j] = 0;
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
						Block->elevation[i][j] = seven;
						break;
					// eighth block
					case 2:
						//print color from argument provided
						Block->elevation[i][j] = eight;
						break;
					// ninth block
					case 3:
						//print color from argument provided
						Block->elevation[i][j] = nine;
						break;
					//debuging
					default:
						// What? how did you get here?
						// if it gets here, prints black
						Block->elevation[i][j] = 0;
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
	
	struct blockData *newOrigin = malloc(sizeof(struct blockData));
	
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
	// set parentView to BLOCK_CHILD_CENTER_CENTER.
	newOrigin->parentView = BLOCK_CHILD_CENTER_CENTER;
	
	// make all elevations equal to the default elevation.
	int i, j;
	for(i=0; i<BLOCK_WIDTH; i++){
		for(j=0; j<BLOCK_HEIGHT; j++){
			newOrigin->elevation[i][j] = BLOCK_DEFAULT_ELEVATION;
		}
	}
	
	// set the level to the default level.
	newOrigin->level = BLOCK_ORIGIN_LEVEL;
	
	// add origin to the block index.
	block_collector(newOrigin, bc_collect);
	
	return newOrigin;
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
				// the level of the child is the level of the parent minus 1.
				(datParent->children[c])->level = datParent->level - 1;
				
				// calculate the child's neighbors.
				block_generate_neighbor(datParent->children[c], BLOCK_NEIGHBOR_ALL);
			
			}
		}
	}
	// successfully generated children
	return 0;
}



/// this function will calculate neighbor block(s) of the "dat" block sent to the function.
// this function IS ONLY GUARANTEED TO WORK FOR A 3x3 BLOCK CHILDREN LAYOUT.
// returns 0 on success.
// returns 1 on NULL dat block pointer.
// returns 2 when it cannot allocate memory for the first stepLink.
// returns 3 when it cannot allocate memory for the second, third, fourth, (etc...) stepLink in the list.
// returns 4 if, when counting down, stepLink->prev is null BEFORE ascend = 0.
short block_generate_neighbor(struct blockData *dat, short neighbor){
	
	if(dat == NULL){
		error("block_generate_neighbor() was sent NULL blockData pointer. dat = NULL");
		return 1;
	}
	
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
	
	
	// the way this checks if the parent has its neighbors is through
	switch(neighbor){
		
		case BLOCK_NEIGHBOR_UP:
			
			// exiting the loop is done inside the loop
			while(1){
				// record the child's relation to the parent so that we can know how to get back down to it when we reach the highest nevessary parent.
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
				if(lastStep >= 3){
					break;	// break out of the while(1) loop.
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
			// zoom in once from the highest-level parent to child of that highest-level parent.
			// the first step of descending is a special case. all the steps after this one are the same.
			probe = probe->children[stepLink->steps[ascend%BLOCK_STEP_SIZE]-3];
			
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
				probe = probe->children[stepLink->steps[ascend] + 6];
			}
			
			// store the pointer to the right block in the neighbor pointer array of the block that we initially wanted to know the upwards neighbor of.
			dat->neighbors[BLOCK_NEIGHBOR_UP] = probe;
			// our work here is done. whew!
			break;
		
		case BLOCK_NEIGHBOR_DOWN:
			
			/// TODO: write find neighbor down
			
			break;
		
		case BLOCK_NEIGHBOR_LEFT:
			
			/// TODO: write find neighbor left
			
			break;
		
		case BLOCK_NEIGHBOR_RIGHT:
			
			/// TODO: write find neighbor right
			
			break;
		
		case BLOCK_NEIGHBOR_ALL:
		default:
			
			// calculate all four neighbors.
			block_generate_neighbor(dat, BLOCK_NEIGHBOR_UP);
			block_generate_neighbor(dat, BLOCK_NEIGHBOR_DOWN);
			block_generate_neighbor(dat, BLOCK_NEIGHBOR_LEFT);
			block_generate_neighbor(dat, BLOCK_NEIGHBOR_RIGHT);
			
			break;
		
	}
	
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

