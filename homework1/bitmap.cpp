// Author:  Charles Lucas
// CS510
// 4/17/2020

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <iomanip>
#include "bitmap.h"

#define DEBUG 0          // Turn on/off all debug messages
#define PIXEL_SIZE 16    // NxN array of pixels to average color over for pixellation
#define GAUSS_SIZE 5     // NxN array of pixels to calulate gausian blue over for each pixel

Bitmap::Bitmap() {}

void Bitmap::readPixel(int x, int y, uint &red, uint &green, uint &blue, uint &alpha) {
    std::vector<char>::iterator ptr;
    //if (DEBUG) std::cout << "Reading pixel color values at " << x << "," << y << std::endl;

    if (this->color_depth == 32) {
        uint32_t color_value;   // 4-byte pixel color value in format [N bits of red, N bits of green, N bits of blue, N bits of alpha]
        uint32_t red_unmasked;
        uint32_t green_unmasked;
        uint32_t blue_unmasked;
        uint32_t alpha_unmasked;
 
        ptr = data.begin();
        ptr = ptr + ((y * this->width_in_pixels) + x) * 4;  // Should point to the blue value of the pixel at x,y

        // Concatenate our ASCII character values into one 32-bit value 
        color_value = (uint8_t)*(ptr+3) << 24 | (uint8_t)*(ptr+2) << 16 | (uint8_t)*(ptr+1) << 8 | (uint8_t)*ptr;
         
        //std::cout << "Color Value " << std::dec << (((y * this->width_in_pixels) + x) * 4) << " = "<< std::hex << color_value << std::endl;

        // Use the color masks to mask off the bits that aren't relevant for each color
        red_unmasked   = color_value & this->red_mask;
        green_unmasked = color_value & this->green_mask;
        blue_unmasked  = color_value & this->blue_mask;
        alpha_unmasked = color_value & this->alpha_mask;

        //std::cout << "ColorValue = " << std::hex << color_value << " Red  " << " = " << std::hex << red_unmasked << "  Green  " << " = " << std::hex << green_unmasked << "  Blue  " << " = " << std::hex << blue_unmasked << "  Alpha  " << " = " << std::hex << alpha_unmasked << std::endl;

        // Bit shift the unmasked pixel values to be bit0-aligned according to the mask value
        uint temp_mask = this->red_mask;
        if (temp_mask > 0) {
            while (temp_mask%2 != 1) {   // While the mask is not odd
                temp_mask    = temp_mask >> 1;
                red_unmasked = red_unmasked >> 1;
            }
        }

        temp_mask = this->green_mask;
        if (temp_mask > 0) {
            while (temp_mask%2 != 1) {   // While the mask is not odd
                temp_mask      = temp_mask >> 1;
                green_unmasked = green_unmasked >> 1;
            }
        }

        temp_mask = this->blue_mask;
        if (temp_mask > 0) {
            while (temp_mask%2 != 1) {   // While the mask is not odd
                temp_mask      = temp_mask >> 1;
                blue_unmasked  = blue_unmasked >> 1;
            }
        }

        temp_mask = this->alpha_mask;
        if (temp_mask > 0) {
            while (temp_mask%2 != 1) {   // While the mask is not odd
                temp_mask      = temp_mask >> 1;
                alpha_unmasked = alpha_unmasked >> 1;
            }
        }

        // Cast the calculated values back to unit before returning
        red   = red_unmasked;
        green = green_unmasked;
        blue  = blue_unmasked;
        alpha = alpha_unmasked;
    }
    else {  // Currently only supports 24-bit color depth
        ptr = data.begin();
        ptr = ptr + ((y * this->width_in_pixels) + x) * 3;  // Should point to the blue value of the pixel at x,y

        red   = (uint8_t)*(ptr+2);
        green = (uint8_t)*(ptr+1);
        blue  = (uint8_t)*ptr;
    }

    return;
}

void Bitmap::writePixel(int x, int y, uint &red, uint &green, uint &blue, uint &alpha) {
    std::vector<char>::iterator ptr;

    //if (DEBUG) std::cout << "Writing pixel color values at " << "x = " << x << ", y = " << y << " - index = " << (((y * this->width_in_pixels) + x) * 4) << std::endl;

    if (this->color_depth == 32) {
        uint32_t color_value = 0;   // 4-byte pixel color value in format [N bits of red, N bits of green, N bits of blue, N bits of alpha]
        uint32_t red_masked = 0;
        uint32_t green_masked = 0;
        uint32_t blue_masked = 0;
        uint32_t alpha_masked = 0;
 
        ptr = data.begin();
        ptr = ptr + ((y * this->width_in_pixels) + x) * 4;  // Should point to the blue value of the pixel at x,y

        // Bit shift the red color values to be mask-aligned and OR them into the combined color value
        uint temp_mask = this->red_mask;
        uint temp_value = red;
        if (temp_mask > 0) {
            while (temp_mask%2 != 1) {   // While the mask is not odd
                //std::cout << "Red Mask " << std::dec << (((y * this->width_in_pixels) + x) * 4) << " = "<< std::hex << temp_mask << "  temp_value = " << temp_value << std::endl;
                temp_mask  = temp_mask  >> 1;
                temp_value = temp_value << 1;
            }
            color_value = color_value | temp_value;  // OR our red value into the appropriate bits in the color value
        }

        // Bit shift the green color values to be mask-aligned and OR them into the combined color value
        temp_mask = this->green_mask;
        temp_value = green;
        if (temp_mask > 0) {
            while (temp_mask%2 != 1) {   // While the mask is not odd
                //std::cout << "Green Mask " << std::dec << (((y * this->width_in_pixels) + x) * 4) << " = "<< std::hex << temp_mask << "  temp_value = " << temp_value << std::endl;
                temp_mask  = temp_mask  >> 1;
                temp_value = temp_value << 1;
            }
            color_value = color_value | temp_value;  // OR our red value into the appropriate bits in the color value
        }

        // Bit shift the blue color values to be mask-aligned and OR them into the combined color value
        temp_mask = this->blue_mask;
        temp_value = blue;
        if (temp_mask > 0) {
            while (temp_mask%2 != 1) {   // While the mask is not odd
                //std::cout << "Blue Mask " << std::dec << (((y * this->width_in_pixels) + x) * 4) << " = "<< std::hex << temp_mask << "  temp_value = " << temp_value << std::endl;
                temp_mask   = temp_mask  >> 1;
                temp_value  = temp_value << 1;
            }
            color_value = color_value | temp_value;  // OR our red value into the appropriate bits in the color value
        }

        // Bit shift the alpha color values to be mask-aligned and OR them into the combined color value
        temp_mask = this->alpha_mask;
        temp_value = alpha;
        if (temp_mask > 0) {
            while (temp_mask%2 != 1) {   // While the mask is not odd
                //std::cout << "Alpha Mask " << std::dec << (((y * this->width_in_pixels) + x) * 4) << " = "<< std::hex << temp_mask << "  temp_value = " << temp_value << std::endl;
                temp_mask   = temp_mask  >> 1;
                temp_value  = temp_value << 1;
            }
            color_value = color_value | temp_value;  // OR our red value into the appropriate bits in the color value
        }

        // Write our combined value back into the proper bytes in the vector
        for (int i = 0; i < 4; i++) {
            uint8_t byte;

            byte = (color_value >> (i * 8)) & 0x000000FF;  // Get the value one byte at a time
            *(ptr+i) = byte;
            //std::cout << "ColorValue[" << std::dec << i << "] = " << std::hex << byte << " ptr  " << " = " << std::hex << *(ptr+i) << std::endl;

        }

    }
    else {  // Currently only supports 24-bit color depth
        ptr = data.begin();
        ptr = ptr + ((y * this->width_in_pixels) + x) * 3;  // Should point to the blue value of the pixel at x,y

        *(ptr+2) = (uint8_t)red  ;
        *(ptr+1) = (uint8_t)green;
        *ptr     = (uint8_t)blue ;
    }

    return;
}

