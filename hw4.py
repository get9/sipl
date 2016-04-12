import sys
import glob
import os.path
import cv2
import numpy as np

def bresenham(start, end):
    # Setup initial conditions
    x1, y1 = start
    x2, y2 = end
    dx = x2 - x1
    dy = y2 - y1
 
    # Determine how steep the line is
    is_steep = abs(dy) > abs(dx)
 
    # Rotate line
    if is_steep:
        x1, y1 = y1, x1
        x2, y2 = y2, x2
 
    # Swap start and end points if necessary and store swap state
    swapped = False
    if x1 > x2:
        x1, x2 = x2, x1
        y1, y2 = y2, y1
        swapped = True
 
    # Recalculate differentials
    dx = x2 - x1
    dy = y2 - y1
 
    # Calculate error
    error = int(dx / 2.0)
    ystep = 1 if y1 < y2 else -1
 
    # Iterate over bounding box generating points between start and end
    y = y1
    points = []
    for x in range(x1, x2 + 1):
        coord = (y, x) if is_steep else (x, y)
        points.append(coord)
        error -= abs(dy)
        if error < 0:
            y += ystep
            error += dx
 
    # Reverse the list if the coordinates were swapped
    if swapped:
        points.reverse()
    return points


def read_image_dir(image_dir, downsample_factor=1):
    fnames = []
    for f in os.listdir(image_dir):
        if f.lower().endswith('.png'):
            fnames.append(os.path.join(image_dir, f))
    mats = []
    #for f in fnames[::downsample_factor]:
    for f in fnames:
        mats.append(cv2.imread(f, -1)[::downsample_factor, ::downsample_factor])
    return np.array(mats)

def draw_img_with_target_line(img, start, end):
    #img = cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)
    img = cv2.line(img, start, end, (0, 0, 255))
    cv2.imshow("image", img)
    cv2.waitKey()

def to_uint8(m):
    return np.clip(m, 0, 255).astype(np.uint8)

def main():
    if len(sys.argv) < 6:
        print("Usage:")
        print("    {} images-dir startx starty endx endy".format(sys.argv[0]))
        sys.exit(1)

    downsample = 1

    images_dir = sys.argv[1]
    startx, starty, endx, endy = list(map(lambda x: int(x) // downsample, sys.argv[2:]))
    stack = read_image_dir(images_dir, downsample_factor=downsample)

    line_idxs = bresenham((startx, starty), (endx, endy))

    # Calculate average image
    bg = np.median(stack, axis=0)

    fgs = []
    for i in stack:
        gray = cv2.cvtColor(to_uint8(np.abs(i - bg)), cv2.COLOR_BGR2GRAY)
        gray = cv2.GaussianBlur(gray, (5, 5), 0)
        fgs.append(to_uint8(gray))
    
    # Calculate temporal mode for each pixel
    slice_img = np.zeros((len(stack) * 2, len(line_idxs)), dtype=np.uint8)
    for i, fg in enumerate(fgs):
        for j, (x, y) in enumerate(line_idxs):
            slice_img[2 * i, j] = fg[y, x]
            slice_img[2 * i + 1, j] = fg[y, x]

        # Subtract temporal average from the image
        #fg = to_uint8(np.abs(i - bg))

        #gray = cv2.cvtColor(fg, cv2.COLOR_BGR2GRAY)
        #gray = cv2.normalize(gray, gray, 0, 255, cv2.NORM_MINMAX)

        ## Threshold
        #_, gray = cv2.threshold(gray, int(255 * 0.17), 255, cv2.THRESH_BINARY)

        ##gray = cv2.dilate(gray, np.ones((3, 3)))
        ##gray = cv2.erode(gray, rect_kernel)
        #gray = cv2.morphologyEx(gray, cv2.MORPH_CLOSE, np.ones((3, 3)))
        ##nlabels, label_img = cv2.connectedComponents(gray, 8)

        #draw_img_with_target_line(gray, (startx, starty), (endx, endy))

    #_, slice_img = cv2.threshold(slice_img, int(0.10 * 255), 255, cv2.THRESH_TOZERO)
    slice_img = cv2.normalize(slice_img, slice_img, 0, 255, cv2.NORM_MINMAX)

    cv2.namedWindow("reslice gray", cv2.WINDOW_NORMAL)
    cv2.imshow("reslice gray", slice_img)

    _, slice_img = cv2.threshold(slice_img, int(0.25 * 255), 255, cv2.THRESH_BINARY)
    slice_img = cv2.medianBlur(slice_img, 5)
    ##ellipse = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (15, 5))
    #slice_img = cv2.morphologyEx(slice_img, cv2.MORPH_CLOSE, np.ones((5, 5), dtype=np.uint8))

    cv2.namedWindow("reslice binary", cv2.WINDOW_NORMAL)
    cv2.imshow("reslice binary", slice_img)
    cv2.waitKey()

main()
