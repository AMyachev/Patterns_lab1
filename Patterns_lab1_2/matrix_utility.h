#pragma once
#include <ctime>
#include "matrix.h"

template <class T> class MatrixInitiator {
public:
	static void fill_matrix(IMatrix<T>* matrix, uint count_not_zero_elemets, T max_value)
	{
		uint count_elem = matrix.count_columns() * matrix.count_rows();
		if (count_not_zero_elemets > count_elem) count_not_zero_elemets =
			count_elem;
		srand(time(NULL));
		std::vector<uint> index;
		for (uint i = 0; i < count_elem; ++i) {
			index.push_back(i);
		}
		for (uint i = 0; i < count_not_zero_elemets; ++i) {
			uint index_cell = rand() % index.size();
			uint number_cell = index[index_cell];
			index.erase(index.begin() + index_cell);
			matrix.set(number_cell / matrix.count_rows(),
				number_cell % matrix.count_columns(), T::rand(max_value));
		}
	}
};

//fill_matrix specialization
void MatrixInitiator<int>::fill_matrix(IMatrix<int>* matrix, uint count_not_zero_elemets, int
	max_value) {
	uint count_elem = matrix->count_columns() * matrix->count_rows();
	if (count_not_zero_elemets > count_elem) count_not_zero_elemets = count_elem;
	srand(time(NULL));
	std::vector<uint> index;
	for (uint i = 0; i < count_elem; ++i) {
		index.push_back(i);
	}
	for (uint i = 0; i < count_not_zero_elemets; ++i) {
		uint index_cell = rand() % index.size();
		uint number_cell = index[index_cell];
		index.erase(index.begin() + index_cell);
		matrix->set(number_cell / matrix->count_rows(),
			number_cell % matrix->count_columns(), 1 + rand() % (max_value -
				1));
	}
}

template <class T, class X> class MatrixStatistics {
	T _sum_all_elements;
	X _average;
	T _max;
	uint _count_not_zero_elements;
public:
	MatrixStatistics(IMatrix<T>* matrix) : _sum_all_elements(T()), _average(X()),
		_max(T()), _count_not_zero_elements(0) {
		_max = T();
		for (uint i = 0; i < matrix->count_rows(); ++i) {
			for (uint j = 0; j < matrix->count_columns(); ++j) {
				T temp(matrix->get(i, j));
				if (temp == T()) continue;
				++_count_not_zero_elements;
				_sum_all_elements += temp;
				if (_max < temp) _max = temp;
			}
		}
		_average = _sum_all_elements / _count_not_zero_elements;
	}
	T Sum() const {
		return _sum_all_elements;
	}
	X Average() const {
		return _average;
	}
	T Max() const {
		return _max;
	}
	uint CountNotZeroElements() const {
		return _count_not_zero_elements;
	}
};