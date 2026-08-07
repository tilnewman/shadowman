#ifndef UTIL_CHECK_MACROS_HPP_INCLUDED
#define UTIL_CHECK_MACROS_HPP_INCLUDED
//
// check-macros.hpp
//
#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

//

#ifdef NDEBUG
#define M_FAIL_HANDLER(exp, desc_str) throw std::runtime_error(desc_str);
#else
#define M_FAIL_HANDLER(exp, desc_str) assert((exp));
#endif

//

#if !defined(M_DISABLE_ALL_CHECK_MACROS)

#define M_MAKE_CODE_POSITION_STR_SS(macro_name, streamable_extra_info)                          \
    std::string _m_desc_str;                                                                    \
    {                                                                                           \
        std::ostringstream _m_desc_ss;                                                          \
                                                                                                \
        _m_desc_ss << streamable_extra_info;                                                    \
        const std::string _m_extra_str{ _m_desc_ss.str() };                                     \
                                                                                                \
        _m_desc_ss.str("");                                                                     \
        _m_desc_ss << macro_name << "::" << __FILE__ << "::" << __func__ << "()::" << __LINE__; \
                                                                                                \
        if (!_m_extra_str.empty())                                                              \
        {                                                                                       \
            _m_desc_ss << " \"" << _m_extra_str << "\"";                                        \
        }                                                                                       \
                                                                                                \
        _m_desc_str = _m_desc_ss.str();                                                         \
    }

#define M_MAKE_CODE_POSITION_STR(macro_name) M_MAKE_CODE_POSITION_STR_SS(macro_name, "");

//

#define M_LOG(streamable_message)                                 \
    {                                                             \
        M_MAKE_CODE_POSITION_STR_SS("M_LOG", streamable_message); \
        std::clog << _m_desc_str << '\n';                         \
    }

//

#define M_MAKE_DESCRIPTION_STR_SS(exp, macro_name, streamable_extra_info)       \
    std::string _m_desc_str;                                                    \
    {                                                                           \
        std::ostringstream _m_desc_ss;                                          \
                                                                                \
        _m_desc_ss << streamable_extra_info;                                    \
        const std::string _m_extra_str{ _m_desc_ss.str() };                     \
                                                                                \
        _m_desc_ss.str("");                                                     \
                                                                                \
        _m_desc_ss << macro_name << '(' << #exp << ") FAILED at:  " << __FILE__ \
                   << "::" << __func__ << "()::" << __LINE__;                   \
                                                                                \
        if (!_m_extra_str.empty())                                              \
        {                                                                       \
            _m_desc_ss << " \"" << _m_extra_str << "\"";                        \
        }                                                                       \
                                                                                \
        _m_desc_str = _m_desc_ss.str();                                         \
    }

#define M_MAKE_DESCRIPTION_STR(exp, macro_name) M_MAKE_DESCRIPTION_STR_SS((exp), macro_name, "");

//

#define M_CHECK_LOG(exp, streamable_extra_info)                                     \
    {                                                                               \
        if (!(exp))                                                                 \
        {                                                                           \
            M_MAKE_DESCRIPTION_STR_SS((exp), "M_CHECK_LOG", streamable_extra_info); \
            std::clog << _m_desc_str << '\n';                                       \
        }                                                                           \
    }

//

#define M_CHECK_THROW(exp, streamable_extra_info)                                     \
    {                                                                                 \
        if (!(exp))                                                                   \
        {                                                                             \
            M_MAKE_DESCRIPTION_STR_SS((exp), "M_CHECK_THROW", streamable_extra_info); \
            std::cerr << _m_desc_str << '\n';                                         \
            throw std::runtime_error(_m_desc_str);                                    \
        }                                                                             \
    }

//

#define M_CHECK_ASSERT(exp, streamable_extra_info)                                     \
    {                                                                                  \
        if (!(exp))                                                                    \
        {                                                                              \
            M_MAKE_DESCRIPTION_STR_SS((exp), "M_CHECK_ASSERT", streamable_extra_info); \
            std::cerr << _m_desc_str << '\n';                                          \
            assert((exp));                                                             \
        }                                                                              \
    }

//

#define M_CHECK(exp, streamable_extra_info)                                     \
    {                                                                           \
        if (!(exp))                                                             \
        {                                                                       \
            M_MAKE_DESCRIPTION_STR_SS((exp), "M_CHECK", streamable_extra_info); \
            std::cerr << _m_desc_str << '\n';                                   \
            M_FAIL_HANDLER((exp), _m_desc_str);                                 \
        }                                                                       \
    }

#else // defined(M_DISABLE_ALL_CHECK_MACROS)

#define M_LOG(streamable_message) ;
#define M_MAKE_DESCRIPTION_STR_SS(exp, macro_name, streamable_extra_info) ;
#define M_MAKE_DESCRIPTION_STR(exp, macro_name) ;
#define M_CHECK_LOG(exp, streamable_extra_info) ;
#define M_CHECK_THROW(exp, streamable_extra_info) ;
#define M_CHECK_ASSERT(exp, streamable_extra_info) ;
#define M_CHECK(exp, streamable_extra_info) ;

#endif // defined(M_DISABLE_ALL_CHECK_MACROS)

#endif // UTIL_CHECK_MACROS_HPP_INCLUDED
