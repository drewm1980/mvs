// Prototype code for parts of a multi-view-stereo system

#include <stdint.h>
#include <math.h>

int indeces_to_linear_index_packed_row_major(int u, int v, int stride) {
    // Converts integer pixel locations to the linear index for a dense row
    // major 2D array.
    // NO BOUNDS CHECKING!
    return v * stride + u;
}

void lookup_adgacent_pixels_mono8_packed_row_major(
    const uint8_t* image, int stride, float x, float y, uint8_t* topleft,
    uint8_t* topright, uint8_t* bottomleft, uint8_t* bottomright)
{
    // Looks up the values of adgacent pixels
    // image points to the input pixel data
    // x,y is a location in the image
    // NO BOUNDS CHECKING!
    // Note: Using booth floor and ceil handles edge cases where we are on one
    // of the edges of the image.
    int left = floorf(x);
    int top = floorf(y);
    int right = ceilf(x);
    int bottom = ceilf(y);
    *topleft =
        image[indeces_to_linear_index_packed_row_major(left, top, stride)];
    *bottomleft =
        image[indeces_to_linear_index_packed_row_major(left, bottom, stride)];
    *topright =
        image[indeces_to_linear_index_packed_row_major(right, top, stride)];
    *bottomright =
        image[indeces_to_linear_index_packed_row_major(right, bottom, stride)];
}

float lookup_mono8_packed_row_major(const uint8_t* image, int stride, float x,
                                    float y) {
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
    /*float right = ceilf(x);*/
    /*float bottom = ceilf(y);*/
    uint8_t topleft_int8, topright_int8, bottomleft_int8, bottomright_int8;
    lookup_adgacent_pixels_mono8_packed_row_major(
        image, stride, x, y, &topleft_int8, &topright_int8, &bottomleft_int8,
        &bottomright_int8);

    float topleft = topleft_int8;
    float topright = topright_int8;
    float bottomleft = bottomleft_int8;
    float bottomright = bottomright_int8;

    // Finally, the Bilinear interpolation
    float rightness = x - left;
    float bottomness = y - top;
    float leftness = 1.0f - rightness;
    float topness = 1.0f - bottomness;
    float z1 = topleft * topness * leftness;
    float z2 = topright * topness * rightness;
    float z3 = bottomleft * bottomness * leftness;
    float z4 = bottomright * bottomness * rightness;
    return z1 + z2 + z3 + z4;
}

void two_rotating_patches_benchmark(const uint8_t* restrict image,
                                    float* restrict output, int num_angles) {
    // Compare two patches with rotated domains with each other as a benchmark.
    float center1_x = 128.0f;
    float center1_y = 128.0f;
    const float radius = 128.0f;
    //const float radius1 = 32.0f;
    const float radius1 = radius;
    float center2_x = 384.0f;
    float center2_y = 384.0f;
    //const float radius2 = 64.0f;
    const float radius2 = radius1;
    const int PATCH_W = 32;
    const float pi = 3.1415;
    const float angle_step = 2.0 * pi / num_angles;
    const int lenaStride = 512;
    for (int angle_index = 0; angle_index < num_angles; angle_index++) {
        float angle = angle_index * angle_step;
        float _sin, _cos;
        //sincosf(angle, &_sin, &_cos);
	_sin = sin(angle);	
	_cos = cos(angle);
        const float r00 = _cos;  // Wikipedia 2x2 rotation definition
        const float r01 = -_sin;
        const float r10 = _sin;
        const float r11 = _cos;
	float sad = 0.0f; // for computing sum of absolute differences
        for (int ix = 0; ix < PATCH_W; ix++) {
/*#pragma unroll 4*/
            for (int iy = 0; iy < PATCH_W; iy++) {
		float left_value;
		{
                    float patch_x = ix * (radius1 / PATCH_W);
                    float patch_y = iy * (radius1 / PATCH_W);
                    float x = patch_x * r00 + patch_y * r01;
                    float y = patch_x * r10 + patch_y * r11;
                    left_value = lookup_mono8_packed_row_major(
                        image, lenaStride, x + center1_x, y + center1_y);
                }

		float right_value;
                {
                    float patch_x = ix * (radius2 / PATCH_W);
                    float patch_y = iy * (radius2 / PATCH_W);
                    float x = patch_x * r00 + patch_y * r01;
                    float y = patch_x * r10 + patch_y * r11;
                    right_value = lookup_mono8_packed_row_major(
                        image, lenaStride, x + center2_x, y + center2_y);
                }
                sad += fabsf(left_value - right_value);
            }
        }
	output[angle_index] = sad;
    }
}
