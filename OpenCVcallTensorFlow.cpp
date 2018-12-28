// OpenCVcallTensorFlow.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <vector>
#include "windows.h"
#include <time.h>     
#include <stdio.h>  
#include <stdlib.h>
#pragma comment(lib, "winmm.lib ")

using namespace std;

using namespace cv;

//===========全局变量=========================================
vector<string> vector_charName;  //字符名称
clock_t  start, end12;  //统计时间
double timesum = 0;     //统计时间
//===========全局变量=========================================


//===========相关函数======================================================================================================
//===========提取特征=========================================

//统计黑点个数
int bSums(Mat src)
{
	int counter = 0;
	//迭代器访问像素点
	Mat_<uchar>::iterator it = src.begin<uchar>();
	Mat_<uchar>::iterator itend = src.end<uchar>();
	for (; it != itend; ++it)
	{
		if ((*it)==0) counter += 1;//二值化后，像素点是0或者255
	}
	return counter;
}

//叠加两张图片，输入皆是黑白图，img1是底层图片，img2是上层图片，返回叠加后的图片
Mat ImageOverlay(Mat img1, Mat img2)
{
	int h = img1.rows;
	int w = img1.cols;
	int rows = img2.rows;
	int cols = img2.cols;
	Mat imageROI = img1(Range((h - rows) / 2, rows + (h - rows) / 2), Range((w - cols) / 2, cols + (w - cols) / 2));
	//【2】定义一个Mat类型并给其设定ROI区域
	//Mat imageROI = img1(Rect((h - rows) / 2, (w - cols) / 2, img2.cols, img2.rows));

	//【3】加载掩膜
	//加载掩膜
	//Mat mask;
	//img2.copyTo(mask);
	//imshow("665", mask);
	////waitKey(0);

	//【4】将掩膜拷贝到ROI
	img2.copyTo(imageROI);
	//imshow("imageROI", imageROI);

	//【5】显示结果
	//namedWindow("1 利用ROI实现图像叠加示例窗口");
	//imshow("1 利用ROI实现图像叠加示例窗口", img1);
	//waitKey(0);
	imshow("6666", img1);
	waitKey(0);
	return img1;
}

//处理白边
Mat HandWhiteEdges(Mat image)
{
	//二值化
	Mat thresh1;
	threshold(image, thresh1, 249, 255, CV_THRESH_BINARY);

	//OpenCV定义的结构元素
	Mat  kernel = getStructuringElement(MORPH_RECT, Size(2, 2));
	//膨胀图像
	dilate(thresh1, thresh1, kernel);

	int row = image.rows;
	int col = image.cols;
	int tempr0 = 0;   //横上
	int tempr1 = 0;   //横下
	int tempc0 = 0;   //竖左
	int tempc1 = 0;   //竖右

	for (int r = 0; r < row;r++) //从上到下
	{
		int sum_col = 0;
		for (int c = 0; c < col; c++)
		{
			sum_col += thresh1.at<uchar>(r, c);
		}
		if (sum_col != 255 * col)
		{
			tempr0 = r;
			break;
		}
	}

	for (int r = row-1; r >= 0; r--) //从下到上
	{
		int sum_col = 0;
		for (int c = 0; c < col; c++)
		{
			sum_col += thresh1.at<uchar>(r, c);
		}
		if (sum_col != 255 * col)
		{
			tempr1 = r;
			break;
		}
	}

	for (int c= 0; c < col; c++) //从左到右
	{
		int sum_row = 0;
		for (int r = 0; r < row; r++)
		{
			sum_row += thresh1.at<uchar>(r, c);
		}
		if (sum_row != 255 * row)
		{
			tempc0 = c;
			break;
		}
	}

	for (int c = col-1; c >=0; c--) //从左到右
	{
		int sum_row = 0;
		for (int r = 0; r < row; r++)
		{
			sum_row += thresh1.at<uchar>(r, c);
		}
		if (sum_row != 255 * row)
		{
			tempc1 = c;
			break;
		}
	}

	//创建全白图片
	Mat imageTemp(64, 64, CV_8UC1, Scalar(255));
	//imshow("66", imageTemp);
	//waitKey(0);
	if (tempr1 - tempr0 == 0 || tempc1 - tempc0 == 0) //空白图
	{
		return imageTemp;
	}

	Mat imageROI = image(Range(tempr0,tempr1), Range(tempc0,tempc1));
	//imshow("66", imageROI);
	Mat rstImg = ImageOverlay(imageTemp, imageROI); //叠加图像
	//二值化
	Mat binary2;
	threshold(rstImg, binary2, 0, 255, CV_THRESH_OTSU);
	
	return binary2;
}

