case "${RUNNER_OS-}" in
"Linux")
    fetch_and_unpack "Linux_64bit.tar.gz" \
        "d421e2b1cbef59c41e46cf06d077214a1d24cb784030462763781c9d3911cc55257fbcc02a7ee6a2ddda5b459101dc83aeda6b3b5198805bfdce856f82774c93" \
        "${urlbase}Linux_64bit.tar.gz"
    ;;
"Windows")
    fetch_and_unpack "Windows_64bit.zip" \
        "05b4eb5820fbaf670de00399d40513ecf2de9d0c2c5593a1227be03b2d11ba53e9d14cf6f934110447d6fd15c6a09769606a34fcab32ec3c2dbaa42f4627b072" \
        "${urlbase}Windows_64bit.zip"
    ;;
"macOS")
    if [ "${RUNNER_ARCH-}" = "ARM64" ] ; then
        fetch_and_unpack "macOS_ARM64.tar.gz" \
            "672693418b730d8ebc57cae2c892553e821706bee06312cc77a598e834afcba7d380df4d337138ecc03a4013a349d89b744b2a3b97fafc214b619856d9162827" \
            "${urlbase}macOS_ARM64.tar.gz"
    else
        fetch_and_unpack "macOS_64bit.tar.gz" \
            "5659f08d787840aa6689fd063477402b4ed572663fea20de496b249d86a440059e3e6f377bd8020fb6b67202c1bdea6f98a4c4e052c31f01b2c9027ebec10b04" \
            "${urlbase}macOS_64bit.tar.gz"
    fi
    ;;
*)
    echo 'Unknown ${RUNNER_OS} = "' ${RUNNER_OS} '"'
    exit 2
esac

