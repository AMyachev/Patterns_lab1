#include "main_wnd.h"

BEGIN_MESSAGE_MAP(CMainWnd, CFrameWnd)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_GENERATE_SIMPLE_MATRIX, IDC_CHECK_BOX_BORDER, OnAllBtnsClick)
END_MESSAGE_MAP()

void CMainWnd::SetClientFont(CString Typeface,
	int Size, BOOL Bold, BOOL Italic)
{
	CWindowDC winDC(this);
	int pixelsPerInch = winDC.GetDeviceCaps(LOGPIXELSY);
	int fontHeight = -MulDiv(Size, pixelsPerInch, 72);
	int Weight = FW_NORMAL;
	if (Bold)
		Weight = FW_BOLD;
	delete m_pFont;
	m_pFont = new CFont;
	m_pFont->CreateFont(fontHeight, 0, 0, 0, Weight, Italic, 0, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, Typeface);
}

CMainWnd::CMainWnd()
{
	Create(NULL, L"Patterns lab 1-2", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, CRect(240, 270, 510, 450),
		NULL, NULL);
	this->SetClientFont("Times New Roman", 12, FALSE, FALSE);

	btn_gnrt_smpl_mtrx = new CButton();
	if (btn_gnrt_smpl_mtrx != NULL) btn_gnrt_smpl_mtrx->Create(L"Generate Simple Matrix", WS_CHILD | WS_VISIBLE | SS_CENTER,
		CRect(50, 10, 200, 50), this, IDC_BUTTON_GENERATE_SIMPLE_MATRIX);
	btn_gnrt_smpl_mtrx->SetFont(m_pFont);

	btn_gnrt_sprs_mtrx = new CButton();
	if (btn_gnrt_sprs_mtrx != NULL) btn_gnrt_sprs_mtrx->Create(L"Generate Sparse Matrix", WS_CHILD | WS_VISIBLE | SS_CENTER,
		CRect(50, 90, 200, 130), this, IDC_BUTTON_GENERATE_SPARSE_MATRIX);
	btn_gnrt_sprs_mtrx->SetFont(m_pFont);

	check_box_brdr = new CButton();
	check_box_brdr->Create(L"visible border", WS_VISIBLE | BS_AUTOCHECKBOX | WS_CHILD, CRect(70, 60, 220, 80), this, IDC_CHECK_BOX_BORDER);
	check_box_brdr->SetFont(m_pFont);

	console_drawer = ConsoleDrawer<int>::init();
}

CMainWnd::~CMainWnd()
{
	if (m_pFont != NULL) delete m_pFont;
	if (btn_gnrt_smpl_mtrx != NULL) delete btn_gnrt_smpl_mtrx;
	if (btn_gnrt_sprs_mtrx != NULL) delete btn_gnrt_sprs_mtrx;
	if (check_box_brdr != NULL) delete check_box_brdr;
}

void CMainWnd::OnAllBtnsClick(unsigned int BttId) {
	SimpleMatrix<int>* simple_matrix = new SimpleMatrix<int>(9, 9);
	SparseMatrix<int>* sparse_matrix = new SparseMatrix<int>(5, 5);
	switch (BttId) {
	case IDC_BUTTON_GENERATE_SIMPLE_MATRIX:
		MatrixInitiator<int>::fill_matrix(simple_matrix, 50, 1000);
		simple_matrix->set(console_drawer);
		system("cls");
		simple_matrix->draw();
		break;
	case IDC_BUTTON_GENERATE_SPARSE_MATRIX:
		MatrixInitiator<int>::fill_matrix(sparse_matrix, 20, 100);
		sparse_matrix->set(console_drawer);
		system("cls");
		sparse_matrix->draw();
		break;
	case IDC_CHECK_BOX_BORDER:
		if (check_box_brdr->GetCheck() == TRUE) //TODO do drawing on signal  
		AfxMessageBox(L"Border");
		break;
	}
	//delete simple_matrix; fix memory leak
	//delete sparse_matrix;
}