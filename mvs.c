// Prototype code for parts of a multi-view-stereo system

#include <stdint.h>
#include <math.h>

int indeces_to_linear_index_packed_row_major(int u, int v, int stride)
{
    // Converts integer pixel locations to the linear index for a dense row major 2D array.
    // NO BOUNDS CHECKING!
    return v*stride+u;
}

void lookup_adgacent_pixels_mono8_packed_row_major(const uint8_t* image, int stride, float x, float y, uint8_t* topleft,
                            uint8_t* topright, uint8_t* bottomleft,
                            uint8_t* bottomright) 
    // Looks up the values of adgacent pixels
    // image points to the input pixel data
    // x,y is a location in the image
    // NO BOUNDS CHECKING!
{
	// Note: Using booth floor and ceil handles edge cases where we are on one of the edges of the image.
	int left = floorf(x);
	int top = floorf(y);
	int right = ceilf(x) ;
	int bottom = ceilf(y);
	*topleft = image[indeces_to_linear_index_packed_row_major(left,top,stride)];
	*bottomleft = image[indeces_to_linear_index_packed_row_major(left,bottom,stride)];
	*topright = image[indeces_to_linear_index_packed_row_major(right,top,stride)];
	*bottomright = image[indeces_to_linear_index_packed_row_major(right,bottom,stride)];
}

void lookup_mono8_packed_row_major(uint8_t * image, int stride, float x, float y, float* value)
{
	// Given an image and pixel coordinates x,y,
	// x going left to right
	// y going top to bottom
	// Perform texture lookup with bilinear interpolation.
	// Assumes square pixels, mono image.
	// WILL CRASH for lookups outside the image.
	// Lookups exactly on the edge of the image are valid.

	// Access
	float left = floorf(x);
	float top = floorf(y);
	float right = ceilf(x) ;
	float bottom = ceilf(y);
    uint8_t topleft_int8, topright_int8, bottomleft_int8, bottomright_int8;
    lookup_adgacent_pixels_mono8_packed_row_major(
        image, stride, x, y, &topleft_int8, &topright_int8, &bottomleft_int8,
        &bottomright_int8);

    float topleft = topleft_int8;
    float topright = topright_int8;
    float bottomleft = bottomleft_int8;
    float bottomright = bottomright_int8;

    // Finally, the Bilinear interpolation
    float rightness = x-left;
    float bottomness = y-top;
    float leftness = 1.0f-rightness;
    float topness = 1.0f-bottomness;
    float z1 = topleft*topness*leftness;
    float z2 = topright*topness*rightness;
    float z3 = bottomleft*bottomness*leftness;
    float z4 = bottomright*bottomness*rightness;
    *value = z1+z2+z3+z4;
}
