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

#ifndef BIT_SERIALIZATION_CPP_WRITER_HPP
#define BIT_SERIALIZATION_CPP_WRITER_HPP

#include <cinttypes>

#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <functional>

namespace bitscpp {
	
	class ByteWriter;
	
	template<typename T, typename... Args>
	inline ByteWriter& op(ByteWriter& writer, const T& data, Args... args) {
		(*(T*)&data).__ByteStream_op(writer, args...);
		return writer;
	}
	
// 	template<typename T, typename... Args>
// 	inline ByteWriter& op(ByteWriter& writer, const T data, Args... args) {
// 		(*(T*)&data).__ByteWriter_op(writer, args...);
// 		return writer;
// 	}
	
	namespace impl {
// 		template<typename T, typename... Args>
// 		static inline ByteWriter& __op(ByteWriter& writer, const T data, Args... args) {
// 			op(writer, data, args...);
// 			return writer;
// 		}
		
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
		inline ByteWriter& op(uint8_t* const data, uint32_t bytes);
		inline ByteWriter& op(int8_t* const data, uint32_t bytes);
		
		// uint32_t size preceeds size of binary data
		inline ByteWriter& op(const std::vector<uint8_t>& binary);
		
		
		inline ByteWriter& op(uint8_t v,  uint32_t bytes);
		inline ByteWriter& op(uint16_t v, uint32_t bytes);
		inline ByteWriter& op(uint32_t v, uint32_t bytes);
		inline ByteWriter& op(uint64_t v, uint32_t bytes);
		inline ByteWriter& op(int8_t v,   uint32_t bytes);
		inline ByteWriter& op(int16_t v,  uint32_t bytes);
		inline ByteWriter& op(int32_t v,  uint32_t bytes);
		inline ByteWriter& op(int64_t v,  uint32_t bytes);
		inline ByteWriter& op(uint8_t v,  int32_t bytes) { return op(v, (uint32_t)bytes); }
		inline ByteWriter& op(uint16_t v, int32_t bytes) { return op(v, (uint32_t)bytes); }
		inline ByteWriter& op(uint32_t v, int32_t bytes) { return op(v, (uint32_t)bytes); }
		inline ByteWriter& op(uint64_t v, int32_t bytes) { return op(v, (uint32_t)bytes); }
		inline ByteWriter& op(int8_t v,   int32_t bytes) { return op(v, (uint32_t)bytes); }
		inline ByteWriter& op(int16_t v,  int32_t bytes) { return op(v, (uint32_t)bytes); }
		inline ByteWriter& op(int32_t v,  int32_t bytes) { return op(v, (uint32_t)bytes); }
		inline ByteWriter& op(int64_t v,  int32_t bytes) { return op(v, (uint32_t)bytes); }
		
		inline ByteWriter& op(uint8_t v);
		inline ByteWriter& op(uint16_t v);
		inline ByteWriter& op(uint32_t v);
		inline ByteWriter& op(uint64_t v);
		inline ByteWriter& op(int8_t v);
		inline ByteWriter& op(int16_t v);
		inline ByteWriter& op(int32_t v);
		inline ByteWriter& op(int64_t v);
		
		
		inline ByteWriter& op(float value);
		inline ByteWriter& op(double value);
		
		inline ByteWriter& op(float value, float min, float max, uint32_t bytes);
		inline ByteWriter& op(double value, double min, double max, uint32_t bytes);
		
		inline ByteWriter& op(float value, float offset, float min, float max, uint32_t bytes);
		inline ByteWriter& op(double value, double offset, double min, double max, uint32_t bytes);
		
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
		
		template<typename T, typename... Args>
		inline ByteWriter& op(const std::set<T>& set, Args... args) {
			op((uint32_t)set.size());
			for(auto& v : set)
				op(v, args...);
			return *this;
		}
		
		template<typename T, typename... Args>
		inline ByteWriter& op(const std::unordered_set<T>& set, Args... args) {
			op((uint32_t)set.size());
			for(auto& v : set)
				op(v, args...);
			return *this;
		}
		
	private:
		
