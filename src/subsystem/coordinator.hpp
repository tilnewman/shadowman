#ifndef SUBSYSTEM_COORDINATOR_HPP_INCLUDED
#define SUBSYSTEM_COORDINATOR_HPP_INCLUDED
//
// coordinator.hpp
//
#include "avatar/avatar.hpp"
#include "enemy/fly-manager.hpp"
#include "map/indirect-level.hpp"
#include "map/level-files.hpp"
#include "shadowman/settings.hpp"
#include "state/state-manager.hpp"
#include "subsystem/context.hpp"
#include "subsystem/font.hpp"
#include "subsystem/framerate-display.hpp"
#include "subsystem/screen-layout.hpp"
#include "subsystem/smoke.hpp"
#include "util/music-player.hpp"
#include "util/sound-player.hpp"

#include <memory>
#include <vector>

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>

namespace shadowman
{

    class Coordinator
    {
      public:
        explicit Coordinator(const Settings & t_setting);

        void play();

      private:
        void setup();
        void teardown();
        void gameLoop();
        void handleEvents();
        void handleEvent(const sf::Event & t_event);
        void update(const float t_frameTimeSec);
        void draw();
        void setupRenderWindow(sf::VideoMode & t_videoMode);
        void handleEndOfFrameTasks(const float t_elapsedTimeSec);

      private:
        Settings m_setting;
        sf::RenderStates m_renderStates;
        std::unique_ptr<sf::RenderWindow> m_windowUPtr;
        std::unique_ptr<util::Random> m_randomUPtr;
        std::unique_ptr<util::SoundPlayer> m_soundPlayerUPtr;
        std::unique_ptr<util::MusicPlayer> m_musicPlayerUPtr;
        std::unique_ptr<ScreenLayout> m_screenLayoutUPtr;
        std::unique_ptr<Avatar> m_avatarUPtr;
        std::unique_ptr<FontManager> m_fontManagerUPtr;
        std::unique_ptr<FramerateDisplay> m_framerateDisplayUPtr;
        std::unique_ptr<StateManager> m_stateManagerUPtr;
        std::unique_ptr<IndirectLevel> m_levelUPtr;
        std::unique_ptr<LevelFileManager> m_levelFileManagerUPtr;
        std::unique_ptr<FlyManager> m_flyManagerUPtr;
        std::unique_ptr<SmokeManager> m_smokeManager;

        std::unique_ptr<Context> m_contextUPtr;
    };

} // namespace shadowman

#endif // SUBSYSTEM_COORDINATOR_HPP_INCLUDED
