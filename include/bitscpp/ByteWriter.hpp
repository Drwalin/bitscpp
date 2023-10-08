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

#include <cinttypes>
#include <cstring>

#include <string>
#include <string_view>
#include <vector>

#include "Endianness.hpp"

namespace bitscpp {
	
	class ByteWriter;
	
	template<typename T, typename... Args>
	inline ByteWriter& op(ByteWriter& writer, const T& data, Args... args) {
		(*(T*)&data).__ByteStream_op(writer, args...);
		return writer;
	}
	
	namespace impl {
		template<typename T, typename... Args>
		static inline ByteWriter& __op_ptr(ByteWriter& writer, T*const data, Args... args) {
			op(writer, *data, args...);
			return writer;
		}
		
		template<typename T, typename... Args>
		static inline ByteWriter& __op_ref(ByteWriter& writer, const T& data, Args... args) {
			op(writer, data, args...);
			return writer;
		}
	}
	
	class ByteWriter {
	public:
		
		~ByteWriter() {
			buffer.resize(offset);
		}
		
		template<typename T, typename... Args>
		inline ByteWriter& op(T*const data, Args... args) {
			impl::__op_ptr(*this, data, args...);
			return *this;
		}
		
		template<typename T, typename... Args>
		inline ByteWriter& op(const T& data, Args... args) {
			impl::__op_ref(*this, data, args...);
			return *this;
		}
		
	public:
		
		inline ByteWriter(std::vector<uint8_t>& buffer) : buffer(buffer) {
			offset = buffer.size();
			capacity = offset + 200;
			buffer.resize(capacity);
			ptr = buffer.data();
		}
		
		// NULL-terminated string
		inline ByteWriter& op(const std::string& str);
		inline ByteWriter& op(const std::string_view str);
		// constant size byte array
		template<typename T>
		inline ByteWriter& op(uint8_t* const data, T bytes);
		template<typename T>
		inline ByteWriter& op(int8_t* const data, T bytes);
		
		// uint32_t size preceeds size of binary data
		inline ByteWriter& op(const std::vector<uint8_t>& binary);
		
		
		template<typename T>
		inline ByteWriter& op(uint8_t v,  T bytes);
		template<typename T>
		inline ByteWriter& op(uint16_t v, T bytes);
		template<typename T>
		inline ByteWriter& op(uint32_t v, T bytes);
		template<typename T>
		inline ByteWriter& op(uint64_t v, T bytes);
		template<typename T>
		inline ByteWriter& op(int8_t v,   T bytes);
		template<typename T>
		inline ByteWriter& op(int16_t v,  T bytes);
		template<typename T>
		inline ByteWriter& op(int32_t v,  T bytes);
		template<typename T>
		inline ByteWriter& op(int64_t v,  T bytes);
		template<typename T>
		inline ByteWriter& op(char v,  T bytes);
		template<typename T>
		inline ByteWriter& op(long long v,  T bytes);
		
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
		
		template<typename T, typename... Args>
		inline ByteWriter& op(const T* data, uint32_t elements, Args... args) {
			reserve(offset + sizeof(T)*elements + 4);
			for(uint32_t i=0; i<elements; ++i)
				op(data[i], args...);
			return *this;
		}
		
		template<typename T, typename... Args>
		inline ByteWriter& op(const std::vector<T>& arr, Args... args) {
			reserve(offset + sizeof(T)*arr.size()+4);
			op((uint32_t)arr.size());
			return op<T, Args...>(arr.data(), arr.size(), args...);
		}
		
	private:
		
		inline void reserve(uint32_t newCapacity) {
			if(newCapacity > capacity) {
				newCapacity = (capacity * 3) >> 1; 
				buffer.resize(newCapacity);
				capacity = buffer.size();
				ptr = buffer.data();
			}
		}
		
		std::vector<uint8_t>& buffer;
		uint32_t offset;
		uint32_t capacity;
		uint8_t* ptr;
	};
	
	
	
	inline ByteWriter& ByteWriter::op(const std::string& str) {
		return op((int8_t*const)str.data(), str.size()+1);
	}
	
	inline ByteWriter& ByteWriter::op(const std::string_view str) {
		reserve(offset + str.size()+1);
		memcpy(ptr+offset, str.data(), str.size());
		ptr[offset+str.size()] = 0;
		offset += str.size()+1;
		return *this;
	}
	
	template<typename T>
	inline ByteWriter& ByteWriter::op(uint8_t*const data, T bytes) {
		reserve(offset + bytes);
		memcpy(ptr+offset, data, bytes);
		offset +=bytes;
		return *this;
	}
	
	template<typename T>
	inline ByteWriter& ByteWriter::op(int8_t*const data, T bytes) {
		return op((uint8_t*const)data, bytes);
	}
	
	
	inline ByteWriter& ByteWriter::op(const std::vector<uint8_t>& binary) {
		reserve(offset + binary.size() + 4);
		this->op((uint32_t)binary.size(), 4);
		memcpy(ptr+offset, binary.data(), binary.size());
		offset += binary.size();
		return *this;
	}
	
	
	
