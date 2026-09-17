#include <asset-packer.hpp>

#include <string>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, const char** argv){

    std::string asset_folder_path;
    std::string blob_path;

    if (argc == 1){
        std::cerr << argc-1 << " arguments found " << std::endl;
        std::cerr << "Format: ./asset_packer ASSET_FOLDER_PATH [BLOB_PATH]" << std::endl;
        return -1;
    }

    if (std::string(argv[1]) == "--help"  ||std::string(argv[1]) == "--help" ){
        std::cerr << "Format: ./asset_packer ASSET_FOLDER_PATH [BLOB_PATH] " << std::endl;
        return -1;
    }

    if (argc == 2){
        asset_folder_path = argv[1];
        blob_path = "data.dat";
    }

    else if (argc == 3){
        blob_path = argv[2];
    }

    std::vector<std::string> files_to_pack = {};

    fs::path target_path = asset_folder_path;
    if (fs::exists(target_path) && fs::is_directory(target_path)){
        for (const auto& entry : fs::directory_iterator(target_path)){
            if(entry.is_regular_file()){
                files_to_pack.push_back(entry.path().relative_path().string());
            }
        }
    }

    AssetPacker asset_packer;
    asset_packer.pack_assets(blob_path, files_to_pack);

    return 0;
}
