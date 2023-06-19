# Dithery-Do
Dithery-Do is mostly an artistic project that provides methods to colour an image according
to some pre-defined pallette, kind of like how an artists paints their pictures. More generally
though, this project serves as an exploration of image dithering and efficient image processing.

# Usage
The general CLI syntax for Dithery-Do is the following
```
./DitheryDo -f <input_file> -o <output_file> -p <palette_file>
```
*Note: the necessary parameters can also be found by running `./DitherDo --help`*

Currently the only supported algorithm selects the color with the shortest Euclidian distance from the target.
However, specifying the colour space used *is* supported with the `--color-space` option:
- `--color-space 0` RGB
- `--color-space 1` XYZ 
- `--color-space 2` L*A*B (using D50 as illuminant)

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


