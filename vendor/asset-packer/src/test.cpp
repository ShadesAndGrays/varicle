#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <string>
#include <filesystem>

// Include your library headers here
#include <asset-packer.hpp> 

void run_asset_pipeline_tests() {
    std::cout << "[TEST] Initializing Asset Pipeline Tests...\n";

    // Setup temporary testing files
    std::string test_blob = "test_data.dat";
    std::string text_file = "test_config.json";
    std::string fake_png  = "test_player.png";

    // 1. Create dummy files on disk to pack
    std::ofstream f1(text_file);
    f1 << "{ \"speed\": 250 }";
    f1.close();

    std::ofstream f2(fake_png, std::ios::binary);
    f2 << "FAKE_PNG_BINARY_DATA_HEADER";
    f2.close();

    // -------------------------------------------------------------
    // TEST 1: Packing Verification
    // -------------------------------------------------------------
    AssetPacker packer;
    std::vector<std::string> files_to_pack = { text_file, fake_png };
    
    // This should run without throwing errors or truncating files
    packer.pack_assets(test_blob, files_to_pack);
    assert(std::filesystem::exists(test_blob) && "FAIL: Blob file was not created!");

    // -------------------------------------------------------------
    // TEST 2: Reading and Integrity Verification (Round-Trip)
    // -------------------------------------------------------------
    AssetReader reader(test_blob);
    bool index_loaded = reader.load_index();
    assert(index_loaded && "FAIL: Reader could not parse blob index!");

    // Extract the JSON back out
    std::vector<uint8_t> extracted_json_bytes = reader.extract_asset(text_file);
    std::string extracted_json(extracted_json_bytes.begin(), extracted_json_bytes.end());
    
    // Assert that the file contents match exactly what we put in
    assert(extracted_json == "{ \"speed\": 250 }" && "FAIL: Extracted data does not match original data!");
    std::cout << "  -> Pass: Data round-trip integrity verified.\n";

    // -------------------------------------------------------------
    // TEST 3: Graceful Missing Asset Handling
    // -------------------------------------------------------------
    std::vector<uint8_t> missing = reader.extract_asset("non_existent_file.png");
    assert(missing.empty() && "FAIL: Requesting a missing file should return an empty buffer!");
    std::cout << "  -> Pass: Missing file boundaries handled gracefully.\n";

    // Clean up test file clutter from the hard drive
    std::filesystem::remove(text_file);
    std::filesystem::remove(fake_png);
    std::filesystem::remove(test_blob);

    std::cout << "[TEST] ALL ASSET PIPELINE TESTS PASSED SUCCESSFULY!\n\n";
}

int main() {
    run_asset_pipeline_tests();
    return 0;
}
