#!/bin/bash

#
# Extract the text catalog from the source code,
# and merge with the already translated texts.
#
# Please be aware that this OVERWRITES the existing *.po and *.properties files.
# You may want to commit to your repository before running this script.
#

# by Shigeru KANEMOTO at SWITCHSCIENCE.
# on 2011-10-15

catalog="/tmp/$$.po"
files="/tmp/$$.files"

trap "rm -f '$catalog' '$files'" 0 1 2 15

# Generate the new text catalog without the already translated texts.
# The 'merge existing' option for xgetext does not work propery for our purpose.
find .. -name '*.java' -print > "$files"
xgettext -L Java --from-code=utf-8 -k_ --output="$catalog" --files-from="$files"

update()
{
  echo "Updating $1..."
  cat "$catalog" | python i18n_update.py "$1"
  msgcat -p "$1" > $(basename "$1" .po).properties
  # msgcat may complain about "CHARSET" if you didn't replace "CHARSET" with
  # your correct charset.
}

# Then, merge with already translated texts.
if [ $# = 0 ]; then
  for target in *.po; do
    update $target
  done
else
  for target in $*; do
    update Resources_$target.po
  done
fi
