function nobuildopt_then_buildopt()
{
    build_esp8266_example "Blink"

    local last_sketch=$(most_recent_dir "$sketches")
    assert_build "Blink" "$last_sketch" 0
    assert_core 0

    local blink_commonh_stat=$(make_commonh_stat)
    local blink_core_stat=$(make_core_stat)

    build_esp8266_example "HwdtStackDump"

    last_sketch=$(most_recent_dir "$sketches")
    assert_build "HwdtStackDump" "$last_sketch" 1
    assert_core 1

    local hwdtstackdump_commonh_stat=$(make_commonh_stat)
    local hwdtstackdump_core_stat=$(make_core_stat)

    test "$hwdtstackdump_commonh_stat" != "$blink_commonh_stat"
    test "$hwdtstackdump_core_stat" != "$blink_core_stat"
}

nobuildopt_then_buildopt
