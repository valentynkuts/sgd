#ifndef ___MAIN_CLASS_FOR_BULLETHELL_HPP__
#define ___MAIN_CLASS_FOR_BULLETHELL_HPP__

#include <array>
#include "vectors.hpp"
#include <map>
#include <thread>
#include <vector>
#include <SDL2/SDL.h>

class physical_c
{
public:
    std::array<double, 2> position;
    std::array<double, 2> velocity;
    std::array<double, 2> acceleration;
    double friction;

    void update(double dt_f)
    {
        using namespace tp::operators;
        // apply friction:
        auto new_acceleration = acceleration - velocity * length(velocity) * friction;
        auto new_velocity = velocity + new_acceleration * dt_f;
        auto new_position = position + new_velocity * dt_f + new_acceleration * dt_f * dt_f * 0.5;

        position = new_position;
        velocity = new_velocity;
        acceleration = new_acceleration;
    }
};

class player_c : public physical_c
{
public:
    std::map<std::string, int> intentions;

    // vector<int> - {number_of_sprites_in_row, number_of_row, n_frequence, flag}
    // flag - can be 0 (to move only top, ...) or 1 (to move only left , right)
    std::vector<int> movements;
    std::array<int, 4> diff_x1x2y1y2;

    void set_movements(std::vector<int> m)
    {
        movements = m;
    }

    void set_diff_x1x2y1y2(std::array<int, 4> d)
    {
        diff_x1x2y1y2 = d;
    }

    player_c(std::array<double, 2> position_ = {10, 10}, std::array<double, 2> velocity_ = {0, 0}, std::array<double, 2> acceleration_ = {0, 0}, double friction_ = 0.03)
    {
        position = position_;
        velocity = velocity_;
        acceleration = acceleration_;
        friction = friction_;
    }

    /**
 * applies and clears intentions
 * */
    void apply_intent()
    {

        acceleration = {0, 5};

        if (intentions.count("stop"))
        {

            movements = {1, 3, 400, 0};
            diff_x1x2y1y2 = {20, 77, 9, 99};
        }
        if (intentions.count("steps_right"))
        {
            acceleration[0] += 100;
            movements = {5, 0, 100, 1};
            diff_x1x2y1y2 = {20, 77, 9, 99};
        }
        if (intentions.count("steps_left"))
        {
            acceleration[0] += -100;
            movements = {5, 1, 100, 1};
            diff_x1x2y1y2 = {20, 77, 9, 99};
        }
        if (intentions.count("jump_up"))
        {
            acceleration[1] += -200;
            movements = {1, 3, 200, 0};
            diff_x1x2y1y2 = {20, 77, 9, 99};
        }

        if (intentions.count("forward_jump_right"))
        {
            acceleration[0] += 50;
            acceleration[1] += -200;
            movements = {1, 3, 100, 0};
            diff_x1x2y1y2 = {20, 77, 9, 99};
        }

        if (intentions.count("forward_jump_left"))
        {
            acceleration[0] -= 50;
            acceleration[1] += -200;
            movements = {1, 3, 100, 0};
            diff_x1x2y1y2 = {20, 77, 9, 99};
        }

        if (intentions.count("forward_roll_right"))
        {
            acceleration[0] += 80;
            movements = {4, 4, 100, 1};
            diff_x1x2y1y2 = {20, 77, 37, 99};
        }

        if (intentions.count("forward_roll_left"))
        {
            acceleration[0] -= 80;
            movements = {4, 7, 100, 1};
            diff_x1x2y1y2 = {20, 77, 37, 99};
        }

        intentions.clear();
        movements.clear();
    }

    bool is_safe_place()
    {
        return ((position[0] < 4.0) && (position[1] > 30) && (position[0] > 0.0) && (position[1] < 33));
    }

    bool is_winner()
    {
        return ((position[0] + 30 < 560.0) && (position[1] + 50 > 10) && (position[0] + 30 > 530.0) && (position[1] + 50 < 30));
    }
};

class obstacle_c
{
public:
    std::array<double, 2> position; // left top - x, y
    std::array<double, 2> size;     //  w, h
    std::string texture;
    int angle;

    obstacle_c(std::array<double, 2> position_ = {50, 50}, std::array<double, 2> size_ = {5, 5}, std::string texture_ = "obstacle1", int angle_ = 0)
    {
        position = position_;
        size = size_;
        texture = texture_;
        angle = angle_;
    }
};

class game_c
{
public:
    std::shared_ptr<SDL_Window> window_p;
    std::shared_ptr<SDL_Renderer> renderer_p;
    std::map<std::string, std::shared_ptr<SDL_Texture>> textures;
    std::array<double, 2> prize_position;
    player_c player;
    int end_game;

    std::vector<obstacle_c> obstacles;

    std::chrono::milliseconds dt;

    std::map<std::string, int> keyboard_map;

    bool collision(player_c p, obstacle_c o)
    {

        if (p.position[1] + p.diff_x1x2y1y2[2] > o.position[1] + o.size[1])
        {
            return true;
        }

        if (p.position[1] + p.diff_x1x2y1y2[3] < o.position[1])
        {
            return true;
        }

        if (p.position[0] + p.diff_x1x2y1y2[0] > o.position[0] + o.size[0])
        {
            return true;
        }

        if (p.position[0] + p.diff_x1x2y1y2[1] < o.position[0])
        {
            return true;
        }

        return false;
    }
};

#endif
