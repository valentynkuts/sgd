#ifndef __ANIMATION_VK_HPP___
#define __ANIMATION_VK_HPP___

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <memory>

namespace vk
{

    /**
 * animation
 * */
    void draw_animation(std::shared_ptr<SDL_Renderer> r,
                        std::array<double, 2> p,
                        std::shared_ptr<SDL_Texture> tex,
                        int number_sprites,
                        int n_row,
                        int n = 100,
                        int w = 50,
                        int h = 50,
                        int dest_w = 50,
                        int dest_h = 50)
    {

        Uint32 ticks = SDL_GetTicks(); //get  milliseconds
        // n - number that influences on  speed  // FrameRate
        // number_sprites - number of sprites in row
        // ticks / 1000 - get seconds
        Uint32 n_sprite = (ticks / n) % number_sprites; // number of sprite(frame) from 0 to number_sprites - 1
        // n_row - numder of row
        SDL_Rect srcrect = {n_sprite * w, n_row * h, w, h}; //{x, y, width_sprite, height_sprite}
        // dest_w, dest_h - sizes of animation to see
        // x - p[0], y - p[1]
        SDL_Rect dstrect = {p[0], p[1], dest_w, dest_h};
        SDL_RenderCopy(r.get(), tex.get(), &srcrect, &dstrect);

        // Points
        std::cout << p[0] << "   " << p[1] << std::endl;

        // SDL_RenderDrawPoint(r.get(), p[0] + 22, p[1] + 9);
        // SDL_RenderDrawPoint(r.get(), p[0] + 77, p[1] + 9);
        // SDL_RenderDrawPoint(r.get(), p[0] + 22, p[1] + 99);
        // SDL_RenderDrawPoint(r.get(), p[0] + 77, p[1] + 99);
    }

} // namespace vk
#endif
