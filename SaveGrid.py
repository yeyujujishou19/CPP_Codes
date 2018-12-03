
# -*- coding: utf-8 -*-
import time
import os
import cv2
import numpy as np
import pylab as pl  #画图
import random
import re   #查找字符串   re.finditer(word, path)]

#---------------------2--------------------------------------------------
#叠加两张图片，输入皆是黑白图，img1是底层图片，img2是上层图片，返回叠加后的图片
def ImageOverlay(img1,img2):
    # 把logo放在左上角，所以我们只关心这一块区域
    h = img1.shape[0]
    w = img1.shape[1]
    rows = img2.shape[0]
    cols = img2.shape[1]
    roi = img1[int((h - rows) / 2):rows + int((h - rows) / 2), int((w - cols) / 2):cols + int((w - cols) / 2)]

    # 创建掩膜
    img2gray = img2.copy()
    ret, mask = cv2.threshold(img2gray, 0, 255, cv2.THRESH_OTSU)

    mask_inv = cv2.bitwise_not(mask)
    # 保留除logo外的背景
    img1_bg = cv2.bitwise_and(roi, roi, mask=mask)
    dst = cv2.add(img1_bg, img2)  # 进行融合
    # cv2.imshow('img2', img1)
    # k = cv2.waitKey(0)
    img1[int((h - rows) / 2):rows + int((h - rows) / 2),int((w - cols) / 2):cols + int((w - cols) / 2)] = dst  # 融合后放在原图上
    return img1

#函数功能：处理白边
#找到上下左右的白边位置
#剪切掉白边
#二值化
#将图像放到64*64的白底图像中心
def HandWhiteEdges(img):
    ret, thresh1 = cv2.threshold(img, 249, 255, cv2.THRESH_BINARY)
    cv2.imshow("66", thresh1)
    cv2.waitKey(0)
    # OpenCV定义的结构元素
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (2, 2))
    # 膨胀图像
    thresh1 = cv2.dilate(thresh1, kernel)
    row= img.shape[0]
    col = img.shape[1]
    tempr0 = 0    #横上
    tempr1 = 0    #横下
    tempc0 = 0    #竖左
    tempc1 = 0    #竖右
    # 765 是255+255+255,如果是黑色背景就是0+0+0，彩色的背景，将765替换成其他颜色的RGB之和，这个会有一点问题，因为三个和相同但颜色不一定同
    for r in range(0, row):
        if thresh1.sum(axis=1)[r] != 255 * col:
            tempr0 = r
            break

    for r in range(row - 1, 0, -1):
        if thresh1.sum(axis=1)[r] != 255 * col:
            tempr1 = r
            break

    for c in range(0, col):
        if thresh1.sum(axis=0)[c] != 255 * row:
            tempc0 = c
            break

    for c in range(col - 1, 0, -1):
        if thresh1.sum(axis=0)[c] != 255 * row:
            tempc1 = c
            break

    # 创建全白图片
    imageTemp = np.zeros((64, 64, 3), dtype=np.uint8)
    imageTemp = cv2.cvtColor(imageTemp, cv2.COLOR_BGR2GRAY)
    imageTemp.fill(255)

    if(tempr1-tempr0==0 or tempc1-tempc0==0):   #空图
        return imageTemp    #返回全白图

    new_img = img[tempr0:tempr1, tempc0:tempc1]
    #二值化
    retval,binary = cv2.threshold(new_img,0,255,cv2.THRESH_OTSU)

    #叠加两幅图像
    rstImg=ImageOverlay(imageTemp, binary)
    return rstImg

#函数功能：简单网格
#函数要求：1.无关图像大小；2.输入图像默认为灰度图;3.参数只有输入图像
#返回数据：64*1维特征
def SimpleGridFeature(image,iD):
    '''
    @description:提取字符图像的简单网格特征
    @image:灰度字符图像
    @return:长度为64字符图像的特征向量feature
    @author:RenHui
    '''

    # new_img=HandWhiteEdges(image)  #白边处理
    # cv2.imwrite("d:/du_py.jpg",new_img)
    #new_img=image
    #图像大小归一化
    image = cv2.resize(image,(64,64))
    img_h = image.shape[0]
    img_w = image.shape[1]

    #二值化
    retval,binary = cv2.threshold(image,0,255,cv2.THRESH_OTSU)
    cv2.imwrite(r"D:\\%d.jpg"%iD,binary)
    # cv2.imshow("62", image)
    # cv2.imshow("66",binary)
    # cv2.waitKey(0)
    #计算网格大小
    grid_size=16
    grid_h = binary.shape[0]/grid_size
    grid_w = binary.shape[1]/grid_size
    #定义特征向量
    feature = np.zeros(grid_size*grid_size)
    for j in range(grid_size):
        for i in range(grid_size):
            grid = binary[int(j*grid_h):int((j+1)*grid_h),int(i*grid_w):int((i+1)*grid_w)]
            feature[j*grid_size+i] = int(grid[grid==0].size)
    return feature

