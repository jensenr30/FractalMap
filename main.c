#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"
#include "graphics.h"
#include "globals.h"
#include "graphics.h"
#include "block.h"
#include "rand.h"
#include "filter.h"
#include <time.h>

unsigned int windW = BLOCK_WIDTH*3;
unsigned int windH = BLOCK_HEIGHT*3;


int main(int argc, char *argv[]){
	error("\n\n\n\n== PROGRAM START ======================================================\n\n\n\n");
	//--------------------------------------------------
	// initial gamelog write
	//--------------------------------------------------
	gamelog("\n\n\n\n== PROGRAM START ======================================================\n\n\n\n");
	gamelog_d("main() was sent argc =", argc);
	int arg;
	gamelog("START ARGV ARGUMENT LIST:");
	// print all arguments
	for(arg=0; arg<argc; arg++){
		gamelog(argv[arg]);
	}
	gamelog("END ARGV LIST");
	
	
	//--------------------------------------------------
	// set up surfaces, textures, renderers, windows,
	//--------------------------------------------------
	// set all surfaces, textures, renderers, and windows to NULL initially to satify if statements that free memory that isn't needed any more.
	// this prevents the game from crashing at startup.
	SDL_Surface *mapSurface = NULL;
	SDL_Texture *mapTexture = NULL;
	myWindow = NULL;
	myRenderer = NULL;
	myTexture = NULL;
	
	
	sgenrand(time(NULL));
	
	
	if(SDL_Init(SDL_INIT_EVERYTHING) == -1) return -99;
	
	myWindow = SDL_CreateWindow("My Game Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windW, windH, SDL_WINDOW_RESIZABLE);
	myRenderer = SDL_CreateRenderer(myWindow, -1, 0);
	
	if(myWindow == NULL){
		error("main() could not create window using SDL_CreateWindow");
		return -1;
	}
	if(myRenderer == NULL){
		error("main() could not create renderer using SDL_CreateRenderer");
		return -2;
	}
	
	
	
	SDL_SetRenderDrawColor(myRenderer, 0, 0, 0, 255);
	SDL_RenderClear(myRenderer);
	SDL_RenderPresent(myRenderer);
	
	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear"); // make the scaled rendering look smoother
	//SDL_RenderSetLogicalSize(myRenderer, windW, windH);
	
	myTexture = SDL_CreateTexture(myRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, windW, windH);
	
	//SDL_Texture *glider = load_image_to_texture("glider.jpg");
	
	if(myTexture == NULL){
		error("main() could not create texture using SDL_CreateTexture");
		return -2;
	}
	
	if(myTexture == NULL){
		error("main() could not create glider texture using SDL_CreateTexture");
		return -2;
	}
	
	
	// origin block.
	struct blockData *origin = block_create_origin();
	
	//--------------------------------------------------
	// event handling
	//--------------------------------------------------
	
	// this records if the user wants to quit
	byte quit = 0;
	const int keysSize = 256;
	// this array keeps track of what variables were JUST pushed.
	// A value of 1 will be set to keys that were just stroked.
	// after an iteration through the loop, all keys are reset to 0.
	byte keys[keysSize];
	int i;
	
	while(quit == 0){
		
		// reset all keystroke values
		for(i=0; i<keysSize; i++){
			keys[i] = 0;
		}
		
		while(SDL_PollEvent(&event)){
			// if there is a mouse button down event,
			if(event.type == SDL_MOUSEBUTTONDOWN){
				//down++;
			}
			else if(event.type == SDL_MOUSEBUTTONUP){
				//down--;
			}
			else if(event.type == SDL_KEYDOWN){
				// check if the key is greater than the 'a' character.
				if(event.key.keysym.sym >= 0)
					// set that character, number, or letter to 1.
					keys[(event.key.keysym.sym)%keysSize] = 1;
			}
			else if(event.type == SDL_QUIT){
				quit = 1;
			}
		}
		// if the user pressed the c key
		if(keys['c']){
			// generate children blocks
			block_generate_children(origin);
		}
		
		// generate parent of origin if the p key is pressed
		if(keys['p']){
			block_generate_parent(origin);
		}
		
		// if either the s or g keys were just stroked.
		if(keys['s'] || keys['g']){
			// generate new map in origin if the g key was pressed
			if(keys['g']){
				//block_random_fill(origin, 0, 0xff);
				//block_fill_middle(origin, 0xff, 0x00);
				//block_fill_nine_squares(origin, 100);
				block_fill_nine_squares_own_color(origin, 10000, 20000, 50000, 20000, 10000, 20000, 50000, 20000, 10000);
			}
			
			//block_print_to_file(origin, "origin.txt");
			
			// smooth the block elevation data
			//block_smooth(origin, 0.5);
			if(keys['s']) filter_lowpass_2D_f((float *)origin->elevation, NULL, BLOCK_WIDTH, BLOCK_HEIGHT, 2);
			
			//clear old surface
			if(mapSurface != NULL)SDL_FreeSurface(mapSurface);
			// generate image of map
			mapSurface = create_surface(BLOCK_WIDTH, BLOCK_HEIGHT);
			// print map to mapSurface
			map_print(mapSurface, origin);
			
			// clear the old texture if it exists
			if(mapTexture != NULL)SDL_DestroyTexture(mapTexture);
			// create a texture for the map data
			mapTexture = SDL_CreateTextureFromSurface(myRenderer, mapSurface);
		}
		// render the mapTexture to the window
		SDL_RenderCopy(myRenderer, mapTexture, NULL, NULL);
		
		// display the renderer's result on the screen
		SDL_RenderPresent(myRenderer);
		SDL_RenderClear(myRenderer);
	}
	
	//--------------------------------------------------
	// clean up
	//--------------------------------------------------
	SDL_FreeSurface(mapSurface);
	SDL_DestroyTexture(mapTexture);
	// clean up all SDL subsystems and other non-SDL systems and global memory.
	clean_up();
	
	
	return 0;
}





