
// chair3DDlg.h : ͷ�ļ�
//
#if !defined(AFX_GxSingleCamColorDLG_H__D8D963C2_3BE2_44EC_B0EB_FCE016DDD6FA__INCLUDED_)
#define AFX_GxSingleCamColorDLG_H__D8D963C2_3BE2_44EC_B0EB_FCE016DDD6FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define  WHITE_BALANCE_RNTRY        3         ///< �Զ���ƽ��ö����
//#define  DEFAULT_CONTRAST           0         ///< Ĭ�϶Աȶ�ֵ
//#define  DEFAULT_GAMMA              1         ///< Ĭ��Gammaֵ
//#define  DEFAULT_COLOR_CORRECTION   0         ///< Ĭ����ɫУ��ֵ

///< ������ʾ�����궨��
#define  GX_VERIFY(emStatus) \
if (emStatus != GX_STATUS_SUCCESS)\
							{\
							ShowErrorString(emStatus); \
							return; \
							}
///< �жϷ���ֵ�궨��
#define VERIFY_STATUS_RET(emStatus) \
if (emStatus != GX_STATUS_SUCCESS) \
								   {\
								   return emStatus; \
								   }
#include "fit.h"
#include "engine.h"
#pragma comment( lib, "libeng.lib" )
#pragma comment( lib, "libmx.lib" )
#pragma comment( lib, "libmat.lib" )
#pragma comment( lib, "mclmcrrt.lib" )
#pragma comment( lib, "fit.lib")
//-------
#include "GxIAPI.h"
#include "DxImageProc.h"
#include "FileVersion.h"
#include <time.h>
#include "skeleton.h"
#include "indexTable.h"
#include <string>
#include <direct.h>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <cv.h>
using namespace cv;
using namespace std;

#pragma once//��ֹĳ��ͷ�ļ�����μ���

// Cchair3DDlg �Ի���
class Cchair3DDlg : public CDialogEx
{
// ����
public:
	Cchair3DDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CHAIR3D_DIALOG };
public:
	GX_DEV_HANDLE       m_hDevice;              ///< �豸���

	BITMAPINFO          *m_pBmpInfo;	        ///< BITMAPINFO �ṹָ�룬��ʾͼ��ʱʹ��
	BYTE                *m_pBufferRaw;          ///< ԭʼͼ������
	BYTE                *m_pBufferRGB;	        ///< RGBͼ�����ݣ�������ʾ�ͱ���bmpͼ��
	char                 m_chBmpBuf[2048];	    ///< BIMTAPINFO �洢��������m_pBmpInfo��ָ��˻�����

	int64_t             m_nPayLoadSize;         ///< ͼ���С
	int64_t             m_nImageHeight;         ///< ԭʼͼ���
	int64_t             m_nImageWidth;          ///< ԭʼͼ���
	int64_t             m_nPixelColorFilter;    ///< Bayer��ʽ

	GX_FLOAT_RANGE      m_stShutterFloatRange;  ///< �����ع�ʱ��ķ�Χ
	GX_FLOAT_RANGE      m_stGainFloatRange;     ///< ��������ֵ�ķ�Χ

	CString             m_strFilePath;          ///< BMPͼ��Ĭ�ϱ���·��
	BOOL                m_bIsSaveImg;           ///< ��ʶ�Ƿ񱣴�ͼ��
	BOOL                m_bDevOpened;           ///< ��ʶ�Ƿ��Ѵ��豸
	BOOL                m_bIsSnap;              ///< ��ʶ�Ƿ�ʼ�ɼ�
	//	BOOL                m_bIsImproveImg;        ///< ��ʶ�Ƿ����ͼ����������

	CWnd                *m_pWnd;                ///< ��ʾͼ�񴰿�(�ؼ�)ָ��
	HDC                 m_hDC;                  ///< ����ͼ��DC���
public:
	Mat deal(Mat mat);
	bool PrepareForShowImg();///  Ϊͼ����ʾ׼����Դ
	void InitUI();/// ��ȡ�豸����,��ʼ������ؼ�
	void UpDateUI();/// ˢ��UI����
	static void __stdcall OnFrameCallbackFun(GX_FRAME_CALLBACK_PARAM* pFrame);/// �ص�����
	void UnPrepareForShowImg();///  �ͷ�Ϊͼ����ʾ׼����Դ	
	void DrawImg();/// ��ʾͼ��
	void SaveImage();/// ����ͼ��
//	void SaveImage2(); //�Լ�����ı���·��
	void ShowErrorString(GX_STATUS emErrorStatus);/// ������Ϣ��ʾ
	void InitEnumUI(GX_FEATURE_ID_CMD emFeatureID, CComboBox *pComboBox, bool bIsImplement);/// ��ʼ��ö������UI����
	void InitGainUI();/// ��ʼ���������ؿؼ�	
	void InitShutterUI();/// ��ʼ���ع���ؿؼ�
	GX_STATUS InitDevice();/// ��ʼ�����:����Ĭ�ϲ���
	GX_STATUS SetPixelFormat8bit();/// ������������ݸ�ʽΪ8bit	
	GX_STATUS GetDeviceParam();/// ��ȡ�豸�Ŀ��ߵ�������Ϣ

	double  m_dBalanceRatio;
	double	m_dGainValue;
	double	m_dShutterValue;

	BOOL SetWindowPixelFormat(HDC hDC);   //�趨���ظ�ʽ 
	BOOL CreateViewGLContext(HDC hDC);     //View GL Context                             
	void RenderScene();  //���Ƴ��� 
//	HDC hrenderDC;      //DC 
	HGLRC hrenderRC;  //RC 
	float m_yRotate;       //ת�� 
	int PixelFormat;
	virtual BOOL DestroyWindow();

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

protected:
	afx_msg void OnPaint();
protected:
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnOpenDevice();
	afx_msg void OnBnClickedBtnCloseDevice();
	afx_msg void OnBnClickedBtnStopSnap();
	afx_msg void OnBnClickedBtnStartSnap();
	afx_msg void OnBnClickedCheckSaveBmp();
	afx_msg void OnKillfocusEditShutter();
	afx_msg void OnKillfocusEditGain();
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBinOpenDevice();
	afx_msg void OnBnClickedCalibration();
	afx_msg void OnBnClickedBinStopSnap();
	afx_msg void OnBnClickedBinCloseDevice();
	afx_msg void OnEnChangeEditShutter();
	afx_msg void OnEnChangeEditGain();
	afx_msg void OnBnClickedSaveImage();
	afx_msg void OnBnClickedMesh();
	afx_msg void OnBnClickedprofile();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
#endif