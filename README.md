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

The colour space used can be configured using
- `--color-space 0` RGB (default)
- `--color-space 1` XYZ 
- `--color-space 2` L*A*B (using D50 as illuminant)

Similarly, the selection algorithm can be chosen with
- `--select 0` Euclidian Distance (default)
- `--select 1` Euclidian Distance + 2-color mix
- `--select 2` 2-color mix based on dot-product partitioning
- `--select 3` 2-color mix using LAB Brightness partitioning

The Brightness Partition method attempts to overcome LAB's issue in selecting colors with
appropriate brightness. It does so by selecting a color brighter and darker than the target
and blending between the two.

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