	template<typename T>
	inline ByteWriter& ByteWriter::op(uint8_t v,  T bytes) {
		reserve(offset + 16);
		buffer[offset] = v;
		++offset;
		return *this;
	}
	template<typename T>
	inline ByteWriter& ByteWriter::op(uint16_t v, T bytes) {
		reserve(offset+8);
		if constexpr (!IsBigEndian()) {
			*(uint16_t*)(ptr+offset) = v;
		} else {
			for(int i=0; i<bytes; ++i)
				buffer[offset+i] = v >> (i<<3);
		}
		offset += bytes;
		return *this;
	}
	template<typename T>
	inline ByteWriter& ByteWriter::op(uint32_t v, T bytes) {
		reserve(offset+8);
		if constexpr (!IsBigEndian()) {
			*(uint32_t*)(ptr+offset) = v;
		} else {
			for(int i=0; i<bytes; ++i)
				buffer[offset+i] = v >> (i<<3);
		}
		offset += bytes;
		return *this;
	}
	template<typename T>
	inline ByteWriter& ByteWriter::op(uint64_t v, T bytes) {
		reserve(offset+8);
		if constexpr (!IsBigEndian()) {
			*(uint64_t*)(ptr+offset) = v;
		} else {
			for(int i=0; i<bytes; ++i)
				buffer[offset+i] = v >> (i<<3);
		}
		offset += bytes;
		return *this;
	}
	
	template<typename T>
	inline ByteWriter& ByteWriter::op(int8_t v,   T bytes) { return op(v); }
	template<typename T>
	inline ByteWriter& ByteWriter::op(int16_t v,  T bytes) { return op((uint16_t)v, bytes); }
	template<typename T>
	inline ByteWriter& ByteWriter::op(int32_t v,  T bytes) { return op((uint32_t)v, bytes); }
	template<typename T>
	inline ByteWriter& ByteWriter::op(int64_t v,  T bytes) { return op((uint64_t)v, bytes); }
	template<typename T>
	inline ByteWriter& ByteWriter::op(char v,     T bytes) { return op((uint8_t)v, bytes); }
	template<typename T>
	inline ByteWriter& ByteWriter::op(long long v, T bytes) { return op((uint64_t)v, bytes); }
	
	
	
	inline ByteWriter& ByteWriter::op(uint8_t v)  { return op(v, (uint32_t)1); }
	inline ByteWriter& ByteWriter::op(uint16_t v) {
		reserve(offset+8);
		*(uint16_t*)(ptr+offset) = HostToNetworkUint<uint16_t>(v);
		offset += 2;
		return *this;
	}
	inline ByteWriter& ByteWriter::op(uint32_t v) {
		reserve(offset+8);
		*(uint32_t*)(ptr+offset) = HostToNetworkUint<uint32_t>(v);
		offset += 4;
		return *this;
	}
	inline ByteWriter& ByteWriter::op(uint64_t v) {
		reserve(offset+8);
		*(uint64_t*)(ptr+offset) = HostToNetworkUint<uint64_t>(v);
		offset += 8;
		return *this;
	}
	
	inline ByteWriter& ByteWriter::op(int8_t v)  { return op((uint8_t)v); }
	inline ByteWriter& ByteWriter::op(int16_t v) { return op((uint16_t)v); }
	inline ByteWriter& ByteWriter::op(int32_t v) { return op((uint32_t)v); }
	inline ByteWriter& ByteWriter::op(int64_t v) { return op((uint64_t)v); }
	inline ByteWriter& ByteWriter::op(char v) { return op((uint8_t)v); }
	inline ByteWriter& ByteWriter::op(long long v) { return op((uint64_t)v); }
	
	
	
	inline ByteWriter& ByteWriter::op(float value) {
		return op(*(uint32_t*)&value);
	}
	
	inline ByteWriter& ByteWriter::op(double value) {
		return op(*(uint64_t*)&value);
	}
	
		
	template<typename Tmin, typename Tmax, typename T>
	inline ByteWriter& ByteWriter::op(float value, Tmin min, Tmax max,
			T bytes) {
		float fmask = (((uint32_t)1)<<(bytes<<3))-1;
		float pv = (value-min) * (fmask / (max-min));
		uint32_t v = pv+0.4f;
		return op(v, bytes);
	}
	
	template<typename Tmin, typename Tmax, typename T>
	inline ByteWriter& ByteWriter::op(double value, Tmin min, Tmax max,
			T bytes) {
		double fmask = (((uint64_t)1)<<(bytes<<3))-1ll;
		double pv = (value-min) * (fmask / (max-min));
		uint64_t v = pv+0.4f;
		return op(v, bytes);
	}
	
	template<typename Torig, typename Tmin, typename Tmax, typename T>
	inline ByteWriter& ByteWriter::op(float value, Torig origin, Tmin min,
			Tmax max, T bytes) {
		return op(value - origin, min, max, bytes);
	}
	
	template<typename Torig, typename Tmin, typename Tmax, typename T>
	inline ByteWriter& ByteWriter::op(double value, Torig origin, Tmin min,
			Tmax max, T bytes) {
		return op(value - origin, min, max, bytes);
	}
} // namespace bitscpp

#endif

