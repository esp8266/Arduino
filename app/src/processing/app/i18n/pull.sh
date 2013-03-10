#!/bin/bash

if [ $0 != "./pull.sh" ]; then
  echo "pull.sh: Invoke this script as ./pull.sh"
  exit 1
fi

while [ $# -gt 0 ]; do
  if [ $1 = '-a' ]; then
    for f in Resources_*.po; do
      f=$(expr "$f" : "Resources_\(.*\).po")
      langs="$langs $f"
    done
  else
    langs="$langs $1"
  fi
  shift
done

if [ "$langs" = "" ]; then
  echo "pull.sh: Give at least one language code."
  exit 1
fi

python python/pull.py $langs

for lang in $langs; do
  if [ -f "Resources_$lang.po" ]; then
    msgcat -p Resources_$lang.po > Resources_$lang.properties
  fi
done
