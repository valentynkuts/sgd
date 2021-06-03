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
    game.player = player_c({10, 10});  // todo ????
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
    // game.keyboard_map.push_back({{"right", SDL_SCANCODE_RIGHT},
    //     {"left", SDL_SCANCODE_LEFT},
    //     {"up", SDL_SCANCODE_UP},
    //     {"down", SDL_SCANCODE_DOWN}});
    // stickman
    game.keyboard_map = std::map<std::string, int> {{"right", SDL_SCANCODE_RIGHT},
        {"left", SDL_SCANCODE_LEFT},
        {"up", SDL_SCANCODE_UP},
        {"down", SDL_SCANCODE_DOWN},};
    


    return game;
}

int process_input(game_c& game)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) { // check if there are some events
        if (event.type == SDL_QUIT)
            return false;
    }
    auto kbdstate = SDL_GetKeyboardState(NULL);
    game.player.intentions.clear();
        for (auto [k, v] : game.keyboard_map) {
            if (kbdstate[v]) game.player.intentions[k] = 1;
    }
    return true;
}
//-------------------
// int main(int, char **)
// {
//     using namespace std;
//     using namespace std::chrono;
//     using namespace tp::operators;

//     SDL_Init(SDL_INIT_EVERYTHING);
//     IMG_Init(IMG_INIT_PNG);
//     shared_ptr<SDL_Window> window_p(
//         SDL_CreateWindow("Better Worms", SDL_WINDOWPOS_UNDEFINED,
//                          SDL_WINDOWPOS_UNDEFINED, 640, 640, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE),
//         [](auto *window)
//         { SDL_DestroyWindow(window); });

//     shared_ptr<SDL_Renderer> renderer_p(
//         SDL_CreateRenderer(window_p.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
//         [](auto *renderer)
//         {
//             SDL_DestroyRenderer(renderer);
//         });

//     // to scale window
//     SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
//     SDL_RenderSetLogicalSize(renderer_p.get(), 640, 360);

//     // IMG_LoadTexture - load photo from disk

//     shared_ptr<SDL_Texture> tex_p(IMG_LoadTexture(renderer_p.get(), "data/ring.jpg"),
//                                   [](auto *tex)
//                                   { SDL_DestroyTexture(tex); });

//     shared_ptr<SDL_Texture> tex_p1(IMG_LoadTexture(renderer_p.get(), "data/g4.png"),
//                                    [](auto *tex)
//                                    { SDL_DestroyTexture(tex); });


//     shared_ptr<SDL_Texture> stickman(IMG_LoadTexture(renderer_p.get(), "data/stickman.png"),
//                                      [](auto *tex)
//                                      { SDL_DestroyTexture(tex); });

//     //player_c player();
//      player_c player;

//     milliseconds dt(15);                                         // przyrost czasu
//     steady_clock::time_point current_time = steady_clock::now(); // remember current time

//     for (bool game_active = true; game_active;)
//     {
//         steady_clock::time_point frame_start = steady_clock::now();
//         SDL_Event event;
//         while (SDL_PollEvent(&event))
//         { // check if there are some events
//             if (event.type == SDL_QUIT)
//                 game_active = false;
//         }
//         //key pressed all the time or inactive
//         auto kbdstate = SDL_GetKeyboardState(NULL);

//         if (kbdstate[SDL_SCANCODE_RIGHT])
//             player.intentions["right"] = 1;
//         if (kbdstate[SDL_SCANCODE_LEFT])
//             player.intentions["left"] = 1;
//         if (kbdstate[SDL_SCANCODE_UP])
//             player.intentions["up"] = 1;
//         if (kbdstate[SDL_SCANCODE_DOWN])
//             player.intentions["down"] = 1;


//         /// fizyka
//         double dt_f = dt.count() / 1000.0;
//         player.apply_intent();
//         // player.update1(dt_f, [&](auto p, auto pos, auto vel)
//         //               {
//         //                   if (pos[1] < 30)
//         //                   {
//         //                       p->position = pos;
//         //                       p->velocity = vel;
//         //                       p->friction = 0.2;
//         //                   }
//         //                   else
//         //                   {
//         //                       p->velocity = {(vel[0] * vel[0] > 2.2) ? vel[0] : 0.0, 0};
//         //                       p->position[0] = pos[0];
//         //                       p->friction = 0.3;
//         //                   }
//         //               });

//         player.update(dt_f);

//         /// grafika
//         SDL_SetRenderDrawColor(renderer_p.get(), 0, 100, 20, 255);
//         SDL_RenderClear(renderer_p.get());
//         SDL_SetRenderDrawColor(renderer_p.get(), 255, 100, 200, 255);

//         // show photo 'tex_p' as background
//         SDL_RenderCopy(renderer_p.get(), tex_p.get(), NULL, NULL);

        
//         // show small photo 'tex_p' without rotation
//         draw_o(renderer_p, player.position * 10.0, tex_p1, 100, 160, 0);