uint32_t Bitmap::getRowPaddingSize() const {
    uint32_t rowpaddingsize;
    // If color_depth is 24, rows begin on 4-byte boundaries.
    // Since 24-bit pixels are 3 bytes, there may be one or more padding bytes at the end of the row.
    if (color_depth == 24 && ((width_in_pixels * 3) % 4) != 0) {
        rowpaddingsize = 4 - ((width_in_pixels * 3) % 4);
    }
    else {
        rowpaddingsize = 0;
    }

    return(rowpaddingsize);
}
uint32_t Bitmap::getFileLength() const {
    return(length);
}
void     Bitmap::setFileLength(uint32_t bytes) {
    length = bytes;
}
uint32_t Bitmap::getDataSize() const {
    return(data_size);
}
void     Bitmap::setDataSize(uint32_t bytes) {
    data_size = bytes;
}
uint16_t Bitmap::getColorDepth() const {
     return(color_depth);
}
int32_t  Bitmap::getWidthinPixels() const {
     return(width_in_pixels);
}
void     Bitmap::setWidthinPixels(int width) {
    width_in_pixels = width;
}
int32_t  Bitmap::getHeightinPixels() const {
     return(height_in_pixels);
}
void     Bitmap::setHeightinPixels(int height) {
    height_in_pixels = height;
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
    uint32_t    file_offset = 0;          // Position in the stream we are reading (for Exceptions)
    uint32_t    row_padding_size = 0;  // The number of bytes of padding we have per line (only if 24-bit color depth)

    if (DEBUG) std::cout << std::endl;

    // Bitmap Type                          (2 bytes - always "BM"/0x424D)
    in >> b.bitmap_type[0] >> b.bitmap_type[1];
    if (DEBUG) std::cout << "Bitmap Type read [" << std::dec << file_offset << "]:  " << b.bitmap_type[0] << b.bitmap_type[1] << std::endl;
    if(strncmp(b.bitmap_type, "BM", 2) != 0) {
        throw(BitmapException("Error reading bitmap_type", (uint32_t)1));
    }
    file_offset += sizeof(b.bitmap_type);

    // Length in Bytes                      (4 bytes)
    in.read((char*)&b.length, 4);
    if (DEBUG) std::cout << "Length read [" << std::dec << file_offset << "]:  " << std::dec << b.length << std::endl;
    file_offset += sizeof(b.length);

    // Garbage                              (4 bytes - ignore)
    in.read((char*)&b.garbage, 4);
    if (DEBUG) std::cout << "Garbage read [" << std::dec << file_offset << "]:  " << std::hex << b.garbage << std::endl;
    file_offset += sizeof(b.garbage);

    // Offset to start of the data          (4 bytes)
    in.read((char*)&b.offset, 4);
    if (DEBUG) std::cout << "Offset read [" << std::dec << file_offset << "]:  " << std::hex << b.offset << std::endl;
    file_offset += sizeof(b.offset);

    // Size of the Second Header            (4 bytes - always 40 bytes)
    in.read((char*)&b.size_second_header, 4);
    if (DEBUG) std::cout << "Size read [" << std::dec << file_offset << "]:  " << std::dec << b.size_second_header << std::endl;
    file_offset += sizeof(b.size_second_header);

    // Width in Pixels                      (4 bytes signed)
    in.read((char*)&b.width_in_pixels, 4);
    if (DEBUG) std::cout << "Width read [" << std::dec << file_offset << "]:  " << std::dec << b.width_in_pixels << std::endl;
    file_offset += sizeof(b.width_in_pixels);

    // Height in Pixels                     (4 bytes signed)
    in.read((char*)&b.height_in_pixels, 4);
    if (DEBUG) std::cout << "Height read [" << std::dec << file_offset << "]:  " << std::dec << b.height_in_pixels << std::endl;
    file_offset += sizeof(b.height_in_pixels);

    // Number of Color Planes               (2 bytes - *MUST* be 1 - error test)
    in.read((char*)&b.number_of_color_planes, 2);
    if (DEBUG) std::cout << "Color planes read [" << std::dec << file_offset << "]:  " << std::dec << b.number_of_color_planes << std::endl;
    if(b.number_of_color_planes != 1) {
        throw(BitmapException("Error reading number_of_color_planes", file_offset));
    }
    file_offset += sizeof(b.number_of_color_planes);

    // Color Depth of the Image             (2 bytes - either 24 (RGB) or 32 (RGBA))
    in.read((char*)&b.color_depth, 2);
    if (DEBUG) std::cout << "Color depth read [" << std::dec << file_offset << "]:  " << std::dec << b.color_depth << std::endl;
    if(b.color_depth != 24 && b.color_depth != 32) {
        throw(BitmapException("Error reading color_depth", file_offset));
    }
    file_offset += sizeof(b.color_depth);

    // Compression Method being Used        (4 bytes - always 0(24-bit) or 3(32-bit))
    in.read((char*)&b.compression_method, 4);
    if (DEBUG) std::cout << "Compression method read [" << std::dec << file_offset << "]:  " << std::dec << b.compression_method << std::endl;
    if(!((b.color_depth == 24 && b.compression_method == 0) || (b.color_depth == 32 && b.compression_method == 3))) {
        throw(BitmapException("Error reading compression_method", file_offset));
    }
    file_offset += sizeof(b.compression_method);

    // Size of the Raw Bitmap Data in Bytes (4 bytes)
    in.read((char*)&b.data_size, 4);
    if (DEBUG) std::cout << "Data size read [" << std::dec << file_offset << "]:  " << std::dec << b.data_size << std::endl;
    file_offset += sizeof(b.data_size);

    // Horizontal Resolution in Pixels      (4 bytes - dots per meter - always 2835 - ignore)
    in.read((char*)&b.horizontal_resolution, 4);
    if (DEBUG) std::cout << "Horizontal resolution read [" << std::dec << file_offset << "]:  " << std::dec << b.horizontal_resolution << std::endl;
    file_offset += sizeof(b.horizontal_resolution);

    // Vertical Resolution in Pixels        (4 bytes - dots per meter - always 2835 - ignore)
    in.read((char*)&b.vertical_resolution, 4);
    if (DEBUG) std::cout << "Vertical resolution read [" << std::dec << file_offset << "]:  " << std::dec << b.vertical_resolution << std::endl;
    file_offset += sizeof(b.vertical_resolution);

    // Number of Colors in Color Palette    (4 bytes - always 0 (not using a color palette - ignore))
    in.read((char*)&b.number_of_colors, 4);
    if (DEBUG) std::cout << "Number of colors read [" << std::dec << file_offset << "]:  " << std::dec << b.number_of_colors << std::endl;
    file_offset += sizeof(b.number_of_colors);

    // Number of Important Colors Used      (4 bytes - always 0 (not using a color palette - ignore))
    in.read((char*)&b.important_colors, 4);
    if (DEBUG) std::cout << "Number of important colors read [" << std::dec << file_offset << "]:  " << std::dec << b.important_colors << std::endl;
    file_offset += sizeof(b.important_colors);

    // These fields only exist in bitmaps with 32-bit color depth
    if (b.color_depth == 32) {
        // Red Mask                             (4 bytes - only exists in 32-bit image)
        in.read((char*)&b.red_mask, 4);
        if (DEBUG) std::cout << "Red mask read [" << std::dec << file_offset << "]:  " << std::hex << b.red_mask << std::endl;
        file_offset += sizeof(b.red_mask);

        // Green Mask                           (4 bytes - only exists in 32-bit image)
        in.read((char*)&b.green_mask, 4);
        if (DEBUG) std::cout << "Green mask read [" << std::dec << file_offset << "]:  " << std::hex << b.green_mask << std::endl;
        file_offset += sizeof(b.green_mask);

        // Blue Mask                            (4 bytes - only exists in 32-bit image)
        in.read((char*)&b.blue_mask, 4);
        if (DEBUG) std::cout << "Blue mask read [" << std::dec << file_offset << "]:  " << std::hex << b.blue_mask << std::endl;
        file_offset += sizeof(b.blue_mask);

        // Alpha Mask                           (4 bytes - only exists in 32-bit image)
        in.read((char*)&b.alpha_mask, 4);
        if (DEBUG) std::cout << "Alpha mask read [" << std::dec << file_offset << "]:  " << std::hex << b.alpha_mask << std::endl;
        file_offset += sizeof(b.alpha_mask);

        // Color Space Information              (68 bytes - only exists in 32-bit image - ignore)
        in.read((char*)&b.color_space, 68);
        if (DEBUG) std::cout << "Color space read [" << std::dec << file_offset << "]:  " << std::hex << b.color_space << std::endl;
        file_offset += sizeof(b.color_space);
    }

    if (DEBUG) std::cout << std::endl << "Finished parsing header - " << std::dec << file_offset << " bytes read." << std::endl << std::endl;
    if (DEBUG) std::cout << "Bitmap data starts at offset 0x" << std::hex << file_offset << "." << std::endl;

    // Read in the actual picture data
    if (b.color_depth == 24) {
        int  row_data_length = b.width_in_pixels * 3;
        int  row_total_length = row_data_length + b.getRowPaddingSize();
        char row_buffer[row_total_length];

        b.data.reserve(row_data_length * b.height_in_pixels);  // If we're going to throw a memory exception, do it here

        if (DEBUG) std::cout << "row_data_length:  " << std::dec << row_data_length << std::endl;
        if (DEBUG) std::cout << "Size of b.data:  " << std::dec << b.data.size() << std::endl;

        for (int i=0; i<b.height_in_pixels; i++) {
            in.read(row_buffer, sizeof(row_buffer));  // Read in the entire row of data, including the padding
            file_offset += sizeof(row_buffer);
            b.data.insert(b.data.end(), row_buffer, row_buffer + row_data_length);  // Only transfer the data into the vector, not the padding

            // DEBUG - Print the first two rows we read in
            if ((DEBUG) && (i == 0 || i == 1)) {
                std::cout << "Size of b.data:  " << b.data.size() << std::endl;
                for (int j = row_data_length * i; j < (row_data_length * i) + row_data_length ; j++) {
                    if (j%16 == 0) { printf("%03x:  ", j); }
                    printf("%02hhx ", b.data[j]);
                    if (j%16 == 15) { printf("\n"); }
                }
                printf("\n\n");
                std::cout << std::endl;
            }
        }
    }
    else {  // Must be 32-bit color depth
        int data_length = b.length - file_offset;         // The remaining length to read in is the total file length - the header
        char buffer[data_length];

        b.data.reserve(data_length);                      // If we're going to throw a memory exception, do it here

        in.read(buffer, sizeof(buffer));
        b.data.insert(b.data.end(), buffer, buffer+sizeof(buffer));
        file_offset += sizeof(buffer);
    }

    std::cout << "Bitmap parsed successfully - " << file_offset << " bytes read." << std::endl;

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
    uint32_t    file_offset = 0;          // Position in the stream we are writing (for Exceptions)

    if (DEBUG) std::cout << std::endl;

    // Bitmap Type                          (2 bytes - always "BM"/0x424D)
    //out << b.bitmap_type;
    out.write((char*)&b.bitmap_type, 2);
    if (DEBUG) std::cout << "Bitmap Type write [" << std::dec << file_offset << "]:  " << b.bitmap_type[0] << b.bitmap_type[1] << std::endl;
    file_offset += sizeof(b.bitmap_type);

    // Length in Bytes                      (4 bytes)
    out.write((char*)&b.length, 4);
    if (DEBUG) std::cout << "Length write [" << std::dec << file_offset << "]:  " << std::dec << b.length << std::endl;
    file_offset += sizeof(b.length);

    // Garbage                              (4 bytes - ignore)
    out.write((char*)&b.garbage, 4);
    if (DEBUG) std::cout << "Garbage write [" << std::dec << file_offset << "]:  " << std::hex << b.garbage << std::endl;
    file_offset += sizeof(b.garbage);

    // Offset to start of the data          (4 bytes)
    out.write((char*)&b.offset, 4);
    if (DEBUG) std::cout << "Offset write [" << std::dec << file_offset << "]:  " << std::hex << b.offset << std::endl;
    file_offset += sizeof(b.offset);

    // Size of the Second Header            (4 bytes - always 40 bytes)
    out.write((char*)&b.size_second_header, 4);
    if (DEBUG) std::cout << "Size write [" << std::dec << file_offset << "]:  " << std::dec << b.size_second_header << std::endl;
    file_offset += sizeof(b.size_second_header);

    // Width in Pixels                      (4 bytes signed)
    out.write((char*)&b.width_in_pixels, 4);
    if (DEBUG) std::cout << "Width write [" << std::dec << file_offset << "]:  " << std::dec << b.width_in_pixels << std::endl;
    file_offset += sizeof(b.width_in_pixels);

    // Height in Pixels                     (4 bytes signed)
    out.write((char*)&b.height_in_pixels, 4);
    if (DEBUG) std::cout << "Height write [" << std::dec << file_offset << "]:  " << std::dec << b.height_in_pixels << std::endl;
    file_offset += sizeof(b.height_in_pixels);

    // Number of Color Planes               (2 bytes - *MUST* be 1 - error test)
    out.write((char*)&b.number_of_color_planes, 2);
    if (DEBUG) std::cout << "Color planes write [" << std::dec << file_offset << "]:  " << std::dec << b.number_of_color_planes << std::endl;
    if(b.number_of_color_planes != 1) {
        throw(BitmapException("Error writing number_of_color_planes", file_offset));
    }
    file_offset += sizeof(b.number_of_color_planes);

    // Color Depth of the Image             (2 bytes - either 24 (RGB) or 32 (RGBA))
    out.write((char*)&b.color_depth, 2);
    if (DEBUG) std::cout << "Color depth write [" << std::dec << file_offset << "]:  " << std::dec << b.color_depth << std::endl;
    file_offset += sizeof(b.color_depth);

    // Compression Method being Used        (4 bytes - always 0(24-bit) or 3(32-bit))
    out.write((char*)&b.compression_method, 4);
    if (DEBUG) std::cout << "Compression method write [" << std::dec << file_offset << "]:  " << std::dec << b.compression_method << std::endl;
    file_offset += sizeof(b.compression_method);

    // Size of the Raw Bitmap Data in Bytes (4 bytes)
    out.write((char*)&b.data_size, 4);
    if (DEBUG) std::cout << "Data size write [" << std::dec << file_offset << "]:  " << std::dec << b.data_size << std::endl;
    file_offset += sizeof(b.data_size);

    // Horizontal Resolution in Pixels      (4 bytes - dots per meter - always 2835 - ignore)
    out.write((char*)&b.horizontal_resolution, 4);
    if (DEBUG) std::cout << "Horizontal resolution write [" << std::dec << file_offset << "]:  " << std::dec << b.horizontal_resolution << std::endl;
    file_offset += sizeof(b.horizontal_resolution);

    // Vertical Resolution in Pixels        (4 bytes - dots per meter - always 2835 - ignore)
    out.write((char*)&b.vertical_resolution, 4);
    if (DEBUG) std::cout << "Vertical resolution write [" << std::dec << file_offset << "]:  " << std::dec << b.vertical_resolution << std::endl;
    file_offset += sizeof(b.vertical_resolution);

    // Number of Colors in Color Palette    (4 bytes - always 0 (not using a color palette - ignore))
    out.write((char*)&b.number_of_colors, 4);
    if (DEBUG) std::cout << "Number of colors write [" << std::dec << file_offset << "]:  " << std::dec << b.number_of_colors << std::endl;
    file_offset += sizeof(b.number_of_colors);

    // Number of Important Colors Used      (4 bytes - always 0 (not using a color palette - ignore))
    out.write((char*)&b.important_colors, 4);
    if (DEBUG) std::cout << "Number of important colors write [" << std::dec << file_offset << "]:  " << std::dec << b.important_colors << std::endl;
    file_offset += sizeof(b.important_colors);

    // These fields only exist in bitmaps with 32-bit color depth
    if (b.color_depth == 32) {
        // Red Mask                             (4 bytes - only exists in 32-bit image)
        out.write((char*)&b.red_mask, 4);
        if (DEBUG) std::cout << "Red mask write [" << std::dec << file_offset << "]:  " << std::hex << b.red_mask << std::endl;
        file_offset += sizeof(b.red_mask);

        // Green Mask                           (4 bytes - only exists in 32-bit image)
        out.write((char*)&b.green_mask, 4);
        if (DEBUG) std::cout << "Green mask write [" << std::dec << file_offset << "]:  " << std::hex << b.green_mask << std::endl;
        file_offset += sizeof(b.green_mask);

        // Blue Mask                            (4 bytes - only exists in 32-bit image)
        out.write((char*)&b.blue_mask, 4);
        if (DEBUG) std::cout << "Blue mask write [" << std::dec << file_offset << "]:  " << std::hex << b.blue_mask << std::endl;
        file_offset += sizeof(b.blue_mask);

        // Alpha Mask                           (4 bytes - only exists in 32-bit image)
        out.write((char*)&b.alpha_mask, 4);
        if (DEBUG) std::cout << "Alpha mask write [" << std::dec << file_offset << "]:  " << std::hex << b.alpha_mask << std::endl;
        file_offset += sizeof(b.alpha_mask);

        // Color Space Information              (68 bytes - only exists in 32-bit image - ignore)
        out.write((char*)&b.color_space, 68);
        if (DEBUG) std::cout << "Color space write [" << std::dec << file_offset << "]:  " << std::hex << b.color_space << std::endl;
        file_offset += sizeof(b.color_space);
    }

    if (DEBUG) std::cout << std::endl << "Finished writing header - " << std::dec << file_offset << " bytes written." << std::endl;
    if (DEBUG) std::cout << "Bitmap data starts at offset 0x" << std::hex << file_offset << "." << std::endl;

    // Write out the actual picture data
    if (b.color_depth == 32) {
        int data_length;
        const char* data_pointer;

        data_length  = b.length - file_offset;  // Total length - the header length
        data_pointer = b.data.data();
        out.write(data_pointer, data_length);
        file_offset += data_length;
    }
    else {  // Must be 24-bit color depth
        uint32_t  row_data_length;
        uint32_t  row_total_length;
        const char *data_pointer;
        const char padding[3] = {NULL,NULL,NULL};
        int data_offset = 0;

        row_data_length  = b.width_in_pixels * 3;
        row_total_length = row_data_length + b.getRowPaddingSize();

        for (int i = 0; i < b.getHeightinPixels(); i++) {
            // Output each row of picture data
            data_pointer = b.data.data() + data_offset;
            out.write(data_pointer, row_data_length);
            data_offset += row_data_length;
            file_offset += row_data_length;

            // Add padding to the end of the row
            out.write(padding, b.getRowPaddingSize());
            file_offset += b.getRowPaddingSize();
        }
    }

    // Double-check that our final pointer value matches the expected bitmap length
    if (file_offset == b.length) {
        std::cout << "Bitmap written successfully - " << file_offset << " bytes written." << std::endl;
    }
    else {
        std::cout << "Error:  Mismatch between expected file size and bytes written - expected file size = " << std::dec << b.length << ", bytes written = " << file_offset << std::endl;
    }
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
void cellShade(Bitmap& b) {
    std::vector<char>::iterator ptr;
    uint8_t value;

    std::cout << "Applying cell shading transform." << std::endl;

    // Iterate over all the byte values in the data vector and round to one of three values
    for (ptr = b.data.begin(); ptr < b.data.end(); ptr++) {
        value = *ptr;
        if      (value <= 64)                 value = 0;
        else if (value >  64 && value < 192)  value = 127;
        else if (value >= 192)                value = 255;
        *ptr = value;
    }
}

/**
 * Grayscales an image by averaging all of the component colors.
 */
void grayscale(Bitmap& b) {
    std::vector<char>::iterator ptr;
    uint red;
    uint green;
    uint blue;
    uint alpha;  // Unused for this transform

    std::cout << "Applying grayscale transform." << std::endl;

    // Iterate over all the pixels in the data vector and average their color values
    // (Treating it as a one-dimensional array rather than a two-dimensional picture for simplicity)
    for (int i = 0; i < (b.width_in_pixels * b.height_in_pixels); i++) {
        b.readPixel(i, 0, red, green, blue, alpha);
        //std::cout << "Before:  Red = " << std::hex << red << " Green = " << green << " Blue = " << blue << std::endl;
        red = green = blue = (red + green + blue)/3;  // Set all colors to the average of all colors
        //std::cout << "After:   Red = " << std::hex << red << " Green = " << green << " Blue = " << blue << std::endl << std::endl;
        b.writePixel(i, 0, red, green, blue, alpha);
    }

    return;
}

/**
 * Pixelates an image by creating groups of 16*16 pixel blocks.
 */
void pixelate(Bitmap& b) {
    BitmapPixel pixel[PIXEL_SIZE][PIXEL_SIZE];  // NxN array of pixels to average the color over
    int  number_of_colors;                      // Number of color fields per pixes (24-bit vs 32-bit encoding)

    std::cout << "Applying pixelate transform." << std::endl;

    if (b.color_depth == 32) number_of_colors = 4;
    else                     number_of_colors = 3;

    // Iterate over all the pixels in the picture in blocks of 16
    for (int y = 0; y <= (b.height_in_pixels-16); y += PIXEL_SIZE) {
        for (int x = 0; x <= (b.width_in_pixels-16); x += PIXEL_SIZE) {
            uint average_red   = 0;
            uint average_green = 0;
            uint average_blue  = 0;

            for (int py = 0; py < PIXEL_SIZE; py++) {
                for (int px = 0; px < PIXEL_SIZE; px++) {
                    uint red   = 0;
                    uint green = 0;
                    uint blue  = 0;


                    // Now iterate over all the pixels inside the block of 16
                    pixel[py][px].init(b, x + px, y + py);  // Init from the actual image pixel coordinates
                    pixel[py][px].getrgb(red, green, blue);

                    average_red   += red;
                    average_green += green;
                    average_blue  += blue;
                }
            }

            average_red   /= (PIXEL_SIZE * PIXEL_SIZE);
            average_green /= (PIXEL_SIZE * PIXEL_SIZE);
            average_blue  /= (PIXEL_SIZE * PIXEL_SIZE);

            // Write our averaged values back to all the sub-pixels
            for (int py = 0; py < PIXEL_SIZE; py++) {
                for (int px = 0; px < PIXEL_SIZE; px++) {
                    pixel[py][px].setrgb(average_red, average_green, average_blue);
                    pixel[py][px].write();
                }
            }

        }
    }

    return;
}


/**
 * Use gaussian bluring to blur an image.
 */
void blur(Bitmap& b) {
    Bitmap outbmp = b;    // Create a second bitmap to hold our output values

    BitmapPixel target1_pixel;           // The target pixel we will modify the values for
    BitmapPixel temp_pixel;             // Temporary pixel object to hold data while calculating
    int         matrix[GAUSS_SIZE][GAUSS_SIZE] = {{1,  4,  6,  4, 1},
                                                  {4, 16, 24, 16, 4},
                                                  {6, 24, 36, 24, 6},
                                                  {4, 16, 24, 16, 4},
                                                  {1,  4,  6,  4, 1}};
    int         gauss_offset = GAUSS_SIZE / 2;  // The number of pixels around the current pixes to calculate using
    int         number_of_colors;               // Number of color fields per pixes (24-bit vs 32-bit encoding)

    std::cout << "Applying gaussian blurring transform." << std::endl;
    
    if (b.color_depth == 32) number_of_colors = 4;
    else                     number_of_colors = 3;

    // Iterate over all the pixels in the picture except a 2-pixel stripe around the edges
    for (int y = gauss_offset; y < (b.height_in_pixels-GAUSS_SIZE); y++) {
        for (int x = gauss_offset; x < (b.width_in_pixels-GAUSS_SIZE); x++) {
            uint original_red  ;
            uint original_green;
            uint original_blue ;
            uint gauss_red   = 0;
            uint gauss_green = 0;
            uint gauss_blue  = 0;

            // Initialize our target pixel (in the center of the matrix)
            target1_pixel.init(outbmp, x + gauss_offset, y + gauss_offset);    // Init from the center pixel coordinates (output bitmap)
            target1_pixel.getrgb(original_red, original_green, original_blue); // Get it's original values

            // Iterate over all the matrix values, multiply them by the R/G/B value of that pixel, and add them all together.
            for (int py = 0; py < GAUSS_SIZE; py++) {
                for (int px = 0; px < GAUSS_SIZE; px++) {
                    uint pxred   = 0;
                    uint pxgreen = 0;
                    uint pxblue  = 0;

                    temp_pixel.init(b, x + px, y + py);  // Init from the bitmap image pixel coordinates
                    temp_pixel.getrgb(pxred, pxgreen, pxblue);

                    gauss_red   += (pxred   * matrix[py][px]);
                    gauss_green += (pxgreen * matrix[py][px]);
                    gauss_blue  += (pxblue  * matrix[py][px]);
                }
            }

            gauss_red   /= 256;
            gauss_green /= 256;
            gauss_blue  /= 256;

            target1_pixel.setrgb(gauss_red, gauss_green, gauss_blue);
            target1_pixel.write();  // Write our calculated files to the output bitmap
        }
    }

    b = outbmp;  // Return outbmp instead of b

    return;
}

/**
 * rotates image 90 degrees, swapping the height and width.
 */
void rot90(Bitmap& b) {
    imageTransform(b, 0);  // Image transform mode 0 (ROT90)
}

/**
 * rotates an image by 180 degrees.
 */
void rot180(Bitmap& b) {
    imageTransform(b, 1);  // Image transform mode 1 (ROT180)
}

/**
 * rotates image 270 degrees, swapping the height and width.
 */
void rot270(Bitmap& b) {
    imageTransform(b, 2);  // Image transform mode 1 (ROT270)
}

/**
 * flips and image over the vertical axis.
 */
void flipv(Bitmap& b) {
    imageTransform(b, 3);  // Image transform mode 3 (FLIPV)
}

/**
 * flips and image over the horizontal axis.
 */
void fliph(Bitmap& b) {
    imageTransform(b, 4);  // Image transform mode 4 (FLIPH)
}

/**
 * flips and image over the line y = -x, swapping the height and width.
 */
void flipd1(Bitmap& b) {
    imageTransform(b, 5);  // Image transform mode 5 (FLIPD1)
}

/**
 * flips and image over the line y = xr, swapping the height and width.
 */
void flipd2(Bitmap& b) {
    imageTransform(b, 6);  // Image transform mode 6 (FLIPD2)
}

/**
 * scales the image by a factor of 2.
 */
void scaleUp(Bitmap& b) {
    Bitmap outbmp = b;                   // Create a second bitmap to hold our output values
    BitmapPixel source_pixel;            // Pixel object to hold the source data
    BitmapPixel target1_pixel;           // A pixel to write back to the target image
    BitmapPixel target2_pixel;           // A pixel to write back to the target image
    BitmapPixel target3_pixel;           // A pixel to write back to the target image
    BitmapPixel target4_pixel;           // A pixel to write back to the target image
    int  number_of_colors;               // Number of color fields per pixel (24-bit vs 32-bit encoding)
    uint difference_in_pixels;           // The difference in the number of pixels between source and target
    uint difference_in_bytes;            // The difference in the number of bytes between source and target
    uint source_num_padding_bytes;       // The number of padding bytes in the source bitmap
    uint target_num_padding_bytes;       // The number of padding bytes in the target bitmap

    if (b.color_depth == 32) number_of_colors = 4;
    else                     number_of_colors = 3;

    std::cout << "Applying scale up transform." << std::endl;

    // Double the width *and* height of our output bitmap
    difference_in_pixels = ((b.getWidthinPixels()*2 * b.getHeightinPixels()*2)) - (b.getWidthinPixels() * b.getHeightinPixels());
    difference_in_bytes  = difference_in_pixels * number_of_colors;
    outbmp.setWidthinPixels(b.getWidthinPixels()*2);
    outbmp.setHeightinPixels(b.getHeightinPixels()*2);
    outbmp.data.resize(b.data.size()*4);  // Quadruple the size of our output bitmap's vector

    // Iterate over all the pixels in the picture
    for (int y = 0; y < b.getHeightinPixels(); y++) {
        for (int x = 0; x < b.getWidthinPixels(); x++) {
            uint red   = 0;
            uint green = 0;
            uint blue  = 0;
            uint alpha = 0;
            uint target1_x, target1_y;
            uint target2_x, target2_y;
            uint target3_x, target3_y;
            uint target4_x, target4_y;

            // Initialize our source pixel
            source_pixel.init(b, x, y);
            if (b.color_depth == 32) {
                source_pixel.getrgba(red, green, blue, alpha);
            }
            else {  // 24-bit
                source_pixel.getrgb(red, green, blue);
            }

            // *** Duplicate each row (doubling the number of rows) ***
            target1_x =  2 * x;
            target1_y =  2 * y;
            target2_x =  2 * x;
            target2_y = (2 * y) + 1;

            // *** Duplicate each column (doubling the number of column) ***
            target3_x = (2 * x) + 1;
            target3_y =  2 * y;
            target4_x = (2 * x) + 1;
            target4_y = (2 * y) + 1;

            // Set the colors for our 4x4 block of copied pixel(s) and write them to the target image
            if (DEBUG) std::cout << "Target pixel1:  x/y:  " << std::dec << y << "/" << x << " mx/my:  " << outbmp.getHeightinPixels() << "/" << outbmp.getWidthinPixels() << " tx/ty:  " << target1_y << "/" << target1_x << std::endl;
            if (DEBUG) std::cout << "Target pixel2:  x/y:  " << std::dec << y << "/" << x << " mx/my:  " << outbmp.getHeightinPixels() << "/" << outbmp.getWidthinPixels() << " tx/ty:  " << target2_y << "/" << target2_x << std::endl;
            if (DEBUG) std::cout << "Target pixel3:  x/y:  " << std::dec << y << "/" << x << " mx/my:  " << outbmp.getHeightinPixels() << "/" << outbmp.getWidthinPixels() << " tx/ty:  " << target3_y << "/" << target3_x << std::endl;
            if (DEBUG) std::cout << "Target pixel4:  x/y:  " << std::dec << y << "/" << x << " mx/my:  " << outbmp.getHeightinPixels() << "/" << outbmp.getWidthinPixels() << " tx/ty:  " << target4_y << "/" << target4_x << std::endl << std::endl;
            target1_pixel.init(outbmp, target1_x, target1_y);
            target2_pixel.init(outbmp, target2_x, target2_y);
            target3_pixel.init(outbmp, target3_x, target3_y);
            target4_pixel.init(outbmp, target4_x, target4_y);

            if (b.color_depth == 32) {
                target1_pixel.setrgb(red, green, blue);
                target2_pixel.setrgb(red, green, blue);
                target3_pixel.setrgb(red, green, blue);
                target4_pixel.setrgb(red, green, blue);
            }
            else {
                target1_pixel.setrgba(red, green, blue, alpha);
                target2_pixel.setrgba(red, green, blue, alpha);
                target3_pixel.setrgba(red, green, blue, alpha);
                target4_pixel.setrgba(red, green, blue, alpha);
            }

            target1_pixel.write();
            target2_pixel.write();
            target3_pixel.write();
            target4_pixel.write();
        }

        if (b.getWidthinPixels() != outbmp.getWidthinPixels() || b.getHeightinPixels() != outbmp.getHeightinPixels()) {
            // Adjust the file length and data size based on the changed number of padding bytes, if necessary.
            source_num_padding_bytes =      b.getRowPaddingSize();  // If 32-bit, getRowPaddingSize will be 0
            target_num_padding_bytes = outbmp.getRowPaddingSize();  // If 32-bit, getRowPaddingSize will be 0
    
            outbmp.setFileLength(b.getFileLength() + difference_in_bytes - (source_num_padding_bytes * b.height_in_pixels) + (target_num_padding_bytes * outbmp.height_in_pixels * 2));
            outbmp.setDataSize  (b.getDataSize()   + difference_in_bytes - (source_num_padding_bytes * b.height_in_pixels) + (target_num_padding_bytes * outbmp.height_in_pixels * 2));
        }
    }

    b = outbmp;  // Return outbmp instead of b

    return;
}

/**
 * scales the image by a factor of 1/2.
 */
void scaleDown(Bitmap& b) {
    Bitmap outbmp = b;                   // Create a second bitmap to hold our output values
    BitmapPixel source_pixel;            // Pixel object to hold the source data
    BitmapPixel target1_pixel;           // A pixel to write back to the target image
    int  number_of_colors;               // Number of color fields per pixel (24-bit vs 32-bit encoding)
    uint difference_in_pixels;           // The difference in the number of pixels between source and target
    uint difference_in_bytes;            // The difference in the number of bytes between source and target
    uint source_num_padding_bytes;       // The number of padding bytes in the source bitmap
    uint target_num_padding_bytes;       // The number of padding bytes in the target bitmap

    if (b.color_depth == 32) number_of_colors = 4;
    else                     number_of_colors = 3;

    std::cout << "Applying scale down transform." << std::endl;

    // Double the width *and* height of our output bitmap
    difference_in_pixels = ((b.getWidthinPixels() * b.getHeightinPixels()) - (b.getWidthinPixels()/2 * b.getHeightinPixels()/2));
    difference_in_bytes  = difference_in_pixels * number_of_colors;
    outbmp.setWidthinPixels(b.getWidthinPixels()/2);
    outbmp.setHeightinPixels(b.getHeightinPixels()/2);
    outbmp.data.resize(b.data.size()/4);  // Divide the size of our output bitmap's vector by four (optional, just to save memory)

    // Iterate over all the pixels in the picture
    for (int y = 0; y < b.getHeightinPixels(); y++) {
        for (int x = 0; x < b.getWidthinPixels(); x++) {
            uint red   = 0;
            uint green = 0;
            uint blue  = 0;
            uint alpha = 0;
            uint target1_x, target1_y;

            // Initialize our source pixel
            source_pixel.init(b, x, y);
            if (b.color_depth == 32) {
                source_pixel.getrgba(red, green, blue, alpha);
            }
            else {  // 24-bit
                source_pixel.getrgb(red, green, blue);
            }

            // *** Ignore every other row (halving the number of rows) ***
            if (y%2 == 1) {
                target1_x =  x/2;
                target1_y =  y/2;

                // Set the colors for our 4x4 block of copied pixel(s) and write them to the target image
                if (DEBUG) std::cout << "Target pixel1:  x/y:  " << std::dec << y << "/" << x << " mx/my:  " << outbmp.getHeightinPixels() << "/" << outbmp.getWidthinPixels() << " tx/ty:  " << target1_y << "/" << target1_x << std::endl;
                target1_pixel.init(outbmp, target1_x, target1_y);

                if (b.color_depth == 32) {
                    target1_pixel.setrgb(red, green, blue);
                }
                else {
                    target1_pixel.setrgba(red, green, blue, alpha);
                }

                target1_pixel.write();
            }
        }

        if (b.getWidthinPixels() != outbmp.getWidthinPixels() || b.getHeightinPixels() != outbmp.getHeightinPixels()) {
            // Adjust the file length and data size based on the changed number of padding bytes, if necessary.
            source_num_padding_bytes =      b.getRowPaddingSize();  // If 32-bit, getRowPaddingSize will be 0
            target_num_padding_bytes = outbmp.getRowPaddingSize();  // If 32-bit, getRowPaddingSize will be 0
    
            outbmp.setFileLength(b.getFileLength() - difference_in_bytes - (source_num_padding_bytes * b.height_in_pixels) + (target_num_padding_bytes * outbmp.height_in_pixels * 2));
            outbmp.setDataSize  (b.getDataSize()   - difference_in_bytes - (source_num_padding_bytes * b.height_in_pixels) + (target_num_padding_bytes * outbmp.height_in_pixels * 2));
        }
    }

    b = outbmp;  // Return outbmp instead of b

    return;
}

/**
 * Transform the image, depending on mode:
 * 0 = ROT90
 * 1 = ROT180
 * 2 = ROT270
 * 3 = FLIPV
 * 4 = FLIPH
 * 5 = FLIPD1
 * 6 = FLIPD2
 */
void imageTransform(Bitmap& b, uint mode) {
    Bitmap outbmp = b;                   // Create a second bitmap to hold our output values
    BitmapPixel source_pixel;            // Pixel object to hold the source data
    BitmapPixel target1_pixel;           // A pixel to write back to the target image
    uint source_num_padding_bytes;       // The number of padding bytes in the source bitmap
    uint target_num_padding_bytes;       // The number of padding bytes in the target bitmap

    // Edit our output file header settings dependent on mode
    if      (mode == 0) {   // ROT90
        std::cout << "Applying rotate 90 transform." << std::endl;
        // Swap the height and width values in our output bitmap
        outbmp.setWidthinPixels(b.getHeightinPixels());
        outbmp.setHeightinPixels(b.getWidthinPixels());
    }
    else if (mode == 1) {   // ROT180
        std::cout << "Applying rotate 180 transform." << std::endl;
    }
    else if (mode == 2) {   // ROT270
        std::cout << "Applying rotate 270 transform." << std::endl;
        // Swap the height and width values in our output bitmap
        outbmp.setWidthinPixels(b.getHeightinPixels());
        outbmp.setHeightinPixels(b.getWidthinPixels());
    }
    else if (mode == 3) {   // FLIPV
        std::cout << "Applying flip vertical transform." << std::endl;
    }
    else if (mode == 4) {   // FLIPH
        std::cout << "Applying flip horizontal transform." << std::endl;
    }
    else if (mode == 5) {   // FLIPD1
        std::cout << "Applying flip diagonal 1 transform." << std::endl;
        // Swap the height and width values in our output bitmap
        outbmp.setWidthinPixels(b.getHeightinPixels());
        outbmp.setHeightinPixels(b.getWidthinPixels());
    }
    else if (mode == 6) {   // FLIPD2
        std::cout << "Applying flip diagonal 2 transform." << std::endl;
        // Swap the height and width values in our output bitmap
        outbmp.setWidthinPixels(b.getHeightinPixels());
        outbmp.setHeightinPixels(b.getWidthinPixels());
    }
    else {
        std::cout << "Error - Invalid tranform mode selected." << std::endl;
        return;
    }

    // Iterate over all the pixels in the picture
    for (int y = 0; y < b.getHeightinPixels(); y++) {
        for (int x = 0; x < b.getWidthinPixels(); x++) {
            uint red   = 0;
            uint green = 0;
            uint blue  = 0;
            uint alpha = 0;
            uint target1_x, target1_y;

            // Initialize our source pixel
            source_pixel.init(b, x, y);
            if (b.color_depth == 32) {
                source_pixel.getrgba(red, green, blue, alpha);
            }
            else {  // 24-bit
                source_pixel.getrgb(red, green, blue);
            }

            // Transform our picture depending on mode
            if (mode == 0) {       // ROT90
                // *** Swap X and Y, but reverse Y so we don't reverse the image ***
                target1_x = y;
                target1_y = (outbmp.getHeightinPixels()-1) - x;
            }
            else if (mode == 1) {  // ROT180
                // *** Reverse X and Y ***
                target1_x = (outbmp.getWidthinPixels()-1)  - x;
                target1_y = (outbmp.getHeightinPixels()-1) - y;
            }
            else if (mode == 2) {  // ROT270
                // *** Swap X and Y, but reverse Y so we don't reverse the image ***
                target1_x = (outbmp.getWidthinPixels()-1) - y;
                target1_y = x;
            }
            else if (mode == 3) {  // FLIPV
                // *** Reverse X ***
                target1_x = (outbmp.getWidthinPixels()-1) - x;
                target1_y = y;
            }
            else if (mode == 4) {  // FLIPH
                // *** Reverse Y ***
                target1_x = x;
                target1_y = (outbmp.getHeightinPixels()-1) - y;
            }
            else if (mode == 5) {  // FLIPD1
                // *** Flip X and Y ***
                target1_x = y;
                target1_y = x;
            }
            else if (mode == 6) {  // FLIPD2
                // *** Flip *and* Reverse X and Y ***
                target1_x = (outbmp.getWidthinPixels()-1) - y;
                target1_y = (outbmp.getHeightinPixels()-1)  - x;
            }

            // Write our copied pixel to the target image
            if (DEBUG) std::cout << "Target pixel1:  x/y:  " << std::dec << y << "/" << x << " mx/my:  " << outbmp.getHeightinPixels() << "/" << outbmp.getWidthinPixels() << " tx/ty:  " << target1_y << "/" << target1_x << std::endl;
            target1_pixel.init(outbmp, target1_x, target1_y);
            if (b.color_depth == 32) {
                target1_pixel.setrgb(red, green, blue);
            }
            else {
                target1_pixel.setrgba(red, green, blue, alpha);
            }
            target1_pixel.write();
        }
    }

    if (b.getWidthinPixels() != outbmp.getWidthinPixels() || b.getHeightinPixels() != outbmp.getHeightinPixels()) {
        // Adjust the file length and data size based on the changed number of padding bytes, if necessary.
        source_num_padding_bytes =      b.getRowPaddingSize();  // If 32-bit, getRowPaddingSize will be 0
        target_num_padding_bytes = outbmp.getRowPaddingSize();  // If 32-bit, getRowPaddingSize will be 0

        outbmp.setFileLength(b.getFileLength() - (source_num_padding_bytes * b.height_in_pixels) + (target_num_padding_bytes * outbmp.height_in_pixels));
        outbmp.setDataSize  (b.getDataSize()   - (source_num_padding_bytes * b.height_in_pixels) + (target_num_padding_bytes * outbmp.height_in_pixels));
    }

    b = outbmp;  // Return outbmp instead of b

    return;
}

/**
 * BitmapException denotes an exception from reading in a bitmap.
 */
BitmapException::BitmapException(const std::string& message, uint32_t file_offset) {
    _message  = message;
    _position = file_offset;
}
BitmapException::BitmapException(std::string&& message, uint32_t file_offset) {
    _message  = message;
    _position = file_offset;
}

void BitmapException::print_exception() {
    std::cout << "Exception:  " << _message << " - Position " << _position << std::endl;
}

/**
 * BitmapPixel - to handle functions associated with Bitmap pixel transforms
 */
BitmapPixel::BitmapPixel() {}
BitmapPixel::BitmapPixel(Bitmap& bptr, uint xin, uint yin) {
    b = &bptr;
    x = xin;
    y = yin;
    b->readPixel(x, y, red, green, blue, alpha);
}
void BitmapPixel::init(Bitmap& bptr, uint xin, uint yin) {
    b = &bptr;
    x = xin;
    y = yin;
    b->readPixel(x, y, red, green, blue, alpha);
}
void BitmapPixel::write() {
    b->writePixel(x, y, red, green, blue, alpha);
    return;
}
void BitmapPixel::getrgb(uint &redvalue, uint &greenvalue, uint &bluevalue) {
    redvalue   = red;
    greenvalue = green;
    bluevalue  = blue;

    return;
}
void BitmapPixel::getrgba(uint &redvalue, uint &greenvalue, uint &bluevalue, uint &alphavalue) {
    redvalue   = red;
    greenvalue = green;
    bluevalue  = blue;
    alphavalue = alpha;

    return;
}
void BitmapPixel::setrgb(uint &redvalue, uint &greenvalue, uint &bluevalue) {
    red   = redvalue;
    green = greenvalue;
    blue  = bluevalue;

    return;
}
void BitmapPixel::setrgba(uint &redvalue, uint &greenvalue, uint &bluevalue, uint &alphavalue) {
    red   = redvalue;
    green = greenvalue;
    blue  = bluevalue;
    alpha = alphavalue;

    return;
}
