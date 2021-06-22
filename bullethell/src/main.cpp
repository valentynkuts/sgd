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
#include <ctime>
#include "animation.hpp"
#include "bmpfont.hpp"
#include "main.hpp"

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

    game.textures["stickman"] = std::shared_ptr<SDL_Texture>(IMG_LoadTexture(game.renderer_p.get(), "data/stickman.png"),
                                                             [](auto *tex)
                                                             { SDL_DestroyTexture(tex); });

    game.textures["obstacle1"] = std::shared_ptr<SDL_Texture>(IMG_LoadTexture(game.renderer_p.get(), "data/ob.png"),
                                                              [](auto *tex)
                                                              { SDL_DestroyTexture(tex); });

    game.textures["win_fail"] = std::shared_ptr<SDL_Texture>(IMG_LoadTexture(game.renderer_p.get(), "data/win_fail.png"),
                                                             [](auto *tex)
                                                             { SDL_DestroyTexture(tex); });
    game.textures["prize"] = std::shared_ptr<SDL_Texture>(IMG_LoadTexture(game.renderer_p.get(), "data/prize.png"),
                                                          [](auto *tex)
                                                          { SDL_DestroyTexture(tex); });
    game.textures["font_10_blue"] = std::shared_ptr<SDL_Texture>(IMG_LoadTexture(game.renderer_p.get(), "data/oqls65n_blue.png"),
                                                                 [](auto *tex)
                                                                 { SDL_DestroyTexture(tex); });

    /// PLAYER
    game.player = player_c({5, 5});
    game.player.set_diff_x1x2y1y2({22, 77, 9, 99});

    game.player.movements = {3, 3, 500};
    game.prize_position = {550, 15};

    game.end_game = 100;

    // OBSTACLES
    game.obstacles = {
        obstacle_c({200, 270}, {70, 10}, "obstacle1", 0),
        obstacle_c({100, 80}, {50, 10}, "obstacle1", 0),
        obstacle_c({250, 150}, {90, 10}, "obstacle1", 0),
        obstacle_c({420, 100}, {70, 10}, "obstacle1", 0),
        obstacle_c({110, 220}, {70, 10}, "obstacle1", 0),
        obstacle_c({360, 300}, {100, 10}, "obstacle1", 0),
        obstacle_c({480, 320}, {70, 20}, "obstacle1", 0),
        obstacle_c({530, 70}, {50, 10}, "obstacle1", 0),
        obstacle_c({540, 150}, {30, 5}, "obstacle1", 0),
        obstacle_c({580, 280}, {50, 10}, "obstacle1", 0)};

    /// physics details
    game.dt = std::chrono::milliseconds(15);

    return game;
}

int process_input(game_c &game)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    { // check if there are some events
        if (event.type == SDL_QUIT)
            return false;

        //event.key.keysym.sym - variable stores the value of the button.
        if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_UP)
            {
                game.player.set_movements({1, 2, 200}); //set down to jump
            }
            if (event.key.keysym.sym == SDLK_RIGHT)
            {

                game.player.intentions["steps_right"] = 1;
            }
            if (event.key.keysym.sym == SDLK_LEFT)
            {
                game.player.intentions["steps_left"] = 1;
            }

            if (event.key.keysym.sym == SDLK_s)
            {
                game.player.intentions["forward_roll_right"] = 1;
            }

            if (event.key.keysym.sym == SDLK_a)
            {
                game.player.intentions["forward_roll_left"] = 1;
            }

            if (event.key.keysym.sym == SDLK_w)
            {

                game.player.set_movements({1, 5, 200}); //set down to jump right
            }
            if (event.key.keysym.sym == SDLK_q)
            {

                game.player.set_movements({1, 6, 200}); //set down to jump left
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
            if (event.key.keysym.sym == SDLK_s)
            {
                game.player.intentions["stop"] = 1;
            }

            if (event.key.keysym.sym == SDLK_a)
            {
                game.player.intentions["stop"] = 1;
            }

            if (event.key.keysym.sym == SDLK_w)
            {
                game.player.intentions["forward_jump_right"] = 1;
            }
            if (event.key.keysym.sym == SDLK_q)
            {
                game.player.intentions["forward_jump_left"] = 1;
            }
        }
    }

    return true;
}

void process_physics(game_c &game)
{
    using namespace tp::operators;

    /// fizyka

    double dt_f = game.dt.count() / 100.0;

    auto old_player = game.player;

    // update moves
    game.player.apply_intent();
    game.player.update(dt_f);

    for (auto &o : game.obstacles)
    {

        if (game.player.movements[3] && !game.collision(game.player, o))
        {
            game.player.position[1] = old_player.position[1];
            std::cout << "-----------------------steps" << std::endl;
        }
        if (!game.collision(game.player, o))
        {

            {
                std::cout << "--------------------OLD--" << std::endl;
                game.player.position = old_player.position;
                game.player.velocity = {0, 0};
            }
        }
    }

    // check collisions with ground - always active
    // y - limit
    //int y = 152;
    int y = 250;
    if (game.player.position[1] < y)
    {
        game.player.friction = 0, 3; // influence on speed of falling down
    }
    else
    {

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

    //  background
    SDL_RenderCopy(game.renderer_p.get(), game.textures["background"].get(), NULL, NULL);

    //  obstacles
    for (auto &o : game.obstacles)
    {
        draw_obstacle_a(game.renderer_p, o.position, game.textures.at(o.texture), o.size[0], o.size[1], o.angle);
    }
    
    //  prize
    draw_animation(game.renderer_p, game.prize_position, game.textures["prize"], 1, 0, 100, 20, 20, 30, 30);

    //  stickman
    draw_animation(game.renderer_p, game.player.position, game.textures["stickman"], game.player.movements[0], game.player.movements[1], game.player.movements[2], 50, 50, 100, 100);

    // timer
    Uint32 ticks = SDL_GetTicks();
    int seconds = ticks / 1000;
    tp::draw_text(game.renderer_p, 22, 20, game.textures["font_10_blue"], std::to_string(seconds));

    // failed
    if (seconds > game.end_game)
        draw_animation(game.renderer_p, {250, 100}, game.textures["win_fail"], 1, 1, 100, 200, 100, 200, 200);

    // win
    if (game.player.is_winner())
        draw_animation(game.renderer_p, {250, 100}, game.textures["win_fail"], 1, 0, 100, 200, 100, 200, 200);

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
