#ifndef SUBSYSTEM_COORDINATOR_HPP_INCLUDED
#define SUBSYSTEM_COORDINATOR_HPP_INCLUDED
//
// coordinator.hpp
//
#include "shadowman/settings.hpp"
#include "subsystem/context.hpp"
#include "subsystem/screen-layout.hpp"
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

      private:
        Settings m_setting;
        sf::RenderStates m_renderStates;

        std::unique_ptr<sf::RenderWindow> m_windowUPtr;
        std::unique_ptr<util::Random> m_randomUPtr;
        std::unique_ptr<util::SoundPlayer> m_soundPlayerUPtr;
        std::unique_ptr<ScreenLayout> m_screenLayoutUPtr;
        // std::unique_ptr<StateManager> m_stateUPtr;

        std::unique_ptr<Context> m_contextUPtr;
    };

} // namespace shadowman

#endif // SUBSYSTEM_COORDINATOR_HPP_INCLUDED
