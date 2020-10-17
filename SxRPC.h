#ifndef _SXRPC_H_
#define _SXRPC_H_

#include "math.h"
#include "SxAffine.h"
#include "SxTiffRead.h"

#ifndef NULL
#define NULL 0
#endif 

#ifndef MAX_LENGTH
#define MAX_LENGTH 1024
#endif

class SxRPCData{

public:
	int Order = 3;	//多式式次数
	int CoeffNUm;//参数数量

	double LINE_OFF = 0;
	double SAMP_OFF = 0;
	double LAT_OFF = 0;
	double LONG_OFF = 0;
	double HEIGHT_OFF = 0;
	double LINE_SCALE = 0;
	double SAMP_SCALE = 0;
	double LAT_SCALE = 0;
	double LONG_SCALE = 0;
	double HEIGHT_SCALE = 0;
	
	double SNUM[20] = { 0 };
	double SDEN[20] = { 0 };
	double LNUM[20] = { 0 };
	double LDEN[20] = { 0 };
};


class SxRPC{
public:
	SxRPC();
	~SxRPC();

public:
	SxRPCData* m_rpcData;
	double m_meaHeight;					// 平均高程
	double m_pixelsize, m_pixelsize2;	// 影像一像素大小在地面大小以及其平方项

	SxAffine m_trans;					//用于中间仿射反算

	double affineX[5], affineY[5], affineLat[5], affineLon[5], h;//用于反算建立仿射模型
	double affineOriX[5], affineOriY[5], affineOriLat[5], affineOriLon[5];//原始不精准仿射5个点

	double xArray[4], yArray[4];//四个顶角的像素坐标
	double latArray[4], lonArray[4];//四个顶角的经纬坐标
	int oldWidth, oldHeight, newWidth, newHeight;//新旧图布长宽

	SxTiffRead* demdata;					//高程数据储存

public:
	bool ReadRPCFile(const char* str);//读取tiff数据文件
	SxRPCData* OrderCoe(SxRPCData* origin);//更新系数顺序方便计算
	SxRPCData* ReorderCoe(SxRPCData* origin);
	void GetNewHW(int& newH, int& newW);
	void InitDemData(const char* fn);
	SxTiffRead* GetDemData();

	//正算反算
	bool GetXYByRPC(double lat, double lon, 
		double height, double& x, double& y);//通过经纬度计算像素坐标
	bool GetLatLonByAffine(double x, double y, 
		double H, double& lat, double& lon);//通过像素坐标计算经纬度

	void CalGlobalAffine();//初始化仿射参数
	void RenewAffine();//更新仿射参数
	void ReSetAffine();//重置仿射参数


};

#endif