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
	
	windW = 1300;
	windH = 731;
	
	int i;
	//--------------------------------------------------
	// set up surfaces, textures, renderers, windows,
	//--------------------------------------------------
	// set all surfaces, textures, renderers, and windows to NULL initially to satify if statements that free memory that isn't needed any more.
	// this prevents the game from crashing at startup.
	SDL_Surface *mapSurface = NULL;
	SDL_Surface *spriteSurface = NULL;
	SDL_Texture *mapTexture = NULL;
	myWindow = NULL;
	myRenderer = NULL;
	myTexture = NULL;
	
	SDL_Window *networkWindow = NULL;
	SDL_Renderer *networkRenderer = NULL;
	SDL_Texture *networkTexture = NULL;
	SDL_Surface *networkSurface = NULL;
	
	SDL_Texture *spriteTexture[9];
	for(i = 0; i < 9; i++)
		spriteTexture[i] = NULL;
	
	SDL_Rect treeRect[9];
	for(i = 0; i < 9; i++) {
		treeRect[i].x = 0;
		treeRect[i].y = 0;
		treeRect[i].w = windW/3;
		treeRect[i].h = windW/3;
	}
	
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
	
	/*
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
	*/
	
	SDL_SetRenderDrawColor(networkRenderer, 0, 0, 0, 255);
	SDL_RenderClear(networkRenderer);
	SDL_RenderPresent(networkRenderer);
	
	SDL_SetRenderDrawColor(myRenderer, 0, 0, 0, 255);
	SDL_RenderClear(myRenderer);
	SDL_RenderPresent(myRenderer);
	
	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear"); // make the scaled rendering look smoother
	//SDL_RenderSetLogicalSize(myRenderer, windW, windH);
	
	//myTexture = SDL_CreateTexture(myRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, windW, windH);
	
	//SDL_Texture *glider = load_image_to_texture("glider.jpg");
	/*
	if(myTexture == NULL){
		error("main() could not create texture using SDL_CreateTexture");
		return -2;
	}
	*/
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
	
	// this is similar to mouse. however, this is used to store where the user clicked when a particular mouse button was clicked.
		// mouseClick[SDL_BUTTON_LEFT][0] is the x position where the user clicked the left  mouse button.
		// mouseClick[SDL_BUTTON_LEFT][1] is the y position where the user clicked the left  mouse button.
		// mouseClick[SDL_BUTTON_RIGHT][0] is the x position where the user clicked the RIGHT mouse button.
	// etc...
	int mouseClick[MOUSE_BUTTONS][2] = { {0,0}, {0,0}, {0,0}, {0,0}, {0,0} };
	// these variables keep track of time and FPS
	Uint32 ticksLast = 0;
	Uint32 ticksNow = 0;
	Uint32 frames = 0;
	Uint32 FPS = 0;
	
	float duty_cycle = 0.5f;
	float barWidth = 200;
	
	int steps = 3000;
	float angleIncrement = 3.1415926f/(float)steps;
	float two_pi = 3.1415926f*2.0f;
	int harmonics = 40;
	float cumulativeCos;
	float cumulativeSin;
	float positiveAngle;
	float theta;
	
	int spacing = 1;
	int squareWaveSpace = 131;
	int barHeight;
	int barHeighti;
	SDL_Rect myRect;
	
	int logspace = 0;
	
	int squareWavePeriod = 400; // (pixels)
	
	
	
	while(quit == 0){
		
		// reset all keystroke values
		for(i=0; i<keysSize; i++){
			keys[i] = 0;
		}
		while(SDL_PollEvent(&event)){
			// if there is a mouse button down event,
			if(event.button.type == SDL_MOUSEBUTTONDOWN){
				// set mouse button states
				if(event.button.button == SDL_BUTTON_LEFT){
					// record that the left mouse button is down
					mouse[SDL_BUTTON_LEFT][0] = 1;
					// record where the left mouse button was clicked
					mouseClick[SDL_BUTTON_LEFT][0] = x;
					mouseClick[SDL_BUTTON_LEFT][1] = y;
				}
				else if(event.button.button == SDL_BUTTON_RIGHT){
					// record that the right mouse button is down
					mouse[SDL_BUTTON_RIGHT][0] = 1;
					// record where the right mouse button was clicked
					mouseClick[SDL_BUTTON_RIGHT][0] = x;
					mouseClick[SDL_BUTTON_RIGHT][1] = y;
				}
				else if(event.button.button == SDL_BUTTON_MIDDLE){
					// record that the middle mouse button is down
					mouse[SDL_BUTTON_MIDDLE][0] = 1;
					// record where the middle mouse button was clicked
					mouseClick[SDL_BUTTON_MIDDLE][0] = x;
					mouseClick[SDL_BUTTON_MIDDLE][1] = y;
				}
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
		
		
		
		// clear the old texture if it exists
		if(networkSurface != NULL)SDL_FreeSurface(networkSurface);
		networkSurface = create_surface(windW, windH);
		
		
		// generate the network hierarchy
		//block_print_network_hierarchy(networkSurface, origin->parent, camera->target, 5, 5, 0, 0, windW, 0xff00ff00, 0xff0000ff, 0xffff0000);
		
		// if the left mouse button is currently held
		if(mouse[SDL_BUTTON_LEFT][0])
		{
			duty_cycle = x/((float)windW);
		}
		
		if(mouse[SDL_BUTTON_RIGHT][0] && !mouse[SDL_BUTTON_RIGHT][1])
		{
			logspace = !logspace;
		}
		
		barHeight = windH - squareWaveSpace - 2;
		barWidth = windW/(float)4.35;
		for(i=1; i<=harmonics; i++)
		{
			
			// handle the x position and width of the bars
			if(i==1)
			{
				myRect.x = spacing;
				myRect.w = barWidth;
			}
			else
			{
				// move horizontally to next spot.
				myRect.x += spacing + myRect.w;
				myRect.w = barWidth/(float)i;
			}
			
			cumulativeCos = 0;
			cumulativeSin = 0;
			positiveAngle = two_pi*duty_cycle;
			for(theta=0; theta<positiveAngle; theta+= angleIncrement)
			{
				cumulativeCos += cos(theta*(float)i);
				cumulativeSin += sin(theta*(float)i);
			}
			for(; theta<two_pi; theta+= angleIncrement)
			{
				cumulativeCos -= cos(theta*(float)i);
				cumulativeSin -= sin(theta*(float)i);
			}
			
			if(logspace)	barHeighti = 90*log(barHeight*sqrt(cumulativeCos*cumulativeCos + cumulativeSin*cumulativeSin)/((float)steps));
			else 			barHeighti = 0.75*barHeight*sqrt(cumulativeCos*cumulativeCos + cumulativeSin*cumulativeSin)/((float)steps);
			myRect.y = squareWaveSpace + spacing + barHeight - barHeighti;
			myRect.h = barHeighti;
			
			SDL_FillRect(networkSurface, &myRect, 0xff00ef00);
		}
		
		
		for(i=0; i<windW; i++){
			
			if(duty_cycle*squareWavePeriod < i%squareWavePeriod)
			{
				set_pixel(networkSurface, i, 10, 0xff0000ff);
				set_pixel(networkSurface, i, 11, 0xff0000ff);
			}
			else
			{
				set_pixel(networkSurface, i, squareWaveSpace, 0xff0000ff);
				set_pixel(networkSurface, i, squareWaveSpace +1, 0xff0000ff);
			}
			
		}
		
		
		// generate texture for the block network
		networkTexture = SDL_CreateTextureFromSurface(networkRenderer, networkSurface);
		// render the networkSurface to the networkWindow
		SDL_RenderCopy(networkRenderer, networkTexture, NULL, NULL);
		if(networkTexture != NULL)SDL_DestroyTexture(networkTexture);
		// display the renderer's result on the screen and clear it when done
		SDL_RenderPresent(networkRenderer);
		SDL_RenderClear(networkRenderer);
		
			
		
		/*
		// print the camera to screen
		camera_render(myRenderer, camera, windW, windH);
		
		// display the renderer's result on the screen and clear it when done
		SDL_RenderPresent(myRenderer);
		SDL_RenderClear(myRenderer);
		*/
		
		
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





