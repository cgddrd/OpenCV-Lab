# Title: Haar-cascade face and facial feature detection with OpenCV.
# Author: Connor Goddard
# Date: 5/10/14

import cv2
import sys

# Pass in cascade path as argument - 

inputImage = sys.argv[1]
outputImage = sys.argv[2]
#scaleFactor = sys.argv[3]
#minNeighbors = sys.argv[4]

# Setup OpenCV Haar cascade files.
faceCascade = cv2.CascadeClassifier('./cascades/haarcascade_frontalface_default.xml')
eye_cascade = cv2.CascadeClassifier('./cascades/haarcascade_eye.xml')
smile_cascade = cv2.CascadeClassifier('./cascades/haarcascade_smile.xml')

# Obtain image input.
im = cv2.imread(inputImage)
	
# Generate a grey-scale version of the current image (required for Haar cascade feature analysis)
gray = cv2.cvtColor(im,cv2.COLOR_BGR2GRAY)

# Detect any faces in the current frame using the Haar cascade. (notice we are passing in the grey-scale version).
# We may need to change the 'scaleFactor' based on how well it is finding faces?
faces = faceCascade.detectMultiScale(
    gray,
    scaleFactor=1.3,
    minNeighbors=5,
    #minSize=(50, 50),
   #flags=cv2.cv.CV_HAAR_SCALE_IMAGE
)

# Process any faces that OpenCV has found using the Haar cascade. 
for (x, y, w, h) in faces:

	# Draw a rectangle around the face (with a green line)
	cv2.rectangle(im, (x, y), (x+w, y+h), (0, 255, 0), 2)
	
	# Isolate the "facial area" in order to look for facial features. 
	
	# Get a grey-scale version of the face. 
	roi_gray = gray[y:y+h, x:x+w]
	
	# Get a colour version of the face. 
	roi_color = im[y:y+h, x:x+w]
	
	# Look around the grey-scale facial area for any eyes (using another Haar cascade)
	#eyes = eye_cascade.detectMultiScale(roi_gray)
	
	# Process any eyes that have been found. 
	#for (ex,ey,ew,eh) in eyes:
	
		# Draw a rectangle around the eyes (with a blue line)
	#	cv2.rectangle(roi_color,(ex,ey),(ex+ew,ey+eh),(255,0,0),2)

	#	cv2.putText(roi_color,"Hello World!!!", (ex,ey), cv2.FONT_HERSHEY_SIMPLEX, 10, 255)

# Save the new image.
cv2.imwrite(outputImage,im)