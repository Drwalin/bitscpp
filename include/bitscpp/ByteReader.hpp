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
#include <string_view>
#include <vector>

#include "Endianness.hpp"

namespace bitscpp {
	
	template<bool __safeReading = true>
	class ByteReader;
	
	template<typename T>
	inline ByteReader<true>& op(ByteReader<true>& reader, T& data) {
		data.__ByteStream_op(reader);
		return reader;
	}
	
	template<typename T>
	inline ByteReader<false>& op(ByteReader<false>& reader, T& data) {
		data.__ByteStream_op(reader);
		return reader;
	}
	
	namespace impl {
		template<typename T>
		static inline ByteReader<true>& __op_ptr(ByteReader<true>& reader, T* data) {
			op(reader, *data);
			return reader;
		}
		
		template<typename T>
		static inline ByteReader<true>& __op_ref(ByteReader<true>& reader, T& data) {
			op(reader, data);
			return reader;
		}
		
		template<typename T>
		static inline ByteReader<false>& __op_ptr(ByteReader<false>& reader, T* data) {
			op(reader, *data);
			return reader;
		}
		
		template<typename T>
		static inline ByteReader<false>& __op_ref(ByteReader<false>& reader, T& data) {
			op(reader, data);
			return reader;
		}
	}
	
	template<bool __safeReading>
	class ByteReader {
	public:
		
		template<typename T>
		inline ByteReader& op(T* data) {
			impl::__op_ptr(*this, data);
			return *this;
		}
		
		template<typename T>
		inline ByteReader& op(T& data) {
			impl::__op_ref(*this, data);
			return *this;
		}
		
	public:
		
		inline ByteReader(const uint8_t* buffer, uint32_t offset, uint32_t size) :
				buffer(buffer), size(size), offset(offset),
				errorReading_bufferToSmall(false) {
		}
		inline ByteReader(const uint8_t* buffer, uint32_t size) :
				buffer(buffer), size(size), offset(0),
				errorReading_bufferToSmall(false) {
		}
		
		
		
		// NULL-terminated string
		inline ByteReader& op(std::string& str);
		inline ByteReader& op(std::string_view& str);
		// constant size byte array
		template<typename T>
		inline ByteReader& op(uint8_t* data, T bytes);
		template<typename T>
		inline ByteReader& op(int8_t* data, T bytes);
		
		// uint32_t size preceeds size of binary data
		inline ByteReader& op(std::vector<uint8_t>& binary);
		
		
		template<typename T>
		inline ByteReader& op(uint16_t& v,  T bytes);
		template<typename T>
		inline ByteReader& op(uint32_t& v,  T bytes);
		template<typename T>
		inline ByteReader& op(uint64_t& v,  T bytes);
		template<typename T>
		inline ByteReader& op(int16_t& v,  T bytes) { return op(*(uint16_t*)&v, bytes); }
		template<typename T>
		inline ByteReader& op(int32_t& v,  T bytes) { return op(*(uint32_t*)&v, bytes); }
		template<typename T>
		inline ByteReader& op(int64_t& v,  T bytes) { return op(*(uint64_t*)&v, bytes); }
		
		inline ByteReader& op(bool& v);
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
		
		template<typename T, typename Te>
		inline ByteReader& op(T* data, Te elements) {
			for(uint32_t i=0; i<elements; ++i)
				op(data[i]);
			return *this;
		}
		
		template<typename T>
		inline ByteReader& op(std::vector<T>& arr) {
			uint32_t elems;
			op(elems);
			arr.resize(elems);
			return op<T>(arr.data(), elems);
		}
		
		
		
		inline bool is_valid() const {
			return offset <= size;
		}
		
		inline bool has_any_more() const {
			return offset < size;
		}
		
		inline bool has_bytes_to_read(uint32_t bytes) const {
			if constexpr (__safeReading) {
				return offset+bytes <= size;
			} else {
				return true;
			}
		}
		
		inline uint32_t get_offset() const {
			return offset;
		}
		
		inline const uint8_t* get_buffer() const {
			return buffer;
		}
		
		inline const uint32_t get_remaining_bytes() const {
			return size-offset;
		}
		
	protected:
		
		uint8_t const* buffer;
		uint32_t size;
		uint32_t offset;
		
		bool errorReading_bufferToSmall;
	};
	
	
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(std::string_view& str) {
		const void* ptr = memchr(buffer+offset, 0, size-offset);
		if(!ptr) {
			str = std::string_view();
			errorReading_bufferToSmall = true;
		} else {
			ssize_t len = ((char*)ptr) - ((char*)buffer+offset);
			str = std::string_view((char*)buffer+offset, len);
			offset += str.size() + 1;
		}
		return *this;
	}
	
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(std::string& str) {
		if constexpr (__safeReading) {
			const void* ptr = memchr(buffer+offset, 0, size-offset);
			if(!ptr) {
				errorReading_bufferToSmall = true;
			} else {
				str.clear();
				str.insert(str.begin(), (char*)buffer+offset, (char*)ptr);
				offset += str.size() + 1;
			}
		} else {
			str = (char*)(buffer+offset);
			offset += str.size()+1;
		}
		return *this;
	}
	
	template<bool __safeReading>
	template<typename T>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(uint8_t* data, T bytes) {
		if(!has_bytes_to_read(bytes)) {
			errorReading_bufferToSmall = true;
			return *this;
		}
		memcpy(data, buffer+offset, bytes);
		offset += bytes;
		return *this;
	}
	
