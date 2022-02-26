#include "SDL2/SDL.h"
#include <iostream>
#include <chrono>
#include "GLEW\glew.h"
#include "Renderer.h"
#include <thread>         // std::this_thread::sleep_for

using namespace std;

//Screen attributes
SDL_Window * window;

//OpenGL context 
SDL_GLContext gContext;
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 720;

//Event handler
SDL_Event event;

Renderer * renderer = nullptr;

void clean_up()
{
	delete renderer;

	SDL_GL_DeleteContext(gContext);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

// initialize SDL and OpenGL
bool init()
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		return false;
	}

	// use Double Buffering
	if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) < 0)
		cout << "Error: No double buffering" << endl;

	// set OpenGL Version (3.3)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Create Window
	window = SDL_CreateWindow("Space Run",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (window == NULL)
	{
		printf("Could not create window: %s\n", SDL_GetError());
		return false;
	}

	//Create OpenGL context
	gContext = SDL_GL_CreateContext(window);
	if (gContext == NULL)
	{
		printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	// Disable Vsync
	if (SDL_GL_SetSwapInterval(0) == -1)
		printf("Warning: Unable to disable VSync! SDL Error: %s\n", SDL_GetError());

	// Initialize GLEW
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		printf("Error loading GLEW\n");
		return false;
	}
	// some versions of glew may cause an opengl error in initialization
	glGetError();

	renderer = new Renderer();
	bool engine_initialized = renderer->Init(SCREEN_WIDTH, SCREEN_HEIGHT);

	return engine_initialized;
}

