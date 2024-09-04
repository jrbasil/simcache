## simcache

A cache simulator implemented in the C++ programming language.

#### Instructions

1. Enter, into the trace file, lines; each comprised of the following inputs:
    1. either 'l' for load or 's' for store (as a char)
    2. 32-bit memory address (as a hex value)
    3. bytes being requested (as an integer)
    * example: s 0x12345678 4
2. Compile the CPP file and run the program.
3. Enter, into the terminal, four inputs corresponding to:
    1. cache size (as an integer)
    2. line size (as a integer)
    3. lines per set (as an integer) 
    4. update on use (as a boolean)
    * example: 2048 8 4 1
* Inputs can be formed on a single line with a space between inputs or on multiple lines with a carriage return between inputs.

#### Meta

* date:  30 Nov 2022
* author: JR Basil
