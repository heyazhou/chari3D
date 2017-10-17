
// chair3DDlg.cpp : ʵ���ļ�
//��������
//CString���󣺶��ַ����� 
//���imread �޷���ȡͼƬ�����ڼ�����release��lib
//1641
#include "stdafx.h"
#include "chair3D.h"
#include "chair3DDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE 
static char THIS_FILE[] = __FILE__;
#endif

//result�޷���ȡ�ڴ�
//---------------------------------------------------------------------------------
                                  /*   ��ʼ������  */
#define PI 3.14159265;
#define numofLine 18                             //ÿ�ű궨ͼƬ������
#define yuzhi 40								//��ֵ��С
#define size_pixel 0.00375						//��ԭ��С 3.75um��mmΪ��λ
#define angle 60								    //���Ƿ�͸���뷨�߼н�
#define len_base 600							//͸������׼��ľ���
#define F 8									    //͸����CCD���룬һ�㰴����
#define width 1292								//ͼ����
#define height 964								//ͼ��߶�
//#define numofpoint 23356                         //��ɢ��ĸ���=numofLine*ͼ����
//#define N 9                                      //ʹ�ñ궨ͼƬ��Ŀ����ͬ�߶Ȳɼ���ͼƬ
//#define IS_SNAP_SINGLE 0

double result[height][width];									 //����ÿ�������ά�߶�
//int w[numofLine][];									 //����ÿ���ߵ�xֵ������ȷ�������߼��


//----------------------------------------------------------------------------------

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };
	CString	m_strProductVersion;
	CString	m_strLegalCopyRight;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
	m_strProductVersion = _T("");
	m_strLegalCopyRight = _T("");
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Cchair3DDlg �Ի���



Cchair3DDlg::Cchair3DDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cchair3DDlg::IDD, pParent)
	, m_bDevOpened(FALSE)
	, m_bIsSnap(FALSE)
	, m_bIsSaveImg(FALSE)
	, m_hDevice(NULL)
	, m_pBmpInfo(NULL)
	, m_pBufferRaw(NULL)
	, m_pBufferRGB(NULL)
	, m_nPayLoadSize(0)
	, m_nImageHeight(0)
	, m_nImageWidth(0)
	, m_nPixelColorFilter(GX_COLOR_FILTER_NONE)
	, m_strFilePath("")
	, m_pWnd(NULL)
	, m_hDC(NULL)
{
	PixelFormat = 0;////////////////////////////////////////////////////
	m_yRotate = 0;//////////////////////////////////
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_dBalanceRatio = 0.0;
	m_dGainValue = 0.0;
	m_dShutterValue = 0.0;
	// ��ʼ������
	memset(&m_stGainFloatRange, 0, sizeof(GX_FLOAT_RANGE));
	memset(&m_stShutterFloatRange, 0, sizeof(GX_FLOAT_RANGE));
	//memset(&m_stFloatRange,0,sizeof(GX_FLOAT_RANGE));
	memset(m_chBmpBuf, 0, sizeof(m_chBmpBuf));

}

void Cchair3DDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_GAIN, m_dGainValue);
	DDX_Text(pDX, IDC_EDIT_SHUTTER, m_dShutterValue);
}

