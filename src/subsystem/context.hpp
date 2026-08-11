#ifndef SUBSYSTEM_CONTEXT_HPP_INCLUDED
#define SUBSYSTEM_CONTEXT_HPP_INCLUDED
//
// context.hpp
//
namespace util
{
    class Random;
    class MusicPlayer;
    class SoundPlayer;
} // namespace util

namespace shadowman
{

    struct Settings;

    class ScreenLayout;
    class FontManager;
    class StateManager;
    class Avatar;
    class IndirectLevel;
    class LevelFileManager;

    struct Context
    {
        Context(
            const Settings & t_settings,
            util::Random & t_random,
            util::SoundPlayer & t_soundPlayer,
            util::MusicPlayer & t_musicPlayer,
            const ScreenLayout & t_screenLayout,
            const FontManager & t_fontManager,
            StateManager & t_stateManager,
            Avatar & t_avatar,
            IndirectLevel & t_indirectLevel,
            LevelFileManager & t_levelFileManager)
            : setting{ t_settings }
            , random{ t_random }
            , audio{ t_soundPlayer }
            , music{ t_musicPlayer }
            , layout{ t_screenLayout }
            , font{ t_fontManager }
            , state{ t_stateManager }
            , avatar{ t_avatar }
            , level{ t_indirectLevel }
            , level_file{ t_levelFileManager }
        {}

        const Settings & setting;
        util::Random & random;
        util::SoundPlayer & audio;
        util::MusicPlayer & music;
        const ScreenLayout & layout;
        const FontManager & font;
        StateManager & state;
        Avatar & avatar;
        IndirectLevel & level;
        LevelFileManager & level_file;
    };

} // namespace shadowman

#endif // SUBSYSTEM_CONTEXT_HPP_INCLUDED
