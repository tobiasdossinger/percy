from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import numpy as np
import cv2

camera = PiCamera()
camera.resolution = (640, 480)
camera.framerate = 32
rawCapture = PiRGBArray(camera, size=(640, 480))

time.sleep(0.25)

def computeIntersect(a, b):
    a = a[0]
    b = b[0]

    if isinstance(a, np.ndarray) and isinstance(b, np.ndarray):
        x1 = a[0]
        y1 = a[1]
        x2 = a[2]
        y2 = a[3]
        x3 = b[0]
        y3 = b[1]
        x4 = b[2]
        y4 = b[3]

        d = ((x1 - x2) * (y3 - y4)) - ((y1 - y2) * (x3 - x4))

        if d:
            ptX = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / d
            ptY = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) / d

            return (ptX, ptY)
        else:
            return False
    else:
        return False

for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
    image = frame.array

    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    blurred = cv2.GaussianBlur(gray, (5, 5), 0)
    edges = cv2.Canny(blurred, 100, 100, 3)
    lines = cv2.HoughLinesP(edges, 1, np.pi / 180, 70, minLineLength=30, maxLineGap=10)
    _, contours, _ = cv2.findContours(edges, cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)

    for i in range(len(lines) - 1):
        for j in range(len(lines) - 1):
            lineIntersection = computeIntersect(lines[i], lines[j])

            if lineIntersection:
                cv2.circle(image,(int(lineIntersection[0]), int(lineIntersection[1])), 5, (0,0,255), -1)

    for line in lines:
        for x1, y1, x2, y2 in line:
            cv2.line(image, (x1,y1), (x2,y2), (0,255,0), 3, 8)
    
    for cnt in contours:
        approx = cv2.approxPolyDP(cnt, 0.01 * cv2.arcLength(cnt, True), True)

        if len(approx) == 4:
            cv2.drawContours(image, [approx], 0, (0, 0, 255), -1)

    cv2.imshow('image', image)
    cv2.imshow('edges', edges)
    
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

    rawCapture.truncate(0)

cv2.destroyAllWindows()
