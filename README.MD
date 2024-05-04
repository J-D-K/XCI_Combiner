# xci_combiner
A simple program to combine split XCI dumps written in C++.

### Why?
I was dumping my entire Switch library to my Fat32 formatted SD card and wanted something that would take care of most of the work while I took care of other things. I also wanted to test, rework, and clean up some old multi-threaded file copying code. Seemed like a good chance to do both.

### How?
1. Clone this repo
2. Type `make` or `mingw32-make`
3. Type `xci_combiner "path:/to/split/dumps/" "path:/to/output/to/"`
4. If you're using the latest nxDumpTool, xci_combiner will use the directory's name and the contents of it to combine them without anymore input needed.
