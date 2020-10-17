#include "SxAffine.h"
#include <math.h>
#include <QProgressDialog>

/*|x'| |a b c| |x|
  |y'|=|d e f|*|y|
  |1 | |0 0 1| |1|   x^T*x*h=x^T*x'    */

SxAffine::SxAffine() {
}

SxAffine::~SxAffine() {
}

void SxAffine::show()
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			std::cout << h[i][j] << "\n";
}

//利用x^T*x*h=x^T*x'
void SxAffine::Init(double* pixelXArray, double* pixelYArray, double* geoLngArray, double* geoLatArray, int count) {
	for (int i = 0; i < 4; i++) {
		oldXArray[i] = pixelXArray[i];
		oldYArray[i] = pixelYArray[i];
		oldLngArray[i] = geoLngArray[i];
		oldLatArray[i] = geoLatArray[i];
	}
	oldWidth = pixelXArray[1] - pixelXArray[0] + 1;
	oldHeight = pixelYArray[2] - pixelYArray[1] + 1;
	double MaxL[3][3] = { 0,0,0,0,0,0,0,0,count };//方程左边的系数矩阵即是（x^T*x)
	double MaxRX[3] = { 0,0,0 };//方程右边常数即是（x^T*x'）
	double MaxRY[3] = { 0,0,0 };
	for (int i = 0; i < count; i++) {
		MaxL[0][0] += pow(pixelXArray[i], 2);
		MaxL[0][1] += pixelXArray[i] * pixelYArray[i];
		MaxL[0][2] += pixelXArray[i];
		MaxL[1][0] += pixelXArray[i] * pixelYArray[i];
		MaxL[1][1] += pow(pixelYArray[i], 2);
		MaxL[1][2] += pixelYArray[i];
		MaxL[2][0] += pixelXArray[i];
		MaxL[2][1] += pixelYArray[i];

		MaxRX[0] += pixelXArray[i] * geoLngArray[i];
		MaxRX[1] += pixelYArray[i] * geoLngArray[i];
		MaxRX[2] += geoLngArray[i];
		MaxRY[0] += pixelXArray[i] * geoLatArray[i];
		MaxRY[1] += pixelYArray[i] * geoLatArray[i];
		MaxRY[2] += geoLatArray[i];
	}
	//利用克拉默法则
	double D = MaxL[0][0] * (MaxL[1][1] * MaxL[2][2] - MaxL[2][1] * MaxL[1][2])
		- MaxL[0][1] * (MaxL[1][0] * MaxL[2][2] - MaxL[2][0] * MaxL[1][2])
		+ MaxL[0][2] * (MaxL[1][0] * MaxL[2][1] - MaxL[2][0] * MaxL[1][1]);
	double Da = MaxRX[0] * (MaxL[1][1] * MaxL[2][2] - MaxL[2][1] * MaxL[1][2])
		- MaxL[0][1] * (MaxRX[1] * MaxL[2][2] - MaxRX[2] * MaxL[1][2])
		+ MaxL[0][2] * (MaxRX[1] * MaxL[2][1] - MaxRX[2] * MaxL[1][1]),
		Db = MaxL[0][0] * (MaxRX[1] * MaxL[2][2] - MaxRX[2] * MaxL[1][2])
		- MaxRX[0] * (MaxL[1][0] * MaxL[2][2] - MaxL[2][0] * MaxL[1][2])
		+ MaxL[0][2] * (MaxL[1][0] * MaxRX[2] - MaxL[2][0] * MaxRX[1]),
		Dc = MaxL[0][0] * (MaxL[1][1] * MaxRX[2] - MaxL[2][1] * MaxRX[1])
		- MaxL[0][1] * (MaxL[1][0] * MaxRX[2] - MaxL[2][0] * MaxRX[1])
		+ MaxRX[0] * (MaxL[1][0] * MaxL[2][1] - MaxL[2][0] * MaxL[1][1]),
		Dd = MaxRY[0] * (MaxL[1][1] * MaxL[2][2] - MaxL[2][1] * MaxL[1][2])
		- MaxL[0][1] * (MaxRY[1] * MaxL[2][2] - MaxRY[2] * MaxL[1][2])
		+ MaxL[0][2] * (MaxRY[1] * MaxL[2][1] - MaxRY[2] * MaxL[1][1]),
		De = MaxL[0][0] * (MaxRY[1] * MaxL[2][2] - MaxRY[2] * MaxL[1][2])
		- MaxRY[0] * (MaxL[1][0] * MaxL[2][2] - MaxL[2][0] * MaxL[1][2])
		+ MaxL[0][2] * (MaxL[1][0] * MaxRY[2] - MaxL[2][0] * MaxRY[1]),
		Df = MaxL[0][0] * (MaxL[1][1] * MaxRY[2] - MaxL[2][1] * MaxRY[1])
		- MaxL[0][1] * (MaxL[1][0] * MaxRY[2] - MaxL[2][0] * MaxRY[1])
		+ MaxRY[0] * (MaxL[1][0] * MaxL[2][1] - MaxL[2][0] * MaxL[1][1]);
	//计算仿射变换矩阵
	h[0][0] = Da / D;
	h[0][1] = Db / D;
	h[0][2] = Dc / D;
	h[1][0] = Dd / D;
	h[1][1] = De / D;
	h[1][2] = Df / D;
	h[2][0] = 0;
	h[2][1] = 0;
	h[2][2] = 1;
	//对于一开始使用的四个经纬坐标进行校正
	for (int i = 0; i < 4; i++) {
		getValue(oldXArray[i], oldYArray[i], newLngArray[i], newLatArray[i]);
	}
	//计算仿射之后新的外界矩形的边长
	newWidth = oldWidth * (newLngArray[1] - newLngArray[3]) /
		(newLngArray[1] - newLngArray[0]);
	newHeight = oldHeight * (newLatArray[0] - newLatArray[2]) /
		(newLatArray[1] - newLatArray[2]);
}

