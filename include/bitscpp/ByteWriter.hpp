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

#include <string>
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
		
		inline ByteWriter(std::vector<uint8_t>& buffer) : buffer(buffer) {}
		
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
			for(uint32_t i=0; i<elements; ++i)
				op(data[i], args...);
			return *this;
		}
		
		template<typename T, typename... Args>
		inline ByteWriter& op(const std::vector<T>& arr, Args... args) {
			op((uint32_t)arr.size());
			return op<T, Args...>(arr.data(), arr.size(), args...);
		}
		
	private:
		
		std::vector<uint8_t>& buffer;
	};
	
	
	
	inline ByteWriter& ByteWriter::op(const std::string& str) {
		return op((int8_t*const)str.data(), str.size()+1);
	}
	
	inline ByteWriter& ByteWriter::op(const std::string_view str) {
		return op((int8_t*const)str.data(), str.size()+1);
	}
	
	template<typename T>
	inline ByteWriter& ByteWriter::op(uint8_t*const data, T bytes) {
		buffer.insert(buffer.end(), data, data+bytes);
		return *this;
	}
	
	template<typename T>
	inline ByteWriter& ByteWriter::op(int8_t*const data, T bytes) {
		buffer.insert(buffer.end(), data, data+bytes);
		return *this;
	}
	
	
	inline ByteWriter& ByteWriter::op(const std::vector<uint8_t>& binary) {
		this->op((uint32_t)binary.size(), 4);
		return op((uint8_t*const)binary.data(), binary.size());
	}
	
	
	
	template<typename T>
	inline ByteWriter& ByteWriter::op(uint8_t v,  T bytes) {
		buffer.emplace_back(v);
		return *this;
	}
	template<typename T>
	inline ByteWriter& ByteWriter::op(uint16_t v, T bytes) {
		for(int i=0; i<bytes; ++i)
			buffer.emplace_back((uint8_t)((v)>>(i<<3)));
		return *this;
	}
	template<typename T>
	inline ByteWriter& ByteWriter::op(uint32_t v, T bytes) {
		for(int i=0; i<bytes; ++i)
			buffer.emplace_back((uint8_t)((v)>>(i<<3)));
		return *this;
	}
	template<typename T>
	inline ByteWriter& ByteWriter::op(uint64_t v, T bytes) {
		for(int i=0; i<bytes; ++i)
			buffer.emplace_back((uint8_t)((v)>>(i<<3)));
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
		const uint32_t s = buffer.size();
		buffer.resize(s+2);
		*(uint16_t*)(buffer.data()+s) = HostToNetworkUint<uint16_t>(v);
		return *this;
	}
	inline ByteWriter& ByteWriter::op(uint32_t v) {
		const uint32_t s = buffer.size();
		buffer.resize(s+4);
		*(uint32_t*)(buffer.data()+s) = HostToNetworkUint<uint32_t>(v);
		return *this;
	}
	inline ByteWriter& ByteWriter::op(uint64_t v) {
		const uint32_t s = buffer.size();
		buffer.resize(s+8);
		*(uint64_t*)(buffer.data()+s) = HostToNetworkUint<uint64_t>(v);
		return *this;
	}
	
	inline ByteWriter& ByteWriter::op(int8_t v)  { return op((uint8_t)v); }
	inline ByteWriter& ByteWriter::op(int16_t v) { return op((uint16_t)v); }
	inline ByteWriter& ByteWriter::op(int32_t v) { return op((uint32_t)v); }
	inline ByteWriter& ByteWriter::op(int64_t v) { return op((uint64_t)v); }
	inline ByteWriter& ByteWriter::op(char v) { return op((uint8_t)v); }
	inline ByteWriter& ByteWriter::op(long long v) { return op((uint64_t)v); }
	
	
	
	inline ByteWriter& ByteWriter::op(float value) {
		return op(*(uint32_t*)&value, 4);
	}
	
	inline ByteWriter& ByteWriter::op(double value) {
		return op(*(uint64_t*)&value, 8);
	}
	
		
	template<typename Tmin, typename Tmax, typename T>
	inline ByteWriter& ByteWriter::op(float value, Tmin min, Tmax max,
			T bytes) {
		float fmask = (((uint64_t)1)<<(bytes<<3))-1ll;
		float pv = value * fmask / (max-min);
		uint64_t v = pv+0.4f;
		return op(v, bytes);
	}
	
	template<typename Tmin, typename Tmax, typename T>
	inline ByteWriter& ByteWriter::op(double value, Tmin min, Tmax max,
			T bytes) {
		double fmask = (((uint64_t)1)<<(bytes<<3))-1ll;
		double pv = value * fmask / (max-min);
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

