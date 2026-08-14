#ifndef UTIL_SLIDERS_HPP_INCLUDED
#define UTIL_SLIDERS_HPP_INCLUDED
//
// sliders.hpp
//
#include "util/random.hpp"
#include "util/util.hpp"

#include <numbers>
#include <type_traits>
#include <utility>

namespace util
{
    // All Sliders:
    //  - Use Smooth Step to achieve sin motion called "sliding" here.
    //  - Start automatically at the given value and isMoving() returns true.
    //  - Call Update() with a fraction of a second to move the value towards the target.
    //  - Update() will call stop() automatically when target value is reached or IsRealClose().
    //  - Continues moving until the target value is exceeded, or IsRealClose(), or until Stop().
    //  - Calling Stop() leaves value unchanged, and then only restart() will get it moving again.
    //  - Default construction sets everything to zero/false and isMoving() returns false.
    //  - Given speeds are clamped to 0.0f or greater.

    //
    // Slides a value from zero to one using smoothstep equation.
    // Motion is fastest (bounce-like) when startAt=0.5.
    // Will not work with negative speeds or update(...).
    //
    template <typename T = float>
    class SliderRatio
    {
        static_assert(std::is_floating_point_v<T>);

      public:
        constexpr SliderRatio() noexcept
            : m_isMoving{ false }
            , m_speed{ 0 }
            , m_value{ 0 }
            , m_base{ 0 }
        {}

        explicit constexpr SliderRatio(const T t_speed, const T t_startAt = T(0)) noexcept
            : m_isMoving{ true }
            , m_speed{ 0 }
            , m_value{ 0 }
            , m_base{ std::clamp(t_startAt, T(0), T(1)) }
        {
            restart(t_speed, t_startAt);
        }

        [[nodiscard]] constexpr T value() const noexcept { return m_value; }
        [[nodiscard]] constexpr T speed() const noexcept { return m_speed; }
        constexpr void speed(const T t_newSpeed) noexcept { m_speed = t_newSpeed; }
        [[nodiscard]] constexpr bool isMoving() const noexcept { return m_isMoving; }
        constexpr void stop() noexcept { m_isMoving = false; }

        constexpr void restart(const T t_speed, const T t_startAt = T(0))
        {
            m_isMoving = true;
            m_speed    = t_speed;
            m_base     = std::clamp(t_startAt, T(0), T(1));
            update(T(0));
        }

        constexpr void restart(const T t_startAt = T(0))
        {
            m_isMoving = true;
            m_base     = std::clamp(t_startAt, T(0), T(1));
            update(T(0));
        }

        constexpr T update(const T t_adjustment) noexcept
        {
            if (m_isMoving)
            {
                m_base += (t_adjustment * m_speed);
                if ((m_base > T(1)) || isRealClose(m_base, T(1)))
                {
                    m_value = T(1);
                    stop();
                }
                else
                {
                    m_value = ((m_base * m_base) * (T(3) - (T(2) * m_base)));
                    m_value = std::clamp(m_value, T(0), T(1));
                }
            }

            return m_value;
        }

      private:
        bool m_isMoving;
        T m_speed;
        T m_value;
        T m_base;
    };

    //
    // Slides Value() over [from, to] smoothly using sine motion and then stops.
    //
    template <typename Value_t, typename Math_t = float>
    class SliderFromTo
    {
        static_assert(std::is_floating_point_v<Math_t>);

      public:
        constexpr SliderFromTo() noexcept
            : m_from{ 0 }
            , m_to{ 0 }
            , m_max{ 0 }
            , m_min{ 0 }
            , m_diff{ 0 }
            , m_speed{ 0 }
            , m_value{ 0 }
            , m_slider()
        {}

        explicit constexpr SliderFromTo(
            const Value_t t_from, const Value_t t_to, const Math_t t_speed) noexcept
            : m_from{ t_from }
            , m_to{ t_to }
            , m_max{ util::max(t_from, t_to) }
            , m_min{ util::min(t_from, t_to) }
            , m_diff{ t_to - t_from }
            , m_speed{ t_speed }
            , m_value{ t_from }
            , m_slider{ m_speed }
        {}

        [[nodiscard]] constexpr Math_t radians() const noexcept { return m_slider.radians(); }

        [[nodiscard]] constexpr Value_t from() const noexcept { return m_from; }
        [[nodiscard]] constexpr Value_t to() const noexcept { return m_to; }
        [[nodiscard]] constexpr Value_t value() const noexcept { return m_value; }
        [[nodiscard]] constexpr Math_t speed() const noexcept { return m_speed; }

        [[nodiscard]] constexpr bool isMoving() const noexcept { return m_slider.isMoving(); }

        constexpr void stop() noexcept { m_slider.stop(); }
        [[nodiscard]] constexpr Math_t ratio() const noexcept { return m_slider.value(); }

        constexpr Value_t update(const Math_t t_adjustment) noexcept
        {
            const Math_t ratio = m_slider.update(t_adjustment);
            m_value = (m_from + static_cast<Value_t>(static_cast<Math_t>(m_diff * ratio)));
            m_value = std::clamp(m_value, m_min, m_max);
            return m_value;
        }

      private:
        Value_t m_from;
        Value_t m_to;
        Value_t m_max;
        Value_t m_min;
        Value_t m_diff;
        Math_t m_speed;
        Value_t m_value;
        SliderRatio<Math_t> m_slider;
    };

