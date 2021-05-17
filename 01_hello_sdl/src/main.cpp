/**
 * This is the simple hello world for SDL2.
 * 
 * You need C++14 to compile this.
 */

#include <SDL2/SDL.h>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <thread>
#include <tuple>
#include <vector>
#include <functional>

// check for errors
#define errcheck(e)                   \
  {                                   \
    if (e)                            \
    {                                 \
      cout << SDL_GetError() << endl; \
      SDL_Quit();                     \
      return -1;                      \
    }                                 \
  }

int main(int, char **)
{
  using namespace std;
  using namespace std::chrono;
  const int width = 808;  //640;
  const int height = 808; //480;

  errcheck(SDL_Init(SDL_INIT_VIDEO) != 0);

  shared_ptr<SDL_Window> window(SDL_CreateWindow(
                                    "Hello SDL", SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN),
                                [](SDL_Window *p) {
                                  SDL_DestroyWindow(p);
                                });

  shared_ptr<SDL_Renderer> renderer(SDL_CreateRenderer(
                                        window.get(), -1, SDL_RENDERER_ACCELERATED),
                                    [](SDL_Renderer *r) {
                                      SDL_DestroyRenderer(r);
                                    });

  //auto dt = 15ms;
  milliseconds dt(15);

  steady_clock::time_point current_time = steady_clock::now(); // remember current time
  double x = 0, y = 220;
  for (bool game_active = true; game_active;)
  {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    { 
      switch (event.type)
      {
        //close window by press X (right top corner)
      case SDL_QUIT:
        game_active = false;
        break;
        //close window by press 'q'
      case SDL_KEYUP:

        if (event.key.keysym.scancode == SDL_SCANCODE_Q)
        {
          game_active = false;
        }
        break;
      }
    }

    double px = x, py = y;

    x += 0.6;
    y = sin(0.1 * x / M_PI) * 200 + 240;

    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
    SDL_RenderClear(renderer.get());

    SDL_SetRenderDrawColor(renderer.get(), 255, 0, 0, 255);
    //SDL_RenderDrawPoint(renderer.get(), x, y);
    SDL_RenderDrawLineF(renderer.get(), x, y, px, py);

    SDL_RenderPresent(renderer.get()); // draw frame to screen

    this_thread::sleep_until(current_time = current_time + dt);
  }

  SDL_Quit();
  return 0;
}
