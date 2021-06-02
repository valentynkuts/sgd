/**
 * This is the simple game for SDL2 and SDL2_Image.
 *
 * You need C++14 to compile this.
 * 
 * Tadeusz Puźniakowski 2021
 * 
 * Unlicensed
 */

#include "vectors.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <array>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <thread>
#include <tuple>
#include <vector>
#include "animation.hpp"

#include "main.hpp"

std::ostream &operator<<(std::ostream &o, const std::array<double, 2> &a)
{
    o << "[" << a[0] << "," << a[1] << "]";
    return o;
}
// renderer, position{x,y}, photo, photo_width, photo_height, rotation  (obrót)
void draw_o(std::shared_ptr<SDL_Renderer> r, std::array<double, 2> p, std::shared_ptr<SDL_Texture> tex, double w, double h, double a)
{
    SDL_Rect dst_rect = {(int)(p[0] - w / 2), (int)(p[1] - h / 2), (int)w, (int)h};
    //show photo 'tex'
    SDL_RenderCopyEx(r.get(), tex.get(), NULL, &dst_rect, a, NULL, SDL_RendererFlip::SDL_FLIP_NONE);
    ////wskaźnik na wartość
}

// class physical_c
// {
// public:
//     std::array<double, 2> position;
//     std::array<double, 2> velocity;
//     std::array<double, 2> acceleration;
//     double friction;

//     void update(double dt_f, std::function<void(physical_c *, std::array<double, 2> &pos, std::array<double, 2> &vel)> callback_f)
//     {
//         using namespace tp::operators;
//         // apply friction:
//         auto new_acceleration = acceleration - velocity * length(velocity) * friction;
//         auto new_velocity = velocity + new_acceleration * dt_f;
//         auto new_position = position + new_velocity * dt_f + new_acceleration * dt_f * dt_f * 0.5;
//         callback_f(this, new_position, new_velocity);

//         //wind resistance todo
//     }
// };

// class player_c : public physical_c
// {
// public:
//     std::map<std::string, int> intentions;

//     player_c()
//     {
//         position = {10, 10};
//         velocity = {0, 0};
//         friction = 0.03;
//         acceleration = {0, 0};
//     }

//     /**
//  * applies and clears intentions
//  * */
//     void apply_intent()
//     {
//         acceleration = {0, 30};
//         if (intentions.count("right"))
//             acceleration[0] += 100;
//         if (intentions.count("left"))
//             acceleration[0] += -100;
//         if (intentions.count("up"))
//             acceleration[1] += -100;
//         if (intentions.count("down"))
//             acceleration[1] += +100;

//         intentions.clear();
//     }
// };

// class game_c
// {
// public:
//     std::shared_ptr<SDL_Window> window_p;
//     std::shared_ptr<SDL_Renderer> renderer_p;
//     std::map<std::string, std::shared_ptr<SDL_Texture>> textures;
//     std::vector<player_c> players;
//     //std::vector<obstacle_c> obstacles;


//     std::chrono::milliseconds dt;

//     std::vector<std::map<std::string, int>> keyboard_map;


// };

game_c initialize_all()
{
    game_c game;
    /// SDL
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);

    /// WINDOW
    game.window_p = std::shared_ptr<SDL_Window>(
        SDL_CreateWindow("Better Worms", SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, 640, 360, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE),
        [](auto* window) { SDL_DestroyWindow(window); });

    game.renderer_p = std::shared_ptr<SDL_Renderer>(
        SDL_CreateRenderer(game.window_p.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
        [](auto* renderer) {
            SDL_DestroyRenderer(renderer);
        });

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_RenderSetLogicalSize(game.renderer_p.get(), 640, 360);

    /// MEDIA
   
    game.textures["background"] = std::shared_ptr<SDL_Texture>(IMG_LoadTexture(game.renderer_p.get(), "data/ring.jpg"),
                                  [](auto *tex)
                                  { SDL_DestroyTexture(tex); });

    game.textures["player1"] = std::shared_ptr<SDL_Texture>(IMG_LoadTexture(game.renderer_p.get(), "data/g4.png"),
                                   [](auto *tex)
                                   { SDL_DestroyTexture(tex); });


    game.textures["stickman"] = std::shared_ptr<SDL_Texture>(IMG_LoadTexture(game.renderer_p.get(), "data/stickman.png"),
                                     [](auto *tex)
                                     { SDL_DestroyTexture(tex); });
    ////-----------

    /// PLAYERS
    //game.players.push_back(player_c({0.2, 10}));
    //game.players.push_back(player_c({1.5, 10}));


    /// OBSTACLES
    // obstacle_c o;
    // o.position = {30,10};
    // o.size = {2,5};
    // o.texture = "font_10_blue";
    // game.obstacles.push_back(o);


    /// physics details
    game.dt = std::chrono::milliseconds(15);

    /// keyboard mapping
    // player 1
    game.keyboard_map.push_back({{"right", SDL_SCANCODE_RIGHT},
        {"left", SDL_SCANCODE_LEFT},
        {"up", SDL_SCANCODE_UP},
        {"down", SDL_SCANCODE_DOWN}});
    // stickman
    game.keyboard_map.push_back({{"right", SDL_SCANCODE_D},
        {"left", SDL_SCANCODE_A},
        {"up", SDL_SCANCODE_W},
        {"down", SDL_SCANCODE_S}});
    


    return game;
}

