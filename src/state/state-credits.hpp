#ifndef STATE_STATE_CREDITS_HPP_INCLUDED
#define STATE_STATE_CREDITS_HPP_INCLUDED
//
// state-credits.hpp
//
#include "state/state.hpp"
#include "subsystem/sky-background.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <string>
#include <vector>

namespace sf
{
    class RenderTarget;
}

namespace shadowman
{
    struct Context;

    //

    struct CreditAnim
    {
        CreditAnim(
            const Context & t_context,
            const std::string & t_imageFilePath,
            const float t_imageScale,
            const std::string & t_name,
            const std::string & t_description,
            const float t_vertPos);

        void move(const float t_amount);
        [[nodiscard]] float bottom() const;
        void draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const;

        sf::Texture texture;
        sf::Sprite sprite;
        sf::Text name;
        sf::Text description;
    };

    //

    class StateCredits final : public IState
    {
      public:
        StateCredits();
        ~StateCredits() final {}

        State which() const final { return State::Credits; }
        void onEnter(const Context & t_context) final;
        void onExit(const Context & t_context) final;
        void update(const Context & t_context, const float t_frameTimeSec) final;
        void handleEvent(const Context & t_context, const sf::Event & t_event) final;

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const final;

      private:
        sf::Text m_titleText;
        std::vector<CreditAnim> m_credits;
        SkyBackground m_skyBackground;
    };

} // namespace shadowman

#endif // STATE_STATE_CREDITS_HPP_INCLUDED
