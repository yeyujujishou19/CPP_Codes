# -*- coding: utf-8 -*-
import os
import cv2
import numpy as np
import re   #查找字符串   re.finditer(word, path)]

c = np.load( "ImgFeatures19_问.npy" )
print(c.shape)
print(c.shape[0])
print(c)