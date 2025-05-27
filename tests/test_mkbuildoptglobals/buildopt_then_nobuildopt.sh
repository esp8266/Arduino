function buildopt_then_nobuildopt()
{
    build_esp8266_example "GlobalBuildOptions"

    local last_sketch=$(most_recent_dir "$sketches")
    assert_build "GlobalBuildOptions" "$last_sketch" 1
    assert_core 1

    local globalbuildoptions_commonh_stat=$(make_commonh_stat)
    local globalbuildoptions_core_stat=$(make_core_stat)

    build_esp8266_example "Blink"

    last_sketch=$(most_recent_dir "$sketches")
    assert_build "Blink" "$last_sketch" 0
    assert_core 0

    local blink_commonh_stat=$(make_commonh_stat)
    local blink_core_stat=$(make_core_stat)

    test "$globalbuildoptions_commonh_stat" != "$blink_commonh_stat"
    test "$globalbuildoptions_core_stat" != "$blink_core_stat"
}

buildopt_then_nobuildopt
