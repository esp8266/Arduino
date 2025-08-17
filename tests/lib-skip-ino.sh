# return 0 if this sketch should not be built in CI (for other archs, not needed, etc.)
function skip_ino()
{
    local ino=$1

    case "$ino" in
    *"/#attic/"* | \
    *"/AvrAdcLogger/"* | \
    *"/RtcTimestampTest/"* | \
    *"/SoftwareSpi/"* | \
    *"/TeensyDmaAdcLogger/"* | \
    *"/TeensyRtcTimestamp/"* | \
    *"/TeensySdioDemo/"* | \
    *"/TeensySdioLogger/"* | \
    *"/UnicodeFilenames/"* | \
    *"/UserChipSelectFunction/"* | \
    *"/UserSPIDriver/"* | \
    *"/debug/"* | \
    *"/examplesV1/"* | \
    *"/onewiretest/"*)
        return 0
        ;;
    *"Teensy"*)
        return 0
        ;;
    *)
        ;;
    esac

    return 1
}

# return reason if this sketch is not the main one or it is explicitly disabled with .test.skip in its directory
function skip_sketch()
{
    local sketch=$1

    local sketchdir
    sketchdir=$(dirname $sketch)

    local sketchdirname
    sketchdirname=$(basename $sketchdir)

    local sketchname
    sketchname=$(basename $sketch)

    if [[ "${sketchdirname}.ino" != "$sketchname" ]]; then
        echo "Skipping $sketch (not the main sketch file)"
    fi
    if skip_ino "$sketch" || [[ -f "$sketchdir/.test.skip" ]]; then
        echo "Skipping $sketch"
    fi
}
