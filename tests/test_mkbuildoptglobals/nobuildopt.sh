function nobuildopt()
{
    build_esp8266_example "Blink"

    local last_sketch=$(most_recent_dir "$sketches")
    assert_build "Blink.ino" "$last_sketch" 0
    assert_core 0

    local blink_commonh_stat=$(make_commonh_stat)
    local blink_core_stat=$(make_core_stat)

    build_esp8266_example "TestEspApi"

    last_sketch=$(most_recent_dir "$sketches")
    assert_build "TestEspApi" "$last_sketch" 0
    assert_core 0

    local testespapi_commonh_stat=$(make_commonh_stat)
    local testespapi_core_stat=$(make_core_stat)

    test "$blink_commonh_stat" = "$testespapi_commonh_stat"
    test "$blink_core_stat" = "$testespapi_core_stat"
}

nobuildopt
