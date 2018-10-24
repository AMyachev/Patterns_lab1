#pragma once
#include <vector>
#include <map>

typedef unsigned int uint;

template <class T> class IVector {
public:
	virtual T get(uint index) const = 0;
	virtual bool set(uint index, T value) = 0;
	virtual uint size() const = 0;
};

template <class T> class SimpleVector : public IVector<T> {
	std::vector<T> _vector;
public:
	SimpleVector(std::vector<T> _vect) : _vector(_vect) {}
	SimpleVector(uint size) : _vector(size) {}
	virtual T get(uint index) const {
		if (index >= size()) throw "Element not found";
		return _vector[index];
	}
	virtual bool set(uint index, T value) {
		if (index < size()) {
			_vector[index] = value;
			return true;
		}
		return false;
	}
	virtual uint size() const {
		return _vector.size();
	}
};

template <class T> class SparseVector : public IVector<T> {
	std::map<uint, T> _map;
	uint _size;
public:
	SparseVector(uint size) : _map(), _size(size) {}
	SparseVector(std::vector<T> _vect) : _size(_vect.size()) {
		for (uint i = 0; i < _size; ++i) {
			if (_vect[i] != 0) {
				_map.insert(make_pair(i, _vect[i]));
			}
		}
	}
	virtual T get(uint index) const {
		for (auto iter = _map.begin(); iter != _map.end(); ++iter) {
			if ((*iter).first < index) {
				continue;
			}
			else if ((*iter).first == index) {
				return (*iter).second;
			}
		}
		return T();
	}
	virtual bool set(uint index, T value) {
		if (index < _size) {
			_map.insert(std::make_pair(index, value));
			return true;
		}
		return false;
	}
	virtual uint size() const {
		return _size;
	}
};