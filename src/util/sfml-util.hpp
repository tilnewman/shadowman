#ifndef UTIL_SFML_UTIL_HPP_INCLUDED
#define UTIL_SFML_UTIL_HPP_INCLUDED
//
// sfml-util.hpp
//
#include "util/util.hpp"

#include <algorithm>
#include <cmath>
#include <initializer_list>
#include <iterator>
#include <ostream>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/VideoMode.hpp>

namespace sf
{
    using Vector2s = sf::Vector2<std::size_t>;
}

namespace util
{

    // two triangles with three verts each, see setupTriangleVerts() below
    constexpr std::size_t verts_per_quad{ 6 };

    [[nodiscard]] inline std::string colorToString(const sf::Color & C)
    {
        std::string str;
        str.reserve(16);

        str += '(';

        if (sf::Color::Black == C)
        {
            str += "Black";
        }
        else if (sf::Color::White == C)
        {
            str += "White";
        }
        else if (sf::Color::Red == C)
        {
            str += "Red";
        }
        else if (sf::Color::Green == C)
        {
            str += "Green";
        }
        else if (sf::Color::Blue == C)
        {
            str += "Blue";
        }
        else if (sf::Color::Yellow == C)
        {
            str += "Yellow";
        }
        else if (sf::Color::Magenta == C)
        {
            str += "Magenta";
        }
        else if (sf::Color::Cyan == C)
        {
            str += "Cyan";
        }
        else
        {
            str += std::to_string(static_cast<unsigned>(C.r));
            str += ',';
            str += std::to_string(static_cast<unsigned>(C.g));
            str += ',';
            str += std::to_string(static_cast<unsigned>(C.b));

            if (C.a != 255)
            {
                str += ';';
                str += std::to_string(static_cast<unsigned>(C.a));
            }
        }

        str += ')';

        return str;
    }
} // namespace util

namespace sf
{

    inline std::ostream & operator<<(std::ostream & os, const sf::Color & C)
    {
        os << util::colorToString(C);
        return os;
    }

    template <typename T>
    [[nodiscard]] bool operator<(const sf::Vector2<T> & left, const sf::Vector2<T> & right)
    {
        if (left.x != right.x)
        {
            return (left.x < right.x);
        }
        else
        {
            return (left.y < right.y);
        }
    }

    template <typename T>
    [[nodiscard]] bool operator<=(const sf::Vector2<T> & left, const sf::Vector2<T> & right)
    {
        return ((left == right) || (left < right));
    }

    template <typename T>
    [[nodiscard]] sf::Vector2<T>
        operator*(const sf::Vector2<T> & left, const sf::Vector2<T> & right)
    {
        return { (left.x * right.x), (left.y * right.y) };
    }

    template <typename T>
    [[nodiscard]] sf::Vector2<T>
        operator/(const sf::Vector2<T> & numerator, const sf::Vector2<T> & denominator)
    {
        return { (numerator.x / denominator.x), (numerator.y / denominator.y) };
    }

    //

    template <typename T>
    [[nodiscard]] bool operator<(const sf::Rect<T> & r1, const sf::Rect<T> & r2)
    {
        return (
            std::tie(r1.position.y, r1.position.x, r1.size.x, r1.size.y) <
            std::tie(r2.position.y, r2.position.x, r2.size.x, r2.size.y));
    }

    //

    template <typename T>
    std::ostream & operator<<(std::ostream & os, const sf::Vector2<T> & vec)
    {
        os << '(' << vec.x << 'x' << vec.y << ')';
        return os;
    }

    template <typename T>
    std::ostream & operator<<(std::ostream & os, const sf::Rect<T> & rect)
    {
        os << '(' << rect.position.x << ',' << rect.position.y << '/' << rect.size.x << 'x'
           << rect.size.y << ')';
        return os;
    }

    inline std::ostream & operator<<(std::ostream & os, const sf::VideoMode & vm)
    {
        os << "(" << vm.size.x << "x" << vm.size.y << ":" << vm.bitsPerPixel << "bpp";

        if (!vm.isValid())
        {
            os << "(invalid)";
        }

        os << ")";

        return os;
    }

} // namespace sf

//

namespace util
{

    // floor

