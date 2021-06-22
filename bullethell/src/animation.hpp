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

    // renderer, position{x,y}, photo, photo_width, photo_height, rotation  (obrót)
    void draw_o(std::shared_ptr<SDL_Renderer> r, std::array<double, 2> p, std::shared_ptr<SDL_Texture> tex, double w, double h, double a)
    {
        SDL_Rect dst_rect = {(int)(p[0] - w / 2), (int)(p[1] - h / 2), (int)w, (int)h};
        //show photo 'tex'
        SDL_RenderCopyEx(r.get(), tex.get(), NULL, &dst_rect, a, NULL, SDL_RendererFlip::SDL_FLIP_NONE);

        std::cout << p[0] << "   " << p[1] << std::endl;
    }

    void draw_obstacle(std::shared_ptr<SDL_Renderer> r, std::array<double, 2> p, std::shared_ptr<SDL_Texture> tex, double w, double h)
    {
        //SDL_Rect dstrect = {p[0], p[1], w, h};
        SDL_Rect dstrect = {(int)p[0], (int)p[1], (int)w, (int)h};
        SDL_RenderCopy(r.get(), tex.get(), NULL, &dstrect);
    }

    void draw_obstacle_a(std::shared_ptr<SDL_Renderer> r, std::array<double, 2> p, std::shared_ptr<SDL_Texture> tex, double w, double h, double angle)
    {
        SDL_Rect dst_rect = {(int)p[0], (int)p[1], (int)w, (int)h};
        SDL_RenderCopyEx(r.get(), tex.get(), NULL, &dst_rect, angle, NULL, SDL_RendererFlip::SDL_FLIP_NONE);
    }

} // namespace vk
#endif
