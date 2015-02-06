# Title: Gaussian blur with video using OpenCV.
# Author: Connor Goddard
# Date: 5/10/14

import cv2

# setup video capture
cap = cv2.VideoCapture(0)

# get frame, apply Gaussian smoothing, show result
while True:

	ret_val, frame = cap.read()
	
	blur = cv2.GaussianBlur(frame,(0,0),5) 
	
	cv2.imshow('Gaussian Blur Video', blur)
	
	if cv2.waitKey(10) == 27:
		break
	
cv2.destroyAllWindows()