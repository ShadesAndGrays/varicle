#pragma once
#include <asset-packer.hpp>

#include <string>
#include <unordered_map>
#include <variant>

struct Image{};
struct Texture{};
struct Sound{};
struct Music{};

namespace varicle {

struct ImageData {
    void* texture;
};

struct SoundData {
    void* sound;
};

struct MusicData {
    void* music;
};

struct JsonData {
    std::string json;
};

struct Asset {
    std::string                                             name;
    std::variant<ImageData, SoundData, MusicData, JsonData> data;
};

class AssetLoader {

  private:
    AssetReader asset_reader;

  protected:
    std::unordered_map<std::string, Asset> assets;
    virtual void load_all_assets(std::string data_path) = 0;
    virtual void unload_all_assets()                    = 0;

  public:
    AssetLoader(std::string asset_data);
    ~AssetLoader()                               = default;
    virtual void  load_asset(std::string path)   = 0;
    virtual void  unload_asset(std::string path) = 0;
    virtual Asset get_asset(std::string path)    = 0;
    ;
    AssetReader get_reader();
};

} // namespace varicle
