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
	int Order = 3;	//��ʽʽ����
	int CoeffNUm;//��������

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
	double m_meaHeight;					// ƽ���߳�
	double m_pixelsize, m_pixelsize2;	// Ӱ��һ���ش�С�ڵ����С�Լ���ƽ����

	SxAffine m_trans;					//�����м���䷴��

	double affineX[5], affineY[5], affineLat[5], affineLon[5], h;//���ڷ��㽨������ģ��
	double affineOriX[5], affineOriY[5], affineOriLat[5], affineOriLon[5];//ԭʼ����׼����5����

	double xArray[4], yArray[4];//�ĸ����ǵ���������
	double latArray[4], lonArray[4];//�ĸ����ǵľ�γ����
	int oldWidth, oldHeight, newWidth, newHeight;//�¾�ͼ������

	SxTiffRead* demdata;					//�߳����ݴ���

public:
	bool ReadRPCFile(const char* str);//��ȡtiff�����ļ�
	SxRPCData* OrderCoe(SxRPCData* origin);//����ϵ��˳�򷽱����
	SxRPCData* ReorderCoe(SxRPCData* origin);
	void GetNewHW(int& newH, int& newW);
	void InitDemData(const char* fn);
	SxTiffRead* GetDemData();

	//���㷴��
	bool GetXYByRPC(double lat, double lon, 
		double height, double& x, double& y);//ͨ����γ�ȼ�����������
	bool GetLatLonByAffine(double x, double y, 
		double H, double& lat, double& lon);//ͨ������������㾭γ��

	void CalGlobalAffine();//��ʼ���������
	void RenewAffine();//���·������
	void ReSetAffine();//���÷������


};

#endif