    template <typename T>
    void floor(sf::Rect<T> & rect)
    {
        rect.position.x = std::floor(rect.position.x);
        rect.position.y = std::floor(rect.position.y);
        rect.size.x     = std::floor(rect.size.x);
        rect.size.y     = std::floor(rect.size.y);
    }

    template <typename T>
    [[nodiscard]] const sf::Rect<T> floorCopy(const sf::Rect<T> & rect)
    {
        auto temp = rect;
        floor(temp);
        return temp;
    }

    template <typename T>
    void floor(sf::Vector2<T> & vec)
    {
        vec.x = std::floor(vec.x);
        vec.y = std::floor(vec.y);
    }

    template <typename T>
    [[nodiscard]] const sf::Vector2<T> floorCopy(const sf::Vector2<T> & vec)
    {
        auto temp = vec;
        floor(temp);
        return temp;
    }

    // position, size, and center

    template <typename T>
    [[nodiscard]] const sf::Vector2f position(const T & thing)
    {
        return thing.getGlobalBounds().position;
    }

    template <typename T>
    [[nodiscard]] const sf::Vector2f positionLocal(const T & thing)
    {
        return thing.getLocalBounds().position;
    }

    template <typename T>
    [[nodiscard]] T right(const sf::Rect<T> & rect)
    {
        return (rect.position.x + rect.size.x);
    }

    template <typename T>
    [[nodiscard]] float right(const T & thing)
    {
        return right(thing.getGlobalBounds());
    }

    template <typename T>
    [[nodiscard]] T bottom(const sf::Rect<T> & rect)
    {
        return (rect.position.y + rect.size.y);
    }

    template <typename T>
    [[nodiscard]] float bottom(const T & thing)
    {
        return bottom(thing.getGlobalBounds());
    }

    template <typename T>
    [[nodiscard]] const sf::Vector2f size(const T & thing)
    {
        return thing.getGlobalBounds().size;
    }

    template <typename T>
    [[nodiscard]] const sf::Vector2f sizeLocal(const T & thing)
    {
        return thing.getLocalBounds().size;
    }

    template <typename T>
    [[nodiscard]] const sf::Vector2<T> center(const sf::Rect<T> & rect)
    {
        return (rect.position + (rect.size / T(2)));
    }

    template <typename T>
    [[nodiscard]] const sf::Vector2f center(const T & thing)
    {
        return center(thing.getGlobalBounds());
    }

    template <typename T>
    [[nodiscard]] const sf::Vector2f centerLocal(const T & thing)
    {
        return center(thing.getLocalBounds());
    }

    template <typename T>
    void setOriginToCenter(T & thing)
    {
        thing.setOrigin(centerLocal(thing));
    }

    // sf::Text needs this correction after changing the string, scale, or characterSize
    template <typename T>
    void setOriginToPosition(T & thing)
    {
        thing.setOrigin(positionLocal(thing));
    }

    template <typename Output_t, typename Input_t>
    const sf::Vector2<Output_t>
        makeVector2MultOf(const sf::Vector2<Input_t> & before, const sf::Vector2<Output_t> & mults)
    {
        static_assert(std::is_integral_v<Output_t>);
        return { makeMultOf(before.x, mults.x), makeMultOf(before.y, mults.y) };
    }

    // vetor and euclidian math

    [[nodiscard]] inline float
        dotProduct(const sf::Vector2f & left, const sf::Vector2f & right) noexcept
    {
        return ((left.x * right.x) + (left.y * right.y));
    }

    [[nodiscard]] inline const sf::Vector2f
        difference(const sf::Vector2f & from, const sf::Vector2f & to) noexcept
    {
        return (to - from);
    }

    [[nodiscard]] inline float magnitude(const sf::Vector2f & vec) noexcept
    {
        return std::sqrt((vec.x * vec.x) + (vec.y * vec.y));
    }

    [[nodiscard]] inline float distance(const sf::Vector2f & from, const sf::Vector2f & to) noexcept
    {
        return magnitude(to - from);
    }

    [[nodiscard]] inline const sf::Vector2f normalize(
        const sf::Vector2f & vec, const sf::Vector2f & returnOnError = { 0.0f, 0.0f }) noexcept
    {
        const float mag{ magnitude(vec) };

        if (mag < tiny)
        {
            return returnOnError;
        }

        return (vec / mag);
    }

