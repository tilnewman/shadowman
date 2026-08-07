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

    //

    struct Context
    {
        Context(
            const Settings & t_settings,
            util::Random & t_random,
            util::SoundPlayer & t_soundPlayer)
            : setting{ t_settings }
            , random{ t_random }
            , audio{ t_soundPlayer }
        {}

        const Settings & setting;
        util::Random & random;
        util::SoundPlayer & audio;
    };

} // namespace shadowman

#endif // SUBSYSTEM_CONTEXT_HPP_INCLUDED