BEGIN_MESSAGE_MAP(Cchair3DDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()/////////////////û�иĺ�����opengl�޷���ʾ/////////////////////////////////
	ON_BN_CLICKED(IDC_BTN_OPEN_DEVICE, &Cchair3DDlg::OnBnClickedBtnOpenDevice)
	ON_BN_CLICKED(IDC_CALIBRATION, &Cchair3DDlg::OnBnClickedCalibration)
	ON_BN_CLICKED(IDC_BTN_STOP_SNAP, &Cchair3DDlg::OnBnClickedBtnStopSnap)
	ON_BN_CLICKED(IDC_BTN_CLOSE_DEVICE, &Cchair3DDlg::OnBnClickedBtnCloseDevice)
	ON_EN_CHANGE(IDC_EDIT_SHUTTER, &Cchair3DDlg::OnEnChangeEditShutter)
	ON_EN_CHANGE(IDC_EDIT_GAIN, &Cchair3DDlg::OnEnChangeEditGain)
	ON_BN_CLICKED(IDC_SAVE_IMAGE, &Cchair3DDlg::OnBnClickedSaveImage)
	ON_BN_CLICKED(IDC_MESH, &Cchair3DDlg::OnBnClickedMesh)
	ON_BN_CLICKED(IDC_profile, &Cchair3DDlg::OnBnClickedprofile)
	ON_EN_CHANGE(IDC_EDIT3, &Cchair3DDlg::OnEnChangeEdit3)
END_MESSAGE_MAP()


// Cchair3DDlg ��Ϣ�������

BOOL Cchair3DDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// ��������...���˵�����ӵ�ϵͳ�˵��С�
	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	ShowWindow(SW_MINIMIZE);

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	//��ʼ����
	emStatus = GXInitLib();
	if (emStatus != GX_STATUS_SUCCESS)
	{
		ShowErrorString(emStatus);
		exit(0);
	}

	//// ��ȡ��ǰ����ִ��·��,����ͼ�񱣴��Ĭ��·��
	//CString strDate = "";
	//char    szName[MAX_PATH] = { 0 };
	//CString strAppFullName = "";
	//GetModuleFileName(NULL, szName, MAX_PATH);
	//strAppFullName = szName;
	//int nPos = strAppFullName.ReverseFind('\\');
	//m_strFilePath += strAppFullName.Left(nPos);
	//m_strFilePath += "\\GxSingleCamColorImages";

	////��ȡͼ����ʾ���ڵ�ָ��ͻ�ͼDC
//	m_pWnd = GetDlgItem(IDC_STATIC_SHOW_FRAME);
//	m_hDC = ::GetDC(m_pWnd->m_hWnd);

	//// ���½���ؼ�
	UpDateUI();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void Cchair3DDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		char strFileName[MAX_PATH] = { '\0' };
		GetModuleFileName(NULL, strFileName, MAX_PATH);
		CFileVersion fv(strFileName);
		CAboutDlg dlgAbout;
		dlgAbout.m_strProductVersion = _T("Version: ") + fv.GetProductVersion();//_t �Ǻ꣬����֧��unicode����
		dlgAbout.m_strLegalCopyRight = fv.GetLegalCopyright();
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void Cchair3DDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR Cchair3DDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//---------------------------------------------------------------------------------
/**
\brief   �ر�Ӧ�ó�����

\return  ��
*/
//----------------------------------------------------------------------------------
void Cchair3DDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	KillTimer(0);

	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	// ��δͣ������ֹͣ�ɼ�
	if (m_bIsSnap)
	{
		// ����ֹͣ�ɼ�����
		emStatus = GXSendCommand(m_hDevice, GX_COMMAND_ACQUISITION_STOP);

		// ע���ص�
		emStatus = GXUnregisterCaptureCallback(m_hDevice);
		m_bIsSnap = FALSE;

		// �ͷ�Ϊ�ɼ�ͼ�񿪱ٵ�ͼ��Buffer
		UnPrepareForShowImg();
	}

	//�ͷ�pDC
	::ReleaseDC(m_pWnd->m_hWnd, m_hDC);

	// δ�ر��豸��ر��豸
	if (m_bDevOpened)
	{
		emStatus = GXCloseDevice(m_hDevice);
		m_bDevOpened = FALSE;
		m_hDevice = NULL;

		// ɾ����ɫУ�����Buffer��Դ
		//	if (m_pGammaLut != NULL)
		//	{
		//		delete[] m_pGammaLut;
		//		m_pGammaLut = NULL;
		//	}
		//	if (m_pContrastLut != NULL)
		//	{
		//		delete[] m_pContrastLut;
		//		m_pContrastLut = NULL;
		//	}
	}

	// �ر��豸��
	emStatus = GXCloseLib();

	CDialog::OnClose();
}
//----------------------------------------------------------------------------------
/**
\brief  ��ȡGxIAPI��������,������������ʾ�Ի���
\param  emErrorStatus  [in]   ������

\return ��
*/
//----------------------------------------------------------------------------------
void Cchair3DDlg::ShowErrorString(GX_STATUS emErrorStatus)
{
	char*     pchErrorInfo = NULL;
	size_t    nSize = 0;
	GX_STATUS emStatus = GX_STATUS_ERROR;

	// ��ȡ������Ϣ���ȣ��������ڴ�ռ�
	emStatus = GXGetLastError(&emErrorStatus, NULL, &nSize);
	pchErrorInfo = new char[nSize];
	if (NULL == pchErrorInfo)
	{
		return;
	}

	// ��ȡ������Ϣ������ʾ
	emStatus = GXGetLastError(&emErrorStatus, pchErrorInfo, &nSize);
	if (emStatus != GX_STATUS_SUCCESS)
	{
		MessageBox("GXGetLastError�ӿڵ���ʧ�ܣ�");
	}
	else
	{
		MessageBox((LPCTSTR)pchErrorInfo);
	}

	// �ͷ�������ڴ�ռ�
	if (NULL != pchErrorInfo)
	{
		delete[] pchErrorInfo;
		pchErrorInfo = NULL;
	}
}
//---------------------------------------------------------------------------------
/**
\brief   ��ʼ��UI����

\return  ��
*/
//----------------------------------------------------------------------------------
void Cchair3DDlg::InitUI()
{
	GX_STATUS emStatus = GX_STATUS_ERROR;
	int  nValue = 0;
	// ����ģʽ����ѡ��Combox�ؼ�
	//	InitTriggerModeUI();
	//	// ��ʼ������ԴCombox�ؼ�
	//	InitTriggerSourceUI();
	// ��ʼ����������Combox�ؼ�
	//	InitTriggerActivationUI();
	// ��ʼ����ƽ�����Combox�ؼ�
	//	InitEnumUI(GX_ENUM_BALANCE_WHITE_AUTO, (CComboBox *)GetDlgItem(IDC_COMBO_AWB), m_bColorFilter);
	// ��ʼ����ƽ��ͨ��Combox�ؼ�
	//	InitEnumUI(GX_ENUM_BALANCE_RATIO_SELECTOR, (CComboBox *)GetDlgItem(IDC_COMBO_RATIO_SELECTOR), m_bBalanceRatioSelector);
	// ��ʼ���ع�ʱ����ؿؼ�
	InitShutterUI();
	// ��ʼ��������ؿؼ�
	InitGainUI();
	// ��ʼ����ƽ��ϵ����ؿؼ�
	//	InitWhiteRatioUI();

}
//---------------------------------------------------------------------------------
/**
\brief   ��ʼ����Ͽ�UI����
\param   emFeatureID   [in]    ������ID
\param   pComboBox     [in]    �ؼ�ָ��
\param   bIsImplement  [in]    �豸�Ƿ�֧��emFeatureID������

\return  ��
*/
//----------------------------------------------------------------------------------
void Cchair3DDlg::InitEnumUI(GX_FEATURE_ID_CMD emFeatureID, CComboBox *pComboBox, bool bIsImplement)
{
	// �жϿؼ�
	if ((pComboBox == NULL) || !bIsImplement)
	{
		return;
	}

	GX_ENUM_DESCRIPTION *pEnum = NULL;
	GX_STATUS emStatus = GX_STATUS_ERROR;
	size_t    nbufferSize = 0;
	uint32_t  nEntryNum = 0;
	int64_t   nEnumValue = -1;
	int       nCursel = 0;
	double    dValue = 0;

	// ��ȡ���ܵ�ö����
	emStatus = GXGetEnumEntryNums(m_hDevice, emFeatureID, &nEntryNum);
	GX_VERIFY(emStatus);

	// Ϊ��ȡö���͵Ĺ�����������ռ�
	nbufferSize = nEntryNum * sizeof(GX_ENUM_DESCRIPTION);
	pEnum = new GX_ENUM_DESCRIPTION[nEntryNum];
	if (pEnum == NULL)
	{
		MessageBox("���仺����ʧ�ܣ�");
		return;
	}

	// ��ȡ���ܵ�ö������
	emStatus = GXGetEnumDescription(m_hDevice, emFeatureID, pEnum, &nbufferSize);
	if (emStatus != GX_STATUS_SUCCESS)
	{
		if (pEnum != NULL)
		{
			delete[]pEnum;
			pEnum = NULL;
		}
		ShowErrorString(emStatus);
		return;
	}


	// ��ȡö���͵ĵ�ǰֵ,����Ϊ���浱ǰ��ʾֵ
	emStatus = GXGetEnum(m_hDevice, emFeatureID, &nEnumValue);
	if (emStatus != GX_STATUS_SUCCESS)
	{
		if (pEnum != NULL)
		{
			delete[]pEnum;
			pEnum = NULL;
		}
		ShowErrorString(emStatus);
		return;
	}

	// ��ʼ����ǰ�ؼ��Ŀ�ѡ��
	pComboBox->ResetContent();
	for (uint32_t i = 0; i < nEntryNum; i++)
	{
		pComboBox->SetItemData(pComboBox->AddString(pEnum[i].szSymbolic), (uint32_t)pEnum[i].nValue);
		if (pEnum[i].nValue == nEnumValue)
		{
			nCursel = i;
		}
	}

	// ���õ�ǰֵΪ�������ʾֵ
	pComboBox->SetCurSel(nCursel);

	// �ͷź�����Դ
	if (pEnum != NULL)
	{
		delete[]pEnum;
		pEnum = NULL;
	}
}
//----------------------------------------------------------------------------------
/**
\brief  ˢ��UI����

\return �޷���ֵ
*/
//----------------------------------------------------------------------------------
void Cchair3DDlg::UpDateUI()
{
	GetDlgItem(IDC_BTN_OPEN_DEVICE)->EnableWindow(!m_bDevOpened);
	GetDlgItem(IDC_BTN_CLOSE_DEVICE)->EnableWindow(m_bDevOpened);
	//	GetDlgItem(IDC_BTN_START_SNAP)->EnableWindow(m_bDevOpened && !m_bIsSnap);
	GetDlgItem(IDC_BTN_STOP_SNAP)->EnableWindow(m_bDevOpened && m_bIsSnap);
	//	GetDlgItem(IDC_BTN_SOFTTRIGGER)->EnableWindow(m_bDevOpened);

	GetDlgItem(IDC_EDIT_GAIN)->EnableWindow(m_bDevOpened);
	GetDlgItem(IDC_EDIT_SHUTTER)->EnableWindow(m_bDevOpened);

}



