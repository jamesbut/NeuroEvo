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

std::vector<std::string> collect_files_recursive(
    const std::string& parent_dir_path
)
{
    std::vector<std::string> file_paths;

    for(const auto& di :
        std::filesystem::recursive_directory_iterator(parent_dir_path))
        if(di.is_regular_file())
            file_paths.push_back(di.path().string());

    return file_paths;
}

} // namespace NeuroEvo
