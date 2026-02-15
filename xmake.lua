add_rules("mode.debug", "mode.release", "mode.releasedbg")

add_repositories("levimc-repo https://github.com/LiteLDev/xmake-repo.git", "groupmountain-repo https://github.com/GroupMountain/xmake-repo.git", "miracleforest https://github.com/MiracleForest/xmake-repo")

option("target_type")
    set_default("server")
    set_showmenu(true)
    set_values("server", "client")
option_end()

add_requires("levilamina", {configs = {target_type = get_config("target_type")}})

add_requires("levibuildscript", "gmlib", "ilistenattentively")

if not has_config("vs_runtime") then
    set_runtimes("MD")
end

target("Cleaner")
    add_rules("@levibuildscript/linkrule")
    add_rules("@levibuildscript/modpacker")
    add_cxflags( "/EHa", "/utf-8", "/W4", "/w44265", "/w44289", "/w44296", "/w45263", "/w44738", "/w45204")
    add_defines("NOMINMAX", "UNICODE", "_HAS_CXX23=1")
    add_packages("levilamina", "gmlib", "ilistenattentively")
    set_exceptions("none")
    set_kind("shared")
    set_languages("c++20")
    set_symbols("debug")
    add_headerfiles("src/**.h")
    add_files("src/**.cpp")
    add_includedirs("src")
    if is_config("target_type", "server") then
    --  add_includedirs("src-server")
    --  add_files("src-server/**.cpp")
    else
    --  add_includedirs("src-client")
    --  add_files("src-client/**.cpp")
    end
    after_build(function (target)
        local mod_packer = import("scripts.after_build")

        local mod_define = {
            modName = target:name(),
            modFile = path.filename(target:targetfile()),
        }
        
        mod_packer.pack_mod(target,mod_define)
    end)