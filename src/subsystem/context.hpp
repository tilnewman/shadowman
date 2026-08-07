#ifndef SUBSYSTEM_CONTEXT_HPP_INCLUDED
#define SUBSYSTEM_CONTEXT_HPP_INCLUDED
//
// context.hpp
//
namespace util
{
    class Random;
    // class MusicPlayer;
    class SoundPlayer;
} // namespace util

namespace shadowman
{

    struct Settings;

    class ScreenLayout;
    class FontManager;
    class StateManager;
    class Avatar;

    struct Context
    {
        Context(
            const Settings & t_settings,
            util::Random & t_random,
            util::SoundPlayer & t_soundPlayer,
            const ScreenLayout & t_screenLayout,
            const FontManager & t_fontManager,
            StateManager & t_stateManager,
            Avatar & t_avatar)
            : setting{ t_settings }
            , random{ t_random }
            , audio{ t_soundPlayer }
            , layout{ t_screenLayout }
            , font{ t_fontManager }
            , state{ t_stateManager }
            , avatar{ t_avatar }
        {}

        const Settings & setting;
        util::Random & random;
        util::SoundPlayer & audio;
        const ScreenLayout & layout;
        const FontManager & font;
        StateManager & state;
        Avatar & avatar;
    };

} // namespace shadowman

#endif // SUBSYSTEM_CONTEXT_HPP_INCLUDED
