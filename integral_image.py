# Title: Calculate Integral image algorithm. 
# Author: Connor Goddard
# Date: 5/10/14

import cv2

# Read in image.
im = cv2.imread('input.jpg')

# Convert to grey-scale. 
gray = cv2.cvtColor(im,cv2.COLOR_BGR2GRAY)

# Compute integral image value. 
intim = cv2.integral(gray)

# Normalize and save...
intim = (255.0*intim) / intim.max()

cv2.imwrite('integral_result.jpg',intim)