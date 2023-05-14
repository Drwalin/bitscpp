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

#ifndef BITSCPP_BYTE_READER_HPP
#define BITSCPP_BYTE_READER_HPP

#include <cinttypes>
#include <cstring>

#include <string>
#include <vector>

#include "Endianness.hpp"

namespace bitscpp {
	
	
	class ByteReader;
	
	template<typename T, typename... Args>
	inline ByteReader& op(ByteReader& reader, T& data, Args... args) {
		data.__ByteStream_op(reader, args...);
		return reader;
	}
	
	namespace impl {
		template<typename T, typename... Args>
		static inline ByteReader& __op_ptr(ByteReader& reader, T* data, Args... args) {
			op(reader, *data, args...);
			return reader;
		}
		
		template<typename T, typename... Args>
		static inline ByteReader& __op_ref(ByteReader& reader, T& data, Args... args) {
			op(reader, data, args...);
			return reader;
		}
	}
	
	class ByteReader {
	public:
		
		template<typename T, typename... Args>
		inline ByteReader& op(T* data, Args... args) {
			impl::__op_ptr(*this, data, args...);
			return *this;
		}
		
		template<typename T, typename... Args>
		inline ByteReader& op(T& data, Args... args) {
			impl::__op_ref(*this, data, args...);
			return *this;
		}
		
	public:
		
		inline ByteReader(const uint8_t* buffer, uint32_t size) : buffer(buffer), size(size), offset(0) {}
		
		
		
		// NULL-terminated string
		inline ByteReader& op(std::string& str);
		// constant size byte array
		template<typename T>
		inline ByteReader& op(uint8_t* data, T bytes);
		template<typename T>
		inline ByteReader& op(int8_t* data, T bytes);
		
		// uint32_t size preceeds size of binary data
		inline ByteReader& op(std::vector<uint8_t>& binary);
		
		
		template<typename T>
		inline ByteReader& op(uint8_t& v,  T bytes);
		template<typename T>
		inline ByteReader& op(uint16_t& v, T bytes);
		template<typename T>
		inline ByteReader& op(uint32_t& v, T bytes);
		template<typename T>
		inline ByteReader& op(uint64_t& v, T bytes);
		template<typename T>
		inline ByteReader& op(int8_t& v,   T bytes);
		template<typename T>
		inline ByteReader& op(int16_t& v,  T bytes);
		template<typename T>
		inline ByteReader& op(int32_t& v,  T bytes);
		template<typename T>
		inline ByteReader& op(int64_t& v,  T bytes);
		template<typename T>
		inline ByteReader& op(char& v,     T bytes);
		template<typename T>
		inline ByteReader& op(long long& v, T bytes);
		
		inline ByteReader& op(uint8_t& v);
		inline ByteReader& op(uint16_t& v);
		inline ByteReader& op(uint32_t& v);
		inline ByteReader& op(uint64_t& v);
		inline ByteReader& op(int8_t& v);
		inline ByteReader& op(int16_t& v);
		inline ByteReader& op(int32_t& v);
		inline ByteReader& op(int64_t& v);
		inline ByteReader& op(char& v);
		inline ByteReader& op(long long& v);
		
		
		inline ByteReader& op(float& v);
		inline ByteReader& op(double& v);
		
		template<typename Tmin, typename Tmax, typename T>
		inline ByteReader& op(float& value, Tmin min, Tmax max, T bytes);
		template<typename Tmin, typename Tmax, typename T>
		inline ByteReader& op(double& value, Tmin min, Tmax max, T bytes);
		
		template<typename Torig, typename Tmin, typename Tmax, typename T>
		inline ByteReader& op(float& value, Torig origin, Tmin min, Tmax max, T bytes);
		template<typename Torig, typename Tmin, typename Tmax, typename T>
		inline ByteReader& op(double& value, Torig origin, Tmin min, Tmax max, T bytes);
		
	public:
		
		template<typename T, typename Te, typename... Args>
		inline ByteReader& op(T* data, Te elements, Args... args) {
			for(uint32_t i=0; i<elements; ++i)
				op(data[i], args...);
			return *this;
		}
		
		template<typename T, typename... Args>
		inline ByteReader& op(std::vector<T>& arr, Args... args) {
			uint32_t elems;
			op(elems);
			arr.resize(elems);
			return op<T, Args...>(arr.data(), elems, args...);
		}
		
		
		
		inline bool is_valid() const {
			return offset <= size;
		}
		
		inline bool has_any_more() const {
			return offset < size;
		}
		
	private:
		
		const uint8_t* buffer;
		const uint32_t size;
		uint32_t offset;
	};
	
	
	
	inline ByteReader& ByteReader::op(std::string& str) {
		str = (char*)(buffer+offset);
		offset += str.size()+1;
		return *this;
	}
	
	template<typename T>
	inline ByteReader& ByteReader::op(uint8_t* data, T bytes) {
		memcpy(data, buffer+offset, bytes);
		offset += bytes;
		return *this;
	}
	
	template<typename T>
	inline ByteReader& ByteReader::op(int8_t* data, T bytes) {
		return op((uint8_t*)data, bytes);
	}
	
	
	inline ByteReader& ByteReader::op(std::vector<uint8_t>& data) {
		uint32_t bytes;
		op(bytes);
		data.resize(bytes);
		memcpy(data.data(), buffer+offset, bytes);
		offset += bytes;
		return *this;
	}
	
	
	
