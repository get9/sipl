#!/usr/bin/env python3

import sys
import cv2
import numpy as np

if len(sys.argv) < 3:
    print("Usage:")
    print("    {} target reference [show]".format(sys.argv[0]))
    sys.exit(1)

target = cv2.imread(sys.argv[1], cv2.IMREAD_ANYCOLOR)
reference = cv2.imread(sys.argv[2], cv2.IMREAD_ANYCOLOR)

# First check - if files aren't the same, bad news
if target.shape != reference.shape:
    print("Images are not same size")
    print("Your image: {}".format(target.shape))
    print("Target image: {}".format(reference.shape))
    sys.exit(1)

# Do image subtraction
diff = np.abs(target - reference)
print("# diff: {}".format(np.count_nonzero(diff)))
print("% diff: {0:2.2f}".format(np.count_nonzero(diff) / diff.size * 100))

# Optionally show image
if len(sys.argv) > 3:
    cv2.namedWindow("Image Diff", cv2.WINDOW_NORMAL)
    cv2.imshow("Image Diff", diff)
    cv2.waitKey()
