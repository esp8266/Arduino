#!/bin/bash

if [ $0 != "./pull.sh" ]; then
  echo "pull.sh: Invoke this script as ./pull.sh"
  exit 1
fi

ALL='n'
while [ $# -gt 0 ]; do
  if [ $1 = '-a' ]; then
    ALL='y'
    # We must do this until java Locale class is updated...
    mv Resources_iw.po Resources_he.po
    mv Resources_iw.properties Resources_he.properties
    mv Resources_in.po Resources_id.po
    mv Resources_in.properties Resources_id.properties

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

if [ "$ALL" = "y" ]; then
  # We must do this until java Locale class is updated...
  mv Resources_he.po Resources_iw.po
  mv Resources_he.properties Resources_iw.properties
  mv Resources_id.po Resources_in.po
  mv Resources_id.properties Resources_in.properties
fi

for lang in $langs; do
  if [ -f "Resources_$lang.po" ]; then
    msgcat -p Resources_$lang.po > Resources_$lang.properties
  fi
done


