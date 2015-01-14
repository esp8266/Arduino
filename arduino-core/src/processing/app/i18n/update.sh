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

catalog()
{
  # Generate the new text catalog without the already translated texts.
  # The 'merge existing' option for xgetext does not work propery for our purpose.
  find ../../../ -name '*.java' -print > "$files"
  find ../../../../../app/src -name '*.java' -print >> "$files"
  xgettext -s -L Java --from-code=utf-8 -k_ --output="$catalog" --files-from="$files"
}

update()
{
  echo -e "Updating $1...\c"
  cat "$catalog" | python python/update.py "$1"
  msgcat -p "$1" > $(basename "$1" .po).properties
  # msgcat may complain about "CHARSET" if you didn't replace "CHARSET" with
  # your correct charset.
  echo
}

if [ $0 != "./update.sh" ]; then
  echo "update.sh: Invoke this script as ./update.sh"
  exit 1
fi

while [ $# -gt 0 ]; do
  if [ $1 = '-a' ]; then
    targets=Resources_*.po
  else
    targets="$targets Resources_$1.po"
  fi
  shift
done

if [ "$targets" = "" ]; then
  echo "update.sh: Give at least one language code or '-a'."
  exit 1
fi

# Then, merge with already translated texts.
catalog
for target in $targets; do
  update $target
done