    [[nodiscard]] inline const sf::Vector2f diffNormal(
        const sf::Vector2f & from,
        const sf::Vector2f & to,
        const sf::Vector2f & returnOnError = { 0.0f, 0.0f }) noexcept
    {
        return normalize(difference(from, to), returnOnError);
    }

    // degrees, assumes 0/360 degrees aims right, and positive degress turns clockwise
    [[nodiscard]] inline float angleFromVector(const sf::Vector2f & velocity)
    {
        const sf::Vector2f posDiffNormal{ normalize(velocity) };
        const float angleRadians{ std::acos(posDiffNormal.x) };
        const float angleDegrees{ radiansToDegrees(angleRadians) };

        // vertical or Y values that are positive move down, so have to flip
        if (velocity.y < 0.0f)
        {
            return -angleDegrees;
        }
        else
        {
            return angleDegrees;
        }
    }

    // assumes 0 and 360 degrees aims right, and positive degress turns clockwise
    [[nodiscard]] inline float angleFromTo(const sf::Vector2f & from, const sf::Vector2f & to)
    {
        return angleFromVector(difference(from, to));
    }

    template <typename T, typename U = T>
    [[nodiscard]] float angleFromTo(const T & from, const U & to)
    {
        sf::Vector2f fromPos{ 0.0f, 0.0f };
        if constexpr (std::is_same_v<std::remove_cv_t<T>, sf::Vector2f>)
        {
            fromPos = from;
        }
        else
        {
            fromPos = center(from);
        }

        sf::Vector2f toPos{ 0.0f, 0.0f };
        if constexpr (std::is_same_v<std::remove_cv_t<U>, sf::Vector2f>)
        {
            toPos = to;
        }
        else
        {
            toPos = center(to);
        }

        return angleFromTo(fromPos, toPos);
    }

    template <typename T>
    void aimAtPosition(T & thing, const sf::Vector2f & pos)
    {
        thing.setRotation(angleFromTo(center(thing), pos));
    }

    template <typename T>
    void aimWithVector(T & thing, const sf::Vector2f & velocity)
    {
        thing.setRotation(angleFromVector(velocity));
    }

    // scales, offsets, and local bounds

    // sfml utils to re-size (scale) any sf::FloatRect without moving it
    inline void scaleRectInPlace(sf::FloatRect & rect, const sf::Vector2f & scale) noexcept
    {
        const auto widthChange((rect.size.x * scale.x) - rect.size.x);
        rect.size.x += widthChange;
        rect.position.x -= (widthChange * 0.5f);

        const float heightChange((rect.size.y * scale.y) - rect.size.y);
        rect.size.y += heightChange;
        rect.position.y -= (heightChange * 0.5f);
    }

    [[nodiscard]] inline const sf::FloatRect
        scaleRectInPlaceCopy(const sf::FloatRect & before, const sf::Vector2f & scale) noexcept
    {
        sf::FloatRect after(before);
        scaleRectInPlace(after, scale);
        return after;
    }

    inline void scaleRectInPlace(sf::FloatRect & rect, const float scale) noexcept
    {
        scaleRectInPlace(rect, { scale, scale });
    }

    [[nodiscard]] inline const sf::FloatRect
        scaleRectInPlaceCopy(const sf::FloatRect & before, const float scale) noexcept
    {
        sf::FloatRect after(before);
        scaleRectInPlace(after, scale);
        return after;
    }

    inline void adjRectInPlace(sf::FloatRect & rect, const float amount) noexcept
    {
        rect.position.x += amount;
        rect.position.y += amount;
        rect.size.x -= (amount * 2.0f);
        rect.size.y -= (amount * 2.0f);
    }

    [[nodiscard]] inline const sf::FloatRect
        adjRectInPlaceCopy(const sf::FloatRect & before, const float amount) noexcept
    {
        sf::FloatRect after(before);
        adjRectInPlace(after, amount);
        return after;
    }

    // re-sizing (scaling), centering, and all while maintaining origins

    template <typename T>
    void centerInside(T & thing, const sf::FloatRect & rect)
    {
        thing.setPosition(center(rect) - (size(thing) * 0.5f));
    }

