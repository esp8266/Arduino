function buildopt_then_buildopt()
{
    build_esp8266_example "GlobalBuildOptions"

    local last_sketch=$(most_recent_dir "$sketches")
    assert_build "GlobalBuildOptions" "$last_sketch" 1
    assert_core 1

    local globalbuildoptions_commonh_stat=$(make_commonh_stat)
    local globalbuildoptions_core_stat=$(make_core_stat)

    build_esp8266_example "HwdtStackDump"

    last_sketch=$(most_recent_dir "$sketches")
    assert_build "HwdtStackDump" "$last_sketch" 1
    assert_core 1

    local hwdtstackdump_commonh_stat=$(make_commonh_stat)
    local hwdtstackdump_core_stat=$(make_core_stat)

    test "$hwdtstackdump_commonh_stat" != "$globalbuildoptions_commonh_stat"
    test "$hwdtstackdump_core_stat" != "$globalbuildoptions_core_stat"
}

buildopt_then_buildopt
