# Presentation

wtools implements few tools aimed to handle, sort and/or store data based on 
key / value principles.

## keytree
Keytree is used to store values linked to stringed keys sorted on an alphabetical search.
Depending on the amount and the variation of the keys, this key/value store may be faster than
a hash table or a red/black tree.
Once, one told me that the performance of this solution may depends on the Processor data cache size.

To be short, imagine you need to store values of these keys: "acdc", "the beattles", "the clash" and "the rolling stones".
The key map will be:
- a - c - d - c
- t - h - e -   - b - e - a - t - t - l - e - s
- t - h - e -   - c - l - a - s - h
- t - h - e -   - r - o - l - l - i - n - g -   - s - t- o - n - e - s

The principle of the lookup function is that you will only have four ascii character comparison to find "the rolling stones" instead of four strcmp().

# Binary Key Value
Bkv helps serializing and parsing of key/value. It can be generated from JSON and JSON can be generated from it.





#BUILD

As usual with cmake:
> mkdir build 
> cd build
> cmake ..
> make

#CMAKE BUILD OPTIONS
Instead of "cmake ..", you can add the following options:

- Debug Mode: CMAKE_BUILD_TYPE=Debug
> cmake .. -DCMAKE_BUILD_TYPE=Debug

- Build Unitary Tests
> cmake .. -DBUILD_TEST=y










 