//----------------------------------------------------------------------------------
/**
\brief   ��ʼ�����:����Ĭ�ϲ���

\return  ��
*/
//----------------------------------------------------------------------------------
GX_STATUS Cchair3DDlg::InitDevice()
{
	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	//���òɼ�ģʽ�����ɼ�
	emStatus = GXSetEnum(m_hDevice, GX_ENUM_ACQUISITION_MODE, GX_ACQ_MODE_CONTINUOUS);
	VERIFY_STATUS_RET(emStatus);

	// ��֪��ǰ���֧���ĸ�8λͼ�����ݸ�ʽ����ֱ������
	// �����豸֧�����ݸ�ʽGX_PIXEL_FORMAT_BAYER_GR8,��ɰ������´���ʵ��
	// emStatus = GXSetEnum(m_hDevice, GX_ENUM_PIXEL_FORMAT, GX_PIXEL_FORMAT_BAYER_GR8);
	// VERIFY_STATUS_RET(emStatus);

	// �������ǰ��������ݸ�ʽʱ�����Ե������º�����ͼ�����ݸ�ʽ����Ϊ8Bit
	emStatus = SetPixelFormat8bit();

	return emStatus;
}

// ---------------------------------------------------------------------------------
/**
\brief   ������������ݸ�ʽΪ8bit

\return  emStatus GX_STATUS_SUCCESS:���óɹ�������:����ʧ��
*/
// ----------------------------------------------------------------------------------
GX_STATUS Cchair3DDlg::SetPixelFormat8bit()
{
	GX_STATUS emStatus = GX_STATUS_SUCCESS;
	int64_t   nPixelSize = 0;
	uint32_t  nEnmuEntry = 0;
	size_t    nBufferSize = 0;
	BOOL      bIs8bit = TRUE;

	GX_ENUM_DESCRIPTION  *pEnumDescription = NULL;
	GX_ENUM_DESCRIPTION  *pEnumTemp = NULL;

	// ��ȡ���ص��С
	emStatus = GXGetEnum(m_hDevice, GX_ENUM_PIXEL_SIZE, &nPixelSize);
	VERIFY_STATUS_RET(emStatus);

	// �ж�Ϊ8bitʱֱ�ӷ���,��������Ϊ8bit
	if (nPixelSize == GX_PIXEL_SIZE_BPP8)
	{
		return GX_STATUS_SUCCESS;
	}
	else
	{
		// ��ȡ�豸֧�ֵ����ظ�ʽö����
		emStatus = GXGetEnumEntryNums(m_hDevice, GX_ENUM_PIXEL_FORMAT, &nEnmuEntry);
		VERIFY_STATUS_RET(emStatus);

		// Ϊ��ȡ�豸֧�ֵ����ظ�ʽö��ֵ׼����Դ
		nBufferSize = nEnmuEntry * sizeof(GX_ENUM_DESCRIPTION);
		pEnumDescription = new GX_ENUM_DESCRIPTION[nEnmuEntry];

		// ��ȡ֧�ֵ�ö��ֵ
		emStatus = GXGetEnumDescription(m_hDevice, GX_ENUM_PIXEL_FORMAT, pEnumDescription, &nBufferSize);
		if (emStatus != GX_STATUS_SUCCESS)
		{
			if (pEnumDescription != NULL)
			{
				delete[]pEnumDescription;
				pEnumDescription = NULL;
			}

			return emStatus;
		}

		// �����豸֧�ֵ����ظ�ʽ,�������ظ�ʽΪ8bit,
		// ���豸֧�ֵ����ظ�ʽΪMono10��Mono8��������ΪMono8
		for (uint32_t i = 0; i<nEnmuEntry; i++)
		{
			if ((pEnumDescription[i].nValue & GX_PIXEL_8BIT) == GX_PIXEL_8BIT)
			{
				emStatus = GXSetEnum(m_hDevice, GX_ENUM_PIXEL_FORMAT, pEnumDescription[i].nValue);
				break;
			}
		}

		// �ͷ���Դ
		if (pEnumDescription != NULL)
		{
			delete[]pEnumDescription;
			pEnumDescription = NULL;
		}
	}

	return emStatus;
}
//---------------------------------------------------------------------------------
/**
\brief   �ص�����
\param   pFrame         �ص�����

\return  ��
*/
//----------------------------------------------------------------------------------
void __stdcall Cchair3DDlg::OnFrameCallbackFun(GX_FRAME_CALLBACK_PARAM* pFrame)
{
	Cchair3DDlg *pDlg = (Cchair3DDlg*)(pFrame->pUserParam);

	if (pFrame->status == 0)
	{
		memcpy(pDlg->m_pBufferRaw, pFrame->pImgBuf, pFrame->nImgSize);

		// RGBת��
		DxRaw8toRGB24(pDlg->m_pBufferRaw
			, pDlg->m_pBufferRGB
			, (VxUint32)(pDlg->m_nImageWidth)
			, (VxUint32)(pDlg->m_nImageHeight)
			, RAW2RGB_NEIGHBOUR
			, DX_PIXEL_COLOR_FILTER(pDlg->m_nPixelColorFilter)
			, TRUE);

		// ͼ����������
		//if (pDlg->m_bIsImproveImg)
		//{
		//	//����ͼ������
		//	DxImageImprovment(pDlg->m_pBufferRGB
		//		, pDlg->m_pBufferRGB
		//		, (VxUint32)(pDlg->m_nImageWidth)
		//		, (VxUint32)(pDlg->m_nImageHeight)
		//		, pDlg->m_nColorCorrection
		//		, pDlg->m_pContrastLut
		//		, pDlg->m_pGammaLut);
		//}

		// ��ʾͼ��
		pDlg->DrawImg();

		// ͼ�񱣴�
		if (pDlg->m_bIsSaveImg)
		{
			pDlg->SaveImage();
		}
	}
}
//---------------------------------------------------------------------------------
/**
\brief   ��m_pBufferRGB��ͼ����ʾ������

\return  ��
*/
//----------------------------------------------------------------------------------
void Cchair3DDlg::DrawImg()
{
	int nWndWidth = 0;
	int nWndHeight = 0;

	// Ϊ��ͼ��׼��
	RECT objRect;
	m_pWnd->GetClientRect(&objRect);
	nWndWidth = objRect.right - objRect.left;
	nWndHeight = objRect.bottom - objRect.top;

	// ������ø���䣬����ͼ�����ˮ��
	::SetStretchBltMode(m_hDC, COLORONCOLOR);
	::StretchDIBits(m_hDC,
		0,
		0,
		nWndWidth,
		nWndHeight,
		0,
		0,
		(int)m_nImageWidth,
		(int)m_nImageHeight,
		m_pBufferRGB,
		m_pBmpInfo,
		DIB_RGB_COLORS,
		SRCCOPY
		);
}

