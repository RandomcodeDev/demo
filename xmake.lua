add_rules(
    "mode.debug",
    "mode.release",
    "plugin.vsxmake.autoupdate"
)

set_project("demo")
set_version("0.0.0", {build = "%Y%m%d%H%M%S"})

set_allowedplats("windows", "linux", "freebsd", "switchhb")
set_allowedarchs("switchhb|arm64")

local switch_title_id = "0100694213488000"

includes("support/support.lua")
setup_support(
    "support",
    path.join("support", "deps"),
    not is_plat("switchhb"),
    false,
    true,
    true,
    "config.h.in",
    switch_title_id
)

target("demo")
    set_kind("binary")
    add_files("demo/*.c")
    add_deps("common", "platform", "util")
    set_default(true)

    support_executable("support")

    on_load(fix_target)
    before_build(function (target)
        target:set("support_data", {
            "Demo",
            "Randomcode Developers",
            switch_title_id
        })

        if not os.exists(path.join(target:targetdir(), "assets")) then
            os.ln(path.absolute(path.join("assets", "out")), path.join(target:targetdir(), "assets"))
        end
    end)
