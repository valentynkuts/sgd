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
                                    "Hello", SDL_WINDOWPOS_UNDEFINED,
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
  double mouse_x = 0, mouse_y = 0;
  double m_click_x = 0, m_click_y = 0;
  double move_x = 0, move_y = 0;
  double temp_old_x, temp_old_y;

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

        if (event.key.keysym.scancode == SDL_SCANCODE_W)
        {
          move_x = move_x + 5;
          move_y = move_y + 5;
        }
        break;

      case SDL_MOUSEMOTION:
        printf("We got a motion event.\n");
        printf("Current mouse position is: (%d, %d)\n", event.motion.x, event.motion.y);
        mouse_x = event.motion.x;
        mouse_y = event.motion.y;
        break;

      case SDL_MOUSEBUTTONDOWN:
        temp_old_x = m_click_x;
        temp_old_y = m_click_y;

        printf("SDL_MOUSEBUTTONDOWN position is: (%d, %d)\n", event.motion.x, event.motion.y);
        m_click_x = event.motion.x;
        m_click_y = event.motion.y;
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
    //------------draw point
    //SDL_SetRenderDrawColor(renderer.get(), 255, 255, 255, 255);
    //SDL_RenderDrawPoint(renderer.get(), 200, 200);

    // ------------------draw line
    //SDL_SetRenderDrawColor(renderer.get(), 255, 255, 255, 255);
    //SDL_RenderDrawLineF(renderer.get(), 100, 100, 150, 150);

    //------------click to see point
    SDL_SetRenderDrawColor(renderer.get(), 200, 200, 150, 255);
    SDL_RenderDrawPoint(renderer.get(), m_click_x, m_click_y);

    //------------click to see line

    SDL_SetRenderDrawColor(renderer.get(), 200, 200, 150, 255);
    SDL_RenderDrawLineF(renderer.get(), m_click_x, m_click_y, temp_old_x, temp_old_y);

    //------------click 'w' to move
    SDL_SetRenderDrawColor(renderer.get(), 200, 200, 200, 255);
    SDL_RenderDrawPoint(renderer.get(), move_x, move_y);
    //-----------------
    SDL_RenderPresent(renderer.get()); // draw frame to screen

    this_thread::sleep_until(current_time = current_time + dt);
  }

  SDL_Quit();
  return 0;
}
