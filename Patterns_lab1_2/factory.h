#pragma once
#include "matrix.h"

#define NEW_ADD_AND_RETURN(type, ...)                       \
{                                                           \
    type##* matrix_name = new type(##__VA_ARGS__);          \
	add(matrix_name);                                       \
    return(matrix_name);                                    \
}

template <class T> class AbstractMatrixFactory {
	std::vector<IMatrix<T>*> _collector;
protected:
	void add(IMatrix<T>* matrix);
public:
	virtual SimpleMatrix<T>* create_simple_matrix(uint count_rows, uint count_columns) = 0;
	virtual SparseMatrix<T>* create_sparse_matrix(uint count_rows, uint count_columns) = 0;
	//decorators//
	virtual NullMatrix<T>*             create_null_matrix              (IMatrix<T>* _decor_matrix) = 0;
	virtual ChangeNumerationMatrix<T>* create_change_numeration_matrix (IMatrix<T>* _decor_matrix) = 0;
	//composites//
	virtual GorizontalGroupMatrix<T>* create_gorizontal_group_matrix (std::vector<IMatrix<T>*> group) = 0;
	virtual VerticalGroupMatrix<T>*   create_vertical_group_matrix   (std::vector<IMatrix<T>*> group) = 0;

	virtual ~AbstractMatrixFactory();
};
template <class T> class MatrixFactory: public AbstractMatrixFactory<T> {
public:
	static AbstractMatrixFactory<T>* init();

	virtual SimpleMatrix<T>* create_simple_matrix(uint count_rows, uint count_columns);
	virtual SparseMatrix<T>* create_sparse_matrix(uint count_rows, uint count_columns);
	//decorators//
	virtual NullMatrix<T>*             create_null_matrix              (IMatrix<T>* _decor_matrix);
	virtual ChangeNumerationMatrix<T>* create_change_numeration_matrix (IMatrix<T>* _decor_matrix);
	//composites//
	virtual GorizontalGroupMatrix<T>* create_gorizontal_group_matrix (std::vector<IMatrix<T>*> group);
	virtual VerticalGroupMatrix<T>*   create_vertical_group_matrix   (std::vector<IMatrix<T>*> group);
};

/////////////////////AbstractMatrixFactory implementation//////////////////////////
template<class T> void AbstractMatrixFactory<T>::add(IMatrix<T>* matrix) {
	_collector.push_back(matrix);
}
template<class T> AbstractMatrixFactory<T>::~AbstractMatrixFactory() {
	for (int i = (int)_collector.size() - 1; i >= 0; --i)
		delete _collector[i];
}

/////////////////////MatrixFactory implementation//////////////////////////////////
template<class T> SimpleMatrix<T>* MatrixFactory<T>::create_simple_matrix(uint count_rows, uint count_columns) {
	NEW_ADD_AND_RETURN(SimpleMatrix<T>, count_rows, count_columns)
}
template<class T> SparseMatrix<T>* MatrixFactory<T>::create_sparse_matrix(uint count_rows, uint count_columns) {
	NEW_ADD_AND_RETURN(SparseMatrix<T>, count_rows, count_columns)
}
template<class T> ChangeNumerationMatrix<T>* MatrixFactory<T>::create_change_numeration_matrix(IMatrix<T>* _decor_matrix) {
	NEW_ADD_AND_RETURN(ChangeNumerationMatrix<T>, _decor_matrix)
}
template<class T> NullMatrix<T>*             MatrixFactory<T>::create_null_matrix             (IMatrix<T>* _decor_matrix) {
	NEW_ADD_AND_RETURN(NullMatrix<T>, _decor_matrix)
}
template<class T> GorizontalGroupMatrix<T>*  MatrixFactory<T>::create_gorizontal_group_matrix (std::vector<IMatrix<T>*> group) {
	NEW_ADD_AND_RETURN(GorizontalGroupMatrix<T>, group)
}
template<class T> VerticalGroupMatrix<T>*    MatrixFactory<T>::create_vertical_group_matrix   (std::vector<IMatrix<T>*> group) {
	NEW_ADD_AND_RETURN(VerticalGroupMatrix<T>, group)
}
template<class T> AbstractMatrixFactory<T>*  MatrixFactory<T>::init() {
	static MatrixFactory<T> matrix_factory;
	return &matrix_factory;
}

#undef NEW_ADD_AND_RETURN