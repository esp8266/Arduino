This is essentially the same as the original, classic, example. However instead of treating the NTP message as a mysterious array of bytes it is referred to by its offical name and structure.
The concept of Big-Endian and Little_Endian data is introduced, which may be new to many readers.
Actually there is a good historical reason for the dominance of BigEndian in internet protocols. In the early days of networking Sun computers were the major force and they had BigEndian processors. Later on Intel entered the field with LittleEndian processors and the rest is history. I wonder how many CPU cycles are wasted every second converting between the two?
Treating byte arrays as structures encourages good programming style as 'magic numbers' are replaced by meaningful names.
