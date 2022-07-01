#include <util/file_utils.h>
#include <filesystem>

namespace NeuroEvo {

std::vector<std::string> collect_dirs_in(const std::string& parent_dir_path)
{
   //First check whether the parent directory exists 
    if(!boost::filesystem::exists(parent_dir_path))
    {
        std::cerr << "No such directory: " << parent_dir_path << std::endl;
        std::cerr << "Check argument for collect_dirs_in" << std::endl;
        exit(0);
    }

    std::vector<std::string> directories; 

    for(const auto& dir : std::filesystem::directory_iterator(parent_dir_path))
        if(std::filesystem::is_directory(dir))
            directories.push_back(dir.path());

    return directories;

}

} // namespace NeuroEvo
