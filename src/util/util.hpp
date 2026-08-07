#ifndef UTIL_UTIL_HPP_INCLUDED
#define UTIL_UTIL_HPP_INCLUDED
//
// util.hpp
//
#include <algorithm>
#include <iomanip>
#include <ios>
#include <iostream>
#include <limits>
#include <locale>
#include <numbers>
#include <source_location>
#include <sstream>
#include <string>
#include <type_traits>

//

constexpr std::size_t operator"" _st(unsigned long long number)
{
    return static_cast<std::size_t>(number);
}

constexpr std::ptrdiff_t operator"" _pd(unsigned long long number)
{
    return static_cast<std::ptrdiff_t>(number);
}

//

namespace util
{

    inline std::ostream & log(const std::source_location location = std::source_location::current())
    {
        std::clog << location.file_name() << "  ";

#ifdef __APPLE__
        std::clog << __PRETTY_FUNCTION__;
#else
        std::clog << location.function_name();
#endif

        std::clog << "  Line:" << location.line() << "  ";

        return std::clog;
    }

    inline std::ostream & logIf(
        const bool willLog, const std::source_location location = std::source_location::current())
    {
        if (!willLog)
        {
            static std::ostringstream oss;
            oss.str("");
            return oss;
        }
        else
        {
            return log(location);
        }
    }

    //
    // abs(), min(a,b,c), max(a,b,c), makeEven()
    //
    // These functions are written here by hand instead of using std because:
    //  * std min/max cannot take more than one arg
    //  * <cmath>'s abs() does not use templates
    //  * some of the std functions are not constexpr when they could be
    //  * some of the std functions are not noexcept when they could be
    //  * comparing reals is only needs to be simple/fast/less-accurate for games

    template <typename T>
    [[nodiscard]] T constexpr abs(const T number) noexcept
    {
        static_assert(std::is_arithmetic_v<T>);

        if constexpr (std::is_unsigned_v<T>)
        {
            return number;
        }
        else
        {
            if (number < T(0))
            {
                return -number;
            }
            else
            {
                return number;
            }
        }
    }

    template <typename T>
    [[nodiscard]] constexpr T max(const T left, const T right) noexcept
    {
        static_assert(std::is_arithmetic_v<T>);

        if (left < right)
        {
            return right;
        }
        else
        {
            return left;
        }
    }

    template <typename T, typename... Ts>
    [[nodiscard]] constexpr T max(const T first, const Ts... allOthers) noexcept
    {
        return max(first, max(allOthers...));
    }

    template <typename T>
    [[nodiscard]] constexpr T min(const T left, const T right) noexcept
    {
        static_assert(std::is_arithmetic_v<T>);

        if (left < right)
        {
            return left;
        }
        else
        {
            return right;
        }
    }

    template <typename T, typename... Ts>
    [[nodiscard]] constexpr T min(const T first, const Ts... allOthers) noexcept
    {
        return min(first, min(allOthers...));
    }

    // this lib is for simple/innaccurate/game apps, so a multiple of epsilon works fine
    template <typename T>
    constexpr T float_compare_epsilon = (std::numeric_limits<T>::epsilon() * T(100));

    template <typename T>
    void makeEven(T & number, const bool willAdd)
    {
        static_assert(std::is_integral_v<T>);

        if ((number % 2) != 0)
        {
            if (willAdd)
            {
                ++number;
            }
            else
            {
                --number;
            }
        }
    }

    template <typename T>
    T makeEvenCopy(const T number, const bool willAdd)
    {
        static_assert(std::is_integral_v<T>);
        T copy{ number };
        makeEven(copy, willAdd);
        return copy;
    }

    //
    // isRealClose()
    //

    template <typename T>
    [[nodiscard]] constexpr bool isRealClose(const T left, const T right) noexcept
    {
        static_assert(std::is_arithmetic_v<T>);

        if constexpr (std::is_integral_v<T>)
        {
            return (left == right);
        }
        else
        {
            const T diffAbs{ abs(right - left) };

            if (diffAbs < T(1))
            {
                return (diffAbs < float_compare_epsilon<T>);
            }
            else
            {
                const T maxForEpsilon{ max(abs(left), abs(right), T(1)) };
                return (diffAbs < (maxForEpsilon * float_compare_epsilon<T>));
            }
        }
    }

    template <typename T>
    [[nodiscard]] constexpr bool isRealCloseOrLess(const T number, const T comparedTo) noexcept
    {
        return ((number < comparedTo) || isRealClose(number, comparedTo));
    }

    template <typename T>
    [[nodiscard]] constexpr bool isRealCloseOrGreater(const T number, const T comparedTo) noexcept
    {
        return ((number > comparedTo) || isRealClose(number, comparedTo));
    }

    //
    // map()
    //

