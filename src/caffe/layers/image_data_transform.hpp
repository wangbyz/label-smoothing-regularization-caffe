#ifndef  IMAGE_DATA_TRANSFORM_HPP_2017_11_15
#define  IMAGE_DATA_TRANSFORM_HPP_2017_11_15
#include <opencv2/opencv.hpp>
#include <time.h>
#include <iostream>
#include <stdlib.h>
#include "caffe/data_transformer.hpp"
#include "caffe/util/io.hpp"
#include "caffe/util/math_functions.hpp"
#include "caffe/util/rng.hpp"

int zoomRationInt = 10; // in 100
int maxShitfLen = 3; //in pixel
int maxModThreshHSV[] = {8, 20, 20}; // in 100, for H, S, V
int maxModThreshRGB[] = {10, 10, 10}; // in 100, for B, G, R
int maxRotateAngle = 300; // in 100

inline void getShiftParam(const cv::Mat& inImg, cv::Mat& outImg, int& w, int& h, int& shiftD)
{
	outImg = cv::Mat(inImg.size(), inImg.type(), cv::Scalar(0, 0, 0, 0));
	w = inImg.cols, h = inImg.rows;
	shiftD = rand()%maxShitfLen + 1; //1, ...., maxShiftLen
}


int shiftToLeft(const cv::Mat& inImg, cv::Mat& outImg)
{
	int shiftD, w, h;
	getShiftParam(inImg, outImg, w, h, shiftD);

	int newW = w - 1 - shiftD;
	inImg(cv::Rect(shiftD, 0, newW, h - 1)).copyTo(outImg(cv::Rect(0, 0, newW, h - 1)));
	return 0;
}


int shiftToRight(const cv::Mat& inImg, cv::Mat& outImg)
{
	int shiftD, w, h;
	getShiftParam(inImg, outImg, w, h, shiftD);
	int newW = w - 1 - shiftD;
	inImg(cv::Rect(0, 0, newW, h - 1)).copyTo(outImg(cv::Rect(shiftD, 0, newW, h - 1)));
	return 0;
}

int shiftToTop(const cv::Mat& inImg, cv::Mat& outImg)
{
	int shiftD, w, h;
	getShiftParam(inImg, outImg, w, h, shiftD);
	int newH = h - 1 - shiftD;
	inImg(cv::Rect(0, shiftD, w - 1, newH)).copyTo(outImg(cv::Rect(0, 0, w - 1, newH)));
	return 0;
}

int shiftToBottom(const cv::Mat& inImg, cv::Mat& outImg)
{
	int shiftD, w, h;
	getShiftParam(inImg, outImg, w, h, shiftD);
	int newH = h - 1 - shiftD;
	inImg(cv::Rect(0, 0, w - 1, newH)).copyTo(outImg(cv::Rect(0, shiftD, w - 1, newH)));
	return 0;
}

int zoomImg(const cv::Mat& inImg, cv::Mat& outImg)
{
	outImg = cv::Mat(inImg.size(), inImg.type(), cv::Scalar(0, 0, 0, 0));

	int zoomIn = rand()%zoomRationInt + 1;
	float zoomRatio = (float)zoomIn / 100.f;
	zoomRatio = (rand()%2 == 1) ? zoomRatio : (-1.0 * zoomRatio);
	zoomRatio += 1.0;
	int w = inImg.cols, h = inImg.rows;
	int newW =(float)w * zoomRatio, newH = (float)h * zoomRatio;

	cv::Mat tmpImg;
	cv::resize(inImg, tmpImg, cv::Size(newW, newH));
	
	if(tmpImg.rows > outImg.rows)
	{
		int startX = (tmpImg.cols - outImg.cols) / 2.0;
		int startY = (tmpImg.rows - outImg.rows) / 2.0;
		tmpImg(cv::Rect(startX, startY, w - 1, h - 1)).copyTo(outImg(cv::Rect(0, 0, w - 1, h -1)));
	}
	else
	{
		int startX = (outImg.cols - tmpImg.cols) / 2.0;
		int startY = (outImg.rows - tmpImg.rows) / 2.0;
		tmpImg(cv::Rect(0, 0, tmpImg.cols - 1, tmpImg.rows -1)).copyTo(outImg(cv::Rect(startX, startY, tmpImg.cols - 1, tmpImg.rows - 1)));
	}
	return 0;
}

