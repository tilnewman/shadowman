#ifndef UTIL_MUSIC_PLAYER_HPP_INCLUDED
#define UTIL_MUSIC_PLAYER_HPP_INCLUDED
//
// msuic-player.hpp
//
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <SFML/Audio/Music.hpp>

namespace util
{
    class MusicPlayer
    {
      public:
        MusicPlayer();
        ~MusicPlayer();

        void setup(const std::filesystem::path & t_path);

        void reset();

        void start(const std::string & t_filename, const float t_volume = 100.0f);

        void pause(const std::string & t_filename);
        void pauseAll();

        void stop(const std::string & t_filename);
        void stopAll();

        [[nodiscard]] float volume(const std::string & t_filename) const;
        void volume(const std::string & t_filename, const float t_newVolume);

      private:
        struct Entry
        {
            std::string filename{};
            sf::Music music{};
        };

      private:
        std::filesystem::path m_path;
        std::vector<std::unique_ptr<Entry>> m_entrys;
    };
} // namespace util

#endif // UTIL_MUSIC_PLAYER_HPP_INCLUDED
