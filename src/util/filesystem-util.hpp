#ifndef UTIL_FILESYSTEM_UTIL_HPP_INCLUDED
#define UTIL_FILESYSTEM_UTIL_HPP_INCLUDED
//
// filesystem-util.hpp
//
#include <filesystem>
#include <ranges>
#include <string>
#include <vector>
#include <algorithm>

namespace util
{

    inline std::vector<std::filesystem::path> findFilesInDirectory(
        const std::filesystem::path & t_dirPath, const std::string & t_extension)
    {
        std::vector<std::filesystem::path> files;

        if (!std::filesystem::exists(t_dirPath) || !std::filesystem::is_directory(t_dirPath))
        {
            return files;
        }

        // based on how many files I know are in the media folders
        files.reserve(64); 

        const std::filesystem::directory_iterator dirIter{
            t_dirPath, std::filesystem::directory_options::skip_permission_denied
        };

        auto regularFilesWithExtensionView =
            dirIter |
            std::views::filter([](const auto & entry) { return entry.is_regular_file(); }) |
            std::views::filter(
                [&](const auto & entry) { return (entry.path().extension() == t_extension); });

        std::ranges::copy(regularFilesWithExtensionView, std::back_inserter(files));
        std::ranges::sort(files);
        return files;
    }

} // namespace util

#endif // UTIL_FILESYSTEM_UTIL_HPP_INCLUDED
