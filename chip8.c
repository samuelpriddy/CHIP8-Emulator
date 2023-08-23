#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"
#include <stdbool.h>

typedef struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
} sdl_t;

typedef struct {
	uint32_t window_width; //SDL Window Width
	uint32_t window_height; //SDL Window Height
	uint32_t fg_color; //SDL Renderer foreground color RGBA8888
	uint32_t bg_color; //SDL Renderer background color RGBA8888
	uint32_t scale_factor; //Amount to scale a CHIP8 pixel by
} config_t;

typedef enum {
	QUIT, 
	RUNNING,
	PAUSED,
} emulator_state_t;

typedef struct {
	emulator_state_t state;
} chip8_t;

//Initialize SDL
int init_sdl(sdl_t *sdl, const config_t config) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
		SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
		return 1;
	}
	sdl->window = SDL_CreateWindow("CHIP8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, config.window_width * config.scale_factor, config.window_height * config.scale_factor, 0);

	if (!sdl->window) {
		SDL_Log("Could not create SDL window %s\n", SDL_GetError());
		return 1;
	}

	sdl->renderer = SDL_CreateRenderer(sdl->window, -1, SDL_RENDERER_ACCELERATED);
	if (!sdl->renderer) {
		SDL_Log("Could not create SDL renderer %s\n", SDL_GetError());
		return 1;
	}
	return 0;
}

//Set up initial emulator config from passed in argument
int set_config_from_args(config_t *config, int argc, char **argv) {
	//Set Defaults
	*config = (config_t){
		.window_width = 64, //Chip8 original x resolution
		.window_height = 32, //Chip8 original y resolution
		.fg_color = 0xFFFFFFFF, //Black
		.bg_color = 0x00000000, //White
		.scale_factor = 20, // Default resolution will be 1280x640
	};

	//Override deafults from passed in arguments
	for (int i = 1; i < argc; i++) {
		(void)argv[i];
	}
	return 0;
}

//Final cleanup
void cleanup(sdl_t *sdl) {
	SDL_DestroyWindow(sdl->window);
	SDL_DestroyRenderer(sdl->renderer);
	SDL_Quit(); //Shutdown SDL subsystem
}

//Clear SDL Window to background color
void clear_screen(const sdl_t sdl, const config_t config) {
	const uint8_t r = (config.bg_color >> 24) & 0xFF;
	const uint8_t g = (config.bg_color >> 16) & 0xFF;
	const uint8_t b = (config.bg_color >> 8) & 0xFF;
	const uint8_t a = config.bg_color & 0xFF;

	SDL_SetRenderDrawColor(sdl.renderer, r, g, b, a);
	SDL_RenderClear(sdl.renderer);
}

void update_screen(const sdl_t sdl) {
	SDL_RenderPresent(sdl.renderer);
}

void handle_input(chip8_t* chip8) {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				// Exit window; End program
				chip8->state = QUIT;
				return;
			case SDL_KEYDOWN:
				break;	
			case SDL_KEYUP:
				break;
			default:
				break;

		}
	}
}

//Initialize chip8 machine
int init_chip8(chip8_t* chip8) {
	chip8->state = RUNNING; //Default machine state to running
	return 0;
}

int main(int argc, char **argv) {
	//Initialize emulator config
	config_t config = {0};
	if (set_config_from_args(&config, argc, argv)) exit(EXIT_FAILURE);

	//Initialize SDL graphical interface
	sdl_t sdl = {0};
	if (init_sdl(&sdl, config)) exit(EXIT_FAILURE);
	
	//Initial screen clear
	clear_screen(sdl, config);

	//Initialize chip8 machine
	chip8_t chip8 = {0};
	if (init_chip8(&chip8)) exit(EXIT_FAILURE);

	//Main emulator loop
	while (chip8.state != QUIT) {
		//Handle user input
		handle_input(&chip8);
	
		SDL_Delay(16);
		update_screen(sdl);
	}

	cleanup(&sdl);

	exit(EXIT_SUCCESS);
}

