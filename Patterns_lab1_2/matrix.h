#pragma once
#include <string>
#include <stdio.h> 
#include <windows.h>
#include <fcntl.h>
#include <io.h> 
#include <iostream>
#include "vector.h"
#include "drawer.h"
#include <sstream>
#include <fstream>
#include <stdlib.h>

template <class T> class IDrawable {
public:
	virtual void draw() = 0;
	virtual void set_drawer(IDrawer<T>* drawer) = 0;
};

template <class T> class IMatrix: public IDrawable<T> {
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
	virtual void set_drawer(IDrawer<T>* drawer) {
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
		if ((index_row >= _count_rows) || (index_col >= _count_col) || (index_row < 0) || (index_col < 0)) 
			throw "element with this position not belong matrix";
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
	virtual T get(uint index_row, uint index_col) const {
		if ((index_row >= count_rows()) || (index_col >= count_columns())) throw "element with this position not belong matrix";
		T temp = SomeMatrix<T>::get(index_row, index_col);
		if (T() == temp) throw "sparse element";
		return temp;
	}
	virtual void draw() {
		draw_border();
		for (uint i = 0; i < count_rows(); ++i)
			for (uint j = 0; j < count_columns(); ++j) {
				try {
					draw_item(i, j);
				}
				catch (char*) {
					continue;
				}
			}
	}
};


////////////////decorators////////////////////////////////////////
template <class T> class ChangeNumerationMatrix : public IMatrix<T>{
	IDrawer<T>* _drawer;
	IMatrix<T>* _source_matrix;
	std::vector<uint> _changed_row;
	std::vector<uint> _changed_columns;
public:
	ChangeNumerationMatrix(IMatrix<T>* _decor_matrix) : _source_matrix(_decor_matrix) {
		if (_source_matrix == nullptr) throw "matrix pointer is null";
		restore();
	}

	virtual void set_drawer(IDrawer<T>* drawer) {
		_drawer = drawer;
	}

	virtual void draw() {
		if (_drawer == nullptr) throw "drawer not set";
		_drawer->draw_border(count_rows(), count_columns());
		for (uint i = 0; i < count_rows(); ++i)
			for (uint j = 0; j < count_columns(); ++j) {
				try {
					_drawer->draw_item(get(i,j), i, j);
				}
				catch (char*) {
					continue;
				}
			}
	}

	virtual T get(uint index_row, uint index_col) const {
		if ((index_row >= count_rows()) || (index_col >= count_columns()) || (index_row < 0) || (index_col < 0))
			throw "element with this position not belong matrix";
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

template <class T> class NullMatrix : public IMatrix<T> {
	IDrawer<T>* _drawer;
	IMatrix<T>* _source_matrix;
public:
	NullMatrix(IMatrix<T>* _decor_matrix) : _source_matrix(_decor_matrix) {
		if (_source_matrix == nullptr) throw "matrix pointer is null";
	}

	virtual void set_drawer(IDrawer<T>* drawer) {
		_drawer = drawer;
	}
	virtual void draw() {
		if (_drawer == nullptr) throw "drawer not set";
		_drawer->draw_border(count_rows(), count_columns());
		for (uint i = 0; i < count_rows(); ++i)
			for (uint j = 0; j < count_columns(); ++j) {
				try {
					_drawer->draw_item(get(i, j), i, j);
				}
				catch (char*) {
					continue;
				}
			}
	}

	virtual T get(uint index_row, uint index_col) const {
		if (index_row < index_col) return T();
		return _source_matrix->get(index_row, index_col);
	}

	virtual bool set(uint index_row, uint index_col, T value) {
		return _source_matrix->set(index_row, index_col, value);
	}

	uint count_rows() const {
		return _source_matrix->count_rows();
	}

	uint count_columns() const {
		return _source_matrix->count_columns();
	}
};
//////////////////////////////////////////////////////////////////

template <class T> class GorizontalGroupMatrix : public IMatrix<T> {
	IDrawer<T>* _drawer;
	std::vector<IMatrix<T>*> _group;
	mutable uint _matrix_shift_for_draw;
	uint _count_rows;
	uint _count_columns;

	virtual void _size_adjustment(IMatrix<T>* matrix) {
		_count_columns += matrix->count_columns();
		if (matrix->count_rows() > _count_rows) _count_rows = matrix->count_rows();
	}
	virtual void _adjust_matrix_shift(IMatrix<T>* matrix) const {
		_matrix_shift_for_draw += matrix->count_columns();
	}
public:
	GorizontalGroupMatrix(std::vector<IMatrix<T>*> group) {
		init(group);
	}
	void init(std::vector<IMatrix<T>*> group) {
		_group = group; _count_rows = 0; _count_columns = 0;
		for (uint i = 0; i < group.size(); ++i) {
			_size_adjustment(_group[i]);
		}
	}
	void add(IMatrix<T>* matrix) {
		_group.push_back(matrix);
		_size_adjustment(matrix);
	}

	virtual T get(uint index_row, uint index_col) const {
		_matrix_shift_for_draw = 0;
		if ((index_row >= count_rows()) || (index_col >= count_columns()) || (index_row < 0) || (index_col < 0))
			throw "element with this position not belong matrix";
		for (uint i = 0; i < _group.size(); ++i) {
			try {
				return _group[i]->get(index_row, index_col - _matrix_shift_for_draw);
			}
			catch (char*) {
				_adjust_matrix_shift(_group[i]);
				continue;
			}
		}
		_matrix_shift_for_draw = 0;
		throw "sparse element";
	}

	virtual void draw() {
		if (_drawer == nullptr) throw "drawer not set";
		_drawer->draw_border(_count_rows, _count_columns);
		_matrix_shift_for_draw = 0;
		for (uint i = 0; i < _group.size(); ++i) {
			for (uint j = 0; j < _group[i]->count_rows(); ++j)
				for (uint k = 0; k < _group[i]->count_columns(); ++k) {
					try {
						_drawer->draw_item(_group[i]->get(j, k), j, k + _matrix_shift_for_draw);
					}
					catch (char*) {
						continue;
					}
				}
			_adjust_matrix_shift(_group[i]);
		}
		_matrix_shift_for_draw = 0;
	}
	virtual bool set(uint index_row, uint index_col, T value) {
		_matrix_shift_for_draw = 0;
		if ((index_row >= _count_rows) || (index_col >= _count_columns)) return FALSE;
		for (uint i = 0; i < _group.size(); ++i) {
			if (TRUE == _group[i]->set(index_row, index_col - _matrix_shift_for_draw, value))  return TRUE;
			_adjust_matrix_shift(_group[i]);
		}
		_matrix_shift_for_draw = 0;
		return FALSE;
	}
	virtual uint count_rows() const {
		return _count_rows;
	}
	virtual uint count_columns() const {
		return _count_columns;
	}
	virtual void set_drawer(IDrawer<T>* drawer) {
		_drawer = drawer;
	}
};

template <class T> class VerticalGroupMatrix : public IMatrix<T> {
	IDrawer<T>* _drawer;
	std::vector<IMatrix<T>*> _group;
	mutable uint _matrix_shift_for_draw;
	uint _count_rows;
	uint _count_columns;

	virtual void _size_adjustment(IMatrix<T>* matrix) {
		_count_rows += matrix->count_rows();
		if (matrix->count_columns() > _count_columns) _count_columns = matrix->count_columns();
	}
	virtual void _adjust_matrix_shift(IMatrix<T>* matrix) const {
		_matrix_shift_for_draw += matrix->count_rows();
	}
public:
	VerticalGroupMatrix(std::vector<IMatrix<T>*> group) {
		init(group);
	}
	void init(std::vector<IMatrix<T>*> group) {
		_group = group; _count_rows = 0; _count_columns = 0;
		for (uint i = 0; i < group.size(); ++i) {
			_size_adjustment(_group[i]);
		}
	}
	void add(IMatrix<T>* matrix) {
		_group.push_back(matrix);
		_size_adjustment(matrix);
	}

	virtual T get(uint index_row, uint index_col) const {
		_matrix_shift_for_draw = 0;
		if ((index_row >= count_rows()) || (index_col >= count_columns()) || (index_row < 0) || (index_col < 0))
			throw "element with this position not belong matrix";
		for (uint i = 0; i < _group.size(); ++i) {
			try {
				return _group[i]->get(index_row - _matrix_shift_for_draw, index_col);
			}
			catch (char*) {
				_adjust_matrix_shift(_group[i]);
				continue;
			}
		}
		_matrix_shift_for_draw = 0;
		throw "sparse element";
	}
	virtual void draw() {
		if (_drawer == nullptr) throw "drawer not set";
		_drawer->draw_border(_count_rows, _count_columns);
		_matrix_shift_for_draw = 0;
		for (uint i = 0; i < _group.size(); ++i) {
			for (uint j = 0; j < _group[i]->count_rows(); ++j)
				for (uint k = 0; k < _group[i]->count_columns(); ++k) {
					try {
						_drawer->draw_item(_group[i]->get(j, k), j + _matrix_shift_for_draw, k);
					}
					catch (char*) {
						continue;
					}
				}
			_adjust_matrix_shift(_group[i]);
		}
		_matrix_shift_for_draw = 0;
	}
	virtual bool set(uint index_row, uint index_col, T value) {
		_matrix_shift_for_draw = 0;
		if ((index_row >= _count_rows) || (index_col >= _count_columns)) return FALSE;
		for (uint i = 0; i < _group.size(); ++i) {
			if (TRUE == _group[i]->set(index_row - _matrix_shift_for_draw, index_col, value))  return TRUE;
			_adjust_matrix_shift(_group[i]);
		}
		_matrix_shift_for_draw = 0;
		return FALSE;
	}
	virtual uint count_rows() const {
		return _count_rows;
	}
	virtual uint count_columns() const {
		return _count_columns;
	}
	virtual void set_drawer(IDrawer<T>* drawer) {
		_drawer = drawer;
	}
};