    template <typename T, typename U = T>
    [[nodiscard]] constexpr U
        map(const T number, const T inMin, const T inMax, const U outMin, const U outMax) noexcept
    {
        if (isRealClose(inMin, inMax))
        {
            return outMax;
        }

        return (outMin + static_cast<U>(((number - inMin) * (outMax - outMin)) / (inMax - inMin)));
    }

    // assumes ratio is [0,1]
    template <typename Ratio_t, typename Number_t>
    [[nodiscard]] constexpr Number_t
        mapRatioTo(const Ratio_t ratio, const Number_t outMin, const Number_t outMax) noexcept
    {
        static_assert(std::is_arithmetic_v<Number_t>);
        static_assert(std::is_floating_point_v<Ratio_t>);

        return (
            outMin + static_cast<Number_t>(
                         ratio * (static_cast<Ratio_t>(outMax) - static_cast<Ratio_t>(outMin))));
    }

    template <typename Number_t, typename Ratio_t = float>
    [[nodiscard]] constexpr Ratio_t
        mapToRatio(const Number_t number, const Number_t inMin, const Number_t inMax) noexcept
    {
        static_assert(std::is_floating_point_v<Ratio_t>);

        if (isRealClose(inMin, inMax))
        {
            return Ratio_t(1);
        }

        return static_cast<Ratio_t>((number - inMin) / (inMax - inMin));
    }

    inline constexpr unsigned char mapRatioToColorValue(const float ratio)
    {
        return map(
            std::clamp(ratio, 0.0f, 1.0f),
            0.0f,
            1.0f,
            static_cast<unsigned char>(0),
            static_cast<unsigned char>(255));
    }

    //
    // math
    //

    constexpr float tiny{ 0.0001f }; // this is for games so this is close enough

    [[nodiscard]] constexpr float degreesToRadians(const float degrees) noexcept
    {
        return (degrees * (std::numbers::pi_v<float> / 180.0f));
    }

    [[nodiscard]] constexpr float radiansToDegrees(const float radians) noexcept
    {
        return (radians * (180.0f / std::numbers::pi_v<float>));
    }

    [[nodiscard]] inline bool isAbsTiny(const float value) noexcept
    {
        return (abs(value) < util::tiny);
    }

    //
    // bit hacking
    //

    template <typename T, typename U>
    inline constexpr bool isBitSet(const T bits, const U toCheck) noexcept
    {
        static_assert(
            (std::is_unsigned_v<T>) ||
            (std::is_enum_v<T> && std::is_unsigned_v<typename std::underlying_type_t<T>>));

        static_assert(!std::is_same_v<std::remove_cv<T>, bool>);

        return ((bits & static_cast<T>(toCheck)) != 0);
    }

    template <typename T, typename U>
    inline constexpr void setBit(T & bits, const U toSet) noexcept
    {
        static_assert(
            (std::is_unsigned_v<T>) ||
            (std::is_enum_v<T> && std::is_unsigned_v<std::underlying_type_t<T>>));

        static_assert(!std::is_same_v<std::remove_cv<T>, bool>);

        bits |= static_cast<T>(toSet);
    }

    template <typename T, typename U>
    inline constexpr T setBitCopy(const T & bits, const U toSet) noexcept
    {
        T copy{ bits };
        setBit(copy, toSet);
        return copy;
    }

    // Counting High Bits
    //  Peter Wegner's Method, which was also discovered independently by Derrick Lehmer in 1964.
    //  This method goes through as many iterations as there are set bits.
    template <typename T>
    [[nodiscard]] std::size_t countHighBits(T number) noexcept
    {
        static_assert(std::is_unsigned_v<T>);
        static_assert(!std::is_same_v<std::remove_cv<T>, bool>);

        std::size_t count{ 0 };
        for (; number; count++)
        {
            number &= (number - 1);
        }

        return count;
    }

    template <typename T>
    [[nodiscard]] inline constexpr bool isPowerOfTwo(const T number) noexcept
    {
        static_assert(std::is_unsigned_v<T>);
        static_assert(!std::is_same_v<std::remove_cv<T>, bool>);

        return ((number > 0) && ((number & (number - 1)) == 0));
    }

    template <typename T>
    [[nodiscard]] inline constexpr T findPowerOfTwoGreaterThan(const unsigned number) noexcept
    {
        static_assert(std::is_unsigned_v<T>);
        static_assert(!std::is_same_v<std::remove_cv<T>, bool>);

        T powerOfTwo{ 2 };

        while (powerOfTwo <= number)
        {
            powerOfTwo <<= 1;
        }

        return powerOfTwo;
    }

    //
    // std lib and algorithms
    //

    template <typename T>
    void sortThenUnique(T & container)
    {
        std::sort(std::begin(container), std::end(container));

        container.erase(
            std::unique(std::begin(container), std::end(container)), std::end(container));
    }

