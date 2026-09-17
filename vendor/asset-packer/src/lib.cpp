// lib.cpp
#include <cstdint>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <format>
#include <unordered_map>
#include <ranges>

#include <asset-packer.hpp>


bool AssetReader::load_index(){
    std::ifstream blob(blob_path,std::ios::binary);
    if(!blob) return false;

    uint32_t file_count = 0;
    blob.read(reinterpret_cast<char*>(&file_count),sizeof(file_count));
    for (uint32_t i = 0; i < file_count; i++){
        AssetMetadata meta;
        blob.read(reinterpret_cast<char*>(&meta), sizeof(AssetMetadata));
        index_table[std::string(meta.name)] = meta;
    }

    blob.close();

    return true;
}

std::vector<uint8_t> AssetReader::extract_asset(const std::string& asset_name){
    if (!index_table.contains(asset_name)){
        std::cerr << "Asset not found in blob: " << asset_name << "\n";
        return {};
    }
    AssetMetadata meta = index_table[asset_name];
    std::ifstream blob(blob_path,std::ios::binary);

    blob.seekg(meta.offset);
    std::vector<uint8_t> buffer(meta.size);
    blob.read(reinterpret_cast<char*>(buffer.data()),meta.size);
    blob.close();

    return buffer;
}



void AssetPacker::pack_assets(const std::string& output_blob_path, const std::vector<std::string>& files_to_pack){
    std::ofstream blob(output_blob_path,std::ios::binary);
    if (!blob){std::cerr << "Failed to create blob file!" << std::endl; return;}

    uint32_t file_count = files_to_pack.size();
    blob.write(reinterpret_cast<char*>(&file_count), sizeof(file_count));
    // Write how many files there are  


    uint64_t current_data_offset = sizeof(file_count) + (file_count * sizeof(AssetMetadata)); // create and offset pointer for the number of file and our files metadata

    std::vector<AssetMetadata> metadata_table;

    for (auto &file_path : files_to_pack){

        std::ifstream file(file_path,std::ios::binary | std::ios::ate);

        if (!file){std::cerr << "Skipping missing file " << file_path << std::endl; continue; }

        uint64_t file_size = file.tellg();

        // create metadata
        AssetMetadata meta{};
        size_t length = file_path.copy(meta.name, sizeof(meta.name) - 1);
        meta.name[length] = '\0'; // Explicitly guarantee null-termination

        meta.offset = current_data_offset;
        meta.size = file_size;

        metadata_table.push_back(meta);
        current_data_offset += file_size;

        file.close();
    } 

    // write meta data
    for (const auto& meta : metadata_table) {
        blob.write(reinterpret_cast<const char*>(&meta), sizeof(AssetMetadata));
    }

    // second pass
    for (const auto& file_path: files_to_pack){
        std::ifstream file (file_path,std::ios::binary);
        if (!file) continue;
        blob << file.rdbuf();
        file.close();
    }

    for (auto meta :metadata_table){
        std:: cout << std::format("stored {}",meta.name) << std::endl;
    }
    blob.close();
    std::cout << "Successfully packed " << metadata_table.size() << " files into " << output_blob_path << "\n";

}


std::vector<std::string> AssetReader::get_asset_list(){
    auto key_view = std::views::keys(index_table);
    return std::vector<std::string>  {key_view.begin(),key_view.end()};
}

AssetMetadata AssetReader::get_asset_metadata(std::string path){
    if (index_table.contains(path)){
        return index_table[path];
    }
    return  {}; 
}