		std::vector<uint8_t>& buffer;
// 		
// 	private:
// 		
// 		template<> ByteWriter& op<uint8_t >(const uint8_t& ) = delete;
// 		template<> ByteWriter& op<uint16_t>(const uint16_t&) = delete;
// 		template<> ByteWriter& op<uint32_t>(const uint32_t&) = delete;
// 		template<> ByteWriter& op<uint64_t>(const uint64_t&) = delete;
// 		template<> ByteWriter& op<int8_t  >(const int8_t&  ) = delete;
// 		template<> ByteWriter& op<int16_t >(const int16_t& ) = delete;
// 		template<> ByteWriter& op<int32_t >(const int32_t& ) = delete;
// 		template<> ByteWriter& op<int64_t >(const int64_t& ) = delete;
// 		
// 		template<> ByteWriter& op<uint8_t , uint32_t>(const uint8_t& , uint32_t) = delete;
// 		template<> ByteWriter& op<uint16_t, uint32_t>(const uint16_t&, uint32_t) = delete;
// 		template<> ByteWriter& op<uint32_t, uint32_t>(const uint32_t&, uint32_t) = delete;
// 		template<> ByteWriter& op<uint64_t, uint32_t>(const uint64_t&, uint32_t) = delete;
// 		template<> ByteWriter& op<int8_t  , uint32_t>(const int8_t&  , uint32_t) = delete;
// 		template<> ByteWriter& op<int16_t , uint32_t>(const int16_t& , uint32_t) = delete;
// 		template<> ByteWriter& op<int32_t , uint32_t>(const int32_t& , uint32_t) = delete;
// 		template<> ByteWriter& op<int64_t , uint32_t>(const int64_t& , uint32_t) = delete;
// 		
// 		template<> ByteWriter& op<int >(const int& ) = delete;
// 		template<> ByteWriter& op<char >(const char& ) = delete;
// 		template<> ByteWriter& op<short >(const short& ) = delete;
// 		template<> ByteWriter& op<long >(const long& ) = delete;
// 		template<> ByteWriter& op<long long >(const long long& ) = delete;
// 		template<> ByteWriter& op<size_t >(const size_t& ) = delete;
// 		
// 		template<> ByteWriter& op<int, uint32_t>(const int&, uint32_t) = delete;
// 		template<> ByteWriter& op<char, uint32_t>(const char&, uint32_t) = delete;
// 		template<> ByteWriter& op<short, uint32_t>(const short&, uint32_t) = delete;
// 		template<> ByteWriter& op<long, uint32_t>(const long&, uint32_t) = delete;
// 		template<> ByteWriter& op<long long, uint32_t>(const long long&, uint32_t) = delete;
// 		template<> ByteWriter& op<size_t, uint32_t>(const size_t&, uint32_t) = delete;
// 		
// 		template<> ByteWriter& op<float>(const float& value) = delete;
// 		template<> ByteWriter& op<double>(const double& value) = delete;
// 		template<> ByteWriter& op<float,float,float,uint32_t>(const float& value, float min, float max, uint32_t bytes) = delete;
// 		template<> ByteWriter& op<double,double,double,uint32_t>(const double& value, double min, double max, uint32_t bytes) = delete;
// 		template<> ByteWriter& op<float,float,float,float,uint32_t>(const float& value, float offset, float min, float max, uint32_t bytes) = delete;
// 		template<> ByteWriter& op<double,double,double,double,uint32_t>(const double& value, double offset, double min, double max, uint32_t bytes) = delete;
// 		
// 		template<> ByteWriter& op<std::string>(const std::string& str) = delete;
// 		template<> ByteWriter& op<std::vector<uint8_t>>(const std::vector<uint8_t>& binary) = delete;
	};
	
	
	
	inline ByteWriter& ByteWriter::op(const std::string& str) {
		return op((int8_t*const)str.data(), (uint32_t)str.size()+1);
	}
	
	inline ByteWriter& ByteWriter::op(const std::string_view str) {
		return op((int8_t*const)str.data(), (uint32_t)str.size()+1);
	}
	
