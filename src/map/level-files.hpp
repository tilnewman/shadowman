#ifndef MAP_LEVEL_FILES_HPP_INCLUDED
#define MAP_LEVEL_FILES_HPP_INCLUDED
//
// level-files.hpp
//
#include <string>
#include <vector>
#include <optional>

namespace shadowman
{

    struct Context;

    class LevelFileManager
    {
      public:
        LevelFileManager();

        inline const std::string current() const 
        {
            if (std::end(m_levelFilenames) != m_iterator)
            {
                return *m_iterator;
            }
            else
            {
                return "";
            }
        }

        inline void increment()
        {
            if (std::end(m_levelFilenames) != m_iterator)
            {
                ++m_iterator;
            }
        }

      private:
        std::vector<std::string> m_levelFilenames;
        std::vector<std::string>::iterator m_iterator;
    };

} // namespace shadowman

#endif // MAP_LEVEL_FILES_HPP_INCLUDED
