// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// sound-player.cpp
//
#include "util/check-macros.hpp"
#include "util/sound-player.hpp"

#include <algorithm>
#include <sstream>

namespace util
{
    SoundPlayer::SoundPlayer(const Random & RANDOM)
        : m_random(RANDOM)
        , m_mediaPath(".")
        , m_isMuted(false)
        , m_volume(0.0f)
        , m_volumeMin(0.0f)                 // this is what sfml uses
        , m_volumeMax(100.0f)               // this is what sfml uses
        , m_volumeInc(m_volumeMax / 10.0f)  // only ten different vol levels possible
        , m_fileExtensions(".ogg.flac.wav") // dots are required here
        , m_soundEffects()
    {
        m_volume = (m_volumeMin + ((m_volumeMax - m_volumeMin) * 0.5f));
    }

    void SoundPlayer::reset()
    {
        stopAll();
        m_soundEffects.clear();
    }

    void SoundPlayer::play(const std::string & NAME, const float PITCH)
    {
        if (NAME.empty() || (m_volume < 1.0f))
        {
            return;
        }

        std::vector<std::size_t> nameMatchingIndexes(findCacheIndexesByName(NAME));
        if (nameMatchingIndexes.empty())
        {
            std::stringstream ss;
            ss << "WARNING:  No sfx by the name \"" << NAME << "\" is loaded and ready to play";

            if (!loadFiles(NAME))
            {
                M_LOG(
                    ss.str() << ", and none were found that could be loaded either.  So nothing "
                                "will happen.");

                return;
            }

            nameMatchingIndexes = findCacheIndexesByName(NAME);

            M_CHECK(
                !nameMatchingIndexes.empty(),
                ss.str()
                    << ", and then an internal error occurred trying to find and load any with "
                       "that name.  loadFiles() worked but findCacheIndexesByName() did not!");

            M_LOG(
                ss.str() << ", but was able to find and load " << nameMatchingIndexes.size()
                         << " matching sfx.  So one of those is gonna play now, but you need to "
                            "fix your code so that all sfx are loaded before trying to play them.");
        }

        sf::Sound & sfx = m_soundEffects.at(m_random.from(nameMatchingIndexes))->sound;

        sfx.setPitch(PITCH);

        if (sfx.isLooping() && (sfx.getStatus() == sf::SoundSource::Status::Playing))
        {
            return;
        }

        sfx.play();
    }

    void SoundPlayer::willLoop(const std::string & NAME, const bool WILL_LOOP)
    {
        M_CHECK(!NAME.empty(), "Name string is empty.");

        for (const std::size_t INDEX : findCacheIndexesByName(NAME))
        {
            m_soundEffects.at(INDEX)->sound.setLooping(WILL_LOOP);
        }
    }

    void SoundPlayer::stopAll()
    {
        for (auto & sfxUPtr : m_soundEffects)
        {
            sfxUPtr->sound.stop();
        }
    }

    void SoundPlayer::stopAllLooped()
    {
        for (auto & soundUPtr : m_soundEffects)
        {
            if (soundUPtr->sound.isLooping())
            {
                soundUPtr->sound.stop();
            }
        }
    }

    void SoundPlayer::stop(const std::string & NAME)
    {
        for (const std::size_t index : findCacheIndexesByName(NAME))
        {
            m_soundEffects.at(index)->sound.stop();
        }
    }

    void SoundPlayer::loadAll()
    {
        reset();
        loadFiles();
    }

    bool SoundPlayer::load(const std::initializer_list<std::string> NAMES)
    {
        bool success{ true };

        for (const std::string & name : NAMES)
        {
            if (!load(name))
            {
                success = false;
            }
        }

        return success;
    }

    bool SoundPlayer::load(const std::string & NAME)
    {
        M_CHECK(!NAME.empty(), "Name string is empty.");

        // check if already loaded
        if (!findCacheIndexesByName(NAME).empty())
        {
            M_LOG(
                "WARNING:  That sfx \""
                << NAME << "\" is already loaded.  Fix your code to only load an sfx once.");

            return true;
        }

        loadFiles(NAME);

        return !findCacheIndexesByName(NAME).empty();
    }

    std::vector<std::size_t> SoundPlayer::findCacheIndexesByName(const std::string & NAME) const
    {
        std::vector<std::size_t> indexes;

        if (NAME.empty())
        {
            return indexes;
        }

        indexes.reserve(m_soundEffects.size());

        for (std::size_t i(0); i < m_soundEffects.size(); ++i)
        {
            const bool startsWith{ m_soundEffects[i]->filename.find(NAME, 0) == 0 };
            if (startsWith)
            {
                indexes.push_back(i);
            }
        }

        return indexes;
    }