    // changes the shape
    template <typename T>
    void scale(T & thing, const sf::Vector2f & size)
    {
        // skip if source size is zero (or close enough) to avoid dividing by zero below
        const sf::FloatRect localBounds{ thing.getLocalBounds() };
        if ((localBounds.size.x < 1.0f) || (localBounds.size.y < 1.0f))
        {
            return;
        }

        const float scaleHoriz{ size.x / localBounds.size.x };
        const float scaleVert{ size.y / localBounds.size.y };
        thing.setScale({ scaleHoriz, scaleVert });

        if constexpr (std::is_same_v<std::remove_cv_t<T>, sf::Text>)
        {
            setOriginToPosition(thing);
        }
    }

    template <typename T>
    void scale(T & thing, const sf::FloatRect & rect)
    {
        scale(thing, { rect.size.x, rect.size.y });
    }

    template <typename T>
    void scale(T & thing, const float newScale)
    {
        scale(thing, { newScale, newScale });
    }

    template <typename T>
    void scaleAndCenterInside(T & thing, const sf::FloatRect & rect)
    {
        scale(thing, rect);
        centerInside(thing, rect);
    }

    // one dimm perfectly fits and one is left LESS than without changing the shape
    template <typename T>
    void fit(T & thing, const sf::Vector2f & size)
    {
        // skip if source size is zero (or close enough) to avoid dividing by zero below
        const sf::FloatRect localBounds{ thing.getLocalBounds() };
        if ((localBounds.size.x < 1.0f) || (localBounds.size.y < 1.0f))
        {
            return;
        }

        const float scaleHoriz{ size.x / localBounds.size.x };
        thing.setScale({ scaleHoriz, scaleHoriz });

        if (thing.getGlobalBounds().size.y > size.y)
        {
            const float scaleVert{ size.y / localBounds.size.y };
            thing.setScale({ scaleVert, scaleVert });
        }

        if constexpr (std::is_same_v<std::remove_cv_t<T>, sf::Text>)
        {
            setOriginToPosition(thing);
        }
    }

    template <typename T>
    void fit(T & thing, const sf::FloatRect & rect)
    {
        fit(thing, { rect.size.x, rect.size.y });
    }

    template <typename T>
    void fit(T & thing, const float newScale)
    {
        fit(thing, { newScale, newScale });
    }

    template <typename T>
    void fitAndCenterInside(T & thing, const sf::FloatRect & rect)
    {
        fit(thing, rect);
        centerInside(thing, rect);
    }

    // one dimm perfectly fits and one is left MORE than without changing the shape
    template <typename T>
    void grow(T & thing, const sf::Vector2f & size)
    {
        // skip if source size is zero (or close enough) to avoid dividing by zero below
        const sf::FloatRect localBounds{ thing.getLocalBounds() };
        if ((localBounds.size.x < 1.0f) || (localBounds.size.y < 1.0f))
        {
            return;
        }

        const float scaleHoriz{ size.x / localBounds.size.x };
        thing.setScale(scaleHoriz, scaleHoriz);

        if (size.y > thing.getGlobalBounds().size.y)
        {
            const float scaleVert{ size.y / localBounds.size.y };
            thing.setScale(scaleVert, scaleVert);
        }

        if constexpr (std::is_same_v<std::remove_cv_t<T>, sf::Text>)
        {
            setOriginToPosition(thing);
        }
    }

    template <typename T>
    void grow(T & thing, const sf::FloatRect & rect)
    {
        grow(thing, { rect.size.x, rect.size.y });
    }

    template <typename T>
    void grow(T & thing, const float newScale)
    {
        grow(thing, { newScale, newScale });
    }

    template <typename T>
    void growAndCenterInside(T & thing, const sf::FloatRect & rect)
    {
        grow(thing, rect);
        centerInside(thing, rect);
    }

    // quad making and appending

