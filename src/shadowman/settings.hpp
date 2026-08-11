#ifndef SHADOWMAN_SETTINGS_HPP_INCLUDED
#define SHADOWMAN_SETTINGS_HPP_INCLUDED
//
// settings.hpp
//
#include <filesystem>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/VideoMode.hpp>

namespace shadowman
{

    struct Settings
    {
        // see main.cpp for where this is really set
        std::filesystem::path media_path{ "./media" };

        // don't change, the game will display the actual raw framerate to track performance
        float framerate{ 60.0f };

        sf::VideoMode video_mode{ { 2056u, 1329u }, 32u };

        float avatar_scale{ 0.5f };
        float map_tile_scale{ 1.0f };
        float avatar_walk_moves_map_ratio{ 0.5f };

        float avatar_gravity{ 10.0f };
        float avatar_jump_speed{ 500.0f };
        float avatar_walk_acc{ 10.0f };
        float avatar_walk_speed_limit{ 3.0f };
        float avatar_run_acc{ 20.0f };
        float avatar_run_speed_limit{ 5.0f };
        float avatar_jump_horiz_move_divisor{ 6.0f };

        float fly_scale{ 0.6f };

        sf::Color map_outline_color{ 127, 127, 127 };

        sf::Color smoke_color{ 0, 0, 0 };
        float smoke_animation_time_between_frames{ 0.1f };
    };

} // namespace shadowman

#endif // SHADOWNMAN_SETTINGS_HPP_INCLUDED
