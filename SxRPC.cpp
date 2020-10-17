#define _CRT_SECURE_NO_WARNINGS 0
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif
#include "SxRPC.h"
#include "SxAffine.h"
#include "string"
#include "math.h"

SxRPC::SxRPC()
{
	;
}

SxRPC::~SxRPC()
{
	double demdata;
}

bool SxRPC::ReadRPCFile(const char* str)
{
	SxRPCData* RPCData = new SxRPCData();
	RPCData->Order = 3;
	RPCData->CoeffNUm = 20;

	FILE* fp = fopen(str, "r");
	if (fp == nullptr) {
		std::cout << "rpc File open failed!!\n";
		return false;
	}
	fscanf(fp, "%*s%lf%*s", &RPCData->LINE_OFF);
	fscanf(fp, "%*s%lf%*s", &RPCData->SAMP_OFF);
	fscanf(fp, "%*s%lf%*s", &RPCData->LAT_OFF);
	fscanf(fp, "%*s%lf%*s", &RPCData->LONG_OFF);
	fscanf(fp, "%*s%lf%*s", &RPCData->HEIGHT_OFF);
	fscanf(fp, "%*s%lf%*s", &RPCData->LINE_SCALE);
	fscanf(fp, "%*s%lf%*s", &RPCData->SAMP_SCALE);
	fscanf(fp, "%*s%lf%*s", &RPCData->LAT_SCALE);
	fscanf(fp, "%*s%lf%*s", &RPCData->LONG_SCALE);
	fscanf(fp, "%*s%lf%*s", &RPCData->HEIGHT_SCALE);
	int i;
	for (i = 0; i < RPCData->CoeffNUm; i++)
		fscanf(fp, "%*s%lf", &RPCData->LNUM[i]);
	for (i = 0; i < RPCData->CoeffNUm; i++)
		fscanf(fp, "%*s%lf", &RPCData->LDEN[i]);
	for (i = 0; i < RPCData->CoeffNUm; i++)
		fscanf(fp, "%*s%lf", &RPCData->SNUM[i]);
	for (i = 0; i < RPCData->CoeffNUm; i++)
		fscanf(fp, "%*s%lf", &RPCData->SDEN[i]);

	this->m_rpcData = this->OrderCoe(RPCData);
	fclose(fp);
}

//重排参数
//===================================================//
//origin:1 L P H LP LH PH LL PP
//		 HH PLH LLL LPP LHH LLP PPP PHH LLH PPH HHH
//destiny:1 H HH HHH L LH LHH LL LLH LLL 
//		  P PH PHH PL PLH PLL PP PPH PPL PPP
//===================================================//
SxRPCData* SxRPC::OrderCoe(SxRPCData* origin)
{
	SxRPCData* result = new SxRPCData(*origin);
	double* p = NULL;
	double* q = NULL;
	for (int i = 0; i < 4; i++)
	{
		if (i == 0) { p = result->SNUM;	q = origin->SNUM; }
		if (i == 1) { p = result->SDEN;	q = origin->SDEN; }
		if (i == 2) { p = result->LNUM;	q = origin->LNUM; }
		if (i == 3) { p = result->LDEN;	q = origin->LDEN; }
		p[0] = q[0];	p[1] = q[3];	p[2] = q[9];	p[3] = q[19];	p[4] = q[1];
		p[5] = q[5];	p[6] = q[13];	p[7] = q[7];	p[8] = q[17];	p[9] = q[11];
		p[10] = q[2];	p[11] = q[6];	p[12] = q[16];	p[13] = q[4];	p[14] = q[10];
		p[15] = q[14];	p[16] = q[8];	p[17] = q[18];	p[18] = q[12];	p[19] = q[15];
	}
	return result;
}