//---------------------------------------------------------------------------------
/**
\brief   Ϊͼ����ʾ׼����Դ

\return  true:Ϊͼ����ʾ׼����Դ�ɹ�  false:׼����Դʧ��
*/
//----------------------------------------------------------------------------------
bool Cchair3DDlg::PrepareForShowImg()
{
	//--------------------------------------------------------------------
	//---------------------------��ʼ��bitmapͷ---------------------------
	m_pBmpInfo = (BITMAPINFO *)m_chBmpBuf;
	m_pBmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBmpInfo->bmiHeader.biWidth = (LONG)m_nImageWidth;
	m_pBmpInfo->bmiHeader.biHeight = (LONG)m_nImageHeight;

	m_pBmpInfo->bmiHeader.biPlanes = 1;
	m_pBmpInfo->bmiHeader.biBitCount = 24;
	m_pBmpInfo->bmiHeader.biCompression = BI_RGB;
	m_pBmpInfo->bmiHeader.biSizeImage = 0;
	m_pBmpInfo->bmiHeader.biXPelsPerMeter = 0;
	m_pBmpInfo->bmiHeader.biYPelsPerMeter = 0;
	m_pBmpInfo->bmiHeader.biClrUsed = 0;
	m_pBmpInfo->bmiHeader.biClrImportant = 0;

	//---------------------------------------------------------------------
	//----------------------------Ϊͼ�����ݷ���Buffer---------------------
	//Ϊ�洢RGBͼ����������ռ�
	m_pBufferRGB = new BYTE[(size_t)(m_nImageWidth * m_nImageHeight * 3)];
	if (m_pBufferRGB == NULL)
	{
		return false;
	}

	//Ϊ�洢ԭʼͼ����������ռ�
	m_pBufferRaw = new BYTE[(size_t)m_nPayLoadSize];
	if (m_pBufferRaw == NULL)
	{
		delete[]m_pBufferRGB;
		m_pBufferRGB = NULL;

		return false;
	}

	return true;
}

//---------------------------------------------------------------------------------
/**
\brief   �ͷ�Ϊͼ����ʾ׼����Դ

\return  ��
*/
//----------------------------------------------------------------------------------
void Cchair3DDlg::UnPrepareForShowImg()
{
	if (m_pBufferRGB != NULL)
	{
		delete[]m_pBufferRGB;
		m_pBufferRGB = NULL;
	}
	if (m_pBufferRaw != NULL)
	{
		delete[]m_pBufferRaw;
		m_pBufferRaw = NULL;
	}
}
//----------------------------------------------------------------------------------
/**
\brief  ���"���豸"�ؼ���Ӧ����

\return ��
*/
//----------------------------------------------------------------------------------
uint32_t    nDevNum = 0;
void Cchair3DDlg::OnBnClickedBtnOpenDevice()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	GX_STATUS      emStatus = GX_STATUS_SUCCESS;
	//uint32_t       nDevNum = 0;
	GX_OPEN_PARAM  stOpenParam;
	stOpenParam.accessMode = GX_ACCESS_EXCLUSIVE;
	stOpenParam.openMode = GX_OPEN_INDEX;
	stOpenParam.pszContent = "1";

	// ö���豸
	emStatus = GXUpdateDeviceList(&nDevNum, 1000);
	GX_VERIFY(emStatus);

	// �ж��豸����
	if (nDevNum <= 0)
	{
		MessageBox("û�з����豸!");
		return;
	}

	// ����豸�Ѿ�����ر�,��֤����ڳ�ʼ������������ٴδ�
	if (m_hDevice != NULL)
	{
		emStatus = GXCloseDevice(m_hDevice);
		GX_VERIFY(emStatus);
		m_hDevice = NULL;
	}

	// ���豸
	emStatus = GXOpenDevice(&stOpenParam, &m_hDevice);
	GX_VERIFY(emStatus);
	m_bDevOpened = TRUE;

	// ���������Ĭ�ϲ���:�ɼ�ģʽ:�����ɼ�,���ݸ�ʽ:8-bit
	emStatus = InitDevice();
	GX_VERIFY(emStatus);

	// ��ȡ�豸�Ŀ��ߵ�������Ϣ
	emStatus = GetDeviceParam();
	GX_VERIFY(emStatus);

	// ��ȡ�������,��ʼ������ؼ�
	InitUI();

	// ��ȡͼ��������������
	//	GetImageImproveParam();

	//���ö�ʱ�����½���
	SetTimer(0, 1000, NULL);

	// ���½���ؼ�
	UpDateUI();
	//��ʼ�ɼ�
	emStatus = GX_STATUS_ERROR;

	// ��ʼ��BMPͷ��Ϣ������BufferΪͼ��ɼ���׼��
	if (!PrepareForShowImg())
	{
		MessageBox("Ϊͼ����ʾ������Դʧ��!");
		return;
	}

	//ע��ص�
	emStatus = GXRegisterCaptureCallback(m_hDevice, this, OnFrameCallbackFun);
	if (emStatus != GX_STATUS_SUCCESS)
	{
		UnPrepareForShowImg();
		ShowErrorString(emStatus);
		return;
	}

	//����ʼ�ɼ�����
	emStatus = GXSendCommand(m_hDevice, GX_COMMAND_ACQUISITION_START);
	if (emStatus != GX_STATUS_SUCCESS)
	{
		UnPrepareForShowImg();
		ShowErrorString(emStatus);
		return;
	}

	m_bIsSnap = TRUE;

	// ���½���UI
	UpDateUI();
}
GX_STATUS Cchair3DDlg::GetDeviceParam()
{
	GX_STATUS emStatus = GX_STATUS_ERROR;
	bool      bColorFliter = false;

	// ��ȡͼ���С
	emStatus = GXGetInt(m_hDevice, GX_INT_PAYLOAD_SIZE, &m_nPayLoadSize);
	VERIFY_STATUS_RET(emStatus);

	// ��ȡ���
	emStatus = GXGetInt(m_hDevice, GX_INT_WIDTH, &m_nImageWidth);
	VERIFY_STATUS_RET(emStatus);


	// ��ȡ�߶�
	emStatus = GXGetInt(m_hDevice, GX_INT_HEIGHT, &m_nImageHeight);

	return emStatus;
}
//----------------------------------------------------------------------------------
/**
\brief  ���"�궨"�ؼ���Ӧ����

\return ��
*/
//----------------------------------------------------------------------------------
void Cchair3DDlg::OnBnClickedCalibration()
{

	////���ж��Ƿ�궨
	//char cal[MAX_PATH];
	//_getcwd(cal, MAX_PATH);
	//CString cs = cal;
	//cs += "/�궨";
	//
	//ifstream fin(cs + "/y_index.yml");
	//if (fin){
	//	MessageBox("�Ѿ��궨�������ظ��궨��");
	//}
	//else{
	//	/********************Calibration******************************/
	//	//Rect rect;
	//	//rect = Rect(0, 0, 1292, 964);//-------ͼ���С--------------�޸�------------------
	//	string index;
	//	string s2;
	//	for (int line = 1; line <= numofLine; line++){    //line������ʶ�ǵڼ��й���
	//		stringstream ss;
	//		ss << line;
	//		ss >> index;
	//		ss.str("");
	//		//		cout<<index<<endl;
	//		string table = cs;
	//		table = table + "/table" + index + ".yml";
	//		float high[numofLine + 2] = { 0 };  //�����˲�ͬ�궨ƽ���ʵ�ʸ߶�--------------�ı�------------------
	//		Mat	imgMap;  //
	//		ifstream fin(cs + "/high.in");
	//		string filename;
	//		//for (int i = 0; i < N;i++){
	//		//	fin >> high[i];
	//		//	if (high[i]<1e-5)     //�߶�ֵ�в�Ӧ���ڸ�ֵ
	//		//		high[i] = 250;      //��250��ʱ���0����������������
	//		//}
	//		//fin.close();

	//		for (int i = 0; i < N; i++)
	//		{

	//			fin >> high[i];
	//			if (high[i]<1e-5)     //�߶�ֵ�в�Ӧ���ڸ�ֵ
	//				high[i] = 250;      //��250��ʱ���0����������������

	//			string type = ".bmp";
	//			stringstream ss2;
	//			ss2 << i;
	//			ss2 >> s2;
	//			ss.str("");
	//			filename = cs;//��0��ʼ��10�ű궨ͼ
	//			filename = filename + "/" + s2;
	//			filename += type;
	//			Mat img = imread(filename, 0);//��ͨ��
	//			//imshow("img",img);
	//			//waitKey(0);
	//			if (img.empty())
	//			{
	//				MessageBox("�궨ͼƬ�޷���������");
	//			}

	//			Mat img_sk = skeleton(img, high[i], line);   //ÿ��ͼƬ�����ǻ������ߣ�����ͼƬ�߶�
	//			if (i == 0)
	//				imgMap = img_sk;
	//			else
	//				imgMap += img_sk;

	//		}
	//		indexTable(imgMap, N, table);//����table�ļ�
	//	}
	//	//����y_index�ļ�
	//	bool bl = false;//falseδ��ȡy������;����Ѿ����ɣ� ����Ҫ��ǰ��Ĵ���
	//	string file1 = cs + "/0.bmp"; //0.bmp�ٶ�Ϊ��׼ƽ��
	//	Mat imgRoi1 = imread(file1, 0);//0����Ҷ�ͼ��
	//	Mat imgblur1;
	//	threshold(imgRoi1, imgblur1, yuzhi, 255, CV_THRESH_BINARY);//-------��ֵ�޸�---------------
	//	medianBlur(imgblur1, imgblur1, 3);//��ֵ�˲�
	//	medianBlur(imgblur1, imgblur1, 3);//ͼ��ƽ������
	//	dilate(imgblur1, imgblur1, (5, 5));//���ʹ���
	//	int w1 = imgRoi1.cols, l1 = imgRoi1.rows;//col ����
	//
	//	Mat y_index1 = Mat::zeros(numofLine + 2, m_nImageWidth, CV_32FC1);
	//	if (!bl){
	//		for (int j = 0; j < w1; j++)//1292
	//		{
	//			int isLaser = 0;
	//			int up1 = 0;
	//			int buttom1 = 0;
	//			int line_panduan1 = 0; //����ƥ��line
	//			for (int i = 0; i < l1; i++)//964
	//			{
	//				if (imgblur1.at<uchar>(i, j) >yuzhi && !isLaser) //-------�޸�------
	//				{
	//					isLaser = 1;
	//					++line_panduan1;
	//					up1 = i;
	//				}
	//				else if (imgblur1.at<uchar>(i, j) <yuzhi && isLaser)
	//				{
	//					buttom1 = i - 1;
	//					y_index1.at<float>(line_panduan1 - 1, j) = (up1 + buttom1) / 2.0;
	//					isLaser = 0;
	//				}
	//			}
	//		}
	//		//����������y
	//		string s = cs; s += "/y_index.yml";
	//		FileStorage fs(s, FileStorage::WRITE);
	//		fs << "TABLE" << y_index1;
	//		fs.release();
	//	}
	//	MessageBox("�궨��ɣ�");
	//}
}

