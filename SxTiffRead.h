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
	char* fileName = NULL; //tiff路径
	GDALDataset* DataSet;//数据文件指针
	int m_width;//像素长度
	int m_height;//像素高度
	int bandNum;//波段数

	double geoTransform[6];//存储一些基准点的坐标和每一个pixel表示的长度等,0左上x，3左上y，1横向长度，5纵向长度2，4是旋度
	GDALDataType rasterDataType;//栅格数据类型
	GDALDataType depth;//像元深度

	double* m_max;//每个波段最大值
	double* m_min;//每个波段最小值
	double* m_mean;//每个波段平均值
	double* m_stdErr;//每个波段方差


	std::vector<void*>imgData;//用来存储图像文件

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

//备注：GDAL图像数据类型对应
/*
GDT_Unknown : 未知数据类型
GDT_Byte : 8bit正整型 (unsigned char)
GDT_UInt16 : 16bit正整型 ( unsigned short)
GDT_Int16 : 16bit整型 ( short 或 short int)
GDT_UInt32 : 32bit 正整型 (unsigned long)
GDT_Int32 : 32bit整型 (int 或 long 或 long int)
GDT_Float32 : 32bit 浮点型 (float)
GDT_Float64 : 64bit 浮点型 (double)
GDT_CInt16 : 16bit复整型
GDT_CInt32 : 32bit复整型
GDT_CFloat32 : 32bit复浮点型
GDT_CFloat64 : 64bit复浮点型
*/
