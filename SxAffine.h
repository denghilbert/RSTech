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
	double h[3][3];//�任����
	//˳�������˳ʱ��,�ĸ���
	double oldXArray[4], oldYArray[4], oldLngArray[4], oldLatArray[4];//����ǰ
	double newXArray[4], newYArray[4], newLngArray[4], newLatArray[4];//������
	int oldWidth, oldHeight, newWidth, newHeight;
public:
	void show();//��ʾ��������
	//��ʼ�����ݣ��������ĸ�����ֱ��������ڼ���6�����ɶ�
	void Init(double* pixelXArray, double* pixelYArray,
		double* geoLngArray, double* geoLatArray, int count);
	//����ת��
	void getValue(double x, double y, double& lng, double& lat);
	//��ȡ�ĸ���������Ͼ�γ����
	double getOldLng(int i);
	double getOldLat(int i);
	double getNewLng(int i);
	double getNewLat(int i);
	double getH(int i, int j);//��ȡ�任ϵ��
	//��ȡͼ��������ر߳�
	int getNewWidth();
	int getNewHeight();
	int getOldWidth();
	int getOldHeight();
	//��ԭͼͶӰ��Ŀ��ͼ,һ�δ���һ�����ε�
	void forwardProjection(unsigned char* recImgDat, unsigned char* oirImgDat);
	//��Ŀ��ͶӰ��ԭͼ,һ�δ���һ�����ε�
	void backwardProjection(vector< unsigned char*> oirImgDat,
		unsigned char* recImgDat, int* bandChose);
};

