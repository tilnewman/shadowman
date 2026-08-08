#ifndef JSON_WRAPPER_HPP_INCLUDED
#define JSON_WRAPPER_HPP_INCLUDED
//
// json-wrapper.hpp
//

#if defined(__gnu_linux__) || defined(__APPLE__)
#pragma GCC diagnostic push
#endif

#if defined(__gnu_linux__)
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wuseless-cast"
#endif

#if defined(__APPLE__)
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wdocumentation-unknown-command"
#pragma GCC diagnostic ignored "-Wdocumentation-pedantic"
#endif

#include "json.hpp"

#if defined(__gnu_linux__) || defined(__APPLE__)
#pragma GCC diagnostic pop
#endif

#endif // JSON_WRAPPER_HPP_INCLUDED
