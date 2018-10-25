#pragma once
#include <string>
#include <stdio.h> 
#include <windows.h>
#include <fcntl.h>
#include <io.h> 
#include <iostream>
#include "vector.h"

template <class T> class IDrawer;

class IDrawable {
public:
	virtual void draw() = 0;
};

template <class T> class IMatrix : public IDrawable {
public:
	virtual T get(uint index_row, uint index_col) const = 0;
	virtual bool set(uint index_row, uint index_col, T value) = 0;
	virtual uint count_rows() const = 0;
	virtual uint count_columns() const = 0;
};

template <class T> class SomeMatrix : public IMatrix<T> {
	IDrawer<T>* _drawer;
	std::vector<IVector<T>*> _data;
	uint _count_rows;
	uint _count_col;
protected:
	virtual IVector<T>* create_vector(uint size) = 0;
protected:
	virtual void draw_border() {
		if (_drawer == NULL) throw "Drawer NULL pointer";
		_drawer->draw_border(count_rows(), count_columns());
	}
	virtual void draw_item(int row, int col) {
		if (_drawer == NULL) throw "Drawer NULL pointer";
		_drawer->draw_item(get(row, col), row, col);
	}
public:
	void set(IDrawer<T>* drawer) {
		_drawer = drawer;
	}
	void init(uint count_rows, uint count_columns) {
		_count_rows = count_rows;
		_count_col = count_columns;
		for (uint i = 0; i < count_rows; ++i) {
			_data.push_back(create_vector(count_columns));
		}
	}
	virtual T get(uint index_row, uint index_col) const {
		return _data[index_row]->get(index_col);
	}
	virtual bool set(uint index_row, uint index_col, T value) {
		if (index_row < count_rows())
			if (index_col < count_columns()) {
				_data[index_row]->set(index_col, value);
				return true;
			}
		return false;
	}
	virtual uint count_rows() const{
		return _count_rows;
	}
	virtual uint count_columns() const {
		return _count_col;
	}
	virtual ~SomeMatrix() {
		for (int i = count_rows() - 1; i >= 0; --i) {
			delete _data[i];
		}
	}
};

template <class T> class SimpleMatrix : public SomeMatrix<T> {
protected:
	virtual IVector<T>* create_vector(uint size) {
		return new SimpleVector<T>(size);
	}
public:
	SimpleMatrix(uint count_rows, uint count_columns) {
		SomeMatrix<T>::init(count_rows, count_columns);
	}
	virtual void draw() {
		draw_border();
		for (uint i = 0; i < count_rows(); ++i)
			for (uint j = 0; j < count_columns(); ++j)
				draw_item(i, j);
	}
};

template <class T> class SparseMatrix : public SomeMatrix<T> {
protected:
	virtual IVector<T>* create_vector(uint size) {
		return new SparseVector<T>(size);
	}
public:
	SparseMatrix(uint count_rows, uint count_columns) {
		SomeMatrix<T>::init(count_rows, count_columns);
	}
	virtual void draw() {
		draw_border();
		for (uint i = 0; i < count_rows(); ++i)
			for (uint j = 0; j < count_columns(); ++j)
				if (get(i, j) != T()) draw_item(i, j);
	}
};

template <class T> class IDrawer {
public:
	virtual void draw_border(uint count_rows, uint count_columns) = 0;
	virtual void draw_item(T elem, uint row, uint col) = 0;
};

template <class T> class ConsoleDrawer : public IDrawer<T> {
	uint _x_first_shift = 3;
	uint _y_first_shift = 3;
	char _symbol_for_border = 'q';
	ConsoleDrawer() {
		// see https://stackoverflow.com/questions/191842/how-do-i-get-console-output-in-c-with-a-windows-program
		// Excellent solution by man with nickname 'Sev'
		// Create a console for this application
		AllocConsole();

		// Get STDOUT handle
		HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		int SystemOutput = _open_osfhandle(intptr_t(ConsoleOutput), _O_TEXT);
		FILE *COutputHandle = _fdopen(SystemOutput, "w");

		// Get STDERR handle
		HANDLE ConsoleError = GetStdHandle(STD_ERROR_HANDLE);
		int SystemError = _open_osfhandle(intptr_t(ConsoleError), _O_TEXT);
		FILE *CErrorHandle = _fdopen(SystemError, "w");

		// Get STDIN handle
		HANDLE ConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
		int SystemInput = _open_osfhandle(intptr_t(ConsoleInput), _O_TEXT);
		FILE *CInputHandle = _fdopen(SystemInput, "r");

		//make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console as well
		std::ios::sync_with_stdio(true);

		// Redirect the CRT standard input, output, and error handles to the console
		freopen_s(&CInputHandle, "CONIN$", "r", stdin);
		freopen_s(&COutputHandle, "CONOUT$", "w", stdout);
		freopen_s(&CErrorHandle, "CONOUT$", "w", stderr);

		//Clear the error state for each of the C++ standard stream objects. We need to do this, as
		//attempts to access the standard streams before they refer to a valid target will cause the
		//iostream objects to enter an error state. In versions of Visual Studio after 2005, this seems
		//to always occur during startup regardless of whether anything has been read from or written to
		//the console or not.
		std::wcout.clear();
		std::cout.clear();
		std::wcerr.clear();
		std::cerr.clear();
		std::wcin.clear();
		std::cin.clear();
		//////////////////////////////////////////
	}
public:
	void set(char symbol_for_border) {
		_symbol_for_border = symbol_for_border;
	}
	static ConsoleDrawer<T>* init() {
		static ConsoleDrawer<T> _drawer;
		return &_drawer;
	}
	virtual void draw_border(uint count_rows, uint count_columns) {
		COORD position;
		position.X = _x_first_shift;
		position.Y = _y_first_shift;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
		std::cout.width(1);

		char top_bottom_border = '-';
		char left_right_border = '|';
		if (_symbol_for_border != 'q') {
			top_bottom_border = _symbol_for_border;
			left_right_border = _symbol_for_border;
		}

		std::cout << std::string(8 * count_columns + 2, top_bottom_border);
		for (uint i = 0; i < count_rows; ++i) {
			position.X = _x_first_shift;
			position.Y = _y_first_shift + 1 + i;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
			std::cout << left_right_border;
			position.X += 8 * count_rows + 1;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
			std::cout << left_right_border;
		}
		position.X = _x_first_shift;
		position.Y += 1;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
		std::cout << std::string(8 * count_columns + 2, top_bottom_border);
	}
	virtual void draw_item(T elem, uint row, uint col) {
		COORD position;
		position.X = _x_first_shift + 8* col + 1;
		position.Y = _y_first_shift + row + 1;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
		std::cout.width(8);
		std::cout << elem;
	}
};
