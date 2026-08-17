//
// crates.cpp
//
#include "crates.hpp"

#include "map/indirect-level.hpp"
#include "shadowman/settings.hpp"
#include "subsystem/context.hpp"
#include "util/sfml-util.hpp"
#include "util/texture-loader.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace shadowman
{

    Crate::Crate(const sf::Texture & t_texture, const sf::FloatRect & t_rect)
        : sprite(t_texture)
        , velocity{ 0.0f, 0.0f }
        , is_landed{ false }
    {
        util::fitAndCenterInside(sprite, t_rect);
    }

    CrateManager::CrateManager()
        : m_texture{}
        , m_crates{}
    {}

    void CrateManager::setup(const Context & t_context)
    {
        util::TextureLoader::load(
            m_texture, (t_context.setting.media_path / "image" / "crate.png"), true);
    }

    void CrateManager::add(const Context &, const std::vector<sf::FloatRect> & t_rects)
    {
        for (const sf::FloatRect & rect : t_rects)
        {
            m_crates.emplace_back(m_texture, rect);
        }
    }

    void CrateManager::update(const Context &, const float) {}

    void CrateManager::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        const sf::Vector2f offset{ t_context.level.mapToOffscreenOffset() };

        for (const Crate & crate : m_crates)
        {
            sf::Sprite tempSprite{ crate.sprite };
            tempSprite.move(offset);
            t_target.draw(tempSprite, t_states);
        }
    }

    void CrateManager::appendCollisionRects(std::vector<sf::FloatRect> & t_rects) const
    {
        for (const Crate & crate : m_crates)
        {
            t_rects.emplace_back(crate.sprite.getGlobalBounds());
        }
    }

    std::optional<std::reference_wrapper<Crate>>
        CrateManager::findIntersecting(const sf::FloatRect & t_rect)
    {
        for (Crate & crate : m_crates)
        {
            if (t_rect.findIntersection(crate.sprite.getGlobalBounds()))
            {
                return crate;
            }
        }

        return {};
    }

} // namespace shadowman
