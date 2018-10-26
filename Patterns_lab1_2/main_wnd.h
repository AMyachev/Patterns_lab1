#pragma once

#include "mfc.h"
#include "matrix.h"
#include "matrix_utility.h"

#define IDC_BUTTON_GENERATE_SIMPLE_MATRIX 100
#define IDC_BUTTON_GENERATE_SPARSE_MATRIX 101
#define IDC_CHECK_BOX_BORDER 102


class CMainWnd : public CFrameWnd
{
public:
	CMainWnd();
	afx_msg void OnAllBtnsClick(unsigned int);
	afx_msg void OnPaint();
	void SetClientFont(CString Typeface,
		int Size, BOOL Bold, BOOL Italic);
	~CMainWnd();
private:
	uint drawing_matrix = 0; // 0 - don't drawing, 1 - drawing simple matrix, 2 - drawing sparse matrix
	CButton* btn_gnrt_smpl_mtrx;
	CButton* btn_gnrt_sprs_mtrx;
	CButton* check_box_brdr;
	CFont* m_pFont;
	ConsoleDrawer<int>* console_drawer;
	WindowDrawer<int>* window_drawer;
	SimpleMatrix<int>* simple_matrix;
	SparseMatrix<int>* sparse_matrix;
	IMatrix<int>* prev_matrix;
	DECLARE_MESSAGE_MAP();
};