    // colors not changed if color given is transparent
    template <typename Container_t>
    void setupTriangleVerts(
        const sf::Vector2f & pos,
        const sf::Vector2f & size,
        const std::size_t index,
        Container_t & verts,
        const sf::Color & color = sf::Color::Transparent)
    {
        // clang-format off
        verts[index + 0].position = pos;
        verts[index + 1].position = sf::Vector2f((pos.x + size.x),  pos.y          );
        verts[index + 2].position = sf::Vector2f( pos.x,           (pos.y + size.y));
        verts[index + 3].position = sf::Vector2f( pos.x          , (pos.y + size.y));
        verts[index + 4].position = sf::Vector2f((pos.x + size.x),  pos.y          );
        verts[index + 5].position = sf::Vector2f((pos.x + size.x), (pos.y + size.y));
        // clang-format on

        if (color != sf::Color::Transparent)
        {
            verts[index + 0].color = color;
            verts[index + 1].color = color;
            verts[index + 2].color = color;
            verts[index + 3].color = color;
            verts[index + 4].color = color;
            verts[index + 5].color = color;
        }
    }

    // colors not changed if color given is transparent
    template <typename Container_t>
    void setupTriangleVerts(
        const sf::FloatRect & rect,
        const std::size_t index,
        Container_t & verts,
        const sf::Color & color = sf::Color::Transparent)
    {
        setupTriangleVerts(rect.position, rect.size, index, verts, color);
    }

    // colors not changed if color given is transparent
    template <typename Container_t>
    void appendTriangleVerts(
        const sf::Vector2f & pos,
        const sf::Vector2f & size,
        Container_t & verts,
        const sf::Color & color = sf::Color::Transparent)
    {
        std::size_t origSize{ 0 };
        if constexpr (std::is_same_v<std::remove_cv_t<Container_t>, sf::VertexArray>)
        {
            origSize = verts.getVertexCount();
        }
        else
        {
            origSize = verts.size();
        }

        verts.resize(origSize + verts_per_quad);

        setupTriangleVerts(pos, size, origSize, verts, color);
    }

    // colors not changed if color given is transparent
    template <typename Container_t>
    void appendTriangleVerts(
        const sf::FloatRect & rect,
        Container_t & verts,
        const sf::Color & color = sf::Color::Transparent)
    {
        appendTriangleVerts(rect.position, rect.size, verts, color);
    }

    template <typename Container_t>
    void appendTriangleVerts(
        const sf::FloatRect & rect, const sf::IntRect & textureCoordinates, Container_t & verts)
    {
        appendTriangleVerts(rect, verts);

        std::size_t index{ 0 };
        if constexpr (std::is_same_v<std::remove_cv_t<Container_t>, sf::VertexArray>)
        {
            index = verts.getVertexCount();
        }
        else
        {
            index = verts.size();
        }

        index -= verts_per_quad;

        const sf::Vector2f pos{ textureCoordinates.position };
        const sf::Vector2f size{ textureCoordinates.size };

        // keep these offsets in sync with those in setupTriangleVerts() above
        // clang-format off
        verts[index + 0].texCoords = pos;
        verts[index + 1].texCoords = sf::Vector2f((pos.x + size.x),  pos.y          );
        verts[index + 2].texCoords = sf::Vector2f( pos.x          , (pos.y + size.y));
        verts[index + 3].texCoords = sf::Vector2f( pos.x          , (pos.y + size.y));
        verts[index + 4].texCoords = sf::Vector2f((pos.x + size.x),  pos.y          );
        verts[index + 5].texCoords = sf::Vector2f((pos.x + size.x), (pos.y + size.y));
        // clang-format on
    }

    template <typename Container_t>
    void appendTriangleVerts(const sf::Sprite & sprite, Container_t & verts)
    {
        appendTriangleVerts(sprite.getGlobalBounds(), sprite.getTextureRect(), verts);
    }

    inline void appendLineVerts(
        const sf::FloatRect & rect, std::vector<sf::Vertex> & verts, const sf::Color & color)
    {
        verts.emplace_back(rect.position + sf::Vector2f(0.0f, 0.0f), color);
        verts.emplace_back(rect.position + sf::Vector2f(rect.size.x, 0.0f), color);

        verts.emplace_back(rect.position + sf::Vector2f(rect.size.x, 0.0f), color);
        verts.emplace_back(rect.position + sf::Vector2f(rect.size.x, rect.size.y), color);

        verts.emplace_back(rect.position + sf::Vector2f(rect.size.x, rect.size.y), color);
        verts.emplace_back(rect.position + sf::Vector2f(0.0f, rect.size.y), color);

        verts.emplace_back(rect.position + sf::Vector2f(0.0f, rect.size.y), color);
        verts.emplace_back(rect.position + sf::Vector2f(0.0f, 0.0f), color);
    }

