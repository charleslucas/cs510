#include <vector>

class Bitmap
{
private:

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
    friend std::istream& operator>>(std::istream& in, Bitmap& b);

    /**
     * Write the binary representation of the image to the stream.
     *
     * @param out the stream to write to.
     * @param b the bitmap that we are writing.
     *
     * @return the stream after we've finished writing.
     *
     * @throws failure if we failed to write.
     */
    friend std::ostream& operator<<(std::ostream& in, const Bitmap& b);

public:
    char         bitmap_type[2];         // Bitmap Type                          (2 bytes - always "BM"/0x424D)
    uint32_t     length;                 // Length in Bytes                      (4 bytes)
    uint32_t     garbage;                // Garbage                              (4 bytes - ignore)
    uint32_t     offset;                 // Offset to start of the data          (4 bytes)
    uint32_t     size_second_header;     // Size of the Second Header            (4 bytes - always 40 bytes)
     int32_t     width_in_pixels;        // Width in Pixels                      (4 bytes signed)
     int32_t     height_in_pixels;       // Height in Pixels                     (4 bytes signed)
    uint16_t     number_of_color_planes; // Number of Color Planes               (2 bytes - *MUST* be 1 - error test)
    uint16_t     color_depth;            // Color Depth of the Image             (2 bytes - either 24 (RGB) or 32 (RGBA))
    uint32_t     compression_method;     // Compression Method being Used        (4 bytes - always 0(24-bit) or 3(32-bit))
    uint32_t     data_size;              // Size of the Raw Bitmap Data in Bytes (4 bytes)
    uint32_t     horizontal_resolution;  // Horizontal Resolution in Pixels      (4 bytes - dots per meter - always 2835 - ignore)
    uint32_t     vertical_resolution;    // Vertical Resolution in Pixels        (4 bytes - dots per meter - always 2835 - ignore)
    uint32_t     number_of_colors;       // Number of Colors in Color Palette    (4 bytes - always 0 (not using a color palette - ignore))
    uint32_t     important_colors;       // Number of Important Colors Used      (4 bytes - always 0 (not using a color palette - ignore))
    uint32_t     red_mask;               // Red Mask                             (4 bytes - only exists in 32-bit image)
    uint32_t     green_mask;             // Green Mask                           (4 bytes - only exists in 32-bit image)
    uint32_t     blue_mask;              // Blue Mask                            (4 bytes - only exists in 32-bit image)
    uint32_t     alpha_mask;             // Alpha Mask                           (4 bytes - only exists in 32-bit image)
    char         color_space[68];        // Color Space Information              (68 bytes - only exists in 32-bit image - ignore)

    std::vector<char> data;              // Actual picture data                  (Formatted depending on 24/32 bit color)

    Bitmap();

    void     readPixel (int x, int y, uint &red, uint &green, uint &blue, uint &alpha);
    void     writePixel(int x, int y, uint &red, uint &green, uint &blue, uint &alpha);
    uint32_t getRowPaddingSize() const;
    uint32_t getFileLength() const;
    void     setFileLength(uint32_t length);
    uint32_t getDataSize() const;
    void     setDataSize(uint32_t length);
    uint16_t getColorDepth() const;
    int32_t  getWidthinPixels() const;
    void     setWidthinPixels(int width);
    int32_t  getHeightinPixels() const;
    void     setHeightinPixels(int height);

};

/**
 * cell shade an image.
 * for each component of each pixel we round to 
 * the nearest number of 0, 180, 255
 *
 * This has the effect of making the image look like
 * it was colored.
 */
void cellShade(Bitmap& b);

/**
 * Grayscales an image by averaging all of the components.
 */
void grayscale(Bitmap& b);

/**
 * Pixelats an image by creating groups of 16*16 pixel blocks.
 */
void pixelate(Bitmap& b);

/**
 * Use gaussian bluring to blur an image.
 */
void blur(Bitmap& b);

/**
 * rotates image 90 degrees, swapping the height and width.
 */
void rot90(Bitmap& b);

/**
 * rotates an image by 180 degrees.
 */
void rot180(Bitmap& b);

/**
 * rotates image 270 degrees, swapping the height and width.
 */
void rot270(Bitmap& b);

/**
 * flips and image over the vertical axis.
 */
void flipv(Bitmap& b);

/**
 * flips and image over the horizontal axis.
 */
void fliph(Bitmap& b);

/**
 * flips and image over the line y = -x, swapping the height and width.
 */
void flipd1(Bitmap& b);

/**
 * flips and image over the line y = xr, swapping the height and width.
 */
void flipd2(Bitmap& b);

/**
 * scales the image by a factor of 2.
 */
void scaleUp(Bitmap& b);

/**
 * scales the image by a factor of 1/2.
 */
void scaleDown(Bitmap& b);

/**
 * Perform the image transforms depending on mode
 * 0 = ROT90
 * 1 = ROT180
 * 2 = ROT270
 * 3 = FLIPV
 * 4 = FLIPH
 * 5 = FLIPD1
 * 6 = FLIPD2
 * 7 = SCALEUP
 * 8 = SCALEDOWN
 */
void imageTransform(Bitmap& b, uint mode);

/**
 * BitmapException denotes an exception from reading in a bitmap.
 */
class BitmapException : public std::exception
{
    // the message to print out
    std::string _message;

    // position in the bitmap file (in bytes) where the error occured.
    uint32_t _position;

public:
    BitmapException() = delete;

    BitmapException(const std::string& message, uint32_t position);
    BitmapException(std::string&& message, uint32_t position);

    /**
     * prints out the exception in the form:
     *
     * "Error in bitmap at position 0xposition :
     * message"
     */
    void print_exception();
};


/**
 * BitmapPixel - to handle functions associated with Bitmap pixel transforms
 */
 class BitmapPixel
 {
 public:
    Bitmap  *b;      // Pointer to the bitmap this pixel exists in
    int      x;
    int      y;
    uint     red;
    uint     green;
    uint     blue;
    uint     alpha;

    BitmapPixel();
    BitmapPixel(Bitmap& b, uint x, uint y);
    void init  (Bitmap& b, uint x, uint y);
    void write ();
    void getrgb (uint &redvalue, uint &greenvalue, uint &bluevalue);
    void getrgba(uint &redvalue, uint &greenvalue, uint &bluevalue, uint &alphavalue);
    void setrgb (uint &redvalue, uint &greenvalue, uint &bluevalue);
    void setrgba(uint &redvalue, uint &greenvalue, uint &bluevalue, uint &alphavalue);

    /**
     * Averages the values of the RGB color fields
     */
    //void average(Pixel& p);
 };