int main(int, char **)
{
    using namespace std;
    using namespace std::chrono;
    using namespace tp::operators;

    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    shared_ptr<SDL_Window> window_p(
        SDL_CreateWindow("Better Worms", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, 640, 640, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE),
        [](auto *window)
        { SDL_DestroyWindow(window); });

    shared_ptr<SDL_Renderer> renderer_p(
        SDL_CreateRenderer(window_p.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
        [](auto *renderer)
        {
            SDL_DestroyRenderer(renderer);
        });

    // to scale window
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_RenderSetLogicalSize(renderer_p.get(), 640, 360);

    // IMG_LoadTexture - load photo from disk

    shared_ptr<SDL_Texture> tex_p(IMG_LoadTexture(renderer_p.get(), "data/ring.jpg"),
                                  [](auto *tex)
                                  { SDL_DestroyTexture(tex); });

    shared_ptr<SDL_Texture> tex_p1(IMG_LoadTexture(renderer_p.get(), "data/g4.png"),
                                   [](auto *tex)
                                   { SDL_DestroyTexture(tex); });


    shared_ptr<SDL_Texture> stickman(IMG_LoadTexture(renderer_p.get(), "data/stickman.png"),
                                     [](auto *tex)
                                     { SDL_DestroyTexture(tex); });

    player_c player;

    milliseconds dt(15);                                         // przyrost czasu
    steady_clock::time_point current_time = steady_clock::now(); // remember current time

    for (bool game_active = true; game_active;)
    {
        steady_clock::time_point frame_start = steady_clock::now();
        SDL_Event event;
        while (SDL_PollEvent(&event))
        { // check if there are some events
            if (event.type == SDL_QUIT)
                game_active = false;
        }
        //key pressed all the time or inactive
        auto kbdstate = SDL_GetKeyboardState(NULL);

        if (kbdstate[SDL_SCANCODE_RIGHT])
            player.intentions["right"] = 1;
        if (kbdstate[SDL_SCANCODE_LEFT])
            player.intentions["left"] = 1;
        if (kbdstate[SDL_SCANCODE_UP])
            player.intentions["up"] = 1;
        if (kbdstate[SDL_SCANCODE_DOWN])
            player.intentions["down"] = 1;


        /// fizyka
        double dt_f = dt.count() / 1000.0;
        player.apply_intent();
        player.update(dt_f, [&](auto p, auto pos, auto vel)
                      {
                          if (pos[1] < 30)
                          {
                              p->position = pos;
                              p->velocity = vel;
                              p->friction = 0.2;
                          }
                          else
                          {
                              p->velocity = {(vel[0] * vel[0] > 2.2) ? vel[0] : 0.0, 0};
                              p->position[0] = pos[0];
                              p->friction = 0.3;
                          }
                      });
        /// grafika
        SDL_SetRenderDrawColor(renderer_p.get(), 0, 100, 20, 255);
        SDL_RenderClear(renderer_p.get());
        SDL_SetRenderDrawColor(renderer_p.get(), 255, 100, 200, 255);

        // show photo 'tex_p' as background
        SDL_RenderCopy(renderer_p.get(), tex_p.get(), NULL, NULL);

        
        // show small photo 'tex_p' without rotation
        draw_o(renderer_p, player.position * 10.0, tex_p1, 100, 160, 0);

        // render, position_x, position_y, texture, n_freq, umber_sprites, w, h, dest_x, dest_y
        vk::draw_animation(renderer_p, 10, 10, stickman, 4, 0, 100, 50, 50, 100, 100);

        //------------------
        SDL_RenderPresent(renderer_p.get());

        this_thread::sleep_until(current_time = current_time + dt);

        steady_clock::time_point frame_end = steady_clock::now();

        // std::cout << "frame time: " << (frame_end - frame_start).count() << std::endl;
    }
    SDL_Quit();
    return 0;
}