	template<bool __safeReading>
	template<typename T>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(int8_t* data, T bytes) {
		return op((uint8_t*)data, bytes);
	}
	
	
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(std::vector<uint8_t>& data) {
		if(!has_bytes_to_read(4)) {
			errorReading_bufferToSmall = true;
			return *this;
		}
		uint32_t bytes;
		op(bytes);
		if(!has_bytes_to_read(bytes)) {
			errorReading_bufferToSmall = true;
			return *this;
		}
		data.resize(bytes);
		memcpy(data.data(), buffer+offset, bytes);
		offset += bytes;
		return *this;
	}
	
	
	
	template<bool __safeReading>
	template<typename T>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(uint16_t& v, T bytes) {
		if(!has_bytes_to_read(bytes)) {
			errorReading_bufferToSmall = true;
			return *this;
		}
		v = 0;
		if constexpr (!IsBigEndian()) {
			memcpy(&v, buffer+offset, bytes);
		} else {
			for(int i=0; i<bytes; ++i)
				v |= ((uint32_t)(buffer[offset+i])) << (i<<3);
		}
		offset += bytes;
		return *this;
	}
	template<bool __safeReading>
	template<typename T>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(uint32_t& v, T bytes) {
		if(!has_bytes_to_read(bytes)) {
			errorReading_bufferToSmall = true;
			return *this;
		}
		v = 0;
		if constexpr (!IsBigEndian()) {
			memcpy(&v, buffer+offset, bytes);
		} else {
			for(int i=0; i<bytes; ++i)
				v |= ((uint32_t)(buffer[offset+i])) << (i<<3);
		}
		offset += bytes;
		return *this;
	}
	template<bool __safeReading>
	template<typename T>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(uint64_t& v, T bytes) {
		if(!has_bytes_to_read(bytes)) {
			errorReading_bufferToSmall = true;
			return *this;
		}
		v = 0;
		if constexpr (!IsBigEndian()) {
			memcpy(&v, buffer+offset, bytes);
		} else {
			for(int i=0; i<bytes; ++i)
				v |= ((uint64_t)(buffer[offset+i])) << (i<<3);
		}
		offset += bytes;
		return *this;
	}
	
	
	
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(bool& v)  {
		if(!has_bytes_to_read(1)) {
			errorReading_bufferToSmall = true;
			return *this;
		}
		v = buffer[offset];
		offset++;
		return *this;
	}
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(uint8_t& v)  {
		if(!has_bytes_to_read(1)) {
			errorReading_bufferToSmall = true;
			return *this;
		}
		v = buffer[offset];
		offset++;
		return *this;
	}
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(uint16_t& v) {
		if(!has_bytes_to_read(2)) {
			errorReading_bufferToSmall = true;
			return *this;
		}
		memcpy(&v, buffer+offset, sizeof(v));
		if constexpr (IsBigEndian()) {
			v = HostToNetworkUint<uint16_t>(v);
		}
		offset += 2;
		return *this;
	}
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(uint32_t& v) {
		if(!has_bytes_to_read(4)) {
			errorReading_bufferToSmall = true;
			return *this;
		}
		memcpy(&v, buffer+offset, sizeof(v));
		if constexpr (IsBigEndian()) {
			v = HostToNetworkUint<uint32_t>(v);
		}
		offset += 4;
		return *this;
	}
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(uint64_t& v) {
		if(!has_bytes_to_read(8)) {
			errorReading_bufferToSmall = true;
			return *this;
		}
		memcpy(&v, buffer+offset, sizeof(v));
		if constexpr (IsBigEndian()) {
			v = HostToNetworkUint<uint64_t>(v);
		}
		offset += 8;
		return *this;
	}
	
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(int8_t& v)  { return op((uint8_t&)v); }
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(int16_t& v) { return op((uint16_t&)v); }
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(int32_t& v) { return op((uint32_t&)v); }
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(int64_t& v) { return op((uint64_t&)v); }
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(char& v) { return op((uint8_t&)v); }
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(long long& v) { return op((uint64_t&)v); }
	
	
	
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(float& value) {
		return op((uint32_t&)value);
	}
	
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(double& value) {
		return op((uint64_t&)value);
	}
	
	
	template<bool __safeReading>
	template<typename Tmin, typename Tmax, typename T>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(float& value, Tmin min, Tmax max,
			T bytes) {
		float fmask = (((uint32_t)1)<<(bytes<<3))-1;
		uint32_t v = 0;
		op(v, bytes);
		value = (v * ((max-min)/fmask)) + min;
		return *this;
	}
	
	template<bool __safeReading>
	template<typename Tmin, typename Tmax, typename T>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(double& value, Tmin min, Tmax max,
			T bytes) {
		double fmask = (((uint64_t)1)<<(bytes<<3))-1ll;
		uint64_t v = 0;
		op(v, bytes);
		value = (v * ((max-min)/fmask)) + min;
		return *this;
	}
	
	template<bool __safeReading>
	template<typename Torig, typename Tmin, typename Tmax, typename T>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(float& value, Torig origin, Tmin min,
			Tmax max, T bytes) {
		op(value, min, max, bytes);
		value += origin;
		return *this;
	}
	
	template<bool __safeReading>
	template<typename Torig, typename Tmin, typename Tmax, typename T>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(double& value, Torig origin, Tmin min,
			Tmax max, T bytes) {
		op(value, min, max, bytes);
		value += origin;
		return *this;
	}
} // namespace bitscpp

#endif

