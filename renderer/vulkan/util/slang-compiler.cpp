#include <slang/slang-com-helper.h>
#include <slang/slang-com-ptr.h>
#include <slang/slang-gfx.h>
#include <slang/slang.h>

#include "slang-compiler.hpp"

inline bool checkSlangResult(
    SlangResult   result,
    slang::IBlob* diagnosticsBlob,
    const char*   fileName,
    int           line
) {
    if (SLANG_FAILED(result)) {
        std::println(
            "Failed Slang check at {}:{}: Code {:#x}",
            fileName,
            line,
            static_cast<uint32_t>(result)
        );

        if (diagnosticsBlob && diagnosticsBlob->getBufferPointer()) {
            std::println(
                "Diagnostics:\n{}",
                static_cast<const char*>(diagnosticsBlob->getBufferPointer())
            );
        }
        return false;
    }
    return true;
}

// Macro helper to pass source location automatically
#define dc(res, diag) checkSlangResult((res), (diag), __FILE_NAME__, __LINE__)

void dcp(void* ptr, Slang::ComPtr<slang::IBlob> diagnostic) {

    if (ptr == nullptr) {
        println("Falied to Pointer check {}: {}", __FILE_NAME__, __LINE__);
    }

    if (diagnostic) {
        println(
            "{} at {}: {}",
            (const char*)diagnostic->getBufferPointer(),
            __FILE_NAME__,
            __LINE__
        );
    }
}

std::vector<CompiledShader> compile_shader_code(
    std::string              path,
    std::vector<std::string> entry_point_names,
    std::vector<std::string> include_dirs
) {

    std::vector<const char*> raw_search_paths;
    for (const auto& dir : include_dirs) {
        raw_search_paths.push_back(dir.c_str());
    }
    // "shaders/include",
    // "shaders/common",

    // slang::IGlobalSession* slang_global_session = nullptr;
    Slang::ComPtr<slang::IGlobalSession> slang_global_session;
    auto result = slang::createGlobalSession(slang_global_session.writeRef());

    if (!(SLANG_SUCCEEDED(result) && slang_global_session)) {
        println("Falied to create global session");
        return {};
    }

    slang::TargetDesc target_desc = {};
    target_desc.format            = SLANG_SPIRV;
    target_desc.profile = slang_global_session->findProfile("spirv_1_5");

    slang::SessionDesc session_desc = {};

    session_desc.searchPaths = raw_search_paths.data();
    session_desc.searchPathCount =
        static_cast<SlangInt>(raw_search_paths.size());

    session_desc.targets     = &target_desc;
    session_desc.targetCount = 1;

    Slang::ComPtr<slang::ISession> slang_session;
    result = slang_global_session->createSession(
        session_desc, slang_session.writeRef()
    );

    if (!(SLANG_SUCCEEDED(result) && slang_session)) {
        println("Falied to create slang session");
        return {};
    }

    Slang::ComPtr<slang::IModule> slang_module;
    Slang::ComPtr<slang::IBlob>   diagnostic_blob;

    slang_module =
        slang_session->loadModule(path.c_str(), diagnostic_blob.writeRef());

    dcp(slang_module, diagnostic_blob);

    std::vector<Slang::ComPtr<slang::IEntryPoint>> entry_points;

    for (auto e : entry_point_names) {
        Slang::ComPtr<slang::IEntryPoint> entry_point;

        result = slang_module->findEntryPointByName(
            e.c_str(), entry_point.writeRef()
        );
        dc(result, nullptr);
        entry_points.push_back(std::move(entry_point));
    }

    std::vector<slang::IComponentType*> components;
    components.push_back(slang_module);
    for (auto e : entry_points) {
        components.push_back(e.get());
    }

    Slang::ComPtr<slang::IComponentType> program;

    result = slang_session->createCompositeComponentType(
        components.data(),
        components.size(),
        program.writeRef(),
        diagnostic_blob.writeRef()
    );
    dc(result, diagnostic_blob);

    std::vector<CompiledShader> compiled_shaders;

    for (auto i = 0; i < entry_point_names.size(); i++) {
        CompiledShader shader;

        Slang::ComPtr<slang::IBlob> compiled_code;

        result = program->getEntryPointCode(
            i, 0, compiled_code.writeRef(), diagnostic_blob.writeRef()
        );

        dc(result, diagnostic_blob);

        const uint32_t* ptr =
            static_cast<const uint32_t*>(compiled_code->getBufferPointer());
        size_t code_size = compiled_code->getBufferSize() / sizeof(uint32_t);

        shader.entry_point = entry_point_names[i];
        shader.code.assign(ptr, ptr + code_size);

        compiled_shaders.push_back(shader);
    }

    return std::move(compiled_shaders);
}
