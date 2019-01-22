import cv2
import numpy as np
# rgb(113, 145, 109)
# define the list of boundaries
boundaries = [
	([73, 105, 69], [153, 185, 149]),
	([86, 31, 4], [220, 88, 50]),
	([25, 146, 190], [62, 174, 250]),
	([103, 86, 65], [145, 133, 128])
]


cap = cv2.VideoCapture(0)

while(True):
    # Capture frame-by-frame
    ret, image = cap.read()

    # loop over the boundaries
    # create NumPy arrays from the boundaries
    lower = np.array(boundaries[0][0], dtype = "uint8")
    upper = np.array(boundaries[0][1], dtype = "uint8")

    # find the colors within the specified boundaries and apply
    # the mask
    mask = cv2.inRange(image, lower, upper)
    output = cv2.bitwise_and(image, image, mask = mask)

    # show the images
    cv2.imshow("images", output)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()