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

     void update2(double dt_f)
    {
        using namespace tp::operators;
        // apply friction:
        auto new_acceleration = acceleration - velocity * length(velocity) * friction;
        auto new_velocity = velocity + new_acceleration * dt_f;
        auto new_position = position + new_velocity * dt_f + new_acceleration * dt_f * dt_f * 0.5;
         //std::array<double, 2> new_position = {0.0, 0.0};
        //////new_position[0] = position[0] + new_velocity[0]*cos(0.4f)*dt_f;
        //////new_position[1] = position[1] + new_velocity[1]*0.5*dt_f - new_acceleration[1] * dt_f * dt_f * 0.5;
        
        /*
        position[0]+= 0.6;
        position[1] = sin(0.1 * position[0]/ M_PI) * 200 + 240;
        
        std::array<double, 2> n_position = {position[0]+0.3, sin(0.1 * position[0]/ M_PI) * 200 + 240};
        auto new_position = n_position + new_velocity * dt_f + new_acceleration * dt_f * dt_f * 0.5;

        //position = {position[0],position[1]};
        */
        
        //new_position[1] = sin(0.1 * new_position[0]/ M_PI) * 200 + 240;
        position = new_position;
        velocity = new_velocity;
        acceleration = new_acceleration;

        
    }

    void update1(double dt_f, std::function<void(physical_c *, std::array<double, 2> &pos, std::array<double, 2> &vel)> callback_f)
    {
        using namespace tp::operators;
        // apply friction:
        auto new_acceleration = acceleration - velocity * length(velocity) * friction;
        auto new_velocity = velocity + new_acceleration * dt_f;
        auto new_position = position + new_velocity * dt_f + new_acceleration * dt_f * dt_f * 0.5;

        callback_f(this, new_position, new_velocity);

        //wind resistance todo
    }


};

class player_c : public physical_c
{
public:
    std::map<std::string, int> intentions;

    // vector<int> - {number_of_sprites_in_row, number_of_row, n_frequence}
    std::vector<int> movements;
   
    void set_movements(std::vector<int> m)
    {
        movements = m;
    }
    

    player_c(std::array<double, 2> position_ = {10, 10}, std::array<double, 2> velocity_ = {0, 0}, std::array<double, 2> acceleration_ = {0, 0}, double friction_ = 0.03)
    {
        position = position_;
        velocity = velocity_;
        acceleration = acceleration_;
        friction = friction_;

        //health = 100;
        //points = 0;
    }

    /**
 * applies and clears intentions
 * */
    void apply_intent()
    {
        //using namespace tp::operators;
        acceleration = {0, 5};
        //acceleration = acceleration + changes;

        if (intentions.count("stop"))
        {

            movements = {1, 3, 400};
        }
        if (intentions.count("steps_right"))
        {
            acceleration[0] += 100;
            // movements = {"move_right", {5, 0}};
            movements = {5, 0, 100};
        }
        if (intentions.count("steps_left"))
        {
            acceleration[0] += -100;
            // movements =  {"move_left", {5, 1}};
            movements = {5, 1, 100};
        }
        if (intentions.count("jump_up"))
        {
            acceleration[1] += -200;
            //acceleration[0] += -100;
            movements = {1, 3, 200};
        }

        if (intentions.count("forward_jump_right"))
        {
            acceleration[0] += 50;
            acceleration[1] += -200;
        
            movements = {1, 3, 100};
        }


        // if (intentions.count("down"))
        //     acceleration[1] += +100;

        if (intentions.count("forward_roll_right"))
        {
            acceleration[0] += 80;
             ////position[1] += 100;
            movements = {5, 4, 100};
        }
        //----------------------------
         if (intentions.count("up"))
        {
            acceleration[1] += -100;
            movements = {1, 3, 200};
        }

        if (intentions.count("forward_jump_test"))
        {
            acceleration[1] += -20;
            acceleration[0] += 20;
             ////position[1] += 100;
            movements = {1, 3, 100};
        }

        intentions.clear();
        movements.clear();
    }

    void apply_intent1()
    {
        acceleration = {0, 30};
        if (intentions.count("right"))
            acceleration[0] += 100;
        if (intentions.count("left"))
            acceleration[0] += -100;
        if (intentions.count("up"))
            acceleration[1] += -100;
        if (intentions.count("down"))
            acceleration[1] += +100;
    }

    bool is_safe_place()
    {
        return ((position[0] < 4.0) && (position[1] > 30) && (position[0] > 0.0) && (position[1] < 33));
    }
};
//-------------
class obstacle_c
{
public:
    std::array<double, 2> position;
    std::array<double, 2> size;
    std::string texture;
};

class game_c
{
public:
    std::shared_ptr<SDL_Window> window_p;
    std::shared_ptr<SDL_Renderer> renderer_p;
    std::map<std::string, std::shared_ptr<SDL_Texture>> textures;
    player_c player;
    
    std::vector<obstacle_c> obstacles;

    std::chrono::milliseconds dt;

    std::map<std::string, int> keyboard_map;
};

#endif