//         // render, position_x, position_y, texture, n_freq, umber_sprites, w, h, dest_x, dest_y
//         vk::draw_animation(renderer_p, 10, 10, stickman, 4, 0, 100, 50, 50, 100, 100);

//         //------------------
//         SDL_RenderPresent(renderer_p.get());

//         this_thread::sleep_until(current_time = current_time + dt);

//         steady_clock::time_point frame_end = steady_clock::now();

//         // std::cout << "frame time: " << (frame_end - frame_start).count() << std::endl;
//     }
//     SDL_Quit();
//     return 0;
// }

//----------------------


void draw_scene(game_c& game)
{
    using namespace tp::operators;
    using namespace vk;

    SDL_SetRenderDrawColor(game.renderer_p.get(), 0, 100, 20, 255);
    SDL_RenderClear(game.renderer_p.get());
    SDL_SetRenderDrawColor(game.renderer_p.get(), 255, 100, 200, 255);

     //-----------------------
    // for (auto &o: game.obstacles) {
    //     draw_obstacle(game.renderer_p, o.position * 10.0, game.textures.at(o.texture), o.size[0]*10, o.size[1]*10, 0);
    // }

    // // DRAW ALL PLAYERS
    // for (unsigned i = 0; i < game.players.size(); i++) {
    //     auto& player = game.players[i];
    //     draw_o(game.renderer_p, player.position * 10.0, game.textures.at("player[" + std::to_string(i) + "]"), 16, 16, player.position[0] * 36 + player.position[1] * 5);
    //     if (player.is_safe_place())
    //         draw_o(game.renderer_p, player.position * 10.0, game.textures.at("player[" + std::to_string(i) + "]"), 16 + 4, 16 + 4, player.position[0] * 36 + player.position[1] * 5);

    //     tp::draw_text(game.renderer_p, 10 + i * 130, 340, game.textures["font_10_red"], std::to_string((int)player.health));
    //     tp::draw_text(game.renderer_p, 10 + i * 130 + 40, 340, game.textures["font_10_blue"], std::to_string((int)player.points));
    // }
    // // DRAW ALL EMITTERS
    // for (unsigned i = 0; i < game.emitters.size(); i++) {
    //     auto& emitter = game.emitters[i];
    //     draw_o(game.renderer_p, emitter.position * 10.0, game.textures.at("emitter[" + std::to_string(i) + "]"), 16, 16, 0.0);
    // }
    // // DRAW ALL BULLETS
    // for (unsigned i = 0; i < game.bullets.size(); i++) {
    //     auto& bullet = game.bullets[i];
    //     draw_o(game.renderer_p, bullet.position * 10.0, game.textures.at(bullet.type), 10, 10, 33.0);
    // }
     //---------------------

    // show photo 'tex_p' as background
        //SDL_RenderCopy(game.renderer_p.get(), game.textures["background"], NULL, NULL);

        
        // show small photo 'tex_p' without rotation
        draw_o(game.renderer_p, game.player.position, game.textures["player1"], 100, 160, 0);

        // render, position_x, position_y, texture, n_freq, umber_sprites, w, h, dest_x, dest_y
        draw_animation(game.renderer_p, 10, 10, game.textures["stickman"], 4, 0, 100, 50, 50, 100, 100);


    SDL_RenderPresent(game.renderer_p.get());
}


int main(int, char**)
{
    using namespace std;
    using namespace std::chrono;

    auto game = initialize_all();
    steady_clock::time_point current_time = steady_clock::now(); // remember current time

    for (bool game_active = true; game_active;) {
        game_active = process_input(game);

         //TODO
         /// fizyka
        double dt_f = game.dt.count() / 100.0;
        //double dt_f = game.dt.count() / 1000.0;
        game.player.apply_intent();
        

        game.player.update(dt_f);

        draw_scene(game);

        /// grafika
        // SDL_SetRenderDrawColor(game.renderer_p.get(), 0, 100, 20, 255);
        // SDL_RenderClear(game.renderer_p.get());
        // SDL_SetRenderDrawColor(game.renderer_p.get(), 255, 100, 200, 255);

        // // show photo 'tex_p' as background
        // //SDL_RenderCopy(game.renderer_p.get(), game.textures["background"], NULL, NULL);

        
        // // show small photo 'tex_p' without rotation
        // draw_o(game.renderer_p, game.player.position, game.textures["player1"], 100, 160, 0);

        // // render, position_x, position_y, texture, n_freq, umber_sprites, w, h, dest_x, dest_y
        // vk::draw_animation(game.renderer_p, 10, 10, game.textures["stickman"], 4, 0, 100, 50, 50, 100, 100);

        // //------------------
        // SDL_RenderPresent(game.renderer_p.get());

        this_thread::sleep_until(current_time = current_time + game.dt);

        steady_clock::time_point frame_end = steady_clock::now();
    }
    SDL_Quit();
    return 0;
}
