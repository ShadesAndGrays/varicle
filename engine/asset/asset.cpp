#include "engine/asset/asset.hpp"
#include "engine/asset/raylib-asset.hpp"

#include <algorithm>
#include <asset-packer.hpp>

#include <filesystem>
#include <iostream>
#include <ranges>
#include <string>
#include <variant>

namespace fs = std::filesystem;

// Helper to determine if the asset path implies streaming music
bool should_stream_as_music(const std::filesystem::path& asset_path) {
    for (const auto& part : asset_path) {
        if (part == "music" || part == "bgm") {
            return true;
        }
    }
    return false;
}

namespace varicle {

AssetLoader::AssetLoader(std::string asset_data) : asset_reader(asset_data) {
    if (!asset_reader.load_index()) {
        std::cerr << "Failed to load assets!!!" << std::endl;
    }
}

AssetReader AssetLoader::get_reader() {
    return asset_reader;
}

RaylibAssetLoader::RaylibAssetLoader(std::string asset_data)
    : AssetLoader(asset_data) {}

RaylibAssetLoader::~RaylibAssetLoader() {
    unload_all_assets();
}

void RaylibAssetLoader::load_asset(std::string path) {
    fs::path file_path(path);
    Asset    new_asset;
    new_asset.name = file_path.filename().string();

    // std::cout << file_path.extension() << std::endl;
    auto bytes = get_reader().extract_asset(path);

    if (file_path.extension() == ".png") {
        // Image img     = LoadImageFromMemory(".png", bytes.data(), bytes.size());
        // auto  texture = LoadTextureFromImage(img);
        // UnloadImage(img);
        // new_asset.data = ImageData{ new Texture(texture) };

    } else if (file_path.extension() == ".ogg") {

        // if (should_stream_as_music(file_path)) {
        //     auto music =
        //         LoadMusicStreamFromMemory(".ogg", bytes.data(), bytes.size());
        //     new_asset.data = MusicData{ new Music(music) };
        // } else {
        //     auto wave  = LoadWaveFromMemory(".ogg", bytes.data(), bytes.size());
        //     auto sound = LoadSoundFromWave(wave);
        //     UnloadWave(wave);
        //     new_asset.data = SoundData{ new Sound(sound) };
        // }

    } else if (file_path.extension() == ".mp3") {
        // if (should_stream_as_music(file_path)) {
        //     auto music =
        //         LoadMusicStreamFromMemory(".mp3", bytes.data(), bytes.size());
        //     new_asset.data = MusicData{ new Music(music) };
        // } else {
        //     auto wave  = LoadWaveFromMemory(".mp3", bytes.data(), bytes.size());
        //     auto sound = LoadSoundFromWave(wave);
        //     UnloadWave(wave);
        //     new_asset.data = SoundData{ new Sound(sound) };
        // }
        //
    } else if (file_path.extension() == ".wav") {
        // if (should_stream_as_music(file_path)) {
        //     auto music =
        //         LoadMusicStreamFromMemory(".wav", bytes.data(), bytes.size());
        //     new_asset.data = MusicData{ new Music(music) };
        // } else {
        //     auto wave  = LoadWaveFromMemory(".wav", bytes.data(), bytes.size());
        //     auto sound = LoadSoundFromWave(wave);
        //     UnloadWave(wave);
        //     new_asset.data = SoundData{ new Sound(sound) };
        // }
    }

    else if (file_path.extension() == ".json") {
        auto json      = std::string(bytes.begin(), bytes.end());
        new_asset.data = JsonData{ json };
    }

    assets[path] = new_asset;
}

void RaylibAssetLoader::unload_asset(std::string path) {
    if (!assets.contains(path))
        return;
    Asset asset = assets[path];

    std::visit(
        [](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, ImageData>) {
                // Texture* tex = static_cast<Texture*>(arg.texture);
                // UnloadTexture(*tex);
                // delete tex;
                arg.texture = nullptr;
            } else if constexpr (std::is_same_v<T, SoundData>) {
                // Sound* sound = static_cast<Sound*>(arg.sound);
                // UnloadSound(*sound);
                // delete sound;
                arg.sound = nullptr;
            } else if constexpr (std::is_same_v<T, MusicData>) {
                // Music* music = static_cast<Music*>(arg.music);
                // UnloadMusicStream(*music);
                // delete music;
                arg.music = nullptr;
            }
            // JsonData requires no manual step because std::string cleans
            // itself up!
        },
        asset.data
    );

    assets.erase(path);
}

void RaylibAssetLoader::unload_all_assets() {
    std::vector<std::string> assets_to_unload;
    std::ranges::copy(
        std::views::keys(assets), std::back_inserter(assets_to_unload)
    );
    for (const auto& path : assets_to_unload) {
        unload_asset(path);
    }
}

Asset RaylibAssetLoader::get_asset(std::string path) {
    if (assets.contains(path)) {
        return assets[path];
    } else {
        return {};
    }
}

Texture* RaylibAssetLoader::get_texture(std::string path) {
    Asset asset = get_asset(path);
    if (auto* ptr = std::get_if<ImageData>(&asset.data)) {
        return (Texture*)ptr->texture;
    }
    return nullptr;
}

Sound* RaylibAssetLoader::get_sound(std::string path) {
    Asset asset = get_asset(path);
    if (auto* ptr = std::get_if<SoundData>(&asset.data)) {
        return (Sound*)ptr->sound;
    }
    return nullptr;
}
Music* RaylibAssetLoader::get_music(std::string path) {
    Asset asset = get_asset(path);
    if (auto* ptr = std::get_if<MusicData>(&asset.data)) {
        return (Music*)ptr->music;
    }
    return nullptr;
}

std::string RaylibAssetLoader::get_json(std::string path) {
    Asset asset = get_asset(path);
    if (auto* ptr = std::get_if<JsonData>(&asset.data)) {
        return ptr->json;
    }
    return "{}";
}
} // namespace varicle
