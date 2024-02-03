# Dithery-Do
Dithery-Do is mostly an artistic project that provides methods to colour an image according
to some pre-defined pallette, kind of like how an artists paints their pictures. More generally
though, this project serves as an exploration of image dithering and efficient image processing.

![DitheryDo](https://github.com/Vafdaf12/dithery-do/assets/25525445/696a93a3-4a34-46c9-8591-986bcd9bbccb)

# Usage
The general CLI syntax for Dithery-Do is the following

```
DitheryDo [--help] [--version] [-o VAR] -algo VAR -space VAR --channel VAR input palette
```

where:
- `input` = The input image
- `palette` = Path to palette file
- `-o` = Path to output file (`output.jpg` by default)
- `-a`/`-algo` = The color selection algorithm to use
- `-s`/`-space` = The color space to use for selection
- `-v`/`--vector` = The direction vector to use for the `dot_blend` algorithm (see below)

*Note: the necessary parameters can also be found by running `./DitherDo --help`*

Dithery Do currently supports RGB, XYZ, and L*a*b* (using D65 or D50 as illuminant)

The selection algorithms are a bit more involved:
- `euclid` = Smallest euclidian distance from the target
- `line` = Blends between 2 selected colors based on a dot product
- `dot_blend` = Partitions the palette based on a dot product and blends between colors from each partition

# Building
Building this project requires CMake, which can be installed on Debian-based OSs with
```bash
sudo apt install cmake
```
Once installed, the project can be built with
```bash
make
```
The compiled binary can be found in `build/DitheryDo`


