// Author:  Charles Lucas
// CS510
// 4/17/2020

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include "bitmap.h"

#define DEBUG 1   // Turn on/off all debug messages

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
    uint32_t    position = 0;          // Position in the stream we are reading (for Exceptions)
    uint32_t    row_padding_size = 0;  // The number of bytes of padding we have per line (only if 24-bit color depth)

    if (DEBUG) std::cout << std::endl;

    // Bitmap Type                          (2 bytes - always "BM"/0x424D)
    in >> b.bitmap_type[0] >> b.bitmap_type[1];
    if (DEBUG) std::cout << "Bitmap Type read [" << std::dec << position << "]:  " << b.bitmap_type[0] << b.bitmap_type[1] << std::endl;
    if(strncmp(b.bitmap_type, "BM", 2) != 0) {
        throw(BitmapException("Error reading bitmap_type", (uint32_t)1));
    }
    position += sizeof(b.bitmap_type);

    // Length in Bytes                      (4 bytes)
    in.read((char*)&b.length, 4);
    if (DEBUG) std::cout << "Length read [" << std::dec << position << "]:  " << std::hex << b.length << std::endl;
    position += sizeof(b.length);

    // Garbage                              (4 bytes - ignore)
    in.read((char*)&b.garbage, 4);
    if (DEBUG) std::cout << "Garbage read [" << std::dec << position << "]:  " << std::hex << b.garbage << std::endl;
    position += sizeof(b.garbage);

    // Offset to start of the data          (4 bytes)
    in.read((char*)&b.garbage, 4);
    if (DEBUG) std::cout << "Offset read [" << std::dec << position << "]:  " << std::hex << b.offset << std::endl;
    position += sizeof(b.offset);

    // Size of the Second Header            (4 bytes - always 40 bytes)
    in.read((char*)&b.size_second_header, 4);
    if (DEBUG) std::cout << "Size read [" << std::dec << position << "]:  " << std::dec << b.size_second_header << std::endl;
    position += sizeof(b.size_second_header);

    // Width in Pixels                      (4 bytes signed)
    in.read((char*)&b.width_in_pixels, 4);
    if (DEBUG) std::cout << "Width read [" << std::dec << position << "]:  " << std::dec << b.width_in_pixels << std::endl;
    position += sizeof(b.width_in_pixels);

    // Height in Pixels                     (4 bytes signed)
    in.read((char*)&b.height_in_pixels, 4);
    if (DEBUG) std::cout << "Height read [" << std::dec << position << "]:  " << std::dec << b.height_in_pixels << std::endl;
    position += sizeof(b.height_in_pixels);

    // Number of Color Planes               (2 bytes - *MUST* be 1 - error test)
    in.read((char*)&b.number_of_color_planes, 2);
    if (DEBUG) std::cout << "Color planes read [" << std::dec << position << "]:  " << std::dec << b.number_of_color_planes << std::endl;
    if(b.number_of_color_planes != 1) {
        throw(BitmapException("Error reading number_of_color_planes", position));
    }
    position += sizeof(b.number_of_color_planes);

    // Color Depth of the Image             (2 bytes - either 24 (RGB) or 32 (RGBA))
    in.read((char*)&b.color_depth, 2);
    if (DEBUG) std::cout << "Color depth read [" << std::dec << position << "]:  " << std::dec << b.color_depth << std::endl;
    if(b.color_depth != 24 && b.color_depth != 32) {
        throw(BitmapException("Error reading color_depth", position));
    }
    position += sizeof(b.color_depth);

    // Compression Method being Used        (4 bytes - always 0(24-bit) or 3(32-bit))
    in.read((char*)&b.compression_method, 4);
    if (DEBUG) std::cout << "Compression Method read [" << std::dec << position << "]:  " << std::dec << b.compression_method << std::endl;
    if(!(b.color_depth == 24 && b.compression_method == 0) || (b.color_depth == 32 && b.compression_method == 3)) {
        throw(BitmapException("Error reading compression_method", position));
    }
    position += sizeof(b.compression_method);

    // Size of the Raw Bitmap Data in Bytes (4 bytes)
    in.read((char*)&b.data_size, 4);
    if (DEBUG) std::cout << "Data size read [" << std::dec << position << "]:  " << std::dec << b.data_size << std::endl;
    position += sizeof(b.data_size);

    // Horizontal Resolution in Pixels      (4 bytes - dots per meter - always 2835 - ignore)
    in.read((char*)&b.horizontal_resolution, 4);
    if (DEBUG) std::cout << "Horizontal resolution read [" << std::dec << position << "]:  " << std::dec << b.horizontal_resolution << std::endl;
    position += sizeof(b.horizontal_resolution);

    // Vertical Resolution in Pixels        (4 bytes - dots per meter - always 2835 - ignore)
    in.read((char*)&b.vertical_resolution, 4);
    if (DEBUG) std::cout << "Vertical resolution read [" << std::dec << position << "]:  " << std::dec << b.vertical_resolution << std::endl;
    position += sizeof(b.vertical_resolution);

    // Number of Colors in Color Palette    (4 bytes - always 0 (not using a color palette - ignore))
    in.read((char*)&b.number_of_colors, 4);
    if (DEBUG) std::cout << "Number of colors read [" << std::dec << position << "]:  " << std::dec << b.number_of_colors << std::endl;
    position += sizeof(b.number_of_colors);

    // Number of Important Colors Used      (4 bytes - always 0 (not using a color palette - ignore))
    in.read((char*)&b.important_colors, 4);
    if (DEBUG) std::cout << "Number of important colors read [" << std::dec << position << "]:  " << std::dec << b.important_colors << std::endl;
    position += sizeof(b.important_colors);

    // These fields only exist in bitmaps with 32-bit color depth
    if (b.color_depth == 32) {
        // Red Mask                             (4 bytes - only exists in 32-bit image)
        in.read((char*)&b.red_mask, 4);
        if (DEBUG) std::cout << "Red mask read [" << std::dec << position << "]:  " << std::dec << b.red_mask << std::endl;
        position += sizeof(b.red_mask);

        // Green Mask                           (4 bytes - only exists in 32-bit image)
        in.read((char*)&b.green_mask, 4);
        if (DEBUG) std::cout << "Green mask read [" << std::dec << position << "]:  " << std::dec << b.green_mask << std::endl;
        position += sizeof(b.green_mask);

        // Blue Mask                            (4 bytes - only exists in 32-bit image)
        in.read((char*)&b.blue_mask, 4);
        if (DEBUG) std::cout << "Blue mask read [" << std::dec << position << "]:  " << std::dec << b.blue_mask << std::endl;
        position += sizeof(b.blue_mask);

        // Alpha Mask                           (4 bytes - only exists in 32-bit image)
        in.read((char*)&b.alpha_mask, 4);
        if (DEBUG) std::cout << "Alpha mask read [" << std::dec << position << "]:  " << std::dec << b.alpha_mask << std::endl;
        position += sizeof(b.alpha_mask);

        // Color Space Information              (68 bytes - only exists in 32-bit image - ignore)
        in.read((char*)&b.color_space, 68);
        if (DEBUG) std::cout << "Color space read [" << std::dec << position << "]:  " << std::dec << b.color_space << std::endl;
        position += sizeof(b.color_space);
    }

    if (DEBUG) std::cout << std::endl;

    if (b.color_depth == 24 && ((b.width_in_pixels * 3) % 4) != 0) {
        row_padding_size = ((b.width_in_pixels * 3) + 1) % 4;
        if (DEBUG) std::cout << "Row padding = " << row_padding_size << std::endl;
    }

    std::cout << "Bitmap parsed successfully!" << std::endl;

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

BitmapException::BitmapException(const std::string& message, uint32_t position) {
    _message  = message;
    _position = position;
}
BitmapException::BitmapException(std::string&& message, uint32_t position) {
    _message  = message;
    _position = position;
}

void BitmapException::print_exception() {
    std::cout << "Exception:  " << _message << " - Position " << _position << std::endl;
}