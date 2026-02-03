// Copyright (C) 2023-2025 Marek Zalewski aka Drwalin
// 
// This file is part of bitscpp project under MIT License
// You should have received a copy of the MIT License along with this program.

#ifndef BITSCPP_BYTE_READER_HPP
#define BITSCPP_BYTE_READER_HPP

#include <cstdint>
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
		
		constexpr static int VERSION = 1;
		
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
				_buffer(buffer), _size(size), ptr(buffer+offset), end(buffer+size),
				errorReading_bufferToSmall(false) {
		}
		inline ByteReader(const uint8_t* buffer, uint32_t size) :
				_buffer(buffer), _size(size), ptr(buffer), end(buffer+size),
				errorReading_bufferToSmall(false) {
		}
		
		
		
		// NULL-terminated string
		inline ByteReader& op(std::string& str);
		inline ByteReader& op(std::string_view& str);
		inline ByteReader& op_string_sized(std::string_view& str, uint32_t bytesOfSize);
		inline ByteReader& op_string_sized(std::string& str, uint32_t bytesOfSize);
		// constant size byte array
		inline ByteReader& op(uint8_t* data, uint32_t bytes);
		inline ByteReader& op(int8_t* data, uint32_t bytes);
		
		// uint32_t size preceeds size of binary data
		inline ByteReader& op(std::vector<uint8_t>& binary);
		
		
		inline ByteReader& op(uint16_t& v, uint32_t bytes);
		inline ByteReader& op(uint32_t& v, uint32_t bytes);
		inline ByteReader& op(uint64_t& v, uint32_t bytes);
		inline ByteReader& op(int16_t& v, uint32_t bytes) { return op(*(uint16_t*)&v, bytes); }
		inline ByteReader& op(int32_t& v, uint32_t bytes) { return op(*(uint32_t*)&v, bytes); }
		inline ByteReader& op(int64_t& v, uint32_t bytes) { return op(*(uint64_t*)&v, bytes); }
		
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
			return errorReading_bufferToSmall == false;
		}
		
		inline bool has_any_more() const {
			return ptr != end;
		}
		
		inline uint32_t get_offset() const {
			return ptr-_buffer;
		}
		
		inline const uint8_t* get_buffer() const {
			return _buffer;
		}
		
		inline uint32_t get_remaining_bytes() const {
			return end-ptr;
		}
		
	protected:
		
		inline bool has_bytes_to_read(uint32_t bytes) const {
			if constexpr (__safeReading) {
				return bytes <= end-ptr;
			} else {
				return true;
			}
		}
		
		uint8_t const* _buffer;
		uint32_t _size;
		
		uint8_t const* ptr;
		uint8_t const* end;
		
		bool errorReading_bufferToSmall;
	};
	
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(std::string_view& str) {
		const void* _end = memchr(ptr, 0, end-ptr);
		if(!ptr) {
			str = std::string_view();
			errorReading_bufferToSmall = true;
		} else {
			ssize_t len = ((char*)_end) - ((char*)ptr);
			str = std::string_view((char*)ptr, len);
			ptr += str.size() + 1;
		}
		return *this;
	}
	
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(std::string& str) {
		std::string_view sv;
		op(sv);
		if (errorReading_bufferToSmall) {
			str.clear();
			return *this;
		}
		if (sv.size() == 0 || sv.size() > 0xFFFFFFFFllu) {
			str.clear();
			return *this;
		}
		str = std::string(sv.data(), sv.size());
		return *this;
	}
	
	
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op_string_sized(std::string_view& str, uint32_t bytesOfSize) {
		uint32_t size;
		op(size, bytesOfSize);
		if(!has_bytes_to_read(size)) {
			errorReading_bufferToSmall = true;
			return *this;
		}
		str = std::string_view((char*)ptr, size);
		ptr += size;
		return *this;
	}
	
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op_string_sized(std::string& str, uint32_t bytesOfSize) {
		std::string_view sv;
		op_string_sized(sv, bytesOfSize);
		str = sv;
		return *this;
	}
	
	
	
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(uint8_t* data, uint32_t bytes) {
		if(!has_bytes_to_read(bytes)) {
			errorReading_bufferToSmall = true;
			return *this;
		}
		memcpy(data, ptr, bytes);
		ptr += bytes;
		return *this;
	}
	
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(int8_t* data, uint32_t bytes) {
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
		memcpy(data.data(), ptr, bytes);
		ptr += bytes;
		return *this;
	}
	
	
	
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(uint16_t& v, uint32_t bytes) {
		if(!has_bytes_to_read(bytes)) {
			errorReading_bufferToSmall = true;
			return *this;
		}
		v = 0;
		if constexpr (Endian::IsLittle()) {
			memcpy(&v, ptr, bytes);
		} else {
			for(int i=0; i<bytes; ++i)
				v |= ((uint32_t)(ptr[i])) << (i<<3);
		}
		ptr += bytes;
		return *this;
	}
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(uint32_t& v, uint32_t bytes) {
		if(!has_bytes_to_read(bytes)) {
			errorReading_bufferToSmall = true;
			return *this;
		}
		v = 0;
		if constexpr (Endian::IsLittle()) {
			memcpy(&v, ptr, bytes);
		} else {
			for(int i=0; i<bytes; ++i)
				v |= ((uint32_t)(ptr[i])) << (i<<3);
		}
		ptr += bytes;
		return *this;
	}
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(uint64_t& v, uint32_t bytes) {
		if(!has_bytes_to_read(bytes)) {
			errorReading_bufferToSmall = true;
			return *this;
		}
		v = 0;
		if constexpr (Endian::IsLittle()) {
			memcpy(&v, ptr, bytes);
		} else {
			for(int i=0; i<bytes; ++i)
				v |= ((uint64_t)(ptr[i])) << (i<<3);
		}
		ptr += bytes;
		return *this;
	}
	
	
	
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(bool& v)  {
		if(!has_bytes_to_read(1)) {
			errorReading_bufferToSmall = true;
			return *this;
		}
		v = *ptr;
		ptr++;
		return *this;
	}
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(uint8_t& v)  {
		if(!has_bytes_to_read(1)) {
			errorReading_bufferToSmall = true;
			return *this;
		}
		v = *ptr;
		ptr++;
		return *this;
	}
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(uint16_t& v) {
		if(!has_bytes_to_read(2)) {
			errorReading_bufferToSmall = true;
			return *this;
		}
		memcpy(&v, ptr, sizeof(v));
		v = HostToNetworkUint(v);
		ptr += 2;
		return *this;
	}
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(uint32_t& v) {
		if(!has_bytes_to_read(4)) {
			errorReading_bufferToSmall = true;
			return *this;
		}
		memcpy(&v, ptr, sizeof(v));
		v = HostToNetworkUint(v);
		ptr += 4;
		return *this;
	}
	template<bool __safeReading>
	inline ByteReader<__safeReading>& ByteReader<__safeReading>::op(uint64_t& v) {
		if(!has_bytes_to_read(8)) {
			errorReading_bufferToSmall = true;
			return *this;
		}
		memcpy(&v, ptr, sizeof(v));
		v = HostToNetworkUint(v);
		ptr += 8;
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