int main(int argc, char *argv[])
{
	//Initialize SDL, glew, engine
	if (init() == false)
	{
		system("pause");
		return EXIT_FAILURE;
	}

	//Quit flag
	bool quit = false;
	bool mouse_button_pressed = false;
	glm::vec2 prev_mouse_position(0);

	auto simulation_start = chrono::steady_clock::now();

	// Wait for user exit
	while (quit == false)
	{
		// While there are events to handle

		// if the spacecraft hits the terrain end the game
		if (renderer->hit == true) {
			std::cout << "You hit a rock. Be careful !" << std::endl;
			quit = true;
		}

		// increase score each time the player obtains a ball
		if (renderer->ball1hit == true) {
			renderer->score1 = 1;
			std::cout << "Score : " << renderer->score1 + renderer->score2 + renderer->score3 + renderer->score4 + renderer->score5 << std::endl;
		}

		if (renderer->ball2hit == true) {
			renderer->score2 = 1;
			std::cout << "Score : " << renderer->score1 + renderer->score2 + renderer->score3 + renderer->score4 + renderer->score5 << std::endl;
		}

		if (renderer->ball3hit == true) {
			renderer->score3 = 1;
			std::cout << "Score : " << renderer->score1 + renderer->score2 + renderer->score3 + renderer->score4 + renderer->score5 << std::endl;
		}

		if (renderer->ball4hit == true) {
			renderer->score4 = 1;
			std::cout << "Score : " << renderer->score1 + renderer->score2 + renderer->score3 + renderer->score4 + renderer->score5 << std::endl;
		}
	
		if (renderer->ball5hit == true) {
			renderer->score5 = 1;
			std::cout << "Score : " << renderer->score1 + renderer->score2 + renderer->score3 + renderer->score4 + renderer->score5 << std::endl;
		}
		

		// if the player gathers all 5 balls end the game.
		if (renderer->score1 + renderer->score2 + renderer->score3 + renderer->score4 + renderer->score5 == 5) {
			std::cout << "Congratulations! You won the game!" << std::endl;
			quit = true;
		}

		// if the spacecraft hits the ground end the game.

		if (renderer->craft_position.y <= -2) {
			std::cout << "Keep the spacecraft off the ground" << std::endl;
			quit = true;
		}

		// if the spacecraft hits the front wall of the terrain end the game.
		// -279.854 60 -372.401 full aristera tou mprostinou toixou
		// 211.803 52.4877 -408.581 full dexia tou mprostinou toixou

		if (renderer->craft_position.z <= -375 && renderer->craft_position.z >= -409) {
			std::cout << "You hit the front wall. Be careful !" << std::endl;
			quit = true;
		}

		// if the spacecraft hits the left wall of the terrain end the game.
		if (renderer->craft_position.x >= -282 && renderer->craft_position.x <= -270) {
			std::cout << "You hit the left wall. Be careful !" << std::endl;
			quit = true;
		}

		// if the spacecraft hits the right wall of the terrain end the game.
		// 285.553 56.8319 -256.392
		if (renderer->craft_position.x >= 190 && renderer->craft_position.x <= 211) {
			std::cout << "You hit the right wall. Be careful !" << std::endl;
			quit = true;
		}

		// if the spacecraft hits the back wall of the terrain end the game.
		if (renderer->craft_position.z <= 167 && renderer->craft_position.z >= 164) {
			std::cout << "You hit the back wall. Be careful !" << std::endl;
			quit = true;
		}
		// full aristera pisw toixou -260.742 60 166.67
		// full dexia pisw toixou 165

		// if the spacecraft reaches height greater than the rocks the game ends. (Out of bounds flying is not tolerated)
		if (renderer->craft_position.y >= 120.f) {
			std::cout << "You flew too close to the sun" << std::endl;
			quit = true;
		}

		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
			else if (event.type == SDL_KEYDOWN)
			{
				// Key down events
				if (event.key.keysym.sym == SDLK_ESCAPE) quit = true;
				else if (event.key.keysym.sym == SDLK_r) renderer->ReloadShaders();
				else if (event.key.keysym.sym == SDLK_w || event.key.keysym.sym == SDLK_UP)
				{
					//renderer->CameraMoveForward(true);
					renderer->CraftTurnDown(true);
				}
				else if (event.key.keysym.sym == SDLK_s || event.key.keysym.sym == SDLK_DOWN)
				{
					//renderer->CameraMoveBackWard(true);
					renderer->CraftTurnUp(true);
				}
				else if (event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_LEFT)
				{
					//renderer->CameraMoveLeft(true);
					//renderer->CraftTurnLeft(true);
					renderer->setTurnHelper(0);
				}
				else if (event.key.keysym.sym == SDLK_d || event.key.keysym.sym == SDLK_RIGHT)
				{
					//renderer->CameraMoveRight(true);
					//renderer->CraftTurnRight(true);
					renderer->setTurnHelper(1);
				}
			}
			else if (event.type == SDL_KEYUP)
			{
				// Key up events
				if (event.key.keysym.sym == SDLK_w || event.key.keysym.sym == SDLK_UP)
				{
					//renderer->CameraMoveForward(false);
					renderer->CraftTurnDown(false);
				}
				else if (event.key.keysym.sym == SDLK_s || event.key.keysym.sym == SDLK_DOWN)
				{
					//renderer->CameraMoveBackWard(false);
					renderer->CraftTurnUp(false);
				}
				else if (event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_LEFT)
				{
					//renderer->CameraMoveLeft(false);
					//renderer->CraftTurnLeft(false);
					renderer->resetLeft(true);
					renderer->UpdateAngle(true);
					renderer->UpdateDirection(1);
					renderer->setTurnHelper(-1);
					
				}
				else if (event.key.keysym.sym == SDLK_d || event.key.keysym.sym == SDLK_RIGHT)
				{
					//renderer->CameraMoveRight(false);
					//renderer->CraftTurnRight(false);
					renderer->resetRight(true);
					renderer->UpdateAngle(false);
					renderer->UpdateDirection(-1);
					renderer->setTurnHelper(-1);
				}
			}
			else if (event.type == SDL_MOUSEMOTION)
			{
				int x = event.motion.x;
				int y = event.motion.y;

				if (mouse_button_pressed)
				{
					renderer->CameraLook(prev_mouse_position - glm::vec2(x, y));
					prev_mouse_position = glm::vec2(x, y);
				}
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					int x = event.button.x;
					int y = event.button.y;
					mouse_button_pressed = (event.type == SDL_MOUSEBUTTONDOWN);
					prev_mouse_position = glm::vec2(x, y);
				}
			}
			else if (event.type == SDL_WINDOWEVENT)
			{
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					renderer->ResizeBuffers(event.window.data1, event.window.data2);
				}
			}
		}

		// Compute the ellapsed time
		auto simulation_end = chrono::steady_clock::now();

		float dt = chrono::duration <float>(
			simulation_end - simulation_start).count(); // in seconds

		simulation_start = chrono::steady_clock::now();

		// Update
		renderer->Update(dt);

		// Draw
		renderer->Render();
		
		//Update screen (swap buffer for double buffering)
		SDL_GL_SwapWindow(window);
	}

	//Clean up
	clean_up();

	return 0;
}