	template<typename T>
	inline ByteReader& ByteReader::op(uint8_t& v, T bytes) {
		v = buffer[offset];
		offset++;
		return *this;
	}
	template<typename T>
	inline ByteReader& ByteReader::op(uint16_t& v, T bytes) {
		if constexpr (!IsBigEndian()) {
			v = (*(uint16_t*)(buffer+offset))
				& (0xFFFF >> ((2-bytes)<<3));
		} else {
			v = 0;
			for(int i=0; i<bytes; ++i)
				v |= ((uint16_t)(buffer[offset+i])) << (i<<3);
		}
		offset += bytes;
		return *this;
	}
	template<typename T>
	inline ByteReader& ByteReader::op(uint32_t& v, T bytes) {
		if constexpr (!IsBigEndian()) {
			v = (*(uint32_t*)(buffer+offset))
				& (0xFFFFFFFF >> ((4-bytes)<<3));
		} else {
			v = 0;
			for(int i=0; i<bytes; ++i)
				v |= ((uint32_t)(buffer[offset+i])) << (i<<3);
		}
		offset += bytes;
		return *this;
	}
	template<typename T>
	inline ByteReader& ByteReader::op(uint64_t& v, T bytes) {
		if constexpr (!IsBigEndian()) {
			v = (*(uint64_t*)(buffer+offset))
				& (0xFFFFFFFFFFFFFFFFll >> ((8-bytes)<<3));
		} else {
			v = 0;
			for(int i=0; i<bytes; ++i)
				v |= ((uint64_t)(buffer[offset+i])) << (i<<3);
		}
		offset += bytes;
		return *this;
	}
	
	template<typename T>
	inline ByteReader& ByteReader::op(int8_t& v,  T bytes) { return op(v); }
	template<typename T>
	inline ByteReader& ByteReader::op(int16_t& v, T bytes) { return op((uint16_t&)v, bytes); }
	template<typename T>
	inline ByteReader& ByteReader::op(int32_t& v, T bytes) { return op((uint32_t&)v, bytes); }
	template<typename T>
	inline ByteReader& ByteReader::op(int64_t& v, T bytes) { return op((uint64_t&)v, bytes); }
	template<typename T>
	inline ByteReader& ByteReader::op(char& v,    T bytes) { return op((uint8_t&)v, bytes); }
	template<typename T>
	inline ByteReader& ByteReader::op(long long& v, T bytes) { return op((uint64_t&)v, bytes); }
	
	
	
	inline ByteReader& ByteReader::op(uint8_t& v)  { return op(v, (uint32_t)1); }
	inline ByteReader& ByteReader::op(uint16_t& v) {
		v = HostToNetworkUint<uint16_t>(*(uint16_t*)(buffer+offset));
		offset += 2;
		return *this;
	}
	inline ByteReader& ByteReader::op(uint32_t& v) {
		v = HostToNetworkUint<uint32_t>(*(uint32_t*)(buffer+offset));
		offset += 4;
		return *this;
	}
	inline ByteReader& ByteReader::op(uint64_t& v) {
		v = HostToNetworkUint<uint64_t>(*(uint64_t*)(buffer+offset));
		offset += 8;
		return *this;
	}
	
	inline ByteReader& ByteReader::op(int8_t& v)  { return op((uint8_t&)v); }
	inline ByteReader& ByteReader::op(int16_t& v) { return op((uint16_t&)v); }
	inline ByteReader& ByteReader::op(int32_t& v) { return op((uint32_t&)v); }
	inline ByteReader& ByteReader::op(int64_t& v) { return op((uint64_t&)v); }
	inline ByteReader& ByteReader::op(char& v) { return op((uint8_t&)v); }
	inline ByteReader& ByteReader::op(long long& v) { return op((uint64_t&)v); }
	
	
	
	inline ByteReader& ByteReader::op(float& value) {
		return op((uint32_t&)value);
	}
	
	inline ByteReader& ByteReader::op(double& value) {
		return op((uint64_t&)value);
	}
	
	
	template<typename Tmin, typename Tmax, typename T>
	inline ByteReader& ByteReader::op(float& value, Tmin min, Tmax max,
			T bytes) {
		float fmask = (((uint32_t)1)<<(bytes<<3))-1;
		uint32_t v = 0;
		op(v, bytes);
		value = (v * ((max-min)/fmask)) + min;
		return *this;
	}
	
	template<typename Tmin, typename Tmax, typename T>
	inline ByteReader& ByteReader::op(double& value, Tmin min, Tmax max,
			T bytes) {
		double fmask = (((uint64_t)1)<<(bytes<<3))-1ll;
		uint64_t v = 0;
		op(v, bytes);
		value = (v * ((max-min)/fmask)) + min;
		return *this;
	}
	
	template<typename Torig, typename Tmin, typename Tmax, typename T>
	inline ByteReader& ByteReader::op(float& value, Torig origin, Tmin min,
			Tmax max, T bytes) {
		op(value, min, max, bytes);
		value += origin;
		return *this;
	}
	
	template<typename Torig, typename Tmin, typename Tmax, typename T>
	inline ByteReader& ByteReader::op(double& value, Torig origin, Tmin min,
			Tmax max, T bytes) {
		op(value, min, max, bytes);
		value += origin;
		return *this;
	}
} // namespace bitscpp

#endif

