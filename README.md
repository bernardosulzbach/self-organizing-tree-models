# Self-organizing tree models for image synthesis

This is a C++ implementation of the main ideas from [Wojciech Palubicki, Kipp
Horel, Steven Longay, Adam Runions, Brendan Lane, Radomír Měch, and Przemyslaw
Prusinkiewicz. 2009. Self-organizing tree models for image synthesis. In ACM
SIGGRAPH 2009 papers (SIGGRAPH '09), Hugues Hoppe (Ed.). ACM, New York, NY,
USA, Article 58](https://doi.org/10.1145/1576246.1531364).

The code is fairly well-documented in order to help an interested reader better
understand how the paper was implemented.

For more details on the implementation, see the LaTeX documentation.

## Building

In your build directory, these commands will compile this project.

```bash
# If you need, set your C and C++ compilers here.
cmake -DCMAKE_BUILD_TYPE=Release ..
# If you made any changes to the source, run clang-format.
clang-format -i ../src/* ../shaders/*
make -j 4
```
