// asset.hpp
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

// Struct size is exactly 80 bytes
struct AssetMetadata {
    char name[64];       // Fixed size string for the filename
    uint64_t offset;     // Where the raw file bytes begin in the blob
    uint64_t size;       // How many bytes long the file is
};

class AssetReader{
    private:
        std::string blob_path;
        std::unordered_map<std::string,AssetMetadata> index_table;
    public:
        AssetReader(const std::string& path): blob_path(path){}
        ~AssetReader() = default;
        bool load_index();
        std::vector<uint8_t> extract_asset(const std::string& asset_name);
       std::vector<std::string> get_asset_list();
       AssetMetadata get_asset_metadata(std::string path);

};

class AssetPacker{
    public:
        AssetPacker() = default;
        ~AssetPacker() = default;
        void pack_assets(const std::string& output_blob_path, const std::vector<std::string>& files_to_pack);
};
