//
// coordinator.hpp
//
#include "subsystem/coordinator.hpp"

#include "util/sfml-defaults.hpp"
#include "util/sfml-util.hpp"

#include <iostream>

#include <SFML/System/Clock.hpp>
#include <SFML/System/Sleep.hpp>

namespace shadowman
{

    Coordinator::Coordinator(const Settings & t_setting)
        : m_setting{ t_setting }
        , m_renderStates{}
        , m_windowUPtr{}
        , m_randomUPtr{}
        , m_soundPlayerUPtr{}
        , m_screenLayoutUPtr{}
        , m_avatarUPtr{}
        , m_fontManagerUPtr{}
        , m_contextUPtr{}
    {}

    void Coordinator::setup()
    {
        m_windowUPtr = std::make_unique<sf::RenderWindow>();
        setupRenderWindow(m_setting.video_mode);
        m_windowUPtr->setMouseCursorVisible(false);
        m_windowUPtr->setVerticalSyncEnabled(true);
        m_windowUPtr->setKeyRepeatEnabled(false);
        m_windowUPtr->setFramerateLimit(60);

        util::SfmlDefaults::instance().setup();

        m_randomUPtr       = std::make_unique<util::Random>();
        m_soundPlayerUPtr  = std::make_unique<util::SoundPlayer>(*m_randomUPtr);
        m_screenLayoutUPtr = std::make_unique<ScreenLayout>();
        m_avatarUPtr       = std::make_unique<Avatar>();
        m_fontManagerUPtr  = std::make_unique<FontManager>();

        m_contextUPtr = std::make_unique<Context>(
            m_setting, *m_randomUPtr, *m_soundPlayerUPtr, *m_screenLayoutUPtr, *m_fontManagerUPtr);

        m_soundPlayerUPtr->mediaPath(m_setting.media_path / "sound");
        m_soundPlayerUPtr->loadAll();
        m_soundPlayerUPtr->willLoop("walk", true);

        m_screenLayoutUPtr->setup(m_windowUPtr->getSize());
        m_fontManagerUPtr->setup(m_setting);
        m_avatarUPtr->setup(*m_contextUPtr);
    }

    void Coordinator::teardown()
    {
        m_soundPlayerUPtr->stopAll();
        m_soundPlayerUPtr->stopAllLooped();

        m_avatarUPtr->teardown();

        m_fontManagerUPtr.reset();
        m_screenLayoutUPtr.reset();
        m_soundPlayerUPtr.reset();
        m_randomUPtr.reset();
        m_avatarUPtr.reset();

        util::SfmlDefaults::instance().teardown();

        m_contextUPtr.reset();

        m_windowUPtr->close();
        m_windowUPtr.reset();

        // util::TextureLoader::dumpInfo();
    }

    void Coordinator::play()
    {
        setup();
        gameLoop();
        teardown();
    }

    void Coordinator::gameLoop()
    {
        sf::Clock frameClock;
        while (m_windowUPtr->isOpen())
        {
            frameClock.restart();

            handleEvents();
            update(frameClock.getElapsedTime().asSeconds());
            draw();
        }
    }

    void Coordinator::handleEvents()
    {
        while (const std::optional event = m_windowUPtr->pollEvent())
        {
            handleEvent(event.value());
        }
    }

    void Coordinator::handleEvent(const sf::Event & t_event)
    {
        if (t_event.is<sf::Event::Closed>())
        {
            m_windowUPtr->close();
            // m_stateUPtr->setChangePending(State::Shutdown);
        }
        else if (const auto * const keyPtr = t_event.getIf<sf::Event::KeyPressed>())
        {
            // TODO remove after testing
            if (keyPtr->scancode == sf::Keyboard::Scancode::Escape)
            {
                m_windowUPtr->close();
            }
        }

        // m_stateUPtr->current().handleEvent(*m_contextUPtr, t_event);
    }

    void Coordinator::draw()
    {
        m_windowUPtr->clear(sf::Color(100, 100, 127));
        m_avatarUPtr->draw(*m_windowUPtr, m_renderStates);
        // m_stateUPtr->current().draw(*m_contextUPtr, *m_windowUPtr, m_renderStates);
        // m_framerateDisplayUPtr->draw(*m_contextUPtr, *m_windowUPtr, m_renderStates);
        m_windowUPtr->display();
    }

    void Coordinator::update(const float t_elapsedSec)
    {
        m_avatarUPtr->update(*m_contextUPtr, t_elapsedSec);
        // m_stateUPtr->current().update(*m_contextUPtr, t_frameTimeSec);
        // m_stateUPtr->changeIfPending(*m_contextUPtr);
    }

    void Coordinator::setupRenderWindow(sf::VideoMode & t_videoMode)
    {
        std::cout << "Attempting video mode " << t_videoMode << "...";

        if (!m_setting.video_mode.isValid())
        {
            std::cout << "but that is not suported.  Valid video modes at "
                      << m_setting.video_mode.bitsPerPixel << "bpp:\n"
                      << util::makeSupportedVideoModesString(m_setting.video_mode.bitsPerPixel)
                      << '\n';

            t_videoMode = util::findVideoModeClosestTo(m_setting.video_mode);
            setupRenderWindow(t_videoMode);
            return;
        }

        m_windowUPtr->create(t_videoMode, "Bramblefore", sf::State::Fullscreen);

        // sometimes the resolution of the window created does not match what was specified
        const unsigned actualWidth  = m_windowUPtr->getSize().x;
        const unsigned actualHeight = m_windowUPtr->getSize().y;
        if ((m_setting.video_mode.size.x == actualWidth) &&
            (m_setting.video_mode.size.y == actualHeight))
        {
            std::cout << "Success." << std::endl;
        }
        else
        {
            std::cout << "Failed" << ".  ";

            m_setting.video_mode.size.x = actualWidth;
            m_setting.video_mode.size.y = actualHeight;

            std::cout << "Using " << m_setting.video_mode << " instead." << std::endl;
        }
    }

} // namespace shadowman
