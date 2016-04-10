import sys
import glob
import os.path
import cv2
import numpy as np

def make_high_boost_filter(c=1):
    return np.array([
        [-c, -c,        -c],
        [-c, 8 * c + 1, -c],
        [-c, -c,        -c],
    ], dtype=np.float64)

def read_image_dir(image_dir, downsample_factor=1):
    fnames = []
    for f in os.listdir(image_dir):
        if f.lower().endswith('.png'):
            fnames.append(os.path.join(image_dir, f))
    mats = []
    for f in fnames[::downsample_factor]:
        mats.append(cv2.imread(f, -1)[::downsample_factor, ::downsample_factor])
    return np.array(mats)

def mode(a):
    return np.bincount(a).argmax()

def draw_img_with_target_line(img, start, end):
    img = cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)
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

    downsample = 2

    images_dir = sys.argv[1]
    startx, starty, endx, endy = list(map(lambda x: int(x) // downsample, sys.argv[2:]))
    stack = read_image_dir(images_dir, downsample_factor=downsample)

    # Calculate average image
    avg_img = np.average(stack, axis=0)
    cv2.imwrite("avg_img.png", to_uint8(avg_img))
    sys.exit()
    #mode_img = np.empty_like(stack[0], dtype=np.int)
    #for i in range(stack.shape[1]):
    #    for j in range(stack.shape[2]):
    #        #mode_img[i, j, 0] = mode(stack[:, i, j, 0])
    #        #mode_img[i, j, 1] = mode(stack[:, i, j, 1])
    #        #mode_img[i, j, 2] = mode(stack[:, i, j, 2])
    #        mode_img[i, j] = mode(stack[:, i, j])

    # Calculate temporal mode for each pixel
    circle = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (7, 7))
    xkernel = np.array([[1, 0, 1],[0, 1, 0], [1, 0, 1]], dtype=np.uint8)
    rect33 = np.array([[1, 1, 1],[1, 1, 1], [1, 1, 1]], dtype=np.uint8)
    rect22 = np.array([[1, 1],[1, 1]], dtype=np.uint8)
    cross = np.array([[0, 1, 0], [1, 1, 1], [0, 1, 0]], dtype=np.uint8)
    diamond55 = np.array([[0,0,1,0,0],[0,1,1,1,0],[1,1,1,1,1],[0,1,1,1,0],[0,0,1,0,0]], dtype=np.uint8)
    for i in stack:

        # Subtract temporal average from the image
        gray = to_uint8(np.abs(i - mode_img))

        #gray = cv2.cvtColor(subtracted, cv2.COLOR_BGR2GRAY)
        gray = cv2.normalize(gray, gray, 0, 255, cv2.NORM_MINMAX)
        #gray = cv2.GaussianBlur(gray, (7, 7), 0)

        # Threshold
        #_, gray = cv2.threshold(gray, int(255 * 0.17), 255, cv2.THRESH_OTSU)

        #gray = cv2.dilate(gray, xkernel)
        #gray = cv2.erode(gray, rect_kernel)
        #gray = cv2.morphologyEx(gray, cv2.MORPH_DILATE, rect22)
        #nlabels, label_img = cv2.connectedComponents(gray, 8)

        #draw_img_with_target_line(to_uint8(label_img * 30), (startx, starty), (endx, endy))
        draw_img_with_target_line(gray, (startx, starty), (endx, endy))

main()