//提取特征
Mat SimpleGridFeature(Mat image)
{
	Mat new_img = HandWhiteEdges(image);  //白边处理
	//imwrite("D:\\du_cpp2.jpg", new_img);
	//imshow("22", new_img);
	//waitKey(0);

	resize(new_img, image, Size(64, 64));
	int img_h = image.rows;
	int img_w = image.cols;

	//二值化
	Mat binary;
	threshold(image, binary,0, 255.0, CV_THRESH_OTSU);
	//imshow("66", binary);
	//waitKey();

	//计算网格大小
	int grid_size = 16;
	int grid_h = img_h / grid_size;
	int grid_w = img_w / grid_size;

	//定义特征向量
	Mat img_feature(1, grid_size*grid_size, CV_32FC1, Scalar(0));
	for (int h = 0; h < grid_size; h++)
	{
		for (int w = 0; w < grid_size; w++)
		{
			Mat imageROI = binary(Range(h*grid_h,(h+1)*grid_h), Range(w*grid_w, (w+1)*grid_w));
			//imshow("66", imageROI);
			//waitKey(0);
			int a = bSums(imageROI);
			img_feature.at<float>(0, h*grid_size + w)=bSums(imageROI);
		}
	}
	return img_feature;
}
//===========提取特征=========================================

//排序          数组， 数组长度 ，下标
void Mysort(Mat a, int length, int* b)
{
	float t;
	int i, j, t1;
	for (j = 0; j<length; j++)
		for (i = 0; i<length - 1 - j; i++)
			if (a.at<float>(0, i)<a.at<float>(0, i+1))
			{
				t = a.at<float>(0, i);
				a.at<float>(0, i) = a.at<float>(0, i + 1);
				a.at<float>(0, i + 1) = t;

				t1 = b[i];
				b[i] = b[i + 1];
				b[i + 1] = t1;
			}
}

//读取txt文档
void readTxt(string file)
{
	ifstream myfile(file);

	string temp;
	if (!myfile.is_open())
	{
		cout << "未成功打开文件" << endl;
	}
	while (getline(myfile, temp))
	{
		vector_charName.push_back(temp);
		//cout << temp << endl;;
	}
	myfile.close();
}

// LPCWSTR转string
std::string WChar2Ansi(LPCWSTR pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);

	if (nLen <= 0) return std::string("");

	char* pszDst = new char[nLen];
	if (NULL == pszDst) return std::string("");

	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen - 1] = 0;

	std::string strTemp(pszDst);
	delete[] pszDst;

	return strTemp;
}

// 利用winWIN32_FIND_DATA读取文件下的文件名
void readImgNamefromFile(char* fileName, vector <string> &imgNames)
{
	// vector清零 参数设置
	imgNames.clear();
	WIN32_FIND_DATA file;
	int i = 0;
	char tempFilePath[MAX_PATH + 1];
	char tempFileName[50];
	// 转换输入文件名
	sprintf_s(tempFilePath, "%s/*", fileName);
	// 多字节转换
	WCHAR   wstr[MAX_PATH] = { 0 };
	MultiByteToWideChar(CP_ACP, 0, tempFilePath, -1, wstr, sizeof(wstr));
	// 查找该文件待操作文件的相关属性读取到WIN32_FIND_DATA
	HANDLE handle = FindFirstFile(wstr, &file);
	if (handle != INVALID_HANDLE_VALUE)
	{
		FindNextFile(handle, &file);
		FindNextFile(handle, &file);
		// 循环遍历得到文件夹的所有文件名    
		do
		{
			sprintf_s(tempFileName, "%s", fileName);
			imgNames.push_back(WChar2Ansi(file.cFileName));
			imgNames[i].insert(0, tempFileName);
			i++;
		} while (FindNextFile(handle, &file));
	}
	FindClose(handle);
}

