/**
 * This is the simple game for SDL2 and SDL2_Image.
 *
 * You need C++14 to compile this.
 * 
 * Valentyn Kuts 2021
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

    std::cout << p[0] << "   " << p[1] << std::endl;
}

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
        [](auto *window)
        { SDL_DestroyWindow(window); });

    game.renderer_p = std::shared_ptr<SDL_Renderer>(
        SDL_CreateRenderer(game.window_p.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
        [](auto *renderer)
        {
            SDL_DestroyRenderer(renderer);
        });

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_RenderSetLogicalSize(game.renderer_p.get(), 640, 360);

    /// MEDIA
    
    game.textures["background"] = std::shared_ptr<SDL_Texture>(IMG_LoadTexture(game.renderer_p.get(), "data/bg0000.png"), //"data/ring.jpg"
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
    game.player = player_c({50, 50});
    //game.player = player_c();

    /// OBSTACLES
    // obstacle_c o;
    // o.position = {30,10};
    // o.size = {2,5};
    // o.texture = "font_10_blue";
    // game.obstacles.push_back(o);

    /// physics details
    game.dt = std::chrono::milliseconds(15);

    /// keyboard mapping

    game.keyboard_map = std::map<std::string, int>{
        // {"right", SDL_SCANCODE_RIGHT},
        // {"left", SDL_SCANCODE_LEFT},
        // {"up", SDL_SCANCODE_UP},
        // {"down", SDL_SCANCODE_DOWN},
        // {"stop", SDL_KEYUP},
        // {"jump_up", SDL_KEYUP},
        // {"jump_down", SDL_KEYDOWN},
        {"forward_roll", SDL_SCANCODE_D},
    };

    game.player.movements = {3, 3, 500};

    return game;
}

int process_input(game_c &game)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    { // check if there are some events
        if (event.type == SDL_QUIT)
            return false;

        //------------todo
        if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_UP)
            {
                game.player.set_movements({1, 2, 200}); //set down to jump
            }
            if (event.key.keysym.sym == SDLK_DOWN)
            {
                //game.player.set_movements({1, 3, 200});
            }
            if (event.key.keysym.sym == SDLK_RIGHT)
            {

                game.player.intentions["steps_right"] = 1;
            }
            if (event.key.keysym.sym == SDLK_LEFT)
            {
                game.player.intentions["steps_left"] = 1;
            }

            if (event.key.keysym.sym == SDLK_d)
            {
                game.player.intentions["forward_roll_right"] = 1;
            }

            if (event.key.keysym.sym == SDLK_q)
            {

                game.player.set_movements({1, 5, 200}); //set down to jump
            }
        }

        if (event.type == SDL_KEYUP)
        {
            if (event.key.keysym.sym == SDLK_UP)
            {
                game.player.intentions["jump_up"] = 1;
            }
            if (event.key.keysym.sym == SDLK_DOWN)
            {
            }
            if (event.key.keysym.sym == SDLK_RIGHT)
            {
                game.player.intentions["stop"] = 1;
            }
            if (event.key.keysym.sym == SDLK_LEFT)
            {
                game.player.intentions["stop"] = 1;
            }
            if (event.key.keysym.sym == SDLK_d)
            {
                game.player.intentions["stop"] = 1;
            }

            if (event.key.keysym.sym == SDLK_q)
            {
                game.player.intentions["forward_jump_right"] = 1;
            }
        }
        //---------------------
    }
    auto kbdstate = SDL_GetKeyboardState(NULL);
    //game.player.intentions.clear();
    //game.player.movements.clear();
    // for (auto [k, v] : game.keyboard_map)
    // {
    //     if (kbdstate[v])
    //         game.player.intentions[k] = 1;
    // }

    //if (kbdstate[SDL_SCANCODE_UP])
    // game.player.intentions["up"] = 1;

    //if (kbdstate[SDL_SCANCODE_W])
    //  game.player.intentions["forward_jump_test"] = 1;

    return true;
}

void process_physics(game_c &game)
{
    using namespace tp::operators;
    /// fizyka

    double dt_f = game.dt.count() / 100.0;
    //double dt_f = game.dt.count() / 1000.0;

    auto old_player = game.player;
    // update moves
    game.player.apply_intent();
    game.player.update(dt_f);
    //game.player.update2(dt_f);

    // // check colision between players
    // // they can interact with each other
    // for (unsigned i = 0; i < game.players.size(); i++) {
    //     for (unsigned j = i + 1; j < game.players.size(); j++) {
    //         if (length(game.players[i].position - game.players[j].position) < 1.0) {
    //             game.players[i].position = old_players[i].position;
    //             game.players[j].position = old_players[j].position;
    //             auto vec = game.players[i].position - game.players[j].position;
    //             vec = vec * (1.0 / length(vec));
    //             game.players[i].velocity = vec; //old_players[i].position;
    //             game.players[j].velocity = vec * -1.0;
    //         }
    //     }
    // }

    // for (unsigned i = 0; i < game.players.size(); i++) {
    //     auto &p = game.players[i];
    //     for (auto &o : game.obstacles) {
    //         if (!(((p.position[0]+0.7) < o.position[0]) ||
    //             ((p.position[0]-0.7) > (o.position[0]+o.size[0])) ||
    //             ((p.position[1]+0.7) < o.position[1]) ||
    //             ((p.position[1]-0.7) > (o.position[1]+o.size[1])))) {
    //                 p.position = old_players[i].position;
    //                 p.velocity = {0,0};
    //             }
    //     }
    // }

    // // TODO

    // check collisions with ground - always active
    // y - limit
    //int y = 152;
    int y = 250;
    if (game.player.position[1] < y) //
    {
        game.player.friction = 0, 3; // скорость падения
    }
    else
    {
        // game.player.velocity = {(game.player.velocity[0] * game.player.velocity[0] > 2.2) ? game.player.velocity[0] : 0.0, 0};
        // game.player.position[1] = y;
        // game.player.friction = 0.3;

        game.player.velocity = {(game.player.velocity[0] * game.player.velocity[0] > 2.2) ? game.player.velocity[0] : 0.0, 0.0};
        game.player.position[1] = y;
        game.player.friction = 0.3;
    }
}

void draw_scene(game_c &game)
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

    //  background 
    SDL_RenderCopy(game.renderer_p.get(), game.textures["background"].get(), NULL, NULL);

    // show small photo 'tex_p' without rotation
    // draw_o(game.renderer_p, game.player.position, game.textures["player1"], 100, 160, 0);

    ////draw_o(game.renderer_p, game.player.position, game.textures["background"], 1000, 1600, 0);

    // render, position_x, position_y, texture, n_freq, number_sprites, n_row, w, h, dest_x, dest_y
    //draw_animation(game.renderer_p, 10, 10, game.textures["stickman"], 4, 0, 100, 50, 50, 100, 100);
    //draw_animation(game.renderer_p, game.player.position[0],  game.player.position[1], game.textures["stickman"], game.player.movements[0], game.player.movements[1], game.player.movements[2], 50, 50, 100, 100);

    draw_animation(game.renderer_p, game.player.position, game.textures["stickman"], game.player.movements[0], game.player.movements[1], game.player.movements[2], 50, 50, 100, 100);

    SDL_RenderPresent(game.renderer_p.get());
}

int main(int, char **)
{
    using namespace std;
    using namespace std::chrono;

    auto game = initialize_all();
    steady_clock::time_point current_time = steady_clock::now(); // remember current time

    for (bool game_active = true; game_active;)
    {
        game_active = process_input(game);

        process_physics(game);
        draw_scene(game);
        
        this_thread::sleep_until(current_time = current_time + game.dt);

        steady_clock::time_point frame_end = steady_clock::now();
    }
    SDL_Quit();
    return 0;
}
