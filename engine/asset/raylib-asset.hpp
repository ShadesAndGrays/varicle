#pragma once
// #include "asset_packer/asset.hpp"
#include "engine/asset/asset.hpp"


namespace varicle {

class RaylibAssetLoader : public AssetLoader {
  protected:
    void load_all_assets(std::string asset_list) override {};
    void unload_all_assets() override;

  public:
    RaylibAssetLoader(std::string asset_data = "data.dat");
    ~RaylibAssetLoader();

    void  load_asset(std::string path) override;
    void  unload_asset(std::string path) override;
    Asset get_asset(std::string path) override;

    Texture*    get_texture(std::string path);
    Sound*      get_sound(std::string path);
    Music*      get_music(std::string path);
    std::string get_json(std::string path);
};

// extern RaylibAssetLoader asset_loader;

} // namespace varicle