//从Mat图像到二进制文件
bool ImageToStreamFile(cv::Mat image, string filename)
{
	if (image.empty()) return false;
	const char *filenamechar = filename.c_str();
	FILE *fpw = fopen(filenamechar, "wb");//如果没有则创建，如果存在则从头开始写
	if (fpw == NULL)
	{
		fclose(fpw);
		return false;
	}
	int channl = image.channels();//第一个字节  通道
	int rows = image.rows;     //四个字节存 行数
	int cols = image.cols;   //四个字节存 列数

	fwrite(&channl, sizeof(char), 1, fpw);
	fwrite(&rows, sizeof(char), 4, fpw);
	fwrite(&cols, sizeof(char), 4, fpw);
	char* dp = (char*)image.data;
	if (channl == 3)
	{
		for (int i = 0; i < rows*cols; i++)
		{
			fwrite(&dp[i * 3], sizeof(char), 1, fpw);
			fwrite(&dp[i * 3 + 1], sizeof(char), 1, fpw);
			fwrite(&dp[i * 3 + 2], sizeof(char), 1, fpw);
		}
	}
	else if (channl == 1)
	{
		for (int i = 0; i < rows*cols; i++)
		{
			fwrite(&dp[i], sizeof(char), 1, fpw);
		}
	}
	fclose(fpw);
	return true;
}

//从二进制文件到Mat
bool StreamFileToImage(std::string filename, cv::Mat &image)
{
	const char *filenamechar = filename.c_str();
	FILE *fpr = fopen(filenamechar, "rb");
	if (fpr == NULL)
	{
		fclose(fpr);
		return false;
	}
	int channl(0);
	int imagerows(0);
	int imagecols(0);
	fread(&channl, sizeof(char), 1, fpr);//第一个字节 通道
	fread(&imagerows, sizeof(char), 4, fpr); //四个字节存 行数
	fread(&imagecols, sizeof(char), 4, fpr); //四个字节存 列数
	if (channl == 3)
	{
		image = Mat::zeros(imagerows, imagecols, CV_8UC3);
		char* pData = (char*)image.data;
		for (int i = 0; i < imagerows*imagecols; i++)
		{
			fread(&pData[i * 3], sizeof(char), 1, fpr);
			fread(&pData[i * 3 + 1], sizeof(char), 1, fpr);
			fread(&pData[i * 3 + 2], sizeof(char), 1, fpr);
		}
	}
	else if (channl == 1)
	{
		image = Mat::zeros(imagerows, imagecols, CV_8UC1);
		char* pData = (char*)image.data;
		for (int i = 0; i < imagerows*imagecols; i++)
		{
			fread(&pData[i], sizeof(char), 1, fpr);
		}
	}
	fclose(fpr);
	return true;
}
//===========相关函数======================================================================================================


//预测
void  MyPrediction(string filename, String pbPath, String txtpath)
{
	Mat img;
	StreamFileToImage(filename, img);  //通过二进制文件读图

	dnn::Net net = cv::dnn::readNetFromTensorflow(pbPath); //读取TensorFlow模型

	readTxt(txtpath);  //读取字符标签

	Mat Img_feature = SimpleGridFeature(img); //提取特征

	Mat inputBlob = dnn::blobFromImage(Img_feature, 1.0, Size(256, 1), Scalar(), true, false); //转化格式

	net.setInput(inputBlob);  //输入数据

	cv::Mat pred = net.forward();//网络前向传播

	int len = 653;
	int idstID[653];
	for (int i = 0; i < len; i++) //重新初始化下标数组
	{
		idstID[i] = i;
	}

	Mysort(pred, len, idstID); //排序

	//===========输出top5=========================
	for (int i = 0; i < 5; i++)
	{
		float ff = pred.at<float>(0, i);
		cout << "预测结果top" << i + 1 << ":\t" << vector_charName[idstID[i]] << "，\t 概率：" << ff << endl;
		int a = 0;
	}
	cout << endl;
	//===========输出top5=========================
}

