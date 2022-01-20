#!/bin/sh

set -e

org=$(cd ${0%/*}; pwd)
cd ${org}/..
pwd
test -d cores/esp8266
test -d libraries

#########################################

makeClangConf()
{
    IndentWidth="$1"
    IndentCaseLabels="$2"
    BreakBeforeBraces="$3"

    cat << EOF > .clang-format
BasedOnStyle: WebKit
SortIncludes: false
AlignTrailingComments: true
ColumnLimit: 0
KeepEmptyLinesAtTheStartOfBlocks: false
SpaceBeforeInheritanceColon: false
SpacesBeforeTrailingComments: 2
AlignTrailingComments: true
AlignConsecutiveAssignments: Consecutive
AlignConsecutiveBitFields: Consecutive
AlignConsecutiveDeclarations: Consecutive
AlignAfterOpenBracket: Align
BreakConstructorInitializers: AfterColon
BreakBeforeBinaryOperators: All
BreakBeforeConceptDeclarations: true
FixNamespaceComments: true
NamespaceIndentation: Inner
BreakBeforeBraces: ${BreakBeforeBraces}
IndentWidth: ${IndentWidth}
IndentCaseLabels: ${IndentCaseLabels}
EOF

}

#########################################
# 'all' variable should be "cores/esp8266 libraries"

all="
libraries/ESP8266mDNS
libraries/Wire
libraries/lwIP*
cores/esp8266/Lwip*
cores/esp8266/debug*
cores/esp8266/core_esp8266_si2c.cpp
cores/esp8266/StreamString.*
cores/esp8266/StreamSend.*
libraries/Netdump
tests
"

#########################################
# restyling core

makeClangConf 4 false Allman

for d in $all; do
    if [ -d "$d" ]; then
        echo "-------- directory $d:"
        for e in c cpp h; do
            find $d -name "*.$e" -exec clang-format-12 -i {} \;
        done
    else
        echo "-------- file $d:"
        clang-format-12 -i ${d}
    fi
done

#########################################
# restyling arduino examples

makeClangConf 2 true Attach

for d in libraries; do
    echo "-------- examples in $d:"
    find $d -name "*.ino" -exec clang-format-12 -i {} \;
done
