//// OS Specific Imports
#ifndef __linux__
#include <Windows.h>
#endif

//// Configurations
#define STB_IMAGE_IMPLEMENTATION

//// Imports
#include <SDL/SDL.h>
#include <GL/glew.h>
#include "glwindow.h"

//// Environmental Guards
#ifdef __linux__
int main(int argc, char** argv)
#else
int SDL_main(int argc, char** argv)
#endif
{
    ///////////
    // Setup //
    ///////////

    // initialize
    std::cout << "\n[Advanced OpenGL]\n";
	// check for SDL
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error", "Unable to initialize SDL", 0);
        return 1;
    }
	// Create Window
    SWPTAS001::OpenGLWindow window;
    window.initGL();

    //////////////
    // Run Loop //
    //////////////

    // Run-Loop
    bool running = true;
    while(running)
    {
        // Check for a quit event before passing to the GLWindow
        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT)
            {
                running = false;
            }
            else if(!window.handleEvent(e))
            {
                running = false;
            }
        }
		// render screen
        window.render();
        // We sleep for 10ms here so as to prevent excessive CPU usage
        SDL_Delay(50);
    }

    //////////
    // Done //
    //////////
    
	// clean up
    window.cleanup();
    SDL_Quit();
    // done
    std::cout << "\n[Goodbye]\n";
    return 0;
}

