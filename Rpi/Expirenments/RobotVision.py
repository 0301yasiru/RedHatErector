import cv2 as cv
import numpy as np
from datetime import datetime

def getImPath(ref1, ref2):
    """
    DOCSTRING: This function will return the image path
    """
    return r'ML/{}_{}.jpg'.format(ref1, ref2)

def rescaleFrame(frame, scale = 0.17):
    """
    DOCSTRING: thid function will resize the image for the given scale
    """
    width = int(frame.shape[1] * scale)
    height = int(frame.shape[0] * scale)

    return cv.resize(frame, (width, height), interpolation=cv.INTER_AREA)

def detectCorners(img):
    gray = cv.cvtColor(img,cv.COLOR_BGR2GRAY)
    gray = np.float32(gray)
    dst = cv.cornerHarris(gray,4,3,0.01)
    #result is dilated for marking the corners, not important
    dst = cv.dilate(dst,None)
    # Threshold for an optimal value, it may vary depending on the image.
    img[dst>0.01*dst.max()]=[0,0,255]
    cv.imshow('dst',img)
    if cv.waitKey(0) & 0xff == 27:
        cv.destroyAllWindows()


def main():

    im1 = cv.imread(getImPath(1,2))
    im1 = rescaleFrame(im1)

    # Now get edges
    # start = datetime.now()
    # cany = cv.Canny(im1, 125, 175)
    # start = datetime.now() - start
    # print(f'Amount of Time taken for process -> {start.microseconds}')

    # for corner in detectCorners(im1):
    #     print (corner)
    #     # cv.circle(im1, corner[0].astype('int16'), 3, (0, 0,255),-1)

    # cv.imshow('Image Frame', im1)
    # cv.waitKey(0)
    detectCorners(im1)


if __name__ == '__main__':
    main()