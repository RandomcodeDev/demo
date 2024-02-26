add_rules(
    "mode.debug",
    "mode.release",
    "plugin.vsxmake.autoupdate"
)

set_project("demo")
set_version("0.0.0", {build = "%Y%m%d%H%M%S"})

includes("support/support.lua")
setup_support("support", path.join("support", "deps"), true, false, true, true, "config.h.in")

target("demo")
    set_kind("binary")
    add_files("*.c")
    add_deps("common", "platform", "util")
    set_default(true)

    support_executable("support")

    on_load(fix_target)
    before_build(function (target)
        if not os.exists(path.join(target:targetdir(), "assets")) then
            os.ln(path.absolute(path.join("assets", "out")), path.join(target:targetdir(), "assets"))
        end
    end)
