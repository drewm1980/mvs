#!/usr/bin/env python3

# Code for testing the code in mvs.c

import numpy
import ctypes

from ctypes import c_float as float32
from ctypes import c_int32 as int32

from numpy.ctypeslib import ndpointer
_libmvs = numpy.ctypeslib.load_library('libmvs','.')

Float3Vector = ndpointer(dtype=numpy.float32, ndim=1, flags='CONTIGUOUS,ALIGNED',shape=(3,))
FloatPointer = ndpointer(dtype=numpy.float32, ndim=1, flags='ALIGNED') # Zero dimensional turned out to be a hassle

lenaStride = 512
LenaMonoImagePointer = ndpointer(dtype=numpy.uint8, ndim=2, flags='ALIGNED', shape=(lenaStride,lenaStride)) # Zero dimensional turned out to be a hassle

_libmvs.two_rotating_patches_benchmark.argtypes = [LenaMonoImagePointer, FloatPointer, int32] ;# image, output, num_angles
def two_rotating_patches_benchmark(image, num_angles):
    output = numpy.zeros(num_angles, dtype=numpy.float32)
    assert image.shape[1] == lenaStride
    assert image.shape[0] == lenaStride
    _libmvs.two_rotating_patches_benchmark(image, output, num_angles)
    return output

def test_lena_benchmark():
    from mvs import load_image_preserving_type, rgb_to_mono
    import pylab
    num_angles = 1000
    image_color = load_image_preserving_type('lena.png')
    image = rgb_to_mono(image_color)
    from time import time
    t1 = time()
    output = two_rotating_patches_benchmark(image, num_angles)
    t2 = time()
    print('Benchmark took ', (t2-t1), ' sec')

if __name__ == '__main__':
    test_lena_benchmark()
