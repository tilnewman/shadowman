//
// level-files.cpp
//
#include "level-files.hpp"

namespace shadowman
{

	LevelFileManager::LevelFileManager()
        : m_levelFilenames{ "level-2.json", "level-1.json" }
        , m_iterator{ std::begin(m_levelFilenames) }
    {}

}
