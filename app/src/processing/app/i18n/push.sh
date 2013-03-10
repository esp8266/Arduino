#!/bin/bash

if [ $0 != "./push.sh" ]; then
  echo "push.sh: Invoke this script as ./push.sh"
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
  echo "push.sh: Give at least one language code."
  exit 1
fi

exec python python/push.py $langs
