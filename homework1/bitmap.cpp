#include <iostream>
#include <fstream>
#include <string>
#include "bitmap.h"

Bitmap::Bitmap()
{
}

/**
 * Read in an image.
 * reads a bitmap in from the stream
 *
 * @param in the stream to read from.
 * @param b the bitmap that we are creating.
 *
 * @return the stream after we've read in the image.
 *
 * @throws BitmapException if it's an invalid bitmap.
 * @throws bad_alloc exception if we failed to allocate memory.
 */
std::istream& operator>>(std::istream& in, Bitmap& b)
{
    return in;
}
/**
 * Write the binary representation of the image to the stream.
 *
 * @param out the stream to write to.
 * @param b the bitmap that we are writing.
 *
 * @return the stream after we've finished writting.
 *
 * @throws failure if we failed to write.
 */
std::ostream& operator<<(std::ostream& out, const Bitmap& b)
{
    return out;
}

/**
 * cell shade an image.
 * for each component of each pixel we round to 
 * the nearest number of 0, 180, 255
 *
 * This has the effect of making the image look like.
 * it was colored.
 */
void cellShade(Bitmap& b) {}

/**
 * Grayscales an image by averaging all of the components.
 */
void grayscale(Bitmap& b) {}

/**
 * Pixelats an image by creating groups of 16*16 pixel blocks.
 */
void pixelate(Bitmap& b) {}

/**
 * Use gaussian bluring to blur an image.
 */
void blur(Bitmap& b) {}

/**
 * rotates image 90 degrees, swapping the height and width.
 */
void rot90(Bitmap& b) {}

/**
 * rotates an image by 180 degrees.
 */
void rot180(Bitmap& b) {}

/**
 * rotates image 270 degrees, swapping the height and width.
 */
void rot270(Bitmap& b) {}

/**
 * flips and image over the vertical axis.
 */
void flipv(Bitmap& b) {}

/**
 * flips and image over the horizontal axis.
 */
void fliph(Bitmap& b) {}

/**
 * flips and image over the line y = -x, swapping the height and width.
 */
void flipd1(Bitmap& b) {}

/**
 * flips and image over the line y = xr, swapping the height and width.
 */
void flipd2(Bitmap& b) {}

/**
 * scales the image by a factor of 2.
 */
void scaleUp(Bitmap& b) {}

/**
 * scales the image by a factor of 1/2.
 */
void scaleDown(Bitmap& b) {}


void BitmapException::print_exception() {}