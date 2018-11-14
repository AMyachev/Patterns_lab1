#pragma once

template <class T> class IDrawer {
public:
	virtual void draw_item   (T elem, uint index_row, uint index_col) = 0;
	virtual void draw_border (uint count_rows, uint count_columns) = 0;
};
template <class T> class HtmlDrawer : public IDrawer<T> {
	std::ofstream _fout;
	std::string _name_file;
	uint _prev_row;
	uint _prev_col;
	uint _count_rows;
	uint _count_columns;

	HtmlDrawer(std::string name_file);

	void _end_draw        ();
	void _draw_empty_item (uint row, uint col);
public:
	static std::string    default_name_file;
	static HtmlDrawer<T>* init(std::string name_file);

	virtual void draw_item   (T elem, uint index_row, uint index_col);
	virtual void draw_border (uint count_rows, uint count_columns);
	        void reopen_file ();

	~HtmlDrawer();
};
template <class T> class WindowDrawer : public IDrawer<T> {
	uint _max_count_digits_number = 8;
	uint _x_first_shift = 300;
	uint _y_first_shift = 30;
	uint _height_elem = 25;
	uint _width_elem = 10;
	bool _border = FALSE;
	CPaintDC* _dc;
public:
	static  WindowDrawer<T>* init();

			void set         (bool border);
			void set         (CPaintDC* dc);
	virtual void draw_item   (T elem, uint index_row, uint index_col);
	virtual void draw_border (uint count_rows, uint count_columns);
};
template <class T> class ConsoleDrawer : public IDrawer<T> {
	uint _x_first_shift = 3;
	uint _y_first_shift = 3;
	char _symbol_for_border = 'q';

	ConsoleDrawer  ();
	~ConsoleDrawer ();
public:
	static  ConsoleDrawer<T>* init();

	        void set         (char symbol_for_border);
	virtual void draw_item   (T elem, uint index_row, uint index_col);
	virtual void draw_border (uint count_rows, uint count_columns);
};

/////////////////////ConsoleDrawer implementation//////////////////////////
template <class T>      ConsoleDrawer<T>::ConsoleDrawer  () {
	// see https://stackoverflow.com/questions/191842/how-do-i-get-console-output-in-c-with-a-windows-program
	// Excellent solution by man with nickname 'Sev'
	// Create a console for this application
	AllocConsole();
	system("mode con cols=120 lines=50");
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
template <class T>      ConsoleDrawer<T>::~ConsoleDrawer () {
	FreeConsole();
}
template <class T> void ConsoleDrawer<T>::set            (char symbol_for_border) {
	_symbol_for_border = symbol_for_border;
}
template <class T> void ConsoleDrawer<T>::draw_border    (uint count_rows, uint count_columns) {
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
		position.X += 8 * count_columns + 1;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
		std::cout << left_right_border;
	}
	position.X = _x_first_shift;
	position.Y += 1;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
	std::cout << std::string(8 * count_columns + 2, top_bottom_border);
}
template <class T> void ConsoleDrawer<T>::draw_item      (T elem, uint index_row, uint index_col) {
	COORD position;
	position.X = _x_first_shift + 8 * index_col + 1;
	position.Y = _y_first_shift + index_row + 1;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
	std::cout.width(8);
	std::cout << elem;
}
template <class T> ConsoleDrawer<T>* ConsoleDrawer<T>::init() {
	static ConsoleDrawer<T> _drawer;
	return &_drawer;
}

/////////////////////WindowDrawer implementation///////////////////////////
template <class T> void WindowDrawer<T>::set         (bool border) {
	_border = border;
}
template <class T> void WindowDrawer<T>::set         (CPaintDC* dc) {
	_dc = dc;
}
template <class T> void WindowDrawer<T>::draw_border (uint count_rows, uint count_columns) {
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
		_y_first_shift + _height_elem * count_rows);
	_dc->LineTo(_x_first_shift, _y_first_shift + _height_elem * count_rows);
	_dc->LineTo(_x_first_shift, _y_first_shift);

	_dc->SelectObject(old_pen);
	if (new_pen != nullptr) delete new_pen;
}
template <class T> void WindowDrawer<T>::draw_item   (T elem, uint index_row, uint index_col) {
	std::ostringstream oss;
	oss << elem;
	_dc->TextOutW(_x_first_shift + 3 + _max_count_digits_number * _width_elem * index_col,
		_y_first_shift + 3 + _height_elem * index_row, CString(oss.str().c_str()));
}
template <class T> WindowDrawer<T>* WindowDrawer<T>::init() {
	static WindowDrawer<T> _drawer;
	return &_drawer;
}

/////////////////////HtmlDrawer implementation/////////////////////////////
template <class T>      HtmlDrawer<T>::HtmlDrawer        (std::string name_file) : _fout(name_file, std::ios::out),
_prev_row(0), _prev_col(0), _count_rows(0), _count_columns(0) {
	_fout << "<table>" << std::endl;
	_fout << "<tr> ";
}
template <class T>      HtmlDrawer<T>::~HtmlDrawer       () {
	_end_draw();
}
template <class T> void HtmlDrawer<T>::_draw_empty_item  (uint row, uint col) {
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
template <class T> void HtmlDrawer<T>::_end_draw         () {
	if ((_prev_row != _count_rows) || (_prev_col != _count_columns)) _draw_empty_item(_count_rows - 1, _count_columns);
	_fout << "</table>" << std::endl;
	_fout.close();
}
template <class T> void HtmlDrawer<T>::reopen_file       () {
	_end_draw();
	_fout.open(_name_file, std::ios::out); //TODO fix this open
	_prev_row = 0;
	_prev_col = 0;
	_fout << "<table>" << std::endl;
	_fout << "<tr> ";
}
template <class T> void HtmlDrawer<T>::draw_border       (uint count_rows, uint count_columns) {
	_count_rows = count_rows;
	_count_columns = count_columns;
}
template <class T> void HtmlDrawer<T>::draw_item         (T elem, uint index_row, uint index_col) {
	_draw_empty_item(index_row, index_col);
	_fout << "<td>" << elem << "</td> ";
	_prev_row = index_row;
	_prev_col = index_col;
}
template <class T> HtmlDrawer<T>* HtmlDrawer<T>::init(std::string name_file) {
	static HtmlDrawer<T> _drawer(name_file);
	return &_drawer;
}

template <class T>
std::string HtmlDrawer<T>::default_name_file("E:\\matrix.html");