#可以读取带中文路径的图
def cv_imread(file_path,type=0):
    cv_img=cv2.imdecode(np.fromfile(file_path,dtype=np.uint8),-1)
    # print(file_path)
    # print(cv_img.shape)
    # print(len(cv_img.shape))
    if(type==0):
        if(len(cv_img.shape)==3):
            cv_img = cv2.cvtColor(cv_img, cv2.COLOR_BGR2GRAY)
    return cv_img

#遍历文件夹
list = []
def TraverFolders(rootDir):
    for lists in os.listdir(rootDir):
        path = os.path.join(rootDir, lists)
        list.append(path)
        if os.path.isdir(path):
            TraverFolders(path)
    return list

#--------------批量读图------------------------------------------------------------------------------------
#扫描输出图片列表
def eachFile(filepath):
    list = []
    pathDir = os.listdir(filepath)
    for allDir in pathDir:
        child = os.path.join(filepath, allDir)
        list.append(child)
    return list

# path=r"D:\1.jpg"
# image = cv_imread(path,type=0)
# ImgFeatures=[]
# feature = SimpleGridFeature(image)
# print(feature)
# #保存
# fileObject = open("杜.txt", 'w')
# for ip in feature:
#    fileObject.write(str(ip))
#    fileObject.write('\n')

path=r"D:\sxl\处理图片\汉字分类\test2截图"
print("遍历图像中，可能需要花一些时间...")
list=TraverFolders(path)

count1=0
# 读图
ImgFeatures=[]
ImgLabels=[]
time_start=time.time()

strcharName= np.load( "E:\sxl_Programs\Python\存储npy\ImgCharName_23.npy" )
print("共%d个文件" % (len(list)))
count=0
SaveNO=0   #
iNo15ImgNum=0
for filename in list:
    count+=1
    if(count%100==0):
       print("共%d个文件，正在处理第%d个..." % (len(list),count))
    #-----确定子文件夹名称------------------
    word = r'\\'
    a = [m.start() for m in re.finditer(word, filename)]
    if(len(a)==5):   #字文件夹
        # print (len(ImgFeatures))
        iNo15ImgNum = 0
        # 保存
        if(SaveNO>0):
            savePath = r"ImgFeatures23_%s.npy" % (strtemp)
            print(len(ImgFeatures))
            np.save(savePath, ImgFeatures)
            ImgFeatures.clear()

        strtemp=filename[a[-1]+1:]  #文件夹名称-字符名称
        print(strtemp)
    # -----确定子文件夹名称------------------

    # -----子文件夹图片特征提取--------------
    if (len(a) == 6):   #子文件夹下图片
       if('.png'in filename):
          image = cv_imread(filename,0)
          SaveNO += 1
          iNo15ImgNum +=1
          # cv2.imshow("66", image)
          # cv2.waitKey(0)
       else:
           continue
       # 获取特征向量，传入灰度图

       feature = SimpleGridFeature(image,SaveNO)       #简单网格
       ImgFeatures.append(feature)
       if(strtemp=='这'and iNo15ImgNum==8):
           savePath = r"ImgFeatures23_%s.npy" % (strtemp)
           print(len(ImgFeatures))
           np.save(savePath, ImgFeatures)
           ImgFeatures.clear()

       # if(SaveNO==200):
       #     #保存
       #     fileObject = open((r"ImgFeatures_%s.txt" % (strtemp)), 'w')
       #     for ip in ImgFeatures:
       #         # print(str(ip.reshape(-1,256)))
       #         # fileObject.write(str(ip))
       #         # fileObject.write('\n')
       #         # fileObject.write('\n')
       #         for strtemp in ip:
       #             # print(int(strtemp))
       #             fileObject.write(str(int(strtemp))+", ")
       #
       #         fileObject.write('\n')
       #         fileObject.write('\n')
       #     fileObject.close()
       #
       #
       #     # 清空列表
       #     SaveNO=0
       #     ImgFeatures.clear()


       # print(feature)
    # -----子文件夹图片特征提取--------------
time_end=time.time()
time_h=(time_end-time_start)/3600
print('用时：%.6f 小时'% time_h)
print ("运行结束！")