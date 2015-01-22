# you need to have astyle installed before running this
find libraries/ hardware/ -name '*.ino' -exec astyle --options=build/shared/lib/formatter.conf {} \;
