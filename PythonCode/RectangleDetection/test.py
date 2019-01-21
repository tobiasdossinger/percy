import numpy as np
import cv2

cap = cv2.VideoCapture(0)

while(True):
    # Capture frame-by-frame
    ret, frame = cap.read()

    # Our operations on the frame come here
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    ret,thresh = cv2.threshold(gray,127,255,0)
    blurred = cv2.GaussianBlur(gray, (21, 21), 0)
    edged = cv2.Canny(blurred, 0, 20)
    contours, _ = cv2.findContours(thresh,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)

    gray = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
    filtered = cv2.bilateralFilter(gray, 11, 17, 17)
    # blurred = cv2.GaussianBlur(gray, (15, 15), 0)
    edges = cv2.Canny(filtered, 30, 200)
    contours, _ = cv2.findContours(edges, cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)

    for cnt in contours:
        # approx = cv2.approxPolyDP(cnt, 0.051 * cv2.arcLength(cnt, True), True)
        approx = cv2.approxPolyDP(cnt, 0.01 * cv2.arcLength(cnt,True),True)
        area = cv2.contourArea(cnt)

        if len(approx)==4:
            cv2.drawContours(frame,[approx],0,(0,0,255),-1)

    cv2.imshow('frame',edges)




    # gray = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
    # filtered = cv2.bilateralFilter(gray, 11, 17, 17)
    # # blurred = cv2.GaussianBlur(gray, (15, 15), 0)
    # edges = cv2.Canny(filtered, 30, 200)
    # # gray = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
    # # # blurred = cv2.GaussianBlur(gray, (5, 5), 0)
    # # edges = cv2.Canny(gray,50,150,apertureSize = 3)

    # lines = cv2.HoughLines(edges,1,np.pi/180,200)

    # if not lines is None:
    #     for line in lines:
    #         for rho,theta in line:
    #             a = np.cos(theta)
    #             b = np.sin(theta)
    #             x0 = a*rho
    #             y0 = b*rho
    #             x1 = int(x0 + 1000*(-b))
    #             y1 = int(y0 + 1000*(a))
    #             x2 = int(x0 - 1000*(-b))
    #             y2 = int(y0 - 1000*(a))

    #             cv2.line(frame,(x1,y1),(x2,y2),(0,0,255),2)

    # for cnt in contours:
    #     # approx = cv2.approxPolyDP(cnt, 0.051 * cv2.arcLength(cnt, True), True)
    #     approx = cv2.approxPolyDP(cnt, 0.01 * cv2.arcLength(cnt,True),True)
    #     area = cv2.contourArea(cnt)

    #     if len(approx)==4:
    #         cv2.drawContours(frame,[approx],0,(0,0,255),-1)

    # Display the resulting frame
    # cv2.imshow('frame',frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()