//----------------------------------------------------------------------------------
/**
\brief  ���"ֹͣ�ɼ�"�ؼ���Ӧ����

\return ��
*/
//----------------------------------------------------------------------------------
void Cchair3DDlg::OnBnClickedBtnStopSnap()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	GX_STATUS emStatus = GX_STATUS_SUCCESS;

	//����ֹͣ�ɼ�����
	emStatus = GXSendCommand(m_hDevice, GX_COMMAND_ACQUISITION_STOP);
	GX_VERIFY(emStatus);

	//ע���ص�
	emStatus = GXUnregisterCaptureCallback(m_hDevice);
	GX_VERIFY(emStatus);

	m_bIsSnap = FALSE;

	// �ͷ�Ϊ�ɼ�ͼ�񿪱ٵ�ͼ��Buffer������ͷţ�����ͣ���޷�����ͼ��
	//	UnPrepareForShowImg();

	// ���½���UI
	UpDateUI();
}

//----------------------------------------------------------------------------------
/**
\brief  ���"�ر��豸"�ؼ���Ӧ����

\return ��
*/
//----------------------------------------------------------------------------------
void Cchair3DDlg::OnBnClickedBtnCloseDevice()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	GX_STATUS emStatus = GX_STATUS_SUCCESS;
	// δͣ��ʱ��ֹͣ�ɼ�
	if (m_bIsSnap)
	{
		//����ֹͣ�ɼ�����
		emStatus = GXSendCommand(m_hDevice, GX_COMMAND_ACQUISITION_STOP);
		if (emStatus != GX_STATUS_SUCCESS)
		{
			// ������
		}
		//ע���ص�
		emStatus = GXUnregisterCaptureCallback(m_hDevice);
		if (emStatus != GX_STATUS_SUCCESS)
		{
			// ������
		}
		m_bIsSnap = FALSE;
		// �ͷ�Ϊ�ɼ�ͼ�񿪱ٵ�ͼ��Buffer
		UnPrepareForShowImg();
	}
	emStatus = GXCloseDevice(m_hDevice);
	if (emStatus != GX_STATUS_SUCCESS)
	{
		// ������
	}
	m_bDevOpened = FALSE;
	m_hDevice = NULL;
	// ɾ����ɫУ�����Buffer��Դ
	//if (m_pGammaLut != NULL)
	//{
	//	delete[] m_pGammaLut;
	//	m_pGammaLut = NULL;
	//}
	//if (m_pContrastLut != NULL)
	//{
	//	delete[] m_pContrastLut;
	//	m_pContrastLut = NULL;
	//}
	KillTimer(0);
	// ���½���UI
	UpDateUI();
}
//---------------------------------------------------------------------------------
/**
\brief   ��ʼ���ع���ؿؼ�:Static:��ʾ��Χ Edit:����ֵ

\return  ��
*/
//----------------------------------------------------------------------------------
void Cchair3DDlg::InitShutterUI()
{
	CStatic     *pStcShutterShow = (CStatic *)GetDlgItem(IDC_STC_SHUTTER_SHOW);
	CEdit       *pEditShutterVal = (CEdit *)GetDlgItem(IDC_EDIT_SHUTTER);
	int          nPos = 0;

	// �жϿؼ�����Ч��
	if ((pEditShutterVal == NULL) || (pStcShutterShow == NULL))
	{
		return;
	}

	GX_STATUS      emStatus = GX_STATUS_ERROR;
	CString         strTemp = "";
	double    dShutterValue = 0.0;

	// ��ȡ�����ͷ�Χ,����ʼ��SliderCtrl�ؼ���static��Χ��ʾ��
	emStatus = GXGetFloatRange(m_hDevice, GX_FLOAT_EXPOSURE_TIME, &m_stShutterFloatRange);
	GX_VERIFY(emStatus);

	strTemp.Format("�ع�(%.4f~%.4f)%s", m_stShutterFloatRange.dMin, m_stShutterFloatRange.dMax, m_stShutterFloatRange.szUnit);
	pStcShutterShow->SetWindowText(strTemp);

	// ��ȡ��ǰֵ������ǰֵ���µ�����
	emStatus = GXGetFloat(m_hDevice, GX_FLOAT_EXPOSURE_TIME, &dShutterValue);
	GX_VERIFY(emStatus);

	m_dShutterValue = dShutterValue;

	UpdateData(FALSE);
}
// ---------------------------------------------------------------------- -
/**
\brief   �ַ�������Ҫ�����ع�������Edit����Ӧ�س�������Ϣ
\param   pMsg  ��Ϣ�ṹ��

\return �ɹ�:TRUE   ʧ��:FALSE
*/
//-----------------------------------------------------------------------
BOOL Cchair3DDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	CWnd  *pWnd = NULL;
	int   nCtrlID = 0;             //< �����ȡ�Ŀؼ�ID

	//�ж��Ƿ��Ǽ��̻س���Ϣ
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
	{
		//��ȡ��ǰӵ�����뽹��Ĵ���(�ؼ�)ָ��
		pWnd = GetFocus();

		//��õ�ǰ����Ŀؼ�ID
		nCtrlID = pWnd->GetDlgCtrlID();

		//�ж�ID����
		switch (nCtrlID)
		{
			//	case IDC_EDIT_BALANCE_RATIO:
		case IDC_EDIT_SHUTTER:
		case IDC_EDIT_GAIN:

			//ʧȥ����
			SetFocus();

			break;

		default:
			break;
		}

		return TRUE;
	}
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE))
	{
		return  TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

//-----------------------------------------------------------------------
/**
\brief    �����ع�ʱ���Edit��ʧȥ�������Ӧ����

\return   ��
*/
//-----------------------------------------------------------------------
void Cchair3DDlg::OnEnChangeEditShutter()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//�жϾ���Ƿ���Ч�������豸���ߺ�رճ�����ֵ��쳣
	if (m_hDevice == NULL)
	{
		return;
	}

	UpdateData();
	GX_STATUS status = GX_STATUS_SUCCESS;

	//�ж�����ֵ�Ƿ����ع�ʱ��ķ�Χ��
	//���������ֵ���ع�ֵ��Ϊ���ֵ
	if (m_dShutterValue > m_stShutterFloatRange.dMax)
	{
		m_dShutterValue = m_stShutterFloatRange.dMax;
	}
	//��С����Сֵ���ع�ֵ��Ϊ��Сֵ
	if (m_dShutterValue < m_stShutterFloatRange.dMin)
	{
		m_dShutterValue = m_stShutterFloatRange.dMin;
	}
	status = GXSetFloat(m_hDevice, GX_FLOAT_EXPOSURE_TIME, m_dShutterValue);
	GX_VERIFY(status);

	UpdateData(FALSE);

}


//---------------------------------------------------------------------------------
/**
\brief   ��ʼ��������ؿؼ�: Static:��ʾ��Χ Edit:����ֵ

\return  ��
*/
//----------------------------------------------------------------------------------
void Cchair3DDlg::InitGainUI()
{
	CStatic     *pStcGain = (CStatic *)GetDlgItem(IDC_STC_GAIN);
	CEdit       *pEditGainVal = (CEdit *)GetDlgItem(IDC_EDIT_GAIN);
	int          nPos = 0;

	if ((pStcGain == NULL) || (pEditGainVal == NULL))
	{
		return;
	}

	GX_STATUS emStatus = GX_STATUS_ERROR;
	CString   strRange = "";
	double    dGainVal = 0;

	// ��ȡ�����ͷ�Χ,����ʼ��SliderCtrl�ؼ���static��Χ��ʾ��
	emStatus = GXGetFloatRange(m_hDevice, GX_FLOAT_GAIN, &m_stGainFloatRange);
	GX_VERIFY(emStatus);

	// ��ʾ��Χ
	strRange.Format("����(%.4f~%.4f)%s", m_stGainFloatRange.dMin, m_stGainFloatRange.dMax, m_stGainFloatRange.szUnit);
	pStcGain->SetWindowText(strRange);

	// ��ȡ��ǰֵ�����µ�����
	emStatus = GXGetFloat(m_hDevice, GX_FLOAT_GAIN, &dGainVal);
	GX_VERIFY(emStatus);

	m_dGainValue = dGainVal;

	UpdateData(FALSE);
}

//--------------------------------------------------------------------
/**
\brief   ��������ֵ��Edit��ʧȥ�������Ӧ����

\return  ��
*/
//--------------------------------------------------------------------
void Cchair3DDlg::OnEnChangeEditGain()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//�жϾ���Ƿ���Ч�������豸���ߺ�رճ�����ֵ��쳣
	if (m_hDevice == NULL)
	{
		return;
	}

	UpdateData();
	GX_STATUS status = GX_STATUS_SUCCESS;

	//�ж�����ֵ�Ƿ�������ֵ�ķ�Χ��
	//�������ֵ�������ֵ������ֵ���ó����ֵ
	if (m_dGainValue > m_stGainFloatRange.dMax)
	{
		m_dGainValue = m_stGainFloatRange.dMax;
	}

	//�������ֵС����Сֵ�������ֵ���ó���Сֵ
	if (m_dGainValue < m_stGainFloatRange.dMin)
	{
		m_dGainValue = m_stGainFloatRange.dMin;
	}

	status = GXSetFloat(m_hDevice, GX_FLOAT_GAIN, m_dGainValue);
	GX_VERIFY(status);

	UpdateData(FALSE);
}


//----------------------------------------------------------------------------------
/**
\brief   ��m_pBufferRGBͼ�����ݱ����BMPͼƬ

\return  ��
*/
//----------------------------------------------------------------------------------
void Cchair3DDlg::SaveImage()//����ͼ����õĺ���
{
	// TODO: Add your control notification handler code here
	DWORD		         dwImageSize = (DWORD)(m_nImageWidth * m_nImageHeight * 3);
	BITMAPFILEHEADER     stBfh = { 0 };
	DWORD		         dwBytesRead = 0;
	SYSTEMTIME           sysTime;
	CString              strFileName = "";

	char cal[MAX_PATH];
	_getcwd(cal, MAX_PATH);
	CString cs = cal;
	cs += "/����";
	m_strFilePath = cs;
	//��������ͼ����ļ���
	BOOL bRet = CreateDirectory(m_strFilePath, NULL);
	if (bRet)
	{
		SetFileAttributes(m_strFilePath, FILE_ATTRIBUTE_NORMAL);
	}

	// ��ȡ��ǰʱ��Ϊͼ�񱣴��Ĭ������
	GetLocalTime(&sysTime);
	strFileName.Format("%s\\%d.bmp", m_strFilePath, 1);

	stBfh.bfType = (WORD)'M' << 8 | 'B';			 //�����ļ�����
	stBfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);	//�����ļ�ͷ��С��ɫ
	stBfh.bfSize = stBfh.bfOffBits + dwImageSize; //�ļ���С

	//�����ļ�
	HANDLE hFile = ::CreateFile(strFileName,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		::WriteFile(hFile, &stBfh, sizeof(BITMAPFILEHEADER), &dwBytesRead, NULL);
		::WriteFile(hFile, m_pBmpInfo, sizeof(BITMAPINFOHEADER), &dwBytesRead, NULL);  //��ɫ
		::WriteFile(hFile, m_pBufferRGB, dwImageSize, &dwBytesRead, NULL);
		CloseHandle(hFile);
	}
}
void Cchair3DDlg::OnBnClickedSaveImage()
{
	// TODO:  
	if (nDevNum <= 0)
	{
		MessageBox("�豸δ��!");
		return;
	}
	else{
		SaveImage();
		MessageBox("����ɹ�!");
	}
}