int modHSV(const cv::Mat& inImg, cv::Mat& outImg)
{
	cv::Mat tmpImg;
	cv::cvtColor(inImg, tmpImg, CV_RGB2HSV);
	std::vector<cv::Mat> HSV;
	cv::split(tmpImg, HSV);

	int modChannelId = rand()%3;
	float modRatio = (float)(rand()%maxModThreshHSV[modChannelId] + 1) / 100.f;
	modRatio = (rand()%2 == 1) ? modRatio : (-modRatio);
	modRatio += 1.0;

	int maxV[] = {180, 255, 255};
	cv::Mat targetMat = HSV[modChannelId];
	int tempV;
	for( int i = 0; i < targetMat.rows; ++i )
	{
		uchar* rowData = targetMat.ptr<uchar>(i);
		for( int j = 0; j < targetMat.cols; ++j )
		{
			tempV = rowData[j] * modRatio;
			tempV = tempV < 0 ? 0 : tempV;
			tempV = tempV > maxV[modChannelId] ? maxV[modChannelId] : tempV;
			rowData[j] = tempV;
		}
	}

	cv::merge(HSV, outImg);
	cv::cvtColor(outImg, outImg, CV_HSV2RGB);
	return 0;
}

int modRGB(const cv::Mat& inImg, cv::Mat& outImg)
{
	cv::Mat tmpImg = inImg.clone();
	std::vector<cv::Mat> RGB;
	cv::split(tmpImg, RGB);

	int modChannelId = rand()%3;
	float modRatio = (float)(rand()%maxModThreshRGB[modChannelId] + 1) / 100.f;
	modRatio = (rand()%2 == 1) ? modRatio : (-modRatio);
	modRatio += 1.0;

	int maxV[] = {255, 255, 255};
	int tempV;
	
	cv::Mat targetMat = RGB[modChannelId];
	for( int i = 0; i < targetMat.rows; ++i )
	{
		uchar* rowData = targetMat.ptr<uchar>(i);
		for( int j = 0; j < targetMat.cols; ++j )
		{
			tempV = rowData[j] * modRatio;
			tempV = tempV < 0 ? 0 : tempV;
			tempV = tempV > maxV[modChannelId] ? maxV[modChannelId] : tempV;
			rowData[j] = tempV;
		}
	}

	cv::merge(RGB, outImg);
	return 0;
}


int rotateImg(const cv::Mat& inImg, cv::Mat& outImg)
{
	cv::Mat tmpImg = inImg.clone();

	int sign = (rand()%2 == 1) ? -1 : 1;
	float rotateAngle = (float)(rand()%maxRotateAngle + 1) / 100.f;
	rotateAngle = (float)sign * rotateAngle;

	cv::Point2f center(tmpImg.cols/2,tmpImg.rows/2);
	cv::Mat matrix = cv::getRotationMatrix2D(center, rotateAngle, 1);	

	cv::warpAffine(tmpImg, outImg, matrix, tmpImg.size());
	return 0;
}

int notChangeImg(const cv::Mat& inImg, cv::Mat& outImg)
{
	outImg = inImg.clone();
	return 0;
}


int JpegCompression(const cv::Mat &img, cv::Mat& cv_img){
    cv_img = img.clone();
    int QF = 100;
    QF = 95 + 1 * (rand()%6);
    int cp[] = {1, QF};
    std::vector<int> compression_params(cp,cp + 2);
    std::vector<unsigned char> img_jpeg;
    cv::imencode(".jpg", cv_img, img_jpeg, compression_params);
    cv::Mat temp = cv::imdecode(img_jpeg, 1);
    temp.copyTo(cv_img);
    return 0;
}
int SmoothFilter(const cv::Mat &img, cv::Mat &cv_img){
      std::random_device rd;
      cv_img = img.clone();
	int smooth_type = rand()%4; // see opencv_util.hpp
	int smooth_param1 = 1 + 2*(rand()%2);
        switch(smooth_type){
        case 0:
	   //cv::Smooth(cv_img, cv_img, smooth_type, smooth_param1);
	   cv::GaussianBlur(cv_img, cv_img, cv::Size(smooth_param1,smooth_param1),0);
           break;
        case 1:
           cv::blur(cv_img, cv_img, cv::Size(smooth_param1,smooth_param1));
           break;
        case 2:
           cv::medianBlur(cv_img, cv_img, smooth_param1);
           break;
        case 3:
           cv::boxFilter(cv_img, cv_img, -1, cv::Size(smooth_param1*2,smooth_param1*2));
           break;
        }
      //printf("smooth_type:%d smooth_param1:%d\n",smooth_type, smooth_param1);
      return 0;
      //cv::imshow("Smooth Filtering", cv_img);
      //cv::waitKey(0);
}


typedef int(*validFun)(const cv::Mat& inImg, cv::Mat& outImg);

validFun allfun[] = {
	notChangeImg, shiftToRight, shiftToLeft, shiftToTop, shiftToBottom, modHSV, modHSV, modHSV, modRGB, modRGB, modRGB, zoomImg, zoomImg, rotateImg, rotateImg,JpegCompression, JpegCompression,  SmoothFilter, SmoothFilter
	};


int imgMangling(cv::Mat& img)
{
	int mangleInt = rand() % 12;
	if(mangleInt == 0) //do not mod img
	{
		return 0;
	}

	return 0;
}

#endif