//主函数
int main()
{
	//String pbPath = "D:\\sxl\\VisualStudio\\vs2015programs\\OpenCVcallTensorFlow\\x64\\Debug\\1.pb";  //模型路径
	String pbPath = "D:\\20180913_squ_1.pb";  //模型路径
	
	String txtpath = "D:\\sxl\\VisualStudio\\vs2015programs\\OpenCVcallTensorFlow\\x64\\Debug\\ImgHanZiName653.txt";  //根据下标找到对应字符名称
	
	Mat src = imread("D:\\1.jpg",0);         //读图

	string filename = "test";                 //二进制流文件名

	ImageToStreamFile(src, filename);         //图像转换成二进制

	MyPrediction(filename, pbPath, txtpath);  //传入二进制文件路径， pb模型路径，字符名称路径进行预测

	system("pause");

	return 0;

}

//int main()
//{
//	String weights = "D:\\sxl\\VisualStudio\\vs2015programs\\OpenCVcallTensorFlow\\x64\\Debug\\OCRsoftmax.pb";
//	dnn::Net net = cv::dnn::readNetFromTensorflow(weights);
//	String txtpath = "D:\\sxl\\VisualStudio\\vs2015programs\\OpenCVcallTensorFlow\\x64\\Debug\\ImgHanZiName653.txt";
//	readTxt(txtpath);  //字符标签
//	int len = 653;
//	int idstID[653];
//	for (int i = 0; i < len; i++) //初始化下标数组
//	{
//		idstID[i] = i;
//	}
//	=======遍历图像=============================
//	 设置读入图像序列文件夹的路径
//	char* fileName = "D:\\sxl\\处理图片\\汉字分类\\train653\\土\\";
//	std::vector <string>  imgNames;
//	 获取对应文件夹下所有文件名
//	readImgNamefromFile(fileName, imgNames);
//	 遍历对应文件夹下所有文件名
//	cout << imgNames.size() <<endl;
//	for (int i = 0; i < imgNames.size(); i++)
//	{
//		cv::Mat img = cv::imread(imgNames[i],0);
//		if (!img.data)
//		{
//			cout << "非图片类型！";
//			continue;
//		}
//		
//		===========可添加图像处理算法code========================
//		start = clock();
//		Mat img = imread("D:\\1.jpg", 0);
//		Mat Img_feature = SimpleGridFeature(img);
//
//		Mat inputBlob = dnn::blobFromImage(Img_feature, 1.0, Size(256, 1), Scalar(), true, false);
//		net.setInput(inputBlob);
//		cv::Mat pred = net.forward();//网络前向传播
//
//		for (int i = 0; i < len; i++) //初始化下标数组
//		{
//			idstID[i] = i;
//		}
//		Mysort(pred, len, idstID); //排序
//
//		===========输出top5=========================
//		for (int i = 0; i < 5; i++)
//		{
//			float ff=pred.at<float>(0, i);
//			cout <<"预测结果top"<<i+1<<":\t"<< vector_charName[idstID[i]]<< "，\t 概率：" << ff << endl;
//			int a = 0;
//		}
//		cout << endl;
//		===========输出top5=========================
//
//
//		===========统计用时=========================
//		end12 = clock();
//		double f = (end12-start);
//		timesum += f;
//		===========统计用时=========================
//		double minVal; double maxVal; Point minLoc; Point maxLoc;
//		minMaxLoc(pred, &minVal, &maxVal, &minLoc, &maxLoc);
//		//cout << minVal << " " << maxVal << " " << minLoc.x << " " << maxLoc.x << endl;
//		if (i % 1 == 0)
//		{
//			cout << i << ":" << vector_charName[maxLoc.x] << "\t";
//			cout <<endl;
//		}
//		===========可添加图像处理算法code========================
//	}
//	=======遍历图像=============================
//
//	t2 = GetTickCount();
//	printf("\n总用时:%f 毫秒\n", timesum);
//	printf("\n平均用时:%f 毫秒\n", timesum / imgNames.size());
//	system("pause");
//	return 0;
//}


//图像二进制转化
//int main()
//{
//	Mat src = imread("D:\\21.jpg");
//	string filename = "test";
//	ImageToStreamFile(src, filename);
//	Mat dst;
//	StreamFileToImage(filename, dst);
//	imshow("dst", dst);
//	waitKey(0);
//
//}