//----------------------------------------------------------------------------------
/**
\       ����ά�ع�����Ҫ��matlab���

\return  ��
*/
//----------------------------------------------------------------------------------
//double result[numofpoint][3];
//ofstream of;
//int point = 0;
////-----------------------------��������-----------------

void dfs(Mat guhua, int x, int y, int w, int l, int line_panduan,Mat image_pixel){
	if (x >= 0 && x<l && y - 1 >= 0 && y <w && guhua.at<double>(x, y - 1)>0){
		image_pixel.at<double>(line_panduan, y-1) = guhua.at<double>(x,y - 1);
		return dfs(guhua, x, y - 1, w, l, line_panduan, image_pixel);
	}
	else if (x - 2 >= 0 && x<l && y - 1 >= 0 && y <w && guhua.at<double>(x - 2, y - 1)>0){
		image_pixel.at<double>(line_panduan, y - 1) = guhua.at<double>(x - 2, y - 1);

		//int x1 = y_index.at<float>(line_panduan - 1, y - 1);
		//if (table.at<float>(x - 2, y - 1)>0){
		//	result[point][0] = x1; result[point][1] = y - 1; result[point][2] = table.at<float>(x - 2, y - 1);
		//	point++;
		//	of << x1 << " " << y - 1 << " " << table.at<float>(x - 2, y - 1) << endl;
		//}
		return dfs(guhua, x - 2, y - 1, w, l, line_panduan, image_pixel);
	}
	else if (x - 1 >= 0 && x<l && y - 1 >= 0 && y <w && guhua.at<float>(x - 1, y - 1)>0){
		image_pixel.at<double>(line_panduan, y - 1) = guhua.at<double>(x - 1, y - 1);
		return dfs(guhua, x - 1, y - 1, w, l, line_panduan, image_pixel);
	}
	else if (x >= 0 && x + 2<l && y - 1 >= 0 && y <w && guhua.at<float>(x + 2, y - 1)>0){
		image_pixel.at<double>(line_panduan, y - 1) = guhua.at<double>(x + 2, y - 1);
		return dfs(guhua, x +2, y - 1, w, l, line_panduan, image_pixel);
	}
	else if (x >= 0 && x + 1<l && y - 1 >= 0 && y<w && guhua.at<float>(x + 1, y - 1)>0){
		image_pixel.at<double>(line_panduan, y - 1) = guhua.at<double>(x +1, y - 1);
		return dfs(guhua, x + 1, y - 1, w, l, line_panduan, image_pixel);
	}
}
//

void dfs2(Mat guhua, int x, int y, int w, int l, int line_panduan, Mat image_pixel){
	if (x >= 0 && x<l && y >= 0 && y+1 <w && guhua.at<double>(x, y + 1)>0){
		image_pixel.at<double>(line_panduan, y + 1) = guhua.at<double>(x, y + 1);
		return dfs2(guhua, x, y + 1, w, l, line_panduan, image_pixel);
	}
	else if (x - 2 >= 0 && x<l && y >= 0 && y + 1<w && guhua.at<double>(x - 2, y + 1)>yuzhi){
		image_pixel.at<double>(line_panduan, y + 1) = guhua.at<double>(x - 2, y + 1);
		return dfs2(guhua, x - 2, y + 1, w, l, line_panduan, image_pixel);
	}
	else if (x - 1 >= 0 && x<l && y >= 0 && y + 1<w && guhua.at<double>(x - 1, y + 1)>yuzhi){
		image_pixel.at<double>(line_panduan, y + 1) = guhua.at<double>(x - 1, y + 1);
		return dfs2(guhua, x - 1, y + 1, w, l, line_panduan, image_pixel);
	}
	else if (x >= 0 && x + 2<l && y >= 0 && y + 1<w && guhua.at<double>(x + 2, y + 1)>yuzhi){
		image_pixel.at<double>(line_panduan, y + 1) = guhua.at<double>(x + 2, y + 1);
		return dfs2(guhua, x + 2, y + 1, w, l, line_panduan, image_pixel);
	}
	else if (x >= 0 && x + 1 < l && y >= 0 && y + 1 < w && guhua.at<double>(x + 1, y + 1) > yuzhi){
		image_pixel.at<double>(line_panduan, y + 1) = guhua.at<double>(x + 1, y + 1);
		return dfs2(guhua, x + 1, y + 1, w, l, line_panduan, image_pixel);
	}
}
//---------------------------------------------------