	inline ByteWriter& ByteWriter::op(uint8_t*const data, uint32_t bytes) {
		buffer.insert(buffer.end(), data, data+bytes);
		return *this;
	}
	
	inline ByteWriter& ByteWriter::op(int8_t*const data, uint32_t bytes) {
		buffer.insert(buffer.end(), data, data+bytes);
		return *this;
	}
	
	
	inline ByteWriter& ByteWriter::op(const std::vector<uint8_t>& binary) {
		this->op((uint32_t)binary.size());
		return op((uint8_t*const)binary.data(),
				(uint32_t)binary.size());
	}
	
	
	
	inline ByteWriter& ByteWriter::op(uint8_t v,  uint32_t bytes) {
		buffer.emplace_back(v);
		return *this;
	}
	inline ByteWriter& ByteWriter::op(uint16_t v, uint32_t bytes) {
		for(int i=0; i<bytes; ++i)
			buffer.emplace_back((uint8_t)((v)>>(i<<3)));
		return *this;
	}
	inline ByteWriter& ByteWriter::op(uint32_t v, uint32_t bytes) {
		for(int i=0; i<bytes; ++i)
			buffer.emplace_back((uint8_t)((v)>>(i<<3)));
		return *this;
	}
	inline ByteWriter& ByteWriter::op(uint64_t v, uint32_t bytes) {
		for(int i=0; i<bytes; ++i)
			buffer.emplace_back((uint8_t)((v)>>(i<<3)));
		return *this;
	}
	
	inline ByteWriter& ByteWriter::op(int8_t v,   uint32_t bytes) { return op(v); }
	inline ByteWriter& ByteWriter::op(int16_t v,  uint32_t bytes) { return op((uint16_t)v); }
	inline ByteWriter& ByteWriter::op(int32_t v,  uint32_t bytes) { return op((uint32_t)v, bytes); }
	inline ByteWriter& ByteWriter::op(int64_t v,  uint32_t bytes) { return op((uint64_t)v, bytes); }
	
	
	
	inline ByteWriter& ByteWriter::op(uint8_t v)  { return op(v, (uint32_t)1); }
	inline ByteWriter& ByteWriter::op(uint16_t v) { return op(v, (uint32_t)2); }
	inline ByteWriter& ByteWriter::op(uint32_t v) { return op(v, (uint32_t)4); }
	inline ByteWriter& ByteWriter::op(uint64_t v) { return op(v, (uint32_t)8); }
	
	inline ByteWriter& ByteWriter::op(int8_t v)  { return op((uint8_t)v); }
	inline ByteWriter& ByteWriter::op(int16_t v) { return op((uint16_t)v); }
	inline ByteWriter& ByteWriter::op(int32_t v) { return op((uint32_t)v); }
	inline ByteWriter& ByteWriter::op(int64_t v) { return op((uint64_t)v); }
	
	
	
	inline ByteWriter& ByteWriter::op(float value) {
		return op(*(uint32_t*)&value, 4);
	}
	
	inline ByteWriter& ByteWriter::op(double value) {
		return op(*(uint64_t*)&value, 8);
	}
	
	inline ByteWriter& ByteWriter::op(float value, float min, float max,
			uint32_t bytes) {
		float fmask = (((uint64_t)1)<<(bytes<<3))-1ll;
		float pv = value * fmask / (max-min);
		uint64_t v = pv+0.4f;
		return op(v, bytes);
	}
	
	inline ByteWriter& ByteWriter::op(double value, double min, double max,
			uint32_t bytes) {
		double fmask = (((uint64_t)1)<<(bytes<<3))-1ll;
		double pv = value * fmask / (max-min);
		uint64_t v = pv+0.4f;
		return op(v, bytes);
	}
	
	inline ByteWriter& ByteWriter::op(float value, float offset, float min,
			float max, uint32_t bytes) {
		return op(value + offset, min, max, bytes);
	}
	
	inline ByteWriter& ByteWriter::op(double value, double offset, double min,
			double max, uint32_t bytes) {
		return op(value + offset, min, max, bytes);
	}
	
} // namespace bitscpp

#endif

