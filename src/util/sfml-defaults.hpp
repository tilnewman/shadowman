#ifndef UTIL_SFML_DEFAULTS_HPP_INCLUDED
#define UTIL_SFML_DEFAULTS_HPP_INCLUDED
//
// sfml-defaults.hpp
//
#include "util/check-macros.hpp"

#include <memory>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace util
{

    // Ever since SFML 3.0 sprites, text, and sounds have no default constructors.
    // Everywhere you have one you need it's corresponding resource object,
    // which is kinda ridiculous.  So I'm forced to make this singleton that can
    // be accessed anywhere and provide dummy resources for Text and Textures.
    class SfmlDefaults
    {
      private:
        SfmlDefaults()
            : m_fontUPtr{}
            , m_textureUPtr{}
        {}

      public:
        static SfmlDefaults & instance()
        {
            static SfmlDefaults instance;
            return instance;
        }

        void setup()
        {
            m_fontUPtr    = std::make_unique<sf::Font>();
            m_textureUPtr = std::make_unique<sf::Texture>();
        }

        void teardown()
        {
            m_fontUPtr.reset();
            m_textureUPtr.reset();
        }

        const sf::Font & font() const
        {
            M_CHECK(m_fontUPtr, "SfmlDefaults::font() called before setup()!");
            return *m_fontUPtr;
        }

        const sf::Texture & texture() const
        {
            M_CHECK(m_textureUPtr, "SfmlDefaults::texture() called before setup()!");
            return *m_textureUPtr;
        }

      private:
        std::unique_ptr<sf::Font> m_fontUPtr;
        std::unique_ptr<sf::Texture> m_textureUPtr;
    };

} // namespace util

#endif // UTIL_SFML_DEFAULTS_HPP_INCLUDED
