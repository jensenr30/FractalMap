#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"
#include "block.h"
#include "camera.h"
#include "graphics.h"
#include "globals.h"
#include "graphics.h"
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
	
	//--------------------------------------------------
	// blocks and cameras
	//--------------------------------------------------
	
	// origin block.
	struct blockData *origin = block_generate_origin();
	
	// this is the user's camera.
	// this function will set the camera to look at the origin block initially.
	struct cameraData *camera = camera_create(origin);
	
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
	// these keep track of where the mouse is/
	int x, y;
	
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
			else if(event.type == SDL_MOUSEWHEEL){
				// for each time the user scrolls in, increase the zoom by some factor
				for(i=0; i<abs(event.wheel.y); i++){
					if(event.wheel.y < 0)	{camera->scale *= 1.129830964f;}	// the user is moving the mouse wheel "down" or towards himself/herself.
					else					{camera->scale /= 1.129830964f;}	// the user is rotating the mouse wheel "up" or away from himself/herself.
					camera_check(camera);
				}
			}
			else if(event.type == SDL_MOUSEMOTION){
				x = event.motion.x;
				y = event.motion.y;
			}
			else if(event.type == SDL_KEYDOWN){
				// check if the key is greater than the 'a' character.
				if(event.key.keysym.sym >= 0)
					// set that character, number, or letter to 1.
					keys[(event.key.keysym.sym)%keysSize] = 1;
			}
			// check for window events
			else if(event.type == SDL_WINDOWEVENT ){
				// if the window was resized, the new window width and height need to be recorded.
				if( event.window.event == SDL_WINDOWEVENT_RESIZED){
					windW = event.window.data1;
					windH = event.window.data2;
				}
			}
			else if(event.type == SDL_QUIT){
				quit = 1;
			}
		}
		// if the user pressed the c key
		if(keys['c']){
			// generate children blocks
			block_generate_children(camera->target);
		}
		
		// fill up the left half of the screen with a color
		if(keys['v']){
			block_fill_half_vert(camera->target, 0xff, 0);
		}
		
		// generate parent of camera->target if the p key is pressed
		if(keys['p']){
			block_generate_parent(camera->target);
		}
		
		// if either the s or g keys were just stroked.
		if(keys['s'] || keys['g']){
			// generate new map in camera->target if the g key was pressed
			if(keys['g']){
				//block_random_fill(camera->target, 0, 0xff);
				//block_fill_middle(camera->target, 0xff, 0x00);
				//block_fill_nine_squares(camera->target, 100);
				block_fill_nine_squares_own_color(camera->target, 10000, 20000, 50000, 20000, 10000, 20000, 50000, 20000, 10000);
			}
			
			// smooth the block elevation data
			
			/*if(keys['s']){
				block_smooth(camera->target, 1); // using the averaging function
				FILE *bs = fopen("blockSmooth iterations.txt", "a");
				fprintf(bs,"smooth'd");
				fclose(bs);
			}
			*/
			if(keys['s']) filter_lowpass_2D_f((float *)((camera->target)->elevation), NULL, BLOCK_WIDTH, BLOCK_HEIGHT, 10); // using the low-pass filter
		}
		
		if(keys[SDLK_u]){
			block_generate_neighbor(camera->target, BLOCK_NEIGHBOR_UP);
		}
		
		//clear old surface
		if(mapSurface != NULL)SDL_FreeSurface(mapSurface);
		// generate image of map
		mapSurface = create_surface(windW, windH);
		// print map to mapSurface
		// map_print(mapSurface, camera->target);
		camera_print(mapSurface,camera);
		//block_print_to_file(camera->target, "camera-target.txt");
		// this test the frame-rate of the window by printing a single pixel under the mouse pointer tip
		set_pixel(mapSurface, mapSurface->w*(x/(float)windW), mapSurface->h*(y/(float)windH), 0xffffffff);
		
		// clear the old texture if it exists
		if(mapTexture != NULL)SDL_DestroyTexture(mapTexture);
		// create a texture for the map data
		mapTexture = SDL_CreateTextureFromSurface(myRenderer, mapSurface);
		
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