    // slow running but handy debugging shapes

    [[nodiscard]] inline const sf::VertexArray
        makeRectangleVerts(const sf::FloatRect & rect, const sf::Color & color = sf::Color::White)
    {
        sf::VertexArray verts(sf::PrimitiveType::Triangles, verts_per_quad);
        setupTriangleVerts(rect.position, rect.size, 0, verts, color);
        return verts;
    }

    inline void drawRectangleVerts(
        sf::RenderTarget & target,
        const sf::FloatRect & rect,
        const sf::Color & color = sf::Color::White)
    {
        target.draw(makeRectangleVerts(rect, color));
    }

    [[nodiscard]] inline sf::RectangleShape makeRectangleShape(
        const sf::FloatRect & rect,
        const bool willColorFill = false,
        const sf::Color & color  = sf::Color::White)
    {
        sf::RectangleShape rs;

        rs.setOutlineThickness(1.0f);
        rs.setOutlineColor(color);

        if (willColorFill)
        {
            rs.setFillColor(color);
        }
        else
        {
            rs.setFillColor(sf::Color::Transparent);
        }

        rs.setPosition(rect.position);
        rs.setSize(rect.size);
        return rs;
    }

    inline void drawRectangleShape(
        sf::RenderTarget & target,
        const sf::FloatRect & rect,
        const bool willColorFill = false,
        const sf::Color & color  = sf::Color::White)
    {
        target.draw(makeRectangleShape(rect, willColorFill, color));
    }

    [[nodiscard]] inline const sf::CircleShape makeCircleShape(
        const sf::Vector2f & position,
        const float radius,
        const sf::Color & color      = sf::Color::White,
        const std::size_t pointCount = 32)
    {
        sf::CircleShape cs;
        cs.setFillColor(color);
        cs.setPointCount(pointCount);
        cs.setRadius(radius);
        setOriginToCenter(cs);
        cs.setPosition(position);
        return cs;
    }

    inline void drawCircleShape(
        sf::RenderTarget & target,
        const sf::Vector2f & position,
        const float radius,
        const sf::Color & color      = sf::Color::White,
        const std::size_t pointCount = 32)
    {
        target.draw(makeCircleShape(position, radius, color, pointCount));
    }

    [[nodiscard]] inline const sf::CircleShape makeCircleShape(
        const sf::FloatRect & rect,
        const sf::Color & color      = sf::Color::White,
        const std::size_t pointCount = 32)
    {
        return makeCircleShape(
            center(rect), (std::min(rect.size.x, rect.size.y) * 0.5f), color, pointCount);
    }

    inline void drawCircle(
        sf::RenderTarget & target,
        const sf::FloatRect & rect,
        const sf::Color & color = sf::Color::White)
    {
        target.draw(makeCircleShape(rect, color));
    }

    inline const sf::VertexArray makeLines(
        const std::vector<sf::Vector2f> & points, const sf::Color & color = sf::Color::White)
    {
        sf::VertexArray va(sf::PrimitiveType::Lines);

        for (const sf::Vector2f & point : points)
        {
            va.append(sf::Vertex(point, color));
        }

        return va;
    }

    inline void drawlines(
        sf::RenderTarget & target,
        const std::vector<sf::Vector2f> & points,
        const sf::Color & color = sf::Color::White)
    {
        target.draw(makeLines(points, color));
    }

    inline void drawlines(
        sf::RenderTarget & target,
        const std::initializer_list<sf::Vector2f> & points,
        const sf::Color & color = sf::Color::White)
    {
        target.draw(makeLines(points, color));
    }

    // more misc sfml

    inline const sf::Color colorBlend(
        const float ratio,
        const sf::Color & fromColor,
        const sf::Color & toColor,
        const bool willIgnoreAlpha = false)
    {
        if (ratio < 0.0f)
        {
            return fromColor;
        }

        if (ratio > 1.0f)
        {
            return toColor;
        }

        auto calcColorValue = [ratio](const std::uint8_t fromVal, const std::uint8_t toVal) {
            const float diff{ static_cast<float>(toVal) - static_cast<float>(fromVal) };
            const float finalValue{ static_cast<float>(fromVal) + (diff * ratio) };
            return static_cast<std::uint8_t>(finalValue);
        };

        sf::Color color{ toColor };
        color.r = calcColorValue(fromColor.r, toColor.r);
        color.g = calcColorValue(fromColor.g, toColor.g);
        color.b = calcColorValue(fromColor.b, toColor.b);

        if (!willIgnoreAlpha)
        {
            color.a = calcColorValue(fromColor.a, toColor.a);
        }

        return color;
    }