////排序
//int main()
//{
//	float a[] = { 5,7,3,6,4,1,2 };
//	int b[7] = { 0,1,2,3,4,5,6 };//用来保存原数组对应的下标以及排序后的下标
//	Mysort(a, 7, b);
//	printf("排序后的数组为：\n");
//	for (int i = 0; i<7; i++)
//		printf("%f ", a[i]);
//	printf("\n");
//
//	printf("排序后的数组对应的下标：\n");
//	for (int i = 0; i<7; i++)
//		printf("%d ", b[i]);
//	printf("\n");
//	system("pause");
//	return 0;
//}

//int main()
//{
//	String weights = "D:\\sxl\\VisualStudio\\vs2015programs\\OpenCVcallTensorFlow\\x64\\Debug\\expert-graph_t1.pb";
//	//String prototxt = "D:\\sxl\\VisualStudio\\vs2015programs\\OpenCVcallTensorFlow\\x64\\Debug\\OCR.pbtxt";
//	dnn::Net net = cv::dnn::readNetFromTensorflow(weights);
//	Mat img = imread("D:\\3.png", 0);
//	Mat inputBlob = dnn::blobFromImage(img, 1.0 / 255.0, Size(28, 28), Scalar(), false, false);
//	net.setInput(inputBlob);//set the network input, "data" is the name of the input layer     
//	cv::Mat pred = net.forward();//网络前向传播
//	for (int row = 0; row < pred.rows; row++)
//	{
//		for (int col = 0; col < pred.cols; col++)
//		{
//			float fa = pred.at<float>(row, col);
//			int a = 0;
//		}
//	}
//	waitKey();
//	return 0;
//}
//
//bool ROI_AddImage()
//{
//
//	//【1】读入图像
//	Mat srcImage1 = imread("D:\\661_2.jpg", 0);
//	Mat logoImage = imread("D:\\imageROI.jpg", 0);
//	////创建全白图片
//	//Mat imageTemp(logoImage.rows, logoImage.cols, CV_8UC1, Scalar(255));
//	//bitwise_and(logoImage, imageTemp, logoImage);
//	//imshow("22", logoImage);
//	//imwrite("D:\\661_1.jpg", srcImage1);
//	//imwrite("D:\\662_1.jpg", logoImage);
//	if (!srcImage1.data)
//	{
//		printf("fuck, read the picture is wrong!!! \n");
//		return false;
//	}
//
//	if (!logoImage.data)
//	{
//		printf("fuck, read the picture is wrong!!! \n");
//		return false;
//	}
//
//	//【2】定义一个Mat类型并给其设定ROI区域
//	Mat imageROI = srcImage1(Rect(0, 0, logoImage.cols, logoImage.rows));
//
//	//【3】加载掩膜
//	//Mat mask = imread("D:\\21_1.jpg", 0);
//	//加载掩膜
//	Mat mask;
//	logoImage.copyTo(mask);
//
//	//【4】将掩膜拷贝到ROI
//	logoImage.copyTo(imageROI, mask);
//	//【5】显示结果
//	namedWindow("1 利用ROI实现图像叠加示例窗口");
//	imshow("1 利用ROI实现图像叠加示例窗口", srcImage1);
//	waitKey(0);
//	return true;
//}
//
//int main()
//{
//	bool b=ROI_AddImage();
//}

//==================读取python提取的特征====================
//   fstream infile;   //输入流
//int val=0;
//int icount = 0;
//char buffer[10];
//infile.open("E:\\sxl_Programs\\Python\\存储npy\\国.txt", ios::in);
//if (!infile.is_open())
//	cout << "Open file failure" << endl;
//while (icount <= 255)            // 若未到文件结束一直循环
//{
//	infile.getline(buffer, 10, '\n');//getline(char *,int,char) 表示该行字符达到256个或遇到换行就结束
//	cout << buffer << endl;
//	int ff= atof(buffer);
//	Img_feature.at<float>(0, icount)= atof(buffer);
//	icount++;
//}
//==================读取python提取的特征====================


