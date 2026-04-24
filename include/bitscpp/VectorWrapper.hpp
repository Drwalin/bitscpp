// Copyright (C) 2026 Marek Zalewski aka Drwalin
//
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef BITSCPP_VECTOR_WRAPPER_HPP
#define BITSCPP_VECTOR_WRAPPER_HPP

#include <cstdint>
#include <cstring>

#include <vector>

/*
 * BT_TYPE requires following interface:
 * class BT {
 *   uint8_t *data();
 *   size_t size();
 *   void resize(size_t newSize);
 *   size_t capacity();
 *   void reserve(size_t newCapacity);
 *   void push_back(uint8_t byte);
 *   void write(uint8_t *data, uint32_t bytes);
 * };
 *
 * behavior should be similar to std::vector<uint8_t>
 */

namespace bitscpp
{
class VectorWrapper {
public:
	std::vector<uint8_t> vector;
	
public:
	VectorWrapper(std::vector<uint8_t> &&vec) : vector(std::move(vec)) {}
	VectorWrapper() = default;
	~VectorWrapper() = default;
	VectorWrapper(VectorWrapper &&o) = default;
	VectorWrapper &operator =(VectorWrapper &&o) = default;
	VectorWrapper(VectorWrapper &o) = delete;
	VectorWrapper(const VectorWrapper &o) = delete;
	VectorWrapper &operator=(VectorWrapper &o) = delete;
	VectorWrapper &operator=(const VectorWrapper &o) = delete;
	
	inline uint8_t *data() {
		return vector.data();
	}
	inline size_t size() const {
		return vector.size();
	}
	inline size_t capacity() const {
		return vector.capacity();
	}
	inline void resize(size_t s) {
		vector.resize(s);
	}
	inline void reserve(size_t c) {
		if (capacity() >= c) {
			[[likely]];
			return;
		}
		vector.reserve((c*3)/2+32);
	}
	inline void clear() {
		vector.clear();
	}
	inline void push_back(uint8_t byte) {
		vector.resize(vector.size() + 1);
		vector.back() = byte;
	}
	inline void write(const uint8_t *data, uint32_t bytes) {
		vector.insert(vector.end(), data, data+bytes);
	}
};
}

namespace std {
	inline void swap(bitscpp::VectorWrapper &a, bitscpp::VectorWrapper &b) {
		swap(a.vector, b.vector);
	}
	inline void swap(bitscpp::VectorWrapper &a, std::vector<uint8_t> &b) {
		swap(a.vector, b);
	}
	inline void swap(std::vector<uint8_t> &a, bitscpp::VectorWrapper &b) {
		swap(a, b.vector);
	}
}

#endif
