#pragma once
#include <iostream>
#include <vector>
using namespace std;

/*|x'| |a b c| |x|
  |y'|=|d e f|*|y|
  |1 | |0 0 1| |1| */

class SxAffine {
public:
	SxAffine();
	~SxAffine();
private:
	double h[3][3];//变换矩阵
	//顺序从左上顺时针,四个角
	double oldXArray[4], oldYArray[4], oldLngArray[4], oldLatArray[4];//修正前
	double newXArray[4], newYArray[4], newLngArray[4], newLatArray[4];//修正后
	int oldWidth, oldHeight, newWidth, newHeight;
public:
	void show();//显示基本数据
	//初始化数据，数组存放四个顶点分别坐标用于计算6个自由度
	void Init(double* pixelXArray, double* pixelYArray,
		double* geoLngArray, double* geoLatArray, int count);
	//坐标转换
	void getValue(double x, double y, double& lng, double& lat);
	//获取四个顶点的新老经纬坐标
	double getOldLng(int i);
	double getOldLat(int i);
	double getNewLng(int i);
	double getNewLat(int i);
	double getH(int i, int j);//获取变换系数
	//获取图像矩形像素边长
	int getNewWidth();
	int getNewHeight();
	int getOldWidth();
	int getOldHeight();
	//从原图投影到目标图,一次处理一个波段的
	void forwardProjection(unsigned char* recImgDat, unsigned char* oirImgDat);
	//从目标投影到原图,一次处理一个波段的
	void backwardProjection(vector< unsigned char*> oirImgDat,
		unsigned char* recImgDat, int* bandChose);
};

