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
#include "sprites.h"
#include "generation.h"
#include "tree_generation.h"



#define MOUSE_BUTTONS 5


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
	
	windW = BLOCK_WIDTH*3;
	windH = BLOCK_HEIGHT*3;
	//--------------------------------------------------
	// set up surfaces, textures, renderers, windows,
	//--------------------------------------------------
	// set all surfaces, textures, renderers, and windows to NULL initially to satify if statements that free memory that isn't needed any more.
	// this prevents the game from crashing at startup.
	SDL_Surface *mapSurface = NULL;
	SDL_Surface *spriteSurface = NULL;
	SDL_Texture *mapTexture = NULL;
	SDL_Texture *spriteTexture = NULL;
	myWindow = NULL;
	myRenderer = NULL;
	myTexture = NULL;
	
	SDL_Window *networkWindow = NULL;
	SDL_Renderer *networkRenderer = NULL;
	SDL_Texture *networkTexture = NULL;
	SDL_Surface *networkSurface = NULL;
	
	sgenrand(time(NULL));
	
	
	if(SDL_Init(SDL_INIT_EVERYTHING) == -1) return -99;
	
		// set network window
	networkWindow = SDL_CreateWindow("FractalMap - Network Viewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windW, windH, SDL_WINDOW_RESIZABLE);
	networkRenderer = SDL_CreateRenderer(networkWindow, -1, 0);
	
	if(networkWindow == NULL){
		error("main() could not create networkWindow using SDL_CreateWindow");
		return -1;
	}
	if(networkRenderer == NULL){
		error("main() could not create networkRenderer using SDL_CreateRenderer");
		return -2;
	}
	
	
	myWindow = SDL_CreateWindow("FractalMap - Map", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windW, windH, SDL_WINDOW_RESIZABLE);
	myRenderer = SDL_CreateRenderer(myWindow, -1, 0);
	
	if(myWindow == NULL){
		error("main() could not create window using SDL_CreateWindow");
		return -1;
	}
	if(myRenderer == NULL){
		error("main() could not create renderer using SDL_CreateRenderer");
		return -2;
	}
	
	
	SDL_SetRenderDrawColor(networkRenderer, 0, 0, 0, 255);
	SDL_RenderClear(networkRenderer);
	SDL_RenderPresent(networkRenderer);
	
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
	block_generate_parent(origin);
	block_generate_parent(origin->parent);
	
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
	// this keeps track of which keys are HELD DOWN
	int_fast8_t keysHeld[keysSize];
	int i;
	// these keep track of where the mouse is
	int x=0, y=0;
	// these keep track of where the mouse was just moments ago.
	int xlast=0, ylast=0;
	
	// these two 2-element arrays keep the data concerning the state of the right and left mouse buttons.
	// EX:
		// mouse[SDL_BUTTON_LEFT][0] is the CURRENT state of the LEFT mouse button
		// mouse[SDL_BUTTON_LEFT][1] is the LAST    state of the LEFT mouse button
		// mouse[SDL_BUTTON_RIGHT][0] is the CURRENT state of the RIGHT mouse button
		// mouse[SDL_BUTTON_RIGHT][1] is the LAST    state of the RIGHT mouse button
	int mouse[MOUSE_BUTTONS][2] = { {0,0}, {0,0}, {0,0}, {0,0}, {0,0} };
	
	// these variables keep track of time and FPS
	Uint32 ticksLast = 0;
	Uint32 ticksNow = 0;
	Uint32 frames = 0;
	Uint32 FPS = 0;
	
	
	while(quit == 0){
		
		// reset all keystroke values
		for(i=0; i<keysSize; i++){
			keys[i] = 0;
		}
		while(SDL_PollEvent(&event)){
			// if there is a mouse button down event,
			if(event.button.type == SDL_MOUSEBUTTONDOWN){
				// set mouse button states
				if(event.button.button == SDL_BUTTON_LEFT) mouse[SDL_BUTTON_LEFT][0] = 1;
				if(event.button.button == SDL_BUTTON_RIGHT) mouse[SDL_BUTTON_RIGHT][0] = 1;
				if(event.button.button == SDL_BUTTON_MIDDLE) mouse[SDL_BUTTON_MIDDLE][0] = 1;
			}
			else if(event.type == SDL_MOUSEBUTTONUP){
				// set mouse button states
				if(event.button.button == SDL_BUTTON_LEFT) mouse[SDL_BUTTON_LEFT][0] = 0;
				if(event.button.button == SDL_BUTTON_RIGHT) mouse[SDL_BUTTON_RIGHT][0] = 0;
				if(event.button.button == SDL_BUTTON_MIDDLE) mouse[SDL_BUTTON_MIDDLE][0] = 0;
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
				if(event.key.keysym.sym >= 0){
					// set that character, number, or letter to 1.
					keys[(event.key.keysym.sym)%keysSize] = 1;
					keysHeld[(event.key.keysym.sym)%keysSize] = 1;
				}
			}
			else if(event.type == SDL_KEYUP){
				if(event.key.keysym.sym >= 0){
					// set that character, number, or letter to 0.
					keysHeld[(event.key.keysym.sym)%keysSize] = 0;
				}
			}
			// check for window events
			else if(event.type == SDL_WINDOWEVENT ){
				if(event.window.event == SDL_WINDOWEVENT_CLOSE){
					quit = 1;
				}
				
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
		
		// if the left mouse button is currently held
		if(mouse[SDL_BUTTON_LEFT][0]){
			// move the camera x and y based on the distance the user has move the mouse
			camera->x -= x-xlast;
			camera->y -= y-ylast;
			// check the camera.
			// if the user moved too far in some direction, the camera will pan the camera appropriately.
			camera_check(camera);
		}
		
		// the wasd keys are used currently for testing the generation of neighbors.
		if(keys['w']){
			//block_generate_neighbor(camera->target, BLOCK_NEIGHBOR_UP);
			camera_pan(camera, CAMERA_PAN_UP);
			// check the camera to make sure everything is fine
			camera_check(camera);
		}
		if(keys['s']){
			//block_generate_neighbor(camera->target, BLOCK_NEIGHBOR_DOWN);
			camera_pan(camera, CAMERA_PAN_DOWN);
			// check the camera to make sure everything is fine
			camera_check(camera);
		}
		if(keys['a']){
			//block_generate_neighbor(camera->target, BLOCK_NEIGHBOR_LEFT);
			camera_pan(camera, CAMERA_PAN_LEFT);
			// check the camera to make sure everything is fine
			camera_check(camera);
		}
		if(keys['d']){
			//block_generate_neighbor(camera->target, BLOCK_NEIGHBOR_RIGHT);
			camera_pan(camera, CAMERA_PAN_RIGHT);
			// check the camera to make sure everything is fine
			camera_check(camera);
		}
		
		// if the user pressed the r key
		if(keys['r']){
			// generate random noise in the block
			block_random_fill(camera->target, BLOCK_ELEVATION_BOUND_LOWER, BLOCK_ELEVATION_BOUND_UPPER );
			//block_print_to_file(camera->target, "camera target.txt");
			//color_mix_weighted_f(0xff000000, 0xffffffff, 0.667, 0.333);
		}
		
		// if the user pressed the c key
		if(keys['c']){
			// generate children blocks
			block_generate_children(camera->target);
		}
		
		// fill up the left half of the screen with a color
		if(keys['v']){
			block_fill_half_vert(camera->target, 0xffffffff, 0);
		}
		
		// generate parent of camera->target if the p key is pressed
		if(keys['p']){
			block_generate_parent(camera->target);
		}
		
		// if either the f or g keys were just stroked.
		if(keys['f'] || keys['g']){
			// generate new map in camera->target if the g key was pressed
			if(keys['g']){
				//block_random_fill(camera->target, 0, 0xff);
				//block_fill_middle(camera->target, 0xff, 0x00);
				//block_fill_nine_squares(camera->target, 100);
				block_fill_nine_squares_own_color(camera->target, 10000, 20000, 50000, 20000, 10000, 20000, 50000, 20000, 10000);
			}
			
			// the f key is for filtering
			if(keys['f']){
				// using the low-pass filter
				filter_lowpass_2D_f((float *)((camera->target)->elevation), NULL, BLOCK_WIDTH, BLOCK_HEIGHT, 3);
				camera->target->renderMe = 1;
			}
		}
		
		
		
		
		
		// clear old surface
		//if(mapSurface != NULL)SDL_FreeSurface(mapSurface);
		// generate image of map
		//mapSurface = create_surface(windW, windH);

		// print map to mapSurface
		// map_print(mapSurface, camera->target);
		
		if(keys['u']){
			if(spriteSurface != NULL)
				SDL_FreeSurface(spriteSurface);
			spriteSurface = create_surface(BLOCK_WIDTH, BLOCK_HEIGHT);
		
			struct treeData myTree;
			myTree.leafColor = 0xff088c05;
			myTree.trunkColor = 0xffc66505;
			myTree.leavesNumbers = 150;
			myTree.leavesSize = 3;
			myTree.leavesGenerationRadiusX = 20;
			myTree.leavesGenerationRadiusY = 10;
			myTree.leavesDistribution = 5;
			myTree.trunkHeight = 15;
			myTree.trunkIncrement = 5;
			myTree.trunkNegativeLow = -1;
			myTree.trunkNegativeHigh = -10;
			myTree.trunkPositiveLow = 1;
			myTree.trunkPositiveHigh = 10;
			generateTree(spriteSurface, &myTree);
			/*
			draw_line(spriteSurface, 0, 0, BLOCK_WIDTH, BLOCK_HEIGHT, 1, 0xff00ff00);
			draw_line(spriteSurface, BLOCK_WIDTH, 0, 0, BLOCK_HEIGHT, 1, 0xff00ff00);
			draw_circle(spriteSurface, BLOCK_WIDTH/2.0, BLOCK_WIDTH/2.0, 120, 0xff000000);
			draw_circle(spriteSurface, BLOCK_WIDTH/2.0, BLOCK_WIDTH/2.0, 100, 0xffff0000);
			draw_circle(spriteSurface, BLOCK_WIDTH/2.0, BLOCK_WIDTH/2.0, 80, 0xff00ff00);
			draw_circle(spriteSurface, BLOCK_WIDTH/2.0, BLOCK_WIDTH/2.0, 60, 0xff0000ff);
			
			draw_circle(spriteSurface, BLOCK_WIDTH/2.0, BLOCK_WIDTH/2.0, 40, 0xffffff00);
			draw_circle(spriteSurface, BLOCK_WIDTH/2.0, BLOCK_WIDTH/2.0, 20, 0xff00ffff);
			draw_circle(spriteSurface, BLOCK_WIDTH/2.0, BLOCK_WIDTH/2.0, 10, 0xffffffff);
			*/
			//SDL_RenderCopy(myRenderer, spriteSurface, NULL, NULL);
			//SDL_RenderPresent(myRenderer);
			//SDL_RenderClear(myRenderer);
			spriteTexture = SDL_CreateTextureFromSurface(myRenderer, spriteSurface);
		}
		
		
		//draw_rect(mapSurface, 243, 243, x-243, y-243, 9, color_mix_weighted(0xff00ff00,0xff0000ff,1,1), 0, 0);
		// block_generate_neighbor(camera->target, BLOCK_NEIGHBOR_UP);
		
		//block_print_to_file(camera->target, "camera-target.txt");
		// this test the frame-rate of the window by printing a single pixel under the mouse pointer tip
		//set_pixel(mapSurface, mapSurface->w*((x%windW)/(float)windW), mapSurface->h*((y%windH)/(float)windH), 0xffffffff);
		
		
		
		
		
		// clear the old texture if it exists
		if(networkSurface != NULL)SDL_FreeSurface(networkSurface);
		
		networkSurface = create_surface(windW, windH);
		
		// generate the network hierarchy
		block_print_network_hierarchy(networkSurface, origin->parent, camera->target, 5, 5, 0, 0, windW, 0xff00ff00, 0xff0000ff, 0xffff0000);
		// generate texture for the block network
		networkTexture = SDL_CreateTextureFromSurface(networkRenderer, networkSurface);
		// render the networkSurface to the networkWindow
		SDL_RenderCopy(networkRenderer, networkTexture, NULL, NULL);
		if(networkTexture != NULL)SDL_DestroyTexture(networkTexture);
		
		// display the renderer's result on the screen and clear it when done
		SDL_RenderPresent(networkRenderer);
		SDL_RenderClear(networkRenderer);
		
		
		
		// print the camera to screen
		camera_render(myRenderer, camera, windW, windH);
		// print the test sprite to the screen
		SDL_RenderCopy(myRenderer, spriteTexture, NULL, NULL);
		
		// display the renderer's result on the screen and clear it when done
		SDL_RenderPresent(myRenderer);
		SDL_RenderClear(myRenderer);
		
		// store the current x and y values and use them as the "last" values in the next iteration of the loop
		xlast = x;
		ylast = y;
		
		// save all current mouse states.
		for(i=0; i<MOUSE_BUTTONS; i++){
			// set the last state of this mouse button to the current state (for the next loop iteration)
			mouse[i][1] = mouse[i][0];
		}
		
		// increase the frame counter (as we have just successfully rendered a frame)
		frames++;
		// get the current value of ticks
		ticksNow = SDL_GetTicks();
		// if more than a second has passed since the last FPS calculation,
		if(ticksNow - ticksLast >= 1000){
			FPS = (int)(frames/(((float)(ticksNow-ticksLast))/1000.0f) + 0.5f );
			// reset frame counter
			frames = 0;
			// set the last tick time to the current time (this will make the program wait 1 second until recalculating the FPS)
			ticksLast = ticksNow;
			gamelog_d("FPS =", FPS);
		}
		
	}
	
	
	//--------------------------------------------------
	// clean up
	//--------------------------------------------------
	SDL_FreeSurface(mapSurface);
	if(mapTexture != NULL)SDL_DestroyTexture(mapTexture);
	if(networkTexture != NULL)SDL_DestroyTexture(networkTexture);
	// clean up all SDL subsystems and other non-SDL systems and global memory.
	clean_up();
	
	
	return 0;
}