//反排参数
//===================================================//
//origin:1 H HH HHH L LH LHH LL LLH LLL 
//		 P PH PHH PL PLH PLL PP PPH PPL PPP
//destiny:1 L P H LP LH PH LL PP
//		  HH PLH LLL LPP LHH LLP PPP PHH LLH PPH HHH
//===================================================//
SxRPCData* SxRPC::ReorderCoe(SxRPCData* origin)
{
	SxRPCData* result = new SxRPCData(*origin);
	double* p = NULL;
	double* q = NULL;
	for (int i = 0; i < 4; i++)
	{
		if (i == 0) { p = result->SNUM;	q = origin->SNUM; }
		if (i == 1) { p = result->SDEN;	q = origin->SDEN; }
		if (i == 2) { p = result->LNUM;	q = origin->LNUM; }
		if (i == 3) { p = result->LDEN;	q = origin->LDEN; }
		p[0] = q[0];	p[1] = q[3];	p[2] = q[9];	p[3] = q[19];	p[4] = q[1];
		p[5] = q[5];	p[6] = q[13];	p[7] = q[7];	p[8] = q[17];	p[9] = q[11];
		p[10] = q[2];	p[11] = q[6];	p[12] = q[16];	p[13] = q[4];	p[14] = q[10];
		p[15] = q[14];	p[16] = q[8];	p[17] = q[18];	p[18] = q[12];	p[19] = q[15];
	}
	return result;
}

bool SxRPC::GetXYByRPC(double lat, double lon, double height, double& x, double& y)
{
	SxRPCData* rpcData = this->m_rpcData;
	double NumL = 0, DenL = 0, NumS = 0, DenS = 0;
	//BLH坐标正则化
	lat = (lat - rpcData->LAT_OFF) / rpcData->LAT_SCALE;
	lon = (lon - rpcData->LONG_OFF) / rpcData->LONG_SCALE;
	height = (height - rpcData->HEIGHT_OFF) / rpcData->HEIGHT_SCALE;

	//循环带入系数计算
	int index = 0;
	for (int i = 0; i <= rpcData->Order; i++)				// lat 循环
	{
		for (int j = 0; j <= rpcData->Order - i; j++)			// lon 循环
		{
			for (int k = 0; k <= rpcData->Order - i - j; k++)	// H 循环
			{
				NumS += rpcData->SNUM[index] * pow(lat, i) * pow(lon, j) * pow(height, k);
				DenS += rpcData->SDEN[index] * pow(lat, i) * pow(lon, j) * pow(height, k);
				NumL += rpcData->LNUM[index] * pow(lat, i) * pow(lon, j) * pow(height, k);
				DenL += rpcData->LDEN[index] * pow(lat, i) * pow(lon, j) * pow(height, k);
				index++;
			}
		}
	}
	//计算正则化像素坐标
	y = NumL / DenL;
	x = NumS / DenS;
	//通过系数缩放回去
	x = (x)*rpcData->LINE_SCALE + rpcData->LINE_OFF;
	y = (y)*rpcData->SAMP_SCALE + rpcData->SAMP_OFF;

	return true;
}

bool SxRPC::GetLatLonByAffine(double x, double y, double H, double& lat, double& lon)
{
	this->m_trans.getValue(x, y, lon, lat);//第一次从像素坐标变成经纬度坐标
	
	//通过新的地理坐标反算一次原坐标
	double x1, y1;							//反算用来验证的	
	this->GetXYByRPC(lat, lon, H, x1, y1);//从经纬度反过来再算一次像素坐标
	double error = sqrt((x1 - x) * (x1 - x) + (y1 - y) * (y1 - y));

	////获取高程数据
	//int demX, demY;
	//this->demdata->FromLngLat2XY(lon, lat, demX, demY);
	//if (demX >= 0 && demX < demdata->GetWidth() && demY >= 0 && demY < demdata->GetHeight())
	//	H = ((float*)(GetDemData()->GetImgData(0)))
	//	[demX + demY * GetDemData()->GetWidth()];


	for (int i = 0, j = 0; i < 4; i++, j++) {
		if (error <= 1e-5) { break; }//判断error是否在范围内
		else {
			int num = j % 4;

			this->affineX[num] = x1, this->affineY[num] = y1;//更新仿射使用点
			this->affineLon[num] = lon, this->affineLat[num] = lat;

			this->RenewAffine();//重新更交换仿射

			this->m_trans.getValue(x, y, lon, lat);//在依次计算lonlat

			////更新高程
			//this->demdata->FromLngLat2XY(lon, lat, demX, demY);
			//if (demX >= 0 && demX < demdata->GetWidth() && demY >= 0 && demY < demdata->GetHeight())
			//	H = ((float*)(GetDemData()->GetImgData(0)))
			//	[demX + demY * GetDemData()->GetWidth()];

			this->GetXYByRPC(lat, lon, H, x1, y1);//反算回xy
			error = sqrt((x1 - x) * (x1 - x) + (y1 - y) * (y1 - y));//计算error

			//if (bestE < error) {//在误差变大的情况下坚持使用原来的仿射模型
			//	this->affineX[num] = tmpX, this->pixelY[num] = tmpY;
			//	this->lon[num] = tmpLon, this->lat[num] = tmpLat;
			//	j--;
			//}
		}
	}
	return true;
}

