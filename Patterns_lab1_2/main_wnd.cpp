#include "main_wnd.h"

BEGIN_MESSAGE_MAP(CMainWnd, CFrameWnd)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_GENERATE_SIMPLE_MATRIX, IDC_CHECK_BOX_BORDER, OnAllBtnsClick)
	ON_WM_PAINT()
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

void CMainWnd::OnPaint() {
	CPaintDC dc(this);
	window_drawer->set(&dc);
	switch(drawing_matrix) {
	case 1:
		simple_matrix->set_drawer(window_drawer);
		simple_matrix->draw();
		break;
	case 2:
		sparse_matrix->set_drawer(window_drawer);
		sparse_matrix->draw();
		break;
	case 3:
		goriz_group_matrix->set_drawer(window_drawer);
		goriz_group_matrix->draw();
		break;
	}
	window_drawer->set(nullptr);
}

CMainWnd::CMainWnd()
{
	Create(NULL, L"Patterns lab 1-2", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, CRect(240, 270, 1510, 850),
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

	btn_renumber = new CButton();
	if (btn_renumber != NULL) btn_renumber->Create(L"Renumber", WS_CHILD | WS_VISIBLE | SS_CENTER,
		CRect(50, 140, 200, 180), this, IDC_BUTTON_RENUMBER);
	btn_renumber->SetFont(m_pFont);

	btn_restore = new CButton();
	if (btn_restore != NULL) btn_restore->Create(L"Restore", WS_CHILD | WS_VISIBLE | SS_CENTER,
		CRect(50, 190, 200, 230), this, IDC_BUTTON_RESTORE);
	btn_restore->SetFont(m_pFont);

	btn_gnrt_grp_mtrx = new CButton();
	if (btn_gnrt_grp_mtrx != NULL) btn_gnrt_grp_mtrx->Create(L"Generate Group Matrix", WS_CHILD | WS_VISIBLE | SS_CENTER,
		CRect(50, 240, 200, 280), this, IDC_BUTTON_GENERATE_GROUP_MATRIX);
	btn_gnrt_grp_mtrx->SetFont(m_pFont);

	check_box_brdr = new CButton();
	check_box_brdr->Create(L"visible border", WS_VISIBLE | BS_AUTOCHECKBOX | WS_CHILD, CRect(70, 60, 220, 80), this, IDC_CHECK_BOX_BORDER);
	check_box_brdr->SetFont(m_pFont);

	console_drawer = ConsoleDrawer<int>::init();
	window_drawer = WindowDrawer<int>::init();
	if (check_box_brdr->GetCheck() == FALSE) {
		console_drawer->set(' ');
		window_drawer->set(bool(FALSE));
	}
	html_drawer = HtmlDrawer<int>::init(HtmlDrawer<int>::default_name_file);

	simple_matrix = NULL;
	sparse_matrix = NULL;
	prev_matrix = NULL;
	decorator_matrix = NULL;
	goriz_group_matrix = NULL;
}

CMainWnd::~CMainWnd()
{
	if (m_pFont != NULL) delete m_pFont;
	if (btn_gnrt_smpl_mtrx != NULL) delete btn_gnrt_smpl_mtrx;
	if (btn_gnrt_sprs_mtrx != NULL) delete btn_gnrt_sprs_mtrx;
	if (check_box_brdr != NULL) delete check_box_brdr;
	if (btn_gnrt_grp_mtrx != NULL) delete btn_gnrt_grp_mtrx;
	if (simple_matrix != NULL) delete simple_matrix;
	if (sparse_matrix != NULL) delete sparse_matrix;
	if (btn_renumber != NULL) delete btn_renumber;
	if (btn_restore != NULL) delete btn_restore;
	if (decorator_matrix != NULL) delete decorator_matrix;
	if (goriz_group_matrix != NULL) delete goriz_group_matrix;
}

void CMainWnd::OnAllBtnsClick(unsigned int BttId) {
	switch (BttId) {
	case IDC_BUTTON_GENERATE_SIMPLE_MATRIX:
		if (simple_matrix != NULL) delete simple_matrix;
		simple_matrix = new SimpleMatrix<int>(3, 3);
		MatrixInitiator<int>::fill_matrix(simple_matrix, 5, 1000);
		simple_matrix->set_drawer(console_drawer);
		//console//
		system("cls");
		simple_matrix->draw();
		prev_matrix = simple_matrix;
		//window//
		drawing_matrix = 1;
		this->Invalidate();
		//html file//
		simple_matrix->set_drawer(html_drawer);
		simple_matrix->draw();
		html_drawer->reopen_file();
		//for_decorator//
		delete decorator_matrix;
		decorator_matrix = new ChangeNumerationMatrix<int>(simple_matrix);
		decorator_matrix->set_drawer(console_drawer);
		break;
	case IDC_BUTTON_GENERATE_SPARSE_MATRIX:
		if (sparse_matrix != NULL) delete sparse_matrix;
		sparse_matrix = new SparseMatrix<int>(5, 5);
		MatrixInitiator<int>::fill_matrix(sparse_matrix, 20, 100);
		sparse_matrix->set_drawer(console_drawer);
		//console//
		system("cls");
		sparse_matrix->draw();
		prev_matrix = sparse_matrix;
		//window//
		drawing_matrix = 2;
		this->Invalidate();
		//html file//
		sparse_matrix->set_drawer(html_drawer);
		sparse_matrix->draw();
		html_drawer->reopen_file();
		//for_decorator//
		delete decorator_matrix;
		decorator_matrix = new ChangeNumerationMatrix<int>(sparse_matrix);
		decorator_matrix->set_drawer(console_drawer);
		break;
	case IDC_BUTTON_RENUMBER:
		if (prev_matrix == NULL) {
			AfxMessageBox(L"Please, generate some matrix");
			return;
		}
		decorator_matrix->renumber_columns(rand() % decorator_matrix->count_columns(),
			rand() % decorator_matrix->count_columns());
		decorator_matrix->renumber_rows(rand() % decorator_matrix->count_rows(),
			rand() % decorator_matrix->count_rows());
		system("cls");
		decorator_matrix->draw();
		break;
	case IDC_BUTTON_RESTORE:
		if (decorator_matrix == NULL) AfxMessageBox(L"Sorry, nothing to restore");
		else {
			system("cls");
			decorator_matrix->restore();
			decorator_matrix->draw();
		}
		break;
	case IDC_BUTTON_GENERATE_GROUP_MATRIX:
		delete goriz_group_matrix;
		goriz_group_matrix = new GorizontalGroupMatrix<int>({
			MatrixInitiator<int>::fill_matrix(new SimpleMatrix<int>(2, 2), 4, 10),
			MatrixInitiator<int>::fill_matrix(new SimpleMatrix<int>(3, 3), 9, 10),
			MatrixInitiator<int>::fill_matrix(new SimpleMatrix<int>(5, 1), 5, 100),
			MatrixInitiator<int>::fill_matrix(new SimpleMatrix<int>(1, 1), 1, 10)
		});
		//console//
		goriz_group_matrix->set_drawer(console_drawer);
		system("cls");
		goriz_group_matrix->draw();
		prev_matrix = goriz_group_matrix;
		//window//
		drawing_matrix = 3;
		this->Invalidate();
		//for_decorator//
		delete decorator_matrix;
		decorator_matrix = new ChangeNumerationMatrix<int>(goriz_group_matrix);
		decorator_matrix->set_drawer(console_drawer);
		break;
	case IDC_CHECK_BOX_BORDER:
			if (check_box_brdr->GetCheck() == TRUE) {
				console_drawer->set('q');
				window_drawer->set(bool(TRUE));
			}
			else {
				console_drawer->set(' ');
				window_drawer->set(bool(FALSE));
			}
		if (prev_matrix != NULL) {
			console_drawer->draw_border(prev_matrix->count_rows(), prev_matrix->count_columns());
			this->Invalidate();
		}
		break;
	}
}