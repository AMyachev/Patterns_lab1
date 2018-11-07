#pragma once
#include <string>
#include <stdio.h> 
#include <windows.h>
#include <fcntl.h>
#include <io.h> 
#include <iostream>
#include "vector.h"
#include <sstream>
#include <fstream>
#include <stdlib.h>

template <class T> class IDrawer;

template <class T> class IDrawable {
public:
	virtual void draw() = 0;
	virtual bool set_drawer(IDrawer<T>* drawer) = 0;
};

template <class T> class IMatrix : public IDrawable<T> {
public:
	virtual T get(uint index_row, uint index_col) const = 0;
	virtual bool set(uint index_row, uint index_col, T value) = 0;
	virtual uint count_rows() const = 0;
	virtual uint count_columns() const = 0;
	virtual ~IMatrix() {}
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
	bool set_drawer(IDrawer<T>* drawer) {
		return _drawer = drawer;
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
	virtual void draw_item(T elem, uint index_row, uint index_col) = 0;
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
	~ConsoleDrawer() {
		FreeConsole();
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
	virtual void draw_item(T elem, uint index_row, uint index_col) {
		COORD position;
		position.X = _x_first_shift + 8 * index_col + 1;
		position.Y = _y_first_shift + index_row + 1;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
		std::cout.width(8);
		std::cout << elem;
	}
};

template <class T> class WindowDrawer : public IDrawer<T> {
	uint _x_first_shift = 300;
	uint _y_first_shift = 30;
	uint _height_elem = 25;
	uint _width_elem = 10;
	uint _max_count_digits_number = 8;
	bool _border = FALSE;
	CPaintDC* _dc;

public:
	void set(bool border) {
		_border = border;
	}
	void set(CPaintDC* dc) {
		_dc = dc;
	}
	static WindowDrawer<T>* init() {
		static WindowDrawer<T> _drawer;
		return &_drawer;
	}
	virtual void draw_border(uint count_rows, uint count_columns) {
		CPen* old_pen(nullptr), *new_pen(nullptr);
		if (_border == FALSE) {
			new_pen = new CPen(PS_SOLID, 1, RGB(255, 255, 255));
		}
		else {
			new_pen = new CPen(PS_SOLID, 1, RGB(0, 0, 0));
		}
		old_pen = _dc->SelectObject(new_pen);

		_dc->MoveTo(_x_first_shift, _y_first_shift);
		_dc->LineTo(_x_first_shift + _max_count_digits_number *_width_elem * count_columns, _y_first_shift);
		_dc->LineTo(_x_first_shift + _max_count_digits_number * _width_elem * count_columns,
				_y_first_shift +  _height_elem * count_rows);
		_dc->LineTo(_x_first_shift, _y_first_shift + _height_elem * count_rows);
		_dc->LineTo(_x_first_shift, _y_first_shift);	

		_dc->SelectObject(old_pen);
		if (new_pen != nullptr) delete new_pen;
	}
	virtual void draw_item(T elem, uint index_row, uint index_col) {
		std::ostringstream oss;
		oss << elem;
		_dc->TextOutW(_x_first_shift + 3 + _max_count_digits_number * _width_elem * index_col,
				_y_first_shift + 3 + _height_elem * index_row, CString(oss.str().c_str()));
	}
};

template <class T> class HtmlDrawer: public IDrawer<T> {
	std::ofstream _fout;
	std::string _name_file;
	uint _prev_row;
	uint _prev_col;
	uint _count_rows;
	uint _count_columns;

	HtmlDrawer(std::string name_file) : _fout(name_file, std::ios::out),
			_prev_row(0), _prev_col(0), _count_rows(0), _count_columns(0) {
		_fout << "<table>" << std::endl;
		_fout << "<tr> ";
	}

	void _draw_empty_item(uint row, uint col) {
		if ((row < _prev_row) || ((col < _prev_col) && (row == _prev_row))) throw "matrix elements must be passed in sequence";
		for (uint i = _prev_row; i <= row; ++i) {
			uint begin_col = (i == _prev_row) ? (_prev_col + 1) : 0;
			uint end_col = (i == row) ? col : _count_columns;
			if ((begin_col == 0) && (_prev_col != 0)) _fout << "<tr> ";
			for (uint j = begin_col; j < end_col; ++j) {
				_fout << "<td></td> ";
			}
			if (end_col == _count_columns) _fout << "</tr>" << std::endl;
		}
	}

	void _end_draw() {
		if ((_prev_row != _count_rows) || (_prev_col != _count_columns)) _draw_empty_item(_count_rows - 1, _count_columns);
		_fout << "</table>" << std::endl;
		_fout.close();
	}
public:
	static std::string default_name_file;

	static HtmlDrawer<T>* init(std::string name_file) {
		static HtmlDrawer<T> _drawer(name_file);
		return &_drawer;
	}

	void reopen_file() {
		_end_draw();
		_fout.open(_name_file, std::ios::out); //TODO fix this open
		_prev_row = 0;
		_prev_col = 0;
		_fout << "<table>" << std::endl;
		_fout << "<tr> ";
	}

	virtual void draw_border(uint count_rows, uint count_columns) {
		_count_rows = count_rows;
		_count_columns = count_columns;
	}

	virtual void draw_item(T elem, uint index_row, uint index_col) {
		_draw_empty_item(index_row, index_col);
		_fout << "<td>" << elem << "</td> ";
		_prev_row = index_row;
		_prev_col = index_col;
	}

	~HtmlDrawer() {
		_end_draw();
	}
};

template <class T>
std::string HtmlDrawer<T>::default_name_file("E:\\matrix.html");


template <class T>
class ChangeNumerationMatrix : public IMatrix<T>, IDrawer<T> {
	IDrawer<T>* _drawer;
	IMatrix<T>* _source_matrix;
	std::vector<uint> _changed_row;
	std::vector<uint> _changed_columns;

public:
	ChangeNumerationMatrix(IMatrix<T>* _decor_matrix) : _source_matrix(_decor_matrix) {
		if (_source_matrix == nullptr) throw "matrix pointer is null";
		restore();
	}

	bool set_drawer(IDrawer<T>* drawer) {
		return _drawer = drawer;
	}

	virtual void draw_border(uint count_rows, uint count_columns) {
		_drawer->draw_border(count_rows, count_columns);
	}

	virtual void draw_item(T elem, uint index_row, uint index_col) {
		_drawer->draw_item(elem, _changed_row[index_row], _changed_columns[index_col]);
	}

	virtual void draw() {
		if (_drawer == nullptr) throw "drawer not set";
		_source_matrix->set_drawer(this);
		_source_matrix->draw();
	}

	virtual T get(uint index_row, uint index_col) const {
		return _source_matrix->get(_changed_row[index_row], _changed_columns[index_col]);
	}

	virtual bool set(uint index_row, uint index_col, T value) {
		return _source_matrix->set(_changed_row[index_row], _changed_columns[index_col], value);
	}

	virtual void renumber_rows(uint old_index, uint new_index) {
		if (old_index == new_index) return;
		if (old_index > count_rows()) throw "row index > count rows";
		if (new_index > count_rows()) throw "row index > count rows";
		uint for_swap = _changed_row[old_index];
		_changed_row[old_index] = _changed_row[new_index];
		_changed_row[new_index] = for_swap;
	}

	virtual void renumber_columns(uint old_index, uint new_index) {
		if (old_index == new_index) return;
		if (old_index > count_columns()) throw "column index > count columns";
		if (new_index > count_columns()) throw "column index > count columns";
		uint for_swap = _changed_columns[old_index];
		_changed_columns[old_index] = _changed_columns[new_index];
		_changed_columns[new_index] = for_swap;
	}

	void restore() {
		_changed_row.clear();
		_changed_columns.clear();
		for (uint i = 0; i < _source_matrix->count_rows(); ++i) _changed_row.push_back(i);
		for (uint i = 0; i < _source_matrix->count_columns(); ++i) _changed_columns.push_back(i);
	}

	uint count_rows() const {
		return _source_matrix->count_rows();
	}

	uint count_columns() const {
		return _source_matrix->count_columns();
	}
};