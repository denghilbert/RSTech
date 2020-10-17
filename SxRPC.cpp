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

//���Ų���
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

//���Ų���
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
	//BLH��������
	lat = (lat - rpcData->LAT_OFF) / rpcData->LAT_SCALE;
	lon = (lon - rpcData->LONG_OFF) / rpcData->LONG_SCALE;
	height = (height - rpcData->HEIGHT_OFF) / rpcData->HEIGHT_SCALE;

	//ѭ������ϵ������
	int index = 0;
	for (int i = 0; i <= rpcData->Order; i++)				// lat ѭ��
	{
		for (int j = 0; j <= rpcData->Order - i; j++)			// lon ѭ��
		{
			for (int k = 0; k <= rpcData->Order - i - j; k++)	// H ѭ��
			{
				NumS += rpcData->SNUM[index] * pow(lat, i) * pow(lon, j) * pow(height, k);
				DenS += rpcData->SDEN[index] * pow(lat, i) * pow(lon, j) * pow(height, k);
				NumL += rpcData->LNUM[index] * pow(lat, i) * pow(lon, j) * pow(height, k);
				DenL += rpcData->LDEN[index] * pow(lat, i) * pow(lon, j) * pow(height, k);
				index++;
			}
		}
	}
	//����������������
	y = NumL / DenL;
	x = NumS / DenS;
	//ͨ��ϵ�����Ż�ȥ
	x = (x)*rpcData->LINE_SCALE + rpcData->LINE_OFF;
	y = (y)*rpcData->SAMP_SCALE + rpcData->SAMP_OFF;

	return true;
}

bool SxRPC::GetLatLonByAffine(double x, double y, double H, double& lat, double& lon)
{
	this->m_trans.getValue(x, y, lon, lat);//��һ�δ����������ɾ�γ������
	
	//ͨ���µĵ������귴��һ��ԭ����
	double x1, y1;							//����������֤��	
	this->GetXYByRPC(lat, lon, H, x1, y1);//�Ӿ�γ�ȷ���������һ����������
	double error = sqrt((x1 - x) * (x1 - x) + (y1 - y) * (y1 - y));

	////��ȡ�߳�����
	//int demX, demY;
	//this->demdata->FromLngLat2XY(lon, lat, demX, demY);
	//if (demX >= 0 && demX < demdata->GetWidth() && demY >= 0 && demY < demdata->GetHeight())
	//	H = ((float*)(GetDemData()->GetImgData(0)))
	//	[demX + demY * GetDemData()->GetWidth()];


	for (int i = 0, j = 0; i < 4; i++, j++) {
		if (error <= 1e-5) { break; }//�ж�error�Ƿ��ڷ�Χ��
		else {
			int num = j % 4;

			this->affineX[num] = x1, this->affineY[num] = y1;//���·���ʹ�õ�
			this->affineLon[num] = lon, this->affineLat[num] = lat;

			this->RenewAffine();//���¸���������

			this->m_trans.getValue(x, y, lon, lat);//�����μ���lonlat

			////���¸߳�
			//this->demdata->FromLngLat2XY(lon, lat, demX, demY);
			//if (demX >= 0 && demX < demdata->GetWidth() && demY >= 0 && demY < demdata->GetHeight())
			//	H = ((float*)(GetDemData()->GetImgData(0)))
			//	[demX + demY * GetDemData()->GetWidth()];

			this->GetXYByRPC(lat, lon, H, x1, y1);//�����xy
			error = sqrt((x1 - x) * (x1 - x) + (y1 - y) * (y1 - y));//����error

			//if (bestE < error) {//������������¼��ʹ��ԭ���ķ���ģ��
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
	//�����ĸ��Ǻ��м��ľ�γ������
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

	//��ȡ�ĸ��ǵ���������
	this->xArray[0] = this->xArray[3] = 0;
	this->yArray[0] = this->yArray[1] = 0;
	this->xArray[1] = this->xArray[2] = m_rpcData->LINE_OFF +
		m_rpcData->LINE_SCALE - 1;
	this->yArray[2] = this->yArray[3] = m_rpcData->SAMP_OFF +
		m_rpcData->SAMP_SCALE - 1;
	//ԭʼͼ����С
	this->oldWidth = 2 * m_rpcData->LINE_SCALE;
	this->oldHeight = 2 * m_rpcData->SAMP_SCALE;

	//����ѭ������ĸ��������ĸ�����������
	for (int i = 0; i < 5; i++) {
		GetXYByRPC(affineLat[i], affineLon[i], h, affineX[i], affineY[i]);
		affineOriX[i] = affineX[i];
		affineOriY[i] = affineY[i];
		affineOriLon[i] = affineLon[i];
		affineOriLat[i] = affineLat[i];
	}
	//�����pixX��pixY��ƫ���е����׵����������������
	m_trans.Init(affineX, affineY, affineLon, affineLat, 5);

}

//�����µĵ������·���ģ��
void SxRPC::RenewAffine()
{
	m_trans.Init(affineX, affineY, affineLon, affineLat, 5);
}

//ÿ�θ�����һ�������÷���
void SxRPC::ReSetAffine()
{
	m_trans.Init(affineOriX, affineOriY, affineOriLon, affineOriLat, 5);
}

//�����µ�ͼ��
void SxRPC::GetNewHW(int& newH, int& newW)
{
	//�����ĸ��ǵ���������ͷ�������ĸ��ǵľ�γ����
	for (int i = 0; i < 4; i++) {
		GetLatLonByAffine(xArray[i], yArray[i], h,
			latArray[i], lonArray[i]);
	}
	//�����µ�ͼ��
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