//-------------------------------------------------------
BOOL Cchair3DDlg::SetWindowPixelFormat(HDC hDC)
{
	PIXELFORMATDESCRIPTOR pixelDesc;
	pixelDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixelDesc.nVersion = 1;
	pixelDesc.dwFlags = PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER |
		PFD_TYPE_RGBA;
	pixelDesc.iPixelType = PFD_TYPE_RGBA;
	pixelDesc.cColorBits = 32;
	pixelDesc.cRedBits = 0;
	pixelDesc.cRedShift = 0;
	pixelDesc.cGreenBits = 0;
	pixelDesc.cGreenShift = 0;
	pixelDesc.cBlueBits = 0;
	pixelDesc.cBlueShift = 0;
	pixelDesc.cAlphaBits = 0;
	pixelDesc.cAlphaShift = 0;
	pixelDesc.cAccumBits = 0;
	pixelDesc.cAccumRedBits = 0;
	pixelDesc.cAccumGreenBits = 0;
	pixelDesc.cAccumBlueBits = 0;
	pixelDesc.cAccumAlphaBits = 0;
	pixelDesc.cDepthBits = 0;
	pixelDesc.cStencilBits = 1;
	pixelDesc.cAuxBuffers = 0;
	pixelDesc.iLayerType = PFD_MAIN_PLANE;
	pixelDesc.bReserved = 0;
	pixelDesc.dwLayerMask = 0;
	pixelDesc.dwVisibleMask = 0;
	pixelDesc.dwDamageMask = 0;
	PixelFormat = ChoosePixelFormat(hDC, &pixelDesc);
	if (PixelFormat == 0) // Choose default 
	{
		PixelFormat = 1;
		if (DescribePixelFormat(hDC, PixelFormat,
			sizeof(PIXELFORMATDESCRIPTOR), &pixelDesc) == 0)
		{
			return FALSE;
		}
	}
	if (SetPixelFormat(hDC, PixelFormat, &pixelDesc) == FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL Cchair3DDlg::CreateViewGLContext(HDC hDC)
{
	hrenderRC = wglCreateContext(hDC);
	if (hrenderRC == NULL)
		return FALSE;
	if (wglMakeCurrent(hDC, hrenderRC) == FALSE)
		return FALSE;
	return TRUE;
}

void Cchair3DDlg::RenderScene()
{

	///////////////////////////////////////////////// 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -6.0f);      // Move Left 1.5 Units And Into The Screen 6.0 
	glRotated(m_yRotate, 0.0, 1.0, 0.0);
	glBegin(GL_TRIANGLES); // Drawing Using Triangles 

	//float x, y, z;
	//glPointSize(1.0);
	//glBegin(GL_POINTS);//GL_POINTS  
	//ifstream file("out.txt");
	//while (file){
	//	file >> x >> y >> z;
	//	x = (x - 512) / 200;
	//	y = (y - 484) / 200;//2
	//	z = (255 - z) / 300;
	//	glColor3f(1, 0, 0);
	//	glVertex3f(x, y, z);//�����ľ���ֵֻ��С��1������1��û����ʾ
	//}

	//int len = sizeof(result) / sizeof(int);
	//int h1 = sizeof(result[0]) / sizeof(int);
	//int w1 = len / h1;
	for (int i = 0; i < numofLine;i++){
		for (int j = 0; j < width;j++){
			if (result[i][j]>0){
				double z = result[i][j];
				i = (i - 512) / 200;
				j = (j - 484) / 200;//2
				z = (255 - z) / 300;
				glColor3f(1, 0, 0);
				glVertex3f(i, j, z);//�����ľ���ֵֻ��С��1������1��û����ʾ
			}
		}
	}
//	glVertex3f(1, 1, 1); glVertex3f(1,0, 0); glVertex3f(0, 0, 1); glVertex3f(1, 0, 1);
	glEnd();
	SwapBuffers(m_hDC);//------
}
void Cchair3DDlg::OnTimer(UINT_PTR nIDEvent) //ʵʱ���Ƴ��� //  UINT nIDEvent
{
	// TODO: Add your message handler code here and/or call default 
	RenderScene(); 
	
	m_yRotate += 3;
	CDialog::OnTimer(nIDEvent);
}

BOOL Cchair3DDlg::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	wglDeleteContext(hrenderRC);
	return CDialog::DestroyWindow();
}
//�����߹Ǽܻ�
 Mat Cchair3DDlg::deal(Mat mat){
	threshold(mat, mat, yuzhi, 255, CV_THRESH_BINARY); 
	medianBlur(mat, mat, 3);//��ֵ�˲�
	dilate(mat, mat, (5, 5));//����

	Mat out(height,width, CV_32FC1, Scalar(0));//����������

	int w = width, l = height;
	int isLaser, up, buttom;
	for (int j = 0; j < w; j++)//1292
	{
		isLaser = 0;
		up = 0;
		buttom = 0;
		for (int i = 0; i <l; i++)//l:964
		{
			if (mat.at<uchar>(i, j) > yuzhi && !isLaser)
			{
				isLaser = 1;
				up = i;
			}
			else if (mat.at<uchar>(i, j) <yuzhi && isLaser)
			{
				buttom = i - 1;
				isLaser = 0;                                  
				out.at<double>((up + buttom) / 2, j) = (up + buttom) / 2.0;
			}
		}
	}
	return out;
}

void Cchair3DDlg::OnBnClickedMesh()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	char cal[MAX_PATH];
	_getcwd(cal, MAX_PATH);
	string file1 = cal; //ʵ��ɼ�ͼֻ��һ��
	string base0 = file1 + "\\����\\0.bmp";
	string read1 = file1 + "\\����\\1.bmp";

	Mat base_0 = imread(base0,0);//0����Ҷ�ͼ��
	Mat imgRoi = imread(read1, 0);//0����Ҷ�ͼ��

	if (base_0.empty()){
		MessageBox("���Ȳɼ���׼ƽ��ͼ�񲢱���Ϊ0.bmp");
		return;
	}
	if (imgRoi.empty()){
		MessageBox("�ɼ�ͼ��Ϊ��");
		return;
	}

	int w = width, l = height;
//	////	int w = 1292, l = 964;//col ����w=1292

	ofstream of;
	of.open("out.txt");
	Mat base = deal(base_0);								//�����߹Ǽܻ�
	Mat image = deal(imgRoi);								
	Mat base_pixel(numofLine, width, CV_32FC1, Scalar(0));
	Mat image_pixel(numofLine, width, CV_32FC1, Scalar(0));					//�������������������ƥ������
	for (int j = 0; j < w;j++){
		int line = 0;
		for (int i = 0; i < l;i++){
			if (base.at<double>(i,j)>0){
				base_pixel.at<double>(line,j) = base.at<double>(i, j);
				line++;
			}
		}

	}
	//Ѱ��start
	int start;
	for (int j = 0; j < w; j++)//1292
	{
		int line_panduan = 0;//����ƥ��line
		for (int i = 0; i <l; i++)//l:964
		{
			if (image.at<double>(i,j) > 0){												///erroe
				++line_panduan;
			}
		}
		if (line_panduan == numofLine){ start = j; break; }
	}

	////dfs
	int line_panduan = 0;
	for (int i = 0; i <l; i++){
		if (image.at<float>(i, start) > 0){	
			if (line_panduan <= numofLine){
				int k = i; int k2 = i;													
				dfs(image, k, start, w, l, line_panduan, image_pixel);						//left
				dfs2(image, k2, start, w, l, line_panduan, image_pixel);					//right
			}
			++line_panduan;
		}
	}

	//��ά����
	for (int i = 0; i < numofLine;i++){
		for (int j = 0; j < w && image_pixel.at<double>(i,j)>0;j++){					 //�������ڶ��ѵ����
			int pixel = abs(image_pixel.at<double>(i,j) - base_pixel.at<double>(i,j));                     //���ز�
			double h = len_base * pixel * size_pixel* 1 / ( F * 0.6 - pixel * size_pixel * cos(angle));      //sin(angle)=0.6ֱ��ʽ�������Ƿ���������ƫ������Ӧ�����ֵ                                            //ֱ��ʽ�������Ƿ�  �޸�
			int x = base.at<double>(i,j);
			result[x][j] = h;										    //x��ֵΪ�������ȡ-----��
			of << x << " " << j << " " << h << endl;					//
		}
	}

	//////////////////////��ά��ʾ  OpenGL  //////////////////////////////////////

	CWnd *m_pWnd = GetDlgItem(IDC_STATIC_SHOW_FRAME);
	m_hDC = ::GetDC(m_pWnd->m_hWnd);
	if (SetWindowPixelFormat(m_hDC) == FALSE)
		return;
	if (CreateViewGLContext(m_hDC) == FALSE)
		return;

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
	/////////////////////////////////////////// 
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glViewport(0, 0, 300, 330);//����λ�úʹ�С  259, 231
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();//���õ�ǰָ���ľ���Ϊ��λ����.
	gluPerspective(60, 1, 0.1, 100.0);  //�۲���Ӿ�������������ϵ�еľ����С
	glMatrixMode(GL_MODELVIEW);//ָ����һ�������ǵ�ǰ����
	glLoadIdentity();
	glShadeModel(GL_SMOOTH);       // Enable Smooth Shading 
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);    // Black Background 
	glClearDepth(1.0f);         // Depth Buffer Setup --------
	glEnable(GL_DEPTH_TEST);       // Enables Depth Testing 
	glDepthFunc(GL_LEQUAL); // The Type Of Depth Testing To Do 
	///////////////////////////////////////////////////////////////////////// 
	glEnableClientState(GL_VERTEX_ARRAY);//������ɫ����
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	SetTimer(1, 10, 0);

}

//----------------------------------------------
//							�����ȼ���
//----------------------------------------------
void Cchair3DDlg::OnBnClickedprofile()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void Cchair3DDlg::OnEnChangeEdit3()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}



