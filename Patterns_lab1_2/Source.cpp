/*#include <iostream>
#include "matrix.h"
#include "matrix_utility.h"

#include <windows.h>
#include <stdio.h> 
#include <io.h> 
#include <fcntl.h> 
CONSOLE_SCREEN_BUFFER_INFO csbi;
HANDLE hStdOutput;
BOOL bUsePause;

using namespace std;

void main() {
	/*SparseMatrix<int> sparse_matrix(3, 3);
	MatrixInitiator<int>::fill_matrix(&sparse_matrix, 6, 1000);
	MatrixStatistics<int, float> sparse_statistics(&sparse_matrix);
	cout << endl << "Sparse matrix -------------------------------------------" << endl << endl;
	cout << "sum: " << sparse_statistics.Sum() << endl;
	cout << "average: " << sparse_statistics.Average() << endl;
	cout << "max value: " << sparse_statistics.Max() << endl;
	cout << "count not zero element: " << sparse_statistics.CountNotZeroElements() << endl;
	cout << endl << "Simple matrix -------------------------------------------" << endl << endl;
	SimpleMatrix<int> simple_matrix(9, 9);
	MatrixInitiator<int>::fill_matrix(&simple_matrix, 50, 1000);
	MatrixStatistics<int, float> simple_statistics(&simple_matrix);
	cout << "sum: " << simple_statistics.Sum() << endl;
	cout << "average: " << simple_statistics.Average() << endl;
	cout << "max value: " << simple_statistics.Max() << endl;
	cout << "count not zero element: " << simple_statistics.CountNotZeroElements() << endl;
	//button down//

	hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	if (!GetConsoleScreenBufferInfo(hStdOutput, &csbi))
	{
		printf("GetConsoleScreenBufferInfo failed: %d\n", GetLastError());
		return;
	}

	// if cursor position is (0,0) then use pause
	bUsePause = ((!csbi.dwCursorPosition.X) &&
		(!csbi.dwCursorPosition.Y));

	printf("Interesting information to read.\n");
	printf("More interesting information to read.\n");

	// only pause if running in separate console window.
	if (bUsePause)
	{
		int ch;
		printf("\n\tPress any key to exit...\n");
		cin >> ch;
	}

	////////////////
	system("pause");
	return;
}*/