    void SoundPlayer::volumeUp()
    {
        if (m_isMuted)
        {
            m_isMuted = false;
        }

        volume(m_volumeMin + m_volumeInc);
    }

    void SoundPlayer::volumeDown()
    {
        if (m_isMuted)
        {
            return;
        }

        volume(m_volume - m_volumeInc);
    }

    void SoundPlayer::muteButton()
    {
        m_isMuted = !m_isMuted;

        if (m_isMuted)
        {
            volume(m_volumeMin);
        }
        else
        {
            volume(m_volume);
        }
    }

    void SoundPlayer::volume(const float NEW_VOLUME)
    {
        m_volume = std::clamp(NEW_VOLUME, m_volumeMin, m_volumeMax);

        for (auto & sfxUPtr : m_soundEffects)
        {
            sfxUPtr->sound.setVolume(m_volume);
        }
    }

    bool SoundPlayer::loadFiles(const std::string & NAME_MUST_MATCH)
    {
        M_CHECK(
            (std::filesystem::exists(m_mediaPath) && std::filesystem::is_directory(m_mediaPath)),
            "The media path \"" << m_mediaPath << "\" does not exist or is not a directory.");

        std::filesystem::recursive_directory_iterator dirIter(m_mediaPath);

        bool success{ false };
        for (const std::filesystem::directory_entry & entry : dirIter)
        {
            if (!willLoad(entry))
            {
                continue;
            }

            if (loadFile(entry, NAME_MUST_MATCH))
            {
                success = true;
            }
        }

        M_CHECK(
            success,
            "No sfx files found by the name \""
                << NAME_MUST_MATCH << "\" in \"" << m_mediaPath
                << "\".  Remember that MP3s are not supported, only " << m_fileExtensions);

        return true;
    }

    bool SoundPlayer::loadFile(
        const std::filesystem::directory_entry & ENTRY, const std::string & NAME_MUST_MATCH)
    {
        const std::string filename = ENTRY.path().filename().string();

        const bool filenameStartsWith = (filename.find(NAME_MUST_MATCH, 0) == 0);

        if (!NAME_MUST_MATCH.empty() && !filenameStartsWith)
        {
            return false;
        }

        auto sfxUPtr = std::make_unique<SoundEffect>();

        const bool LOAD_SUCCESS = sfxUPtr->buffer.loadFromFile(ENTRY.path().string());

        M_CHECK(
            LOAD_SUCCESS,
            "Found supported sfx file \"" << ENTRY.path().string()
                                          << "\", but an error occurred while loading it.  Check "
                                             "console for SFML error message.");

        sfxUPtr->sound.setBuffer(sfxUPtr->buffer);

        sfxUPtr->filename = filename;

        const bool sfxStartsWith = (sfxUPtr->filename.find(NAME_MUST_MATCH, 0) == 0);

        if (!NAME_MUST_MATCH.empty() && !sfxStartsWith)
        {
            return false;
        }

        if ((m_volume > 0.0f) && !m_isMuted)
        {
            sfxUPtr->sound.setVolume(m_volume);
        }
        else
        {
            sfxUPtr->sound.setVolume(0.0f);
        }

        // std::cout << "Loaded Sound Effect: " << sfxUPtr->toString() << std::endl;
        m_soundEffects.push_back(std::move(sfxUPtr));
        return true;
    }

    bool SoundPlayer::willLoad(const std::filesystem::directory_entry & ENTRY) const
    {
        if (!ENTRY.is_regular_file())
        {
            return false;
        }

        const std::string extension = ENTRY.path().filename().extension().string();

        if ((extension.size() != 4) && (extension.size() != 5))
        {
            return false;
        }

        return (m_fileExtensions.find(extension) < m_fileExtensions.size());
    }

    std::string SoundPlayer::SoundEffect::toString() const
    {
        const std::string pad("  ");

        std::ostringstream ss;

        ss << std::setw(20) << std::right;
        ss << filename << pad;

        // duration in seconds
        const auto durationMs(buffer.getDuration().asMilliseconds());
        const double durationSec(static_cast<double>(durationMs) / 1000.0);

        ss << std::setprecision(2) << std::setw(3) << std::setfill('0') << std::fixed;
        ss << durationSec << "s" << pad;

        // channels
        const auto channelCount(buffer.getChannelCount());
        if (1 == channelCount)
        {
            ss << "mono  ";
        }
        else if (2 == channelCount)
        {
            ss << "stereo";
        }
        else
        {
            ss << channelCount << "ch";
        }
        ss << pad;

        // sample rate in kHz
        const auto sampleRateHz(buffer.getSampleRate());
        const double sampleRakeKHz(static_cast<double>(sampleRateHz) / 1000.0);

        ss << std::setprecision(1) << std::setw(1) << std::setfill('0') << sampleRakeKHz << "kHz";

        return ss.str();
    }
} // namespace util
