#pragma once
#include <gdal.h>
#include <gdal_priv.h>
#include <memory.h>

#ifndef NULL
#define NULL 0
#endif // !NULL


class SxTiffRead{
public:
	SxTiffRead(const char* fn);
	SxTiffRead();
	~SxTiffRead();

private:
	char* fileName = NULL; //tiff·��
	GDALDataset* DataSet;//�����ļ�ָ��
	int m_width;//���س���
	int m_height;//���ظ߶�
	int bandNum;//������

	double geoTransform[6];//�洢һЩ��׼��������ÿһ��pixel��ʾ�ĳ��ȵ�,0����x��3����y��1���򳤶ȣ�5���򳤶�2��4������
	GDALDataType rasterDataType;//դ����������
	GDALDataType depth;//��Ԫ���

	double* m_max;//ÿ���������ֵ
	double* m_min;//ÿ��������Сֵ
	double* m_mean;//ÿ������ƽ��ֵ
	double* m_stdErr;//ÿ�����η���


	std::vector<void*>imgData;//�����洢ͼ���ļ�

public:
	bool InitData();
	void FromLngLat2XY(double lng, double lat, int& x, int& y);
	void FromXY2LngLat(int x, int y, double& lng, double& lat);

	void* GetImgData(int x);
	int GetWidth();
	int GetHeight();
	int GetBandNum();
	GDALDataType GetDataType();
	double GetTransform(int x);
	double GetMax(int x);
	double GetMin(int x);
	double GetMean(int x);
	double GetStdErr(int x);
};

//��ע��GDALͼ���������Ͷ�Ӧ
/*
GDT_Unknown : δ֪��������
GDT_Byte : 8bit������ (unsigned char)
GDT_UInt16 : 16bit������ ( unsigned short)
GDT_Int16 : 16bit���� ( short �� short int)
GDT_UInt32 : 32bit ������ (unsigned long)
GDT_Int32 : 32bit���� (int �� long �� long int)
GDT_Float32 : 32bit ������ (float)
GDT_Float64 : 64bit ������ (double)
GDT_CInt16 : 16bit������
GDT_CInt32 : 32bit������
GDT_CFloat32 : 32bit��������
GDT_CFloat64 : 64bit��������
*/