    inline const sf::Color colorStepToward(
        const std::uint8_t stepSize,
        const sf::Color & fromColor,
        const sf::Color & toColor,
        const bool willIgnoreAlpha = false)
    {
        if (0 == stepSize)
        {
            return fromColor;
        }

        if (255 == stepSize)
        {
            return toColor;
        }

        auto calcColorValue = [stepSize](const std::uint8_t fromVal, const std::uint8_t toVal) {
            if (fromVal == toVal)
            {
                return fromVal;
            }

            const int stepInt{ static_cast<int>(stepSize) };
            const int fromInt{ static_cast<int>(fromVal) };
            const int toInt{ static_cast<int>(toVal) };
            const int diff{ std::min(std::abs(toInt - fromInt), stepInt) };

            int finalValue{ fromInt };
            if (toVal > fromVal)
            {
                finalValue += diff;
            }
            else
            {
                finalValue -= diff;
            }

            return static_cast<std::uint8_t>(std::clamp(finalValue, 0, 255));
        };

        sf::Color color{ toColor };
        color.r = calcColorValue(fromColor.r, toColor.r);
        color.g = calcColorValue(fromColor.g, toColor.g);
        color.b = calcColorValue(fromColor.b, toColor.b);

        if (!willIgnoreAlpha)
        {
            color.a = calcColorValue(fromColor.a, toColor.a);
        }

        return color;
    }

    // returns desktop mode if none found matching the bpp
    inline const sf::VideoMode findVideoModeClosestTo(const sf::VideoMode & targetMode)
    {
        std::vector<sf::VideoMode> videoModes = sf::VideoMode::getFullscreenModes();

        videoModes.erase(
            std::remove_if(
                std::begin(videoModes),
                std::end(videoModes),
                [&](const auto & vm) { return (vm.bitsPerPixel != targetMode.bitsPerPixel); }),
            std::end(videoModes));

        if (videoModes.empty())
        {
            return sf::VideoMode::getDesktopMode();
        }

        std::sort(
            std::begin(videoModes),
            std::end(videoModes),
            [&](const sf::VideoMode A, const sf::VideoMode B) {
                const unsigned int diffA{ (
                    util::abs(A.size.x - targetMode.size.x) +
                    util::abs(A.size.y - targetMode.size.y)) };

                const unsigned int diffB{ (
                    util::abs(B.size.x - targetMode.size.x) +
                    util::abs(B.size.y - targetMode.size.y)) };

                return (diffA < diffB);
            });

        return *videoModes.begin();
    }

    [[nodiscard]] inline std::string makeSupportedVideoModesString(
        const bool willSkipDiffBitsPerPixel = false, const std::string & separator = "\n")
    {
        const unsigned int desktopBitsPerPixel{ sf::VideoMode::getDesktopMode().bitsPerPixel };

        std::vector<sf::VideoMode> videoModes{ sf::VideoMode::getFullscreenModes() };
        std::reverse(std::begin(videoModes), std::end(videoModes));

        // const std::size_t modeCountOrig{ videoModes.size() };

        std::size_t count{ 0 };
        std::ostringstream ss;
        for (const sf::VideoMode & vm : videoModes)
        {
            if (willSkipDiffBitsPerPixel && (vm.bitsPerPixel != desktopBitsPerPixel))
            {
                continue;
            }

            if (count > 0)
            {
                ss << separator;
            }

            ss << vm;
            ++count;
        }

        // const std::size_t modeCountReturned{ count };

        // ss << separator << "(total_supported=" << modeCountOrig << ")";
        // ss << separator << "(total_listed=" << modeCountReturned << ")";

        return ss.str();
    }

} // namespace util

#endif // UTIL_SFML_UTIL_HPP_INCLUDED
