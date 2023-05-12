/*
 *  This file is part of ICon6.
 *  Copyright (C) 2023 Marek Zalewski aka Drwalin
 *
 *  ICon6 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ICon6 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef BIT_SERIALIZATION_CPP_READER_HPP
#define BIT_SERIALIZATION_CPP_READER_HPP

#include <cinttypes>
#include <cstring>

#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <functional>

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
		inline ByteReader& op(uint8_t* data, uint32_t bytes);
		inline ByteReader& op(int8_t* data, uint32_t bytes);
		
		// uint32_t size preceeds size of binary data
		inline ByteReader& op(std::vector<uint8_t>& binary);
		
		
		inline ByteReader& op(uint8_t& v,  uint32_t bytes);
		inline ByteReader& op(uint16_t& v, uint32_t bytes);
		inline ByteReader& op(uint32_t& v, uint32_t bytes);
		inline ByteReader& op(uint64_t& v, uint32_t bytes);
		inline ByteReader& op(int8_t& v,   uint32_t bytes);
		inline ByteReader& op(int16_t& v,  uint32_t bytes);
		inline ByteReader& op(int32_t& v,  uint32_t bytes);
		inline ByteReader& op(int64_t& v,  uint32_t bytes);
		inline ByteReader& op(uint8_t& v,  int32_t bytes) { return op(v, (uint32_t)bytes); }
		inline ByteReader& op(uint16_t& v, int32_t bytes) { return op(v, (uint32_t)bytes); }
		inline ByteReader& op(uint32_t& v, int32_t bytes) { return op(v, (uint32_t)bytes); }
		inline ByteReader& op(uint64_t& v, int32_t bytes) { return op(v, (uint32_t)bytes); }
		inline ByteReader& op(int8_t& v,   int32_t bytes) { return op(v, (uint32_t)bytes); }
		inline ByteReader& op(int16_t& v,  int32_t bytes) { return op(v, (uint32_t)bytes); }
		inline ByteReader& op(int32_t& v,  int32_t bytes) { return op(v, (uint32_t)bytes); }
		inline ByteReader& op(int64_t& v,  int32_t bytes) { return op(v, (uint32_t)bytes); }
		
		inline ByteReader& op(uint8_t& v);
		inline ByteReader& op(uint16_t& v);
		inline ByteReader& op(uint32_t& v);
		inline ByteReader& op(uint64_t& v);
		inline ByteReader& op(int8_t& v);
		inline ByteReader& op(int16_t& v);
		inline ByteReader& op(int32_t& v);
		inline ByteReader& op(int64_t& v);
		
		
		inline ByteReader& op(float& v);
		inline ByteReader& op(double& v);
		
		inline ByteReader& op(float& v, float min, float max, uint32_t bytes);
		inline ByteReader& op(double& v, double min, double max, uint32_t bytes);
		
		inline ByteReader& op(float& v, float offset, float min, float max, uint32_t bytes);
		inline ByteReader& op(double& v, double offset, double min, double max, uint32_t bytes);
		
	public:
		
		template<typename T, typename... Args>
		inline ByteReader& op(T* data, uint32_t elements, Args... args) {
			for(uint32_t i=0; i<elements; ++i)
				op(data[i], args...);
			return *this;
		}
		
		template<typename T, typename... Args>
		inline ByteReader& op(std::vector<T>& arr, Args... args) {
			uint32_t bytes;
			op(bytes);
			arr.resize(bytes);
			return op<T, Args...>(arr.data(), bytes, args...);
		}
		
		template<typename T, typename... Args>
		inline ByteReader& op(std::set<T>& set, Args... args) {
			uint32_t bytes;
			op(bytes);
			for(uint32_t i=0; i<bytes; ++i) {
				T v;
				op(v, args...);
				set.emplace_hint(set.end(), std::move(v));
			}
			return *this;
		}
		
		template<typename T, typename... Args>
		inline ByteReader& op(std::unordered_set<T>& set, Args... args) {
			uint32_t bytes;
			op(bytes);
			for(uint32_t i=0; i<bytes; ++i) {
				T v;
				op(v, args...);
				set.insert(std::move(v));
			}
			return *this;
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
// 		
// 	private:
// 		
// 		template<> ByteReader& op<uint8_t >(uint8_t& ) = delete;
// 		template<> ByteReader& op<uint16_t>(uint16_t&) = delete;
// 		template<> ByteReader& op<uint32_t>(uint32_t&) = delete;
// 		template<> ByteReader& op<uint64_t>(uint64_t&) = delete;
// 		template<> ByteReader& op<int8_t  >(int8_t&  ) = delete;
// 		template<> ByteReader& op<int16_t >(int16_t& ) = delete;
// 		template<> ByteReader& op<int32_t >(int32_t& ) = delete;
// 		template<> ByteReader& op<int64_t >(int64_t& ) = delete;
// 		
// 		template<> ByteReader& op<uint8_t , uint32_t>(uint8_t& , uint32_t) = delete;
// 		template<> ByteReader& op<uint16_t, uint32_t>(uint16_t&, uint32_t) = delete;
// 		template<> ByteReader& op<uint32_t, uint32_t>(uint32_t&, uint32_t) = delete;
// 		template<> ByteReader& op<uint64_t, uint32_t>(uint64_t&, uint32_t) = delete;
// 		template<> ByteReader& op<int8_t  , uint32_t>(int8_t&  , uint32_t) = delete;
// 		template<> ByteReader& op<int16_t , uint32_t>(int16_t& , uint32_t) = delete;
// 		template<> ByteReader& op<int32_t , uint32_t>(int32_t& , uint32_t) = delete;
// 		template<> ByteReader& op<int64_t , uint32_t>(int64_t& , uint32_t) = delete;
// 		
// 		template<> ByteReader& op<int >(int& ) = delete;
// 		template<> ByteReader& op<char >(char& ) = delete;
// 		template<> ByteReader& op<short >(short& ) = delete;
// 		template<> ByteReader& op<long >(long& ) = delete;
// 		template<> ByteReader& op<long long >(long long& ) = delete;
// 		template<> ByteReader& op<size_t >(size_t& ) = delete;
// 		
// 		template<> ByteReader& op<int, uint32_t>(int&, uint32_t) = delete;
// 		template<> ByteReader& op<char, uint32_t>(char&, uint32_t) = delete;
// 		template<> ByteReader& op<short, uint32_t>(short&, uint32_t) = delete;
// 		template<> ByteReader& op<long, uint32_t>(long&, uint32_t) = delete;
// 		template<> ByteReader& op<long long, uint32_t>(long long&, uint32_t) = delete;
// 		template<> ByteReader& op<size_t, uint32_t>(size_t&, uint32_t) = delete;
// 		
// 		template<> ByteReader& op<float>(float& value) = delete;
// 		template<> ByteReader& op<double>(double& value) = delete;
// 		template<> ByteReader& op<float,float,float,uint32_t>(float& value, float min, float max, uint32_t bytes) = delete;
// 		template<> ByteReader& op<double,double,double,uint32_t>(double& value, double min, double max, uint32_t bytes) = delete;
// 		template<> ByteReader& op<float,float,float,float,uint32_t>(float& value, float offset, float min, float max, uint32_t bytes) = delete;
// 		template<> ByteReader& op<double,double,double,double,uint32_t>(double& value, double offset, double min, double max, uint32_t bytes) = delete;
// 		
// 		template<> ByteReader& op<std::string>(std::string& str) = delete;
// 		template<> ByteReader& op<std::vector<uint8_t>>(std::vector<uint8_t>& binary) = delete;
	};
	
	
	
	inline ByteReader& ByteReader::op(std::string& str) {
		str.clear();
		str = (char*)buffer+offset;
		offset += str.size()+1;
		return *this;
	}
	
	inline ByteReader& ByteReader::op(uint8_t* data, uint32_t bytes) {
		memcpy(data, buffer+offset, bytes);
		offset += bytes;
		return *this;
	}
	
	inline ByteReader& ByteReader::op(int8_t* data, uint32_t bytes) {
		return op((uint8_t*)data, bytes);
	}
	
	
	inline ByteReader& ByteReader::op(std::vector<uint8_t>& data) {
		uint32_t bytes;
		op(bytes);
		data.resize(bytes);
		return this->op((uint8_t*)&(data.front()), bytes);
	}
	
	
	
	inline ByteReader& ByteReader::op(uint8_t& v,  uint32_t bytes) {
		v = buffer[offset];
		offset++;
		return *this;
	}
	inline ByteReader& ByteReader::op(uint16_t& v, uint32_t bytes) {
		v = 0;
		for(int i=0; i<bytes; ++i)
			v |= ((uint16_t)buffer[offset++])<<(i<<3);
		return *this;
	}
	inline ByteReader& ByteReader::op(uint32_t& v, uint32_t bytes) {
		v = 0;
		for(int i=0; i<bytes; ++i)
			v |= ((uint32_t)buffer[offset++])<<(i<<3);
		return *this;
	}
	inline ByteReader& ByteReader::op(uint64_t& v, uint32_t bytes) {
		v = 0;
		for(int i=0; i<bytes; ++i)
			v |= ((uint64_t)buffer[offset++])<<(i<<3);
		return *this;
	}
	
	inline ByteReader& ByteReader::op(int8_t& v,   uint32_t bytes) { return op(v); }
	inline ByteReader& ByteReader::op(int16_t& v,  uint32_t bytes) { return op((uint16_t&)v); }
	inline ByteReader& ByteReader::op(int32_t& v,  uint32_t bytes) { return op((uint32_t&)v, bytes); }
	inline ByteReader& ByteReader::op(int64_t& v,  uint32_t bytes) { return op((uint64_t&)v, bytes); }
	
	
	
	inline ByteReader& ByteReader::op(uint8_t& v)  { return op(v, (uint32_t)1); }
	inline ByteReader& ByteReader::op(uint16_t& v) { return op(v, (uint32_t)2); }
	inline ByteReader& ByteReader::op(uint32_t& v) { return op(v, (uint32_t)4); }
	inline ByteReader& ByteReader::op(uint64_t& v) { return op(v, (uint32_t)8); }
	
	inline ByteReader& ByteReader::op(int8_t& v)  { return op((uint8_t&)v); }
	inline ByteReader& ByteReader::op(int16_t& v) { return op((uint16_t&)v); }
	inline ByteReader& ByteReader::op(int32_t& v) { return op((uint32_t&)v); }
	inline ByteReader& ByteReader::op(int64_t& v) { return op((uint64_t&)v); }
	
	
	
	inline ByteReader& ByteReader::op(float& value) {
		return op((uint32_t&)value, 4);
	}
	
	inline ByteReader& ByteReader::op(double& value) {
		return op((uint64_t&)value, 8);
	}
	
	inline ByteReader& ByteReader::op(float& value, float min, float max,
			uint32_t bytes) {
		float fmask = (((uint64_t)1)<<(bytes<<3))-1ll;
		uint64_t v = 0;
		return op(v, bytes);
		value = v * (max-min)/fmask;
	}
	
	inline ByteReader& ByteReader::op(double& value, double min, double max,
			uint32_t bytes) {
		double fmask = (((uint64_t)1)<<(bytes<<3))-1ll;
		uint64_t v = 0;
		return op(v, bytes);
		value = v * (max-min)/fmask;
	}
	
	inline ByteReader& ByteReader::op(float& value, float offset, float min,
			float max, uint32_t bytes) {
		op(value, min, max, bytes);
		value -= offset;
		return *this;
	}
	
	inline ByteReader& ByteReader::op(double& value, double offset, double min,
			double max, uint32_t bytes) {
		op(value, min, max, bytes);
		value -= offset;
		return *this;
	}
	
} // namespace bitscpp

#endif

