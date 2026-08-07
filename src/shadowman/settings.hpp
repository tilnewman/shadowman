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

        sf::VideoMode video_mode{ { 1920u, 1080u }, 32u };

        float avatar_scale{ 0.5f };
    };

} // namespace shadowman

#endif // SHADOWNMAN_SETTINGS_HPP_INCLUDED