    // requires random access but a quick way to erase without invalidating other iters
    template <typename Container_t>
    void swapAndPop(Container_t & container, const typename Container_t::iterator & toErase)
    {
        if (container.empty())
        {
            return;
        }

        if (container.size() > 1)
        {
            std::iter_swap(toErase, (std::end(container) - 1));
        }

        container.pop_back();
    }

    template <typename Container_t>
    [[nodiscard]] inline std::string containerToString(
        const Container_t & container,
        const std::string & separator = ",",
        const std::string & wrap      = {})
    {
        std::ostringstream ss;

        const auto iterBegin{ std::begin(container) };
        for (auto iter(iterBegin); std::end(container) != iter; ++iter)
        {
            if (iterBegin != iter)
            {
                ss << separator;
            }

            ss << *iter;
        }

        const std::string content{ ss.str() };

        if (content.empty())
        {
            return "";
        }
        else
        {
            const std::string wrapFront{ (wrap.size() >= 1) ? std::string(1, wrap[0])
                                                            : std::string() };

            const std::string wrapBack{ (wrap.size() >= 2) ? std::string(1, wrap[1])
                                                           : std::string() };

            return (wrapFront + content + wrapBack);
        }
    }

    //
    // statistics
    //

    template <typename T>
    struct Stats
    {
        std::string toString(const int numberWidth = 5) const
        {
            std::ostringstream ss;
            ss.imbue(std::locale("")); // this is only to put commas in the big numbers

            ss << "x" << count;
            ss << " [" << std::setw(numberWidth) << std::right << min;
            ss << ", " << std::setw(numberWidth) << std::right << static_cast<T>(avg);
            ss << ", " << std::setw(numberWidth) << std::right << max;
            ss << "] sd=" << std::setw(numberWidth) << std::left << sdv;

            return ss.str();
        }

        std::size_t count{ 0 };
        T min{ T(0) };
        T max{ T(0) };
        T sum{ T(0) };
        double avg{ 0.0 };
        double sdv{ 0.0 };
    };

    template <typename T>
    std::ostream & operator<<(std::ostream & os, const Stats<T> & stats)
    {
        os << stats.toString();
        return os;
    }

    template <typename Container_t>
    Stats<typename Container_t::value_type> makeStats(const Container_t & container)
    {
        using T = typename Container_t::value_type;

        Stats<T> stats;

        stats.count = container.size();

        if (0 == stats.count)
        {
            return stats;
        }

        stats.min = std::numeric_limits<T>::max();

        for (const T number : container)
        {
            stats.sum += number;

            if (number < stats.min)
            {
                stats.min = number;
            }

            if (number > stats.max)
            {
                stats.max = number;
            }
        }

        stats.avg = (static_cast<double>(stats.sum) / static_cast<double>(stats.count));

        if (stats.count < 2)
        {
            return stats;
        }

        double deviationSum{ 0.0 };
        for (const T number : container)
        {
            const double diff{ static_cast<double>(number) - stats.avg };
            deviationSum += (diff * diff);
        }

        stats.sdv = std::sqrt(deviationSum / static_cast<double>(stats.count));

        return stats;
    }

    //
    // percents
    //

    template <typename T, typename U = T>
    [[nodiscard]] float calcPercent(const T num, const U den, const std::size_t afterDotCount = 1)
    {
        static_assert(std::is_arithmetic_v<T>);
        static_assert(!std::is_same_v<std::remove_cv_t<T>, bool>);

        static_assert(std::is_arithmetic_v<U>);
        static_assert(!std::is_same_v<std::remove_cv_t<U>, bool>);

        if (!(den > U{ 0 }))
        {
            return 0.0f;
        }

        long double result{ (static_cast<long double>(num) / static_cast<long double>(den)) };
        result *= 100.0L;

        if (afterDotCount > 0)
        {
            const long double afterDotMult{ 10.0L * static_cast<long double>(afterDotCount) };
            if (afterDotMult > 0.0L)
            {
                result *= afterDotMult;
                result = std::round(result);
                result /= afterDotMult;
            }
        }

        return static_cast<float>(result);
    }

    template <typename T, typename U = T>
    [[nodiscard]] std::string makePercentString(
        const T num,
        const U den,
        const std::string & prefix      = {},
        const std::string & postfix     = {},
        const std::size_t afterDotCount = 1,
        const std::string & wrap        = "()")
    {
        std::ostringstream ss;

        if (!wrap.empty())
        {
            ss << wrap.front();
        }

        ss << prefix;
        ss << calcPercent<T, U>(num, den, afterDotCount);
        ss << '%';
        ss << postfix;

        if (!wrap.empty())
        {
            ss << wrap.back();
        }

        return ss.str();
    }

} // namespace util

#endif // UTIL_UTIL_HPP_INCLUDED
