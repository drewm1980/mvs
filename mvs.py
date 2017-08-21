#!/usr/bin/env python3

# Prototype code for parts of a multi-view-stereo system

import numpy

#import pylab
#im = pylab.imread('lena.png')

def load_image_preserving_type(imname):
    from PIL import Image
    im = Image.open(imname)
    return numpy.array(im)
#im_color = load_image_preserving_type('lena.png')

def rgb_to_mono(im):
    """ Color conversion of an image from rgbrgb... to kkkk """
    # Trust numpy not to saturate in the intermediates...
    # seems it converts to float internally
    return im.mean(axis=2).astype(im.dtype)
#im = rgb_to_mono(im_color)

def lookup_monochrome(im, x:float, y:float)->float:
    # Given an image and pixel coordinates x,y,
    # x going left to right
    # y going top to bottom
    # Perform texture lookup with bilinear interpolation.
    # Assumes square pixels, mono image.
    # WILL CRASH for lookups outside the image.
    # Lookups exactly on the edge of the image are valid.

    # Access
    left = int(numpy.floor(x))
    top = int(numpy.floor(y))
    right = int(numpy.ceil(x)) 
    bottom = int(numpy.ceil(y))
    # Note: Using booth floor and ceil handles edge cases where we are on one of the edges of the image.
    topleft = im[top, left]
    bottomleft = im[bottom, left]
    topright = im[top, right]
    bottomright = im[bottom, right]
    #print(topleft,topright,bottomleft,bottomright)

    # Bilinear interpolation
    rightness = x-left
    bottomness = y-top
    leftness = 1.0-rightness
    topness = 1.0-bottomness
    #print(rightness, leftness, topness, bottomness)
    z1 = topleft*topness*leftness
    z2 = topright*topness*rightness
    z3 = bottomleft*bottomness*leftness
    z4 = bottomright*bottomness*rightness
    #print(z1,z2,z3,z4)
    return z1+z2+z3+z4

def float_compare(a,b,eps=.0001):
    return abs(a-b)<eps

def test_lookup_monochrome1():
    im = numpy.array([[1.0,2.0],[3.0,4.0]])
    assert float_compare(lookup_monochrome(im, 0,0), 1.0)
    assert float_compare(lookup_monochrome(im, 1,0), 2.0)
    assert float_compare(lookup_monochrome(im, 0,1), 3.0)
    assert float_compare(lookup_monochrome(im, 1,1), 4.0)

def test_lookup_monochrome2():
    im = numpy.array([[2.0,2.0],[2.0,2.0]])
    assert float_compare(lookup_monochrome(im, 0.5,0.5) , 2.0) 
    
def test_lookup_monochrome3():
    im = numpy.array([[1.0,1.0],[2.0,2.0]])
    assert float_compare(lookup_monochrome(im, 0.5,0.5) , 1.5) 

def test_lookup_monochrome4():
    im = numpy.array([[1.0,2.0],[1.0,2.0]])
    assert float_compare(lookup_monochrome(im, 0.5,0.5) , 1.5) 
    
if __name__=='__main__':
    test_lookup_monochrome1()
