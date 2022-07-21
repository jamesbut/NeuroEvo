#ifndef _FILE_UTILS_H_
#define _FILE_UTILS_H_

/*
 * A few functions to help with processing files and directories
 */

#include <vector>
#include <boost/filesystem.hpp>
#include <iostream>

namespace NeuroEvo {

//Collects the paths to all the subdirectories in a directory
std::vector<std::string> collect_dirs_in(const std::string& parent_dir_path);

//Recursively collects files in directory and sub directories
std::vector<std::string> collect_files_recursive(
    const std::string& parent_dir_path
);

} // namespace NeuroEvo

#endif
