#ifndef UTIL_SOUND_PLAYER_HPP_INCLUDED
#define UTIL_SOUND_PLAYER_HPP_INCLUDED
//
// sound-player.hpp
//
#include "util/random.hpp"

#include <filesystem>
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

#include <SFML/Audio.hpp>

namespace util
{
    class SoundPlayer
    {
      public:
        explicit SoundPlayer(const Random & RANDOM);

        // prevent all copy and assignment
        SoundPlayer(const SoundPlayer &) = delete;
        SoundPlayer(SoundPlayer &&)      = delete;
        //
        SoundPlayer & operator=(const SoundPlayer &) = delete;
        SoundPlayer & operator=(SoundPlayer &&)      = delete;

        void reset();

        std::filesystem::path mediaPath() const { return m_mediaPath; }
        void mediaPath(const std::filesystem::path & MEDIA_PATH) { m_mediaPath = MEDIA_PATH; }

        void play(const std::string & NAME, const float PITCH = 1.0f);
        void willLoop(const std::string & NAME, const bool WILL_LOOP);

        void stop(const std::string & NAME);
        void stopAll();
        void stopAllLooped();

        void loadAll();

        bool load(const std::initializer_list<std::string> NAMES);
        bool load(const std::string & NAME);

        void volume(const float NEW_VOLUME);
        void volumeUp();
        void volumeDown();
        inline float volume() const { return m_volume; }

        void muteButton();
        inline bool isMuted() const { return m_isMuted; }

      private:
        std::vector<std::size_t> findCacheIndexesByName(const std::string & NAME) const;

        bool loadFiles(const std::string & NAME_MUST_MATCH = "");

        bool loadFile(
            const std::filesystem::directory_entry & ENTRY,
            const std::string & NAME_MUST_MATCH = "");

        bool willLoad(const std::filesystem::directory_entry & ENTRY) const;

        struct SoundEffect
        {
            SoundEffect()
                : filename{}
                , buffer{}
                , sound{ buffer }
                , will_loop{ false }
            {}

            std::string toString() const;

            std::string filename;
            sf::SoundBuffer buffer;
            sf::Sound sound;
            bool will_loop;
        };

      private:
        const Random & m_random;
        std::filesystem::path m_mediaPath;

        bool m_isMuted;

        float m_volume;
        float m_volumeMin;
        float m_volumeMax;
        float m_volumeInc;

        std::string m_fileExtensions;
        std::vector<std::unique_ptr<SoundEffect>> m_soundEffects;
    };
} // namespace util

#endif // UTIL_SOUND_PLAYER_HPP_INCLUDED