    //
    // Slides Value() back and forth over [from, to] smoothly using sine motion.
    //
    template <typename Value_t, typename Math_t = float>
    class SliderOscillator
    {
      public:
        constexpr SliderOscillator() noexcept
            : m_from{ 0 }
            , m_to{ 0 }
            , m_slider{}
        {}

        // Use this constructor to start Value() at from.
        constexpr SliderOscillator(
            const Value_t t_from, const Value_t t_to, const Math_t t_speed) noexcept
            : m_from{ 0 }
            , m_to{ 0 }
            , m_slider{}
        {
            restart(t_from, t_to, t_speed, t_from);
        }

        // Use this constructor if you want to specify the starting value.
        constexpr SliderOscillator(
            const Value_t t_from,
            const Value_t t_to,
            const Math_t t_speed,
            const Value_t t_startAt) noexcept
            : m_from{ 0 }
            , m_to{ 0 }
            , m_slider{}
        {
            restart(t_from, t_to, t_speed, t_startAt);
        }

        [[nodiscard]] constexpr Math_t radians() const noexcept { return m_slider.radians(); }

        [[nodiscard]] constexpr Value_t from() const noexcept { return m_from; }
        [[nodiscard]] constexpr Value_t to() const noexcept { return m_to; }
        [[nodiscard]] constexpr Math_t speed() const noexcept { return m_slider.speed(); }

        constexpr void speed(const Math_t t_newSpeed) noexcept { m_slider.speed(t_newSpeed); }

        [[nodiscard]] constexpr Value_t value() const noexcept { return m_slider.value(); }

        [[nodiscard]] constexpr bool isMoving() const noexcept { return m_slider.isMoving(); }

        constexpr void stop() noexcept { m_slider.stop(); }

        constexpr Value_t update(const Math_t t_adjustment) noexcept
        {
            if (m_slider.isMoving())
            {
                m_slider.update(t_adjustment);

                if (!m_slider.isMoving())
                {
                    if (isRealClose(m_slider.to(), m_to))
                    {
                        m_slider = SliderFromTo<Value_t, Math_t>(m_to, m_from, speed());
                    }
                    else
                    {
                        m_slider = SliderFromTo<Value_t, Math_t>(m_from, m_to, speed());
                    }
                }
            }

            return m_slider.value();
        }

        constexpr void restart(
            const Value_t t_from,
            const Value_t t_to,
            const Math_t t_speed,
            const Value_t t_startAt) noexcept
        {
            m_from = t_from;
            m_to   = t_to;

            // If StartAtClamp() set m_value to t_to then start reversed
            if (isRealClose(t_startAt, t_to))
            {
                m_slider = SliderFromTo<Value_t, Math_t>(m_to, m_from, t_speed);
            }
            else
            {
                m_slider = SliderFromTo<Value_t, Math_t>(t_startAt, t_to, t_speed);
            }
        }

      private:
        Value_t m_from;
        Value_t m_to;
        SliderFromTo<Value_t, Math_t> m_slider;
    };

    //
    // Slides between random locations over [from, to]
    //
    template <typename Value_t, typename Math_t = float>
    class SliderDrift
    {
      public:
        constexpr SliderDrift()
            : m_valueRange{ 0, 0 }
            , m_speedRange{ 0, 0 }
            , m_slider{}
        {}

        SliderDrift(
            const Random & t_random,
            const std::pair<Value_t, Value_t> & t_valueRange,
            const std::pair<Math_t, Math_t> & t_speedRange)
            : m_valueRange{ t_valueRange }
            , m_speedRange{ t_speedRange }
            , m_slider{ t_random.fromTo(t_valueRange.first, t_valueRange.second),
                        t_random.fromTo(t_valueRange.first, t_valueRange.second),
                        t_random.fromTo(m_speedRange.first, m_speedRange.second) }
        {}

        void update(const Random & t_random, const Math_t t_adjustment)
        {
            m_slider.update(t_adjustment);

            if (!m_slider.isMoving())
            {
                restart(t_random);
            }
        }

        [[nodiscard]] constexpr Math_t speed() const noexcept { return m_slider.speed(); }

        constexpr void speed(const Math_t t_newSpeed) noexcept { m_slider.speed(t_newSpeed); }

        [[nodiscard]] constexpr Value_t value() const noexcept { return m_slider.value(); }

        [[nodiscard]] constexpr bool isMoving() const noexcept { return m_slider.isMoving(); }

        constexpr void stop() noexcept { m_slider.stop(); }

        [[nodiscard]] constexpr float ratio() const noexcept
        {
            return util::mapToRatio(m_slider.value(), m_valueRange.first, m_valueRange.second);
        }

        void restart(const Random & t_random)
        {
            m_slider = util::SliderFromTo<Math_t>(
                m_slider.value(),
                t_random.fromTo(m_valueRange.first, m_valueRange.second),
                t_random.fromTo(m_speedRange.first, m_speedRange.second));
        }

      private:
        std::pair<Value_t, Value_t> m_valueRange;
        std::pair<Math_t, Math_t> m_speedRange;
        util::SliderFromTo<Math_t> m_slider;
    };

} // namespace util

#endif // UTIL_SLIDERS_HPP_INCLUDED
