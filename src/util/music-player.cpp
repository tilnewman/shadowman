// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// music-player.cpp
//
#include "music-player.hpp"

#include <iostream>

namespace util
{
    MusicPlayer::MusicPlayer()
        : m_path()
        , m_entrys()
    {}

    MusicPlayer::~MusicPlayer() { reset(); }

    void MusicPlayer::setup(const std::filesystem::path & t_path)
    {
        m_path = t_path;

        if (!std::filesystem::exists(m_path) || !std::filesystem::is_directory(m_path))
        {
            std::cerr << "MusicPlayer::setup() given a path that is not an existing directory: "
                      << m_path << std::endl;
        }
    }

    void MusicPlayer::reset()
    {
        stopAll();
        m_entrys.clear();
    }

    void MusicPlayer::start(const std::string & t_filename, const float t_volume)
    {
        const std::filesystem::path path = (m_path / t_filename);
        if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path))
        {
            std::cerr << "MusicPlayer::start(\"" << t_filename << "\", " << t_volume
                      << ") -but that file does not exist!" << m_path << std::endl;

            return;
        }

        // start if already loaded
        for (auto & entryUPtr : m_entrys)
        {
            if (entryUPtr->filename == t_filename)
            {
                entryUPtr->music.setVolume(t_volume);

                if (entryUPtr->music.getStatus() != sf::SoundSource::Status::Playing)
                {
                    entryUPtr->music.setLooping(true);
                    entryUPtr->music.play();
                }

                return;
            }
        }

        // load and start new
        auto & entryUPtr = m_entrys.emplace_back(std::make_unique<Entry>());

        if (!entryUPtr->music.openFromFile(path.string()))
        {
            std::cerr << "MusicPlayer::start(\"" << t_filename << "\", " << t_volume
                      << ") found the file but was unable to load it." << m_path << std::endl;

            m_entrys.pop_back();
            return;
        }

        entryUPtr->filename = t_filename;
        entryUPtr->music.setVolume(t_volume);
        entryUPtr->music.setLooping(true);
        entryUPtr->music.play();
    }

    void MusicPlayer::pause(const std::string & t_filename)
    {
        for (auto & entryUPtr : m_entrys)
        {
            if (entryUPtr->filename == t_filename)
            {
                if (entryUPtr->music.getStatus() == sf::SoundSource::Status::Paused)
                {
                    entryUPtr->music.play();
                }
                else
                {
                    entryUPtr->music.pause();
                }

                return;
            }
        }
    }

    void MusicPlayer::pauseAll()
    {
        for (auto & entryUPtr : m_entrys)
        {
            if (entryUPtr->music.getStatus() == sf::SoundSource::Status::Paused)
            {
                entryUPtr->music.play();
            }
            else
            {
                entryUPtr->music.pause();
            }
        }
    }

    void MusicPlayer::stop(const std::string & t_filename)
    {
        for (auto & entryUPtr : m_entrys)
        {
            if (entryUPtr->filename == t_filename)
            {
                entryUPtr->music.stop();
                return;
            }
        }
    }

    void MusicPlayer::stopAll()
    {
        for (auto & entryUPtr : m_entrys)
        {
            entryUPtr->music.stop();
        }
    }

    float MusicPlayer::volume(const std::string & t_filename) const
    {
        for (auto & entryUPtr : m_entrys)
        {
            if (entryUPtr->filename == t_filename)
            {
                return entryUPtr->music.getVolume();
            }
        }

        std::cerr << "MusicPlayer::getVolume(\"" << t_filename
                  << "\") -but there are none with that filename!" << m_path << std::endl;

        return 0.0f;
    }

    void MusicPlayer::volume(const std::string & t_filename, const float t_newVolume)
    {
        for (auto & entryUPtr : m_entrys)
        {
            if (entryUPtr->filename == t_filename)
            {
                entryUPtr->music.setVolume(t_newVolume);
                return;
            }
        }

        std::cerr << "MusicPlayer::setVolume(\"" << t_filename << "\", " << t_newVolume
                  << ") -but there are none with that filename!" << m_path << std::endl;
    }

} // namespace util
