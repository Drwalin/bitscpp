/*
 *  This file is part of bitscpp.
 *  Copyright (C) 2023 Marek Zalewski aka Drwalin
 *
 *  bitscpp is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  bitscpp is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef BITSCPP_BYTE_WRITER_HPP
#define BITSCPP_BYTE_WRITER_HPP

#include <cstdint>
#include <cstring>

#include <string>
#include <string_view>
#include <vector>

#include "Endianness.hpp"

namespace bitscpp {
	
	template<typename BT>
	class ByteWriter;
	
	template<typename BT, typename T>
	inline ByteWriter<BT>& op(ByteWriter<BT>& writer, const T& data) {
		(*(T*)&data).__ByteStream_op(writer);
		return writer;
	}
	
	namespace impl {
		template<typename BT, typename T>
		static inline ByteWriter<BT>& __op_ptr(ByteWriter<BT>& writer, T*const data) {
			op(writer, *data);
			return writer;
		}
		
		template<typename BT, typename T>
		static inline ByteWriter<BT>& __op_ref(ByteWriter<BT>& writer, const T& data) {
			op(writer, data);
			return writer;
		}
	}
	
	/*
	 * BT requires following signature:
	 * class BT {
	 *   uint8_t *data();
	 *   size_t (*size)();
	 *   void (*resize)(size_t newSize);
	 *   size_t (*capacity)();
	 *   void (*reserve)(size_t newCapacity);
	 * };
	 * 
	 * behavior should be similar to std::vector<uint8_t>
	 */
	template<typename BT = std::vector<uint8_t>>
	class ByteWriter {
	public:
		
		template<typename T>
		inline ByteWriter& op(T*const data) {
			impl::__op_ptr(*this, data);
			return *this;
		}
		
		template<typename T>
		inline ByteWriter& op(const T& data) {
			impl::__op_ref(*this, data);
			return *this;
		}
		
	public:
		
		inline uint32_t GetSize() const { return _buffer->size(); }
		
		void Init(BT *buffer) {
			this->_buffer = buffer;
			ptr = _buffer->data();
		}
		
		inline ByteWriter(BT &buffer) {
			Init(&buffer);
		}
		
		inline ByteWriter(BT *buffer) {
			Init(buffer);
		}
		
		// NULL-terminated string
		inline ByteWriter& op(const std::string& str);
		inline ByteWriter& op(const std::string_view str);
		inline ByteWriter& op(const char* str);
		// constant size byte array
		template<typename T>
		inline ByteWriter& op(uint8_t* const data, T bytes);
		template<typename T>
		inline ByteWriter& op(int8_t* const data, T bytes);
		
		// uint32_t size preceeds size of binary data
		inline ByteWriter& op(const std::vector<uint8_t>& binary);
		
		
		inline ByteWriter& op(uint16_t v, int bytes);
		inline ByteWriter& op(uint32_t v, int bytes);
		inline ByteWriter& op(uint64_t v, int bytes);
		inline ByteWriter& op(int16_t v,  int bytes) { return op((uint16_t)v, bytes); }
		inline ByteWriter& op(int32_t v,  int bytes) { return op((uint32_t)v, bytes); }
		inline ByteWriter& op(int64_t v,  int bytes) { return op((uint64_t)v, bytes); }
		
		inline ByteWriter& op(bool v);
		inline ByteWriter& op(uint8_t v);
		inline ByteWriter& op(uint16_t v);
		inline ByteWriter& op(uint32_t v);
		inline ByteWriter& op(uint64_t v);
		inline ByteWriter& op(int8_t v);
		inline ByteWriter& op(int16_t v);
		inline ByteWriter& op(int32_t v);
		inline ByteWriter& op(int64_t v);
		inline ByteWriter& op(char v);
		inline ByteWriter& op(long long v);
		
		
		inline ByteWriter& op(float value);
		inline ByteWriter& op(double value);
		
		template<typename Tmin, typename Tmax, typename T>
		inline ByteWriter& op(float value, Tmin min, Tmax max, T bytes);
		template<typename Tmin, typename Tmax, typename T>
		inline ByteWriter& op(double value, Tmin min, Tmax max, T bytes);
		
		template<typename Torig, typename Tmin, typename Tmax, typename T>
		inline ByteWriter& op(float value, Torig origin, Tmin min, Tmax max, T bytes);
		template<typename Torig, typename Tmin, typename Tmax, typename T>
		inline ByteWriter& op(double value, Torig origin, Tmin min, Tmax max, T bytes);
		
	public:
		
		template<typename T>
		inline ByteWriter& op(const T* data, uint32_t elements) {
			_reserve_expand(sizeof(T)*elements + 4);
			for(uint32_t i=0; i<elements; ++i)
				op(data[i]);
			return *this;
		}
		
		template<typename T>
		inline ByteWriter& op(const std::vector<T>& arr) {
			_reserve_expand(sizeof(T)*arr.size()+4);
			op((uint32_t)arr.size());
			return op<T>(arr.data(), arr.size());
		}
		
	private:
		
		inline size_t _expand(size_t bytesToExpand) {
			size_t oldSize = _buffer->size();
			_buffer->resize(oldSize+bytesToExpand);
			ptr = _buffer->data();
			return oldSize;
		}
		
		inline void _reserve_expand(size_t bytesToExpand) {
			_reserve(_buffer->size() + bytesToExpand);
		}
		
		inline void _reserve(size_t newCapacity) {
			size_t oldCapacity = _buffer->capacity();
			if(newCapacity > oldCapacity) {
				_buffer->reserve(newCapacity);
				ptr = _buffer->data();
			}
		}
		
		BT *_buffer;
		uint8_t* ptr;
	};
	
	
	
	template<typename BT>
	inline ByteWriter<BT>& ByteWriter<BT>::op(const std::string& str) {
		return op((int8_t*const)str.data(), str.size()+1);
	}
	
	template<typename BT>
	inline ByteWriter<BT>& ByteWriter<BT>::op(const std::string_view str) {
		size_t offset = _expand(str.size()+1);
		memcpy(ptr+offset, str.data(), str.size());
		ptr[offset+str.size()] = 0;
		offset += str.size()+1;
		return *this;
	}
	
	template<typename BT>
	inline ByteWriter<BT>& ByteWriter<BT>::op(const char* str) {
		ssize_t len = strlen(str);
		return op(std::string_view(str, len));
	}
	
	template<typename BT>
	template<typename T>
	inline ByteWriter<BT>& ByteWriter<BT>::op(uint8_t*const data, T bytes) {
		size_t offset = _expand(bytes);
		memcpy(ptr+offset, data, bytes);
		offset +=bytes;
		return *this;
	}
	
	template<typename BT>
	template<typename T>
	inline ByteWriter<BT>& ByteWriter<BT>::op(int8_t*const data, T bytes) {
		return op((uint8_t*const)data, bytes);
	}
	
	
	template<typename BT>
	inline ByteWriter<BT>& ByteWriter<BT>::op(const std::vector<uint8_t>& binary) {
		_reserve_expand(binary.size() + 4);
		this->op((uint32_t)binary.size());
		size_t offset = _expand(binary.size());
		memcpy(ptr+offset, binary.data(), binary.size());
		offset += binary.size();
		return *this;
	}
	
	
	
	template<typename BT>
	inline ByteWriter<BT>& ByteWriter<BT>::op(uint16_t v, int bytes) {
		size_t offset = _expand(bytes);
		if constexpr (!IsBigEndian()) {
			memcpy(ptr+offset, &v, bytes);
		} else {
			for(int i=0; i<bytes; ++i)
				ptr[offset+i] = v >> (i<<3);
		}
		offset += bytes;
		return *this;
	}
	template<typename BT>
	inline ByteWriter<BT>& ByteWriter<BT>::op(uint32_t v, int bytes) {
		size_t offset = _expand(bytes);
		if constexpr (!IsBigEndian()) {
			memcpy(ptr+offset, &v, bytes);
		} else {
			for(int i=0; i<bytes; ++i)
				ptr[offset+i] = v >> (i<<3);
		}
		offset += bytes;
		return *this;
	}
	template<typename BT>
	inline ByteWriter<BT>& ByteWriter<BT>::op(uint64_t v, int bytes) {
		size_t offset = _expand(bytes);
		if constexpr (!IsBigEndian()) {
			memcpy(ptr+offset, &v, bytes);
		} else {
			for(int i=0; i<bytes; ++i)
				ptr[offset+i] = v >> (i<<3);
		}
		offset += bytes;
		return *this;
	}
	
	
	
	template<typename BT>
	inline ByteWriter<BT>& ByteWriter<BT>::op(uint8_t v) {
		size_t offset = _expand(sizeof(v));
		ptr[offset] = v;
		++offset;
		return *this;
	}
	template<typename BT>
	inline ByteWriter<BT>& ByteWriter<BT>::op(uint16_t v) {
		size_t offset = _expand(sizeof(v));
		v = HostToNetworkUint<uint16_t>(v);
		memcpy(ptr+offset, &v, sizeof(v));
		offset += 2;
		return *this;
	}
	template<typename BT>
	inline ByteWriter<BT>& ByteWriter<BT>::op(uint32_t v) {
		size_t offset = _expand(sizeof(v));
		v = HostToNetworkUint<uint32_t>(v);
		memcpy(ptr+offset, &v, sizeof(v));
		offset += 4;
		return *this;
	}
	template<typename BT>
	inline ByteWriter<BT>& ByteWriter<BT>::op(uint64_t v) {
		size_t offset = _expand(sizeof(v));
		v = HostToNetworkUint<uint64_t>(v);
		memcpy(ptr+offset, &v, sizeof(v));
		offset += 8;
		return *this;
	}
	
	template<typename BT>
	inline ByteWriter<BT>& ByteWriter<BT>::op(bool v)  { return op(v?(uint8_t)1:(uint8_t)0); }
	template<typename BT>
	inline ByteWriter<BT>& ByteWriter<BT>::op(int8_t v)  { return op((uint8_t)v); }
	template<typename BT>
	inline ByteWriter<BT>& ByteWriter<BT>::op(int16_t v) { return op((uint16_t)v); }
	template<typename BT>
	inline ByteWriter<BT>& ByteWriter<BT>::op(int32_t v) { return op((uint32_t)v); }
	template<typename BT>
	inline ByteWriter<BT>& ByteWriter<BT>::op(int64_t v) { return op((uint64_t)v); }
	template<typename BT>
	inline ByteWriter<BT>& ByteWriter<BT>::op(char v) { return op((uint8_t)v); }
	template<typename BT>
	inline ByteWriter<BT>& ByteWriter<BT>::op(long long v) { return op((uint64_t)v); }
	
	
	
	
	
	
	template<typename BT>
	inline ByteWriter<BT>& ByteWriter<BT>::op(float value) {
		return op(*(uint32_t*)&value);
	}
	
	template<typename BT>
	inline ByteWriter<BT>& ByteWriter<BT>::op(double value) {
		return op(*(uint64_t*)&value);
	}
	
		
	template<typename BT>
	template<typename Tmin, typename Tmax, typename T>
	inline ByteWriter<BT>& ByteWriter<BT>::op(float value, Tmin min, Tmax max,
			T bytes) {
		float fmask = (((uint32_t)1)<<(bytes<<3))-1;
		float pv = (value-min) * (fmask / (max-min));
		uint32_t v = pv+0.4f;
		return op(v, bytes);
	}
	
	template<typename BT>
	template<typename Tmin, typename Tmax, typename T>
	inline ByteWriter<BT>& ByteWriter<BT>::op(double value, Tmin min, Tmax max,
			T bytes) {
		double fmask = (((uint64_t)1)<<(bytes<<3))-1ll;
		double pv = (value-min) * (fmask / (max-min));
		uint64_t v = pv+0.4f;
		return op(v, bytes);
	}
	
	template<typename BT>
	template<typename Torig, typename Tmin, typename Tmax, typename T>
	inline ByteWriter<BT>& ByteWriter<BT>::op(float value, Torig origin, Tmin min,
			Tmax max, T bytes) {
		return op(value - origin, min, max, bytes);
	}
	
	template<typename BT>
	template<typename Torig, typename Tmin, typename Tmax, typename T>
	inline ByteWriter<BT>& ByteWriter<BT>::op(double value, Torig origin, Tmin min,
			Tmax max, T bytes) {
		return op(value - origin, min, max, bytes);
	}
} // namespace bitscpp

#endif