void SxRPC::CalGlobalAffine()
{
	//计算四个角和中间点的经纬度坐标
	this->h = m_rpcData->HEIGHT_OFF;
	this->affineLat[4] = m_rpcData->LAT_OFF;
	this->affineLon[4] = m_rpcData->LONG_OFF;
	this->affineLat[0] = -m_rpcData->LAT_SCALE + m_rpcData->LAT_OFF;
	this->affineLon[0] = m_rpcData->LONG_SCALE + m_rpcData->LONG_OFF;
	this->affineLat[1] = m_rpcData->LAT_SCALE + m_rpcData->LAT_OFF;
	this->affineLon[1] = m_rpcData->LONG_SCALE + m_rpcData->LONG_OFF;
	this->affineLat[2] = m_rpcData->LAT_SCALE + m_rpcData->LAT_OFF;
	this->affineLon[2] = -m_rpcData->LONG_SCALE + m_rpcData->LONG_OFF;
	this->affineLat[3] = -m_rpcData->LAT_SCALE + m_rpcData->LAT_OFF;
	this->affineLon[3] = -m_rpcData->LONG_SCALE + m_rpcData->LONG_OFF;

	//读取四个角的像素坐标
	this->xArray[0] = this->xArray[3] = 0;
	this->yArray[0] = this->yArray[1] = 0;
	this->xArray[1] = this->xArray[2] = m_rpcData->LINE_OFF +
		m_rpcData->LINE_SCALE - 1;
	this->yArray[2] = this->yArray[3] = m_rpcData->SAMP_OFF +
		m_rpcData->SAMP_SCALE - 1;
	//原始图布大小
	this->oldWidth = 2 * m_rpcData->LINE_SCALE;
	this->oldHeight = 2 * m_rpcData->SAMP_SCALE;

	//利用循环求出四个初步的四个点像素坐标
	for (int i = 0; i < 5; i++) {
		GetXYByRPC(affineLat[i], affineLon[i], h, affineX[i], affineY[i]);
		affineOriX[i] = affineX[i];
		affineOriY[i] = affineY[i];
		affineOriLon[i] = affineLon[i];
		affineOriLat[i] = affineLat[i];
	}
	//这里的pixX和pixY会偏差有点离谱但是最后会修正到最佳
	m_trans.Init(affineX, affineY, affineLon, affineLat, 5);

}

//利用新的点来更新仿射模型
void SxRPC::RenewAffine()
{
	m_trans.Init(affineX, affineY, affineLon, affineLat, 5);
}

//每次更换下一个点重置仿射
void SxRPC::ReSetAffine()
{
	m_trans.Init(affineOriX, affineOriY, affineOriLon, affineOriLat, 5);
}

//计算新的图布
void SxRPC::GetNewHW(int& newH, int& newW)
{
	//利用四个角的像素坐标和仿射算出四个角的经纬坐标
	for (int i = 0; i < 4; i++) {
		GetLatLonByAffine(xArray[i], yArray[i], h,
			latArray[i], lonArray[i]);
	}
	//计算新的图布
	this->newWidth = newW = this->oldWidth * (lonArray[1] - lonArray[3]) /
		(lonArray[1] - lonArray[0]);
	this->newHeight = newH = this->oldHeight * (latArray[0] - latArray[2]) /
		(latArray[0] - latArray[3]);
}

void SxRPC::InitDemData(const char* fn)
{
	this->demdata = new SxTiffRead(fn);
	this->demdata->InitData();
}

SxTiffRead* SxRPC::GetDemData()
{
	return demdata;
}
