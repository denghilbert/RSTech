#include "SxTiffRead.h"
#include <QDebug>


SxTiffRead::SxTiffRead(const char* fn)
{
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	GDALAllRegister();
	fileName = new char[strlen(fn) + 1];//给path分配空间
	strcpy(fileName, fn);//把路径给path
	DataSet = (GDALDataset*)GDALOpen(fileName, GA_ReadOnly);//打开文件数据

}

SxTiffRead::SxTiffRead()
{
}

SxTiffRead::~SxTiffRead()
{
	if (m_max != nullptr)
		delete[] m_max;
	if (m_min != nullptr)
		delete[] m_min;
	if (m_mean != nullptr)
		delete[] m_mean;
	if (m_stdErr != nullptr)
		delete[] m_stdErr;
	if (fileName != nullptr)
		delete[] fileName;//清空路径
	for (int i = 0; i < imgData.size(); i++)
		delete[]imgData[i];

	GDALClose(DataSet);//关闭读取文件数据指针
}

bool SxTiffRead::InitData()
{
	this->m_width = DataSet->GetRasterXSize();
	this->m_height = DataSet->GetRasterYSize();
	this->bandNum = DataSet->GetRasterCount();
	this->rasterDataType = DataSet->GetRasterBand(1)->GetRasterDataType();
	this->depth = GDALDataType(DataSet->GetRasterBand(1)->GetRasterDataType());
	DataSet->GetGeoTransform(geoTransform);

	this->m_max = new double[bandNum];
	this->m_min = new double[bandNum];
	this->m_mean = new double[bandNum];
	this->m_stdErr = new double[bandNum];
	for (int i = 0; i < bandNum; i++)
		DataSet->GetRasterBand(i + 1)->ComputeStatistics(false, &m_min[i],
			&m_max[i], &m_mean[i], &m_stdErr[i], NULL, NULL);

	void* tmp = nullptr;

	switch (rasterDataType) {
	case GDT_Byte: tmp = new unsigned char[m_width * m_height];
		break;
	case GDT_UInt16: tmp = new unsigned short[m_width * m_height];
		break;
	case GDT_Int16: tmp = new short[m_width * m_height];
		break;
	case GDT_UInt32: tmp = new unsigned long[m_width * m_height];
		break;
	case GDT_Int32: tmp = new int[m_width * m_height];
		break;
	case GDT_Float32: tmp = new float[m_width * m_height];
		break;
	case GDT_Float64:tmp = new double[m_width * m_height];
		break;
	case GDT_CInt16:
		break;
	case GDT_CInt32:
		break;
	case GDT_CFloat32:
		break;
	case GDT_CFloat64:
		break;
	default:
		break;
	}

	GDALRasterBand* raster = NULL;
	CPLErr error;
	for (int i = 0; i < bandNum; i++) {
		raster = DataSet->GetRasterBand(i + 1);
		error = raster->RasterIO(GF_Read, 0, 0, m_width, m_height, tmp,
			m_width, m_height, rasterDataType, 0, 0);
		if (error == CE_Failure) {
			qDebug() << "读取失败";
			GDALDestroyDriverManager();
		}
		imgData.push_back(tmp);
	}

	return true;
}

void SxTiffRead::FromLngLat2XY(double lng, double lat, int& x, int& y)
{
	x = (lng - geoTransform[0]) / (geoTransform[1] + geoTransform[2]);
	y = (lat - geoTransform[3]) / (geoTransform[4] + geoTransform[5]);
}

void SxTiffRead::FromXY2LngLat(int x, int y, double& lng, double& lat)
{
	lng = geoTransform[0] + x * (geoTransform[1] + geoTransform[2]);
	lat = geoTransform[3] + y * (geoTransform[4] + geoTransform[5]);
}

void* SxTiffRead::GetImgData(int x)
{
	return imgData[x];
}


int SxTiffRead::GetWidth()
{
	return this->m_width;
}

int SxTiffRead::GetHeight()
{
	return this->m_height;
}

int SxTiffRead::GetBandNum()
{
	return this->bandNum;
}

GDALDataType SxTiffRead::GetDataType()
{
	return this->rasterDataType;
}

double SxTiffRead::GetTransform(int x)
{
	return this->geoTransform[x];
}

double SxTiffRead::GetMax(int x)
{
	return this->m_max[x];
}

double SxTiffRead::GetMin(int x)
{
	return this->m_min[x];
}

double SxTiffRead::GetMean(int x)
{
	return this->m_mean[x];
}

double SxTiffRead::GetStdErr(int x)
{
	return this->m_stdErr[x];
}