void SxAffine::getValue(double x, double y, double& lng, double& lat) {
	lng = h[0][0] * x + h[0][1] * y + h[0][2];
	lat = h[1][0] * x + h[1][1] * y + h[1][2];
}

double SxAffine::getOldLng(int i)
{
	return oldLngArray[i];
}

double SxAffine::getOldLat(int i)
{
	return oldLatArray[i];
}

double SxAffine::getNewLng(int i)
{
	return newLngArray[i];
}

double SxAffine::getNewLat(int i)
{
	return newLatArray[i];
}

double SxAffine::getH(int i, int j)
{
	return h[i][j];
}

int SxAffine::getNewWidth()
{
	return newWidth;
}

int SxAffine::getNewHeight()
{
	return newHeight;
}

int SxAffine::getOldWidth()
{
	return oldWidth;
}

int SxAffine::getOldHeight()
{
	return oldHeight;
}

void SxAffine::forwardProjection(unsigned char* recImgDat, unsigned char* oirImgDat)
{
	//for (int i = 0; i < oldHeight * oldWidth; i++)
	//	recImgDat[i] = 0;
	////通过原图投影到目标图
	//for (int i = 0; i < oldHeight * oldWidth; i++) {
	//	double tmpLng, tmpLag;
	//	getValue(i - (i / 6054) * 6054, i / 6054, tmpLng, tmpLag);
	//	int trueX, trueY;
	//	trueX = (tmpLng - getNewLng(3)) * oldWidth / (getOldLng(1) - getOldLng(3));
	//	trueY = oldHeight - ((tmpLag - getNewLat(2)) * oldHeight / (getOldLat(0) - getOldLat(2)));
	//	if (trueY >= oldHeight)trueY = oldHeight - 1;//有时候会出现个别超出边界
	//	recImgDat[trueX + trueY * 6054] = oirImgDat[i];
	//}
}

void SxAffine::backwardProjection(vector< unsigned char*> oirImgDat,
	unsigned char* recImgDat, int* bandChose)
{
	//从目标图方向投影到原图
	for (int i = 0; i < newHeight * newWidth; i++) {

		double tmpLng, tmpLat;//每个像素对应的经纬坐标
		tmpLng = newLngArray[3] + (newLngArray[1] -
			newLngArray[3]) * (i - (i / newWidth) * newWidth) / (newWidth - 1);
		tmpLat = newLatArray[0] + (newLatArray[2] -
			newLatArray[0]) * (i / newWidth) / (newHeight - 1);

		double D = h[0][0] * h[1][1] -
			h[0][1] * h[1][0];
		double Dx = (tmpLng - h[0][2]) * h[1][1] -
			(tmpLat - h[1][2]) * h[0][1],
			Dy = -(tmpLng - h[0][2]) * h[1][0] +
			(tmpLat - h[1][2]) * h[0][0];

		int oriX, oriY;//每个像素再原图上面对应的坐标
		oriX = Dx / D, oriY = Dy / D;

		if (oriX < 0 || oriX >= oldWidth || oriY < 0 || oriY >= oldHeight) {
			for (int j = 0; j < 3; j++)
				recImgDat[3 * i + j] = 0;
		}
		else { 
			for (int j = 0; j < 3; j++)
				recImgDat[3 * i + j] =
				oirImgDat[bandChose[j]][oriX + oriY * oldWidth]; 
		}
	}
}


