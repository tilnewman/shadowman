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

        sf::VideoMode video_mode{ { 2056u, 1329u }, 32u };

        float avatar_scale{ 0.5f };
        float map_tile_scale{ 1.0f };
        float avatar_walk_moves_map_ratio{ 0.5f };

        sf::Color map_outline_color{ 127, 127, 127 };
    };

} // namespace shadowman

#endif // SHADOWNMAN_SETTINGS_HPP_INCLUDED
