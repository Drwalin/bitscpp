
#include "../include/bitscpp/deprecated/VectorWrapper.hpp" // IWYU pragma: keep
#include "../include/bitscpp/Endianness.hpp"
#include "../include/bitscpp/ByteWriterExtensions.hpp" // IWYU pragma: keep
#include "../include/bitscpp/ByteReaderExtensions.hpp" // IWYU pragma: keep
#include "../include/bitscpp/deprecated/ByteWriterExtensions.hpp" // IWYU pragma: keep
#include "../include/bitscpp/deprecated/ByteReaderExtensions.hpp" // IWYU pragma: keep
#include "../include/bitscpp/deprecated/ByteWriterExtensions_v2.hpp" // IWYU pragma: keep
#include "../include/bitscpp/deprecated/ByteWriter_v2.hpp"
#include "../include/bitscpp/ByteReader_v2.hpp"
#include "../include/bitscpp/ByteWriter_v2_2.hpp"
#include "../src/deprecated/ByteWriter_v2.inl.hpp" // IWYU pragma: keep

#include <iostream>
#include <random>

#include <cstdio>

uint64_t totalErrors = 0;

template<typename T>
std::ostream& operator<<(std::ostream& s, std::set<T> v) {
	s << "{";
	int i=0;
	for(T t : v)
		s << (i++?", ":"") << t;
	s << "}";
	return s;
}

template<typename T>
std::ostream& operator<<(std::ostream& s, std::vector<T> v) {
	s << "{";
	int i=0;
	for(T t : v)
		s << (i++?", ":"") << t;
	s << "}";
	return s;
}

struct Bytes
{
	std::vector<uint8_t> bytes;
	template<typename T>
	Bytes(const T &v)
	{
		bytes.resize(sizeof(T));
		memcpy(bytes.data(), &v, sizeof(T));
	}
};

std::ostream& operator<<(std::ostream& s, Bytes v) {
	s << "BYTES[";
	for (uint8_t b : v.bytes) {
		s << " " << std::hex << (uint32_t)b << std::dec;
	}
	s << " ]";
	return s;
}



#pragma pack(push, 1)
struct Parent {
	int64_t bytes16;
	int64_t bytes32;
	int64_t bytes64;
	
	uint8_t  a8;
	int8_t  ua8;
	uint8_t  b8;
	int8_t  ub8;
	
	uint16_t a16;
	int16_t ua16;
	uint16_t b16;
	int16_t ub16;
	
	uint32_t a32;
	int32_t ua32;
	uint32_t b32;
	int32_t ub32;
	
	uint64_t a64;
	int64_t ua64;
	uint64_t b64;
	int64_t ub64;
	
	double f2;
	float f1;
};
struct Struct : public Parent {
	Struct() {
		clear();
	}

	Struct& operator= (const Struct& other) {
		*(Parent*)this = *(Parent*)&other;
		str = other.str;
		str2 = other.str2;
		is = other.is;
		return *this;
	}

	bool operator==(const Struct& other) const {
		return
			!memcmp(this, &other, sizeof(Parent)) 
			&& str == other.str
			&& str2 == other.str2
			&& is == other.is;
	}

	std::string str;
	std::string str2;
	std::vector<int> is;

	void cmp(Struct& s) {
		std::cout << "\n\n";
		std::cout << " bytes counts\n";
		std::cout << bytes16 	<< "    " << s.bytes16 << "\n";
		std::cout << bytes32 	<< "    " << s.bytes32 << "\n";
		std::cout << bytes64 	<< "    " << s.bytes64 << "\n";
		std::cout << " ints 8\n";
		std::cout << (int)a8 	<< "    " << (int)s.a8 << "\n";
		std::cout << (int)ua8 	<< "    " << (int)s.ua8 << "\n";
		std::cout << (int)b8 	<< "    " << (int)s.b8 << "\n";
		std::cout << (int)ub8 	<< "    " << (int)s.ub8 << "\n";
		std::cout << " ints 16\n";
		std::cout << a16 		<< "    " << s.a16 << "\n";
		std::cout << ua16 		<< "    " << s.ua16 << "\n";
		std::cout << b16 		<< "    " << s.b16 << "\n";
		std::cout << ub16 		<< "    " << s.ub16 << "\n";
		std::cout << " ints 32\n";
		std::cout << a32 		<< "    " << s.a32 << "\n";
		std::cout << ua32 		<< "    " << s.ua32 << "\n";
		std::cout << b32 		<< "    " << s.b32 << "\n";
		std::cout << ub32 		<< "    " << s.ub32 << "\n";
		std::cout << " ints 64\n";
		std::cout << a64 		<< "    " << s.a64 << "\n";
		std::cout << ua64 		<< "    " << s.ua64 << "\n";
		std::cout << b64 		<< "    " << s.b64 << "\n";
		std::cout << ub64 		<< "    " << s.ub64 << "\n";
		std::cout << " floats\n";
		std::cout << f2 		<< "    " << s.f2 << "\n";
		std::cout << f1 		<< "    " << s.f1 << "\n";
		std::cout << " strings\n";
		std::cout << "'"<<str  << "'    '" << s.str  << "'\n";
		std::cout << "'"<<str2 << "'    '" << s.str2 << "'\n";
		std::cout << " vector<int>\n";
		std::cout << "'"<<is << "'\n'" << s.is << "'\n";
	}

	template<typename S>
	S &__ByteStream_op(S& s) {
		serialize(s);
		return s;
	}

	void serialize(auto& s) {
		using S = std::remove_cvref_t<decltype(s)>;
		if constexpr (S::VERSION == 1) {
			s.op(bytes16, 1);
			s.op(bytes32, 1);
			s.op(bytes64, 1);
		} else {
			s.op(bytes16);
			s.op(bytes32);
			s.op(bytes64);
		}
		
		s.op(a8);
		s.op(ua8);
		if constexpr (S::VERSION == 1) {
			s.op(a16, bytes16);
			s.op(ua16, bytes16);
			s.op(a32, bytes32);
			s.op(ua32, bytes32);
			s.op(a64, bytes64);
			s.op(ua64, bytes64);
		} else {
			s.op(a16);
			s.op(ua16);
			s.op(a32);
			s.op(ua32);
			s.op(a64);
			s.op(ua64);
		}
		
		s.op(str);
		if constexpr (S::VERSION == 1) {
			static_assert(S::VERSION == 1);
			s.op_string_sized(str2, 4);
		} else {
			static_assert(S::VERSION != 1);
			s.op(str2);
		}
		
		s.op(b8);
		s.op(ub8);
		s.op(b16);
		s.op(ub16);

		s.op(is);

		s.op(b32);
		s.op(ub32);
		s.op(b64);
		s.op(ub64);

		s.op(f2);
		s.op(f1);
	}

	void clear() {
		str.~basic_string();
		str2.~basic_string();
		is.~vector();
		memset((Parent*)this, 0, sizeof(Parent));
		new (&str) std::string;
		new (&str2) std::string;
		new (&is) std::vector<int>;
	}
};
#pragma pack(pop)



static bitscpp::GenericBufferWriterPimpl buffer_writer_pimpl =
{
    .expand = +[](bitscpp::GenericBufferWriter *buffer, uint8_t minimum, std::size_t hint)
    {
		bitscpp::VectorWrapper *buf = (bitscpp::VectorWrapper *)(buffer->buffer);
		if (buf == nullptr) {
			fprintf(stderr, "Error: no buffer storage\n");
			fflush(stderr);
			exit(1);
		}
		ptrdiff_t offset = 0;
		if (buffer->head) {
			offset = buffer->head - buf->data();
		}
		size_t new_size = minimum + hint + buf->size() + 16;
		new_size = (new_size * 3) / 2;
		buf->resize(new_size);
		buffer->head = buf->data() + offset;
		buffer->end = buf->data() + buf->size();
    },
	.destructor = +[](bitscpp::GenericBufferWriter *buffer)
    {
    },
	.apply_head_end_to_underlying = +[](bitscpp::GenericBufferWriter *buffer)
    {
		bitscpp::VectorWrapper *buf = (bitscpp::VectorWrapper *)(buffer->buffer);
		if (buf == nullptr) {
			fprintf(stderr, "Error: no buffer storage\n");
			fflush(stderr);
			exit(1);
		}
		ptrdiff_t offset = 0;
		if (buffer->head) {
			offset = buffer->head - buf->data();
		}
		buf->resize(offset);
		buffer->head = buf->data() + offset;
		buffer->end = buf->data() + buf->size();
    }
};

template<typename TW>
struct _BufferStorage;

template<>
struct _BufferStorage<bitscpp::v1::deprecated::ByteWriter<bitscpp::VectorWrapper>>
{
	bitscpp::VectorWrapper buf;

	_BufferStorage()
	{
	}

	bitscpp::v1::deprecated::ByteWriter<bitscpp::VectorWrapper> InitWriter()
	{
		return bitscpp::v1::deprecated::ByteWriter<bitscpp::VectorWrapper>(buf);
	}
};

template<>
struct _BufferStorage<bitscpp::v2::deprecated::ByteWriter<bitscpp::VectorWrapper>>
{
	bitscpp::VectorWrapper buf;

	_BufferStorage()
	{
	}

	bitscpp::v2::deprecated::ByteWriter<bitscpp::VectorWrapper> InitWriter()
	{
		return bitscpp::v2::deprecated::ByteWriter<bitscpp::VectorWrapper>(&buf);
	}
};

template<>
struct _BufferStorage<bitscpp::v2_2::ByteWriter>
{
	bitscpp::VectorWrapper buf;

	_BufferStorage()
	{
	}

	bitscpp::v2_2::ByteWriter InitWriter()
	{
		buf.clear();
		return bitscpp::v2_2::ByteWriter(bitscpp::GenericBufferWriter(&buf, buffer_writer_pimpl));
	}
};



template<typename ByteReader, typename ByteWriter>
struct Test {

uint64_t I = 1;
void Random(void* ptr, uint32_t bytes) {
	static uint64_t I = 1;
	for(uint32_t i=0; i<bytes; ++i) {
		((uint8_t*)ptr)[i] = ++I;
	}
}

template<typename T>
void Random(T& v) {
	v = 0;
	for(size_t i=0; i<sizeof(T); ++i) {
		v |= ((T)(++I)&255)<<(i<<3);
	}
}

void Random(Struct& s) {
	static int _b16 = -1;
	static int _b32 = -1;
	static int _b64 = -1;
	
	s.clear();
	
	s.str = "ala ma hfdjsak lfdhsjk fljhasd fljadskl fdha klfdasjf l";
	s.str2 = "ala ma hfdjsak lfdhsjk fljhasd fljadskl fdha klfdasjf l";
// 	s.str += std::to_string(++I);
	static uint32_t issize = -1;
	s.is.resize(++issize);
	for(int&v : s.is) {
		Random(v);
		v %= 1<<24;
	}
	
	
	Random(s.a8);
	Random(s.ua8);
	Random(s.b8);
	Random(s.ub8);
	Random(s.a16);
	Random(s.ua16);
	Random(s.b16);
	Random(s.ub16);
	Random(s.a32);
	Random(s.ua32);
	Random(s.b32);
	Random(s.ub32);
	Random(s.a64);
	Random(s.ua64);
	Random(s.b64);
	Random(s.ub64);
// 	Random((uint64_t&)s.f2);
// 	Random((uint32_t&)s.f1);
	
	
	
// 	s.a8 = ++I;
// 	s.ua8 = ++I;
// 	s.b8 = ++I;
// 	s.ub8 = ++I;
// 	s.a16 = ++I;
// 	s.ua16 = ++I;
// 	s.b16 = ++I;
// 	s.ub16 = ++I;
// 	s.a32 = ++I;
// 	s.ua32 = ++I;
// 	s.b32 = ++I;
// 	s.ub32 = ++I;
// 	s.a64 = ++I;
// 	s.ua64 = ++I;
// 	s.b64 = ++I;
// 	s.ub64 = ++I;
	s.f2 = ++I^123412;
	s.f1 = ++I^43424;
	
	s.bytes16 = ((++_b16)&1) + 1;
	s.bytes32 = ((++_b32)&3) + 1;
	s.bytes64 = ((++_b64)&7) + 1;
		
	s.a16 &=  (1llu<<(s.bytes16<<3)) - 1;
	s.ua16 &= (1llu<<(s.bytes16<<3)) - 1;
	s.a32 &=  (1llu<<(s.bytes32<<3)) - 1;
	s.ua32 &= (1llu<<(s.bytes32<<3)) - 1;
	s.a64 &=  (1llu<<(s.bytes64<<3)) - 1;
	s.ua64 &= (1llu<<(s.bytes64<<3)) - 1;
}

#define COMP(T, orig, X) { \
		T v = orig; \
		std::cout << v << "  ==  "; \
		_BufferStorage<ByteWriter> buf; \
		{ ByteWriter s(buf.InitWriter()); \
		X;\
		} \
		v = 0; \
		{ ByteReader s(buf.buf.data(), buf.buf.size()); \
		X;} \
		std::cout << v << "\n"; \
}


#define COMPARE(binary, T, orig, value, X) { \
		T v = orig; \
		_BufferStorage<ByteWriter> buf; \
		{ \
			ByteWriter s(buf.InitWriter()); \
			X; \
		} \
		v = T(); \
		{ \
			ByteReader s(buf.buf.data(), buf.buf.size()); \
			X; \
			if (s.is_valid()) { \
				correct++; \
			} else { \
				std::cout << " READING BUFFER ERROR\n"; incorrect++; \
			} \
		} \
		if(v == value) { \
			std::cout << " TRUE      "; correct++; \
		} else { \
			std::cout << " FALSE      "; incorrect++; \
		} \
		std::cout << value << "  ==  " << v; \
		if constexpr (binary) { \
			std::cout << " . . . " << Bytes(value) << " == " << Bytes(v); \
		} \
		std::cout << "\n"; \
}

template<typename Tdst, typename Tsrc>
Tdst RC(Tsrc v) {
	return *(Tdst*)&v;
}

int main() {
	
	int correct = 0, incorrect = 0;
	
	{
	_BufferStorage<ByteWriter> buf;

	for(int i=0; i<16; ++i) {
		auto &buffer = buf.buf;
		if (buffer.size()) {
			memset(buffer.data(), 0, buffer.size());
		}
		buffer.resize(10000);
		Random((void*)buffer.data(), buffer.size());
		buffer.clear();
		
		Struct s1, s2;
		s1.clear();
		s2.clear();
		Random(s1);
		
		ByteWriter writer(buf.InitWriter());
		writer.op(s1);
		
		ByteReader reader(buffer.data(), buffer.size());
		reader.op(s2);
		
		if(s1 == s2) {
			correct++;
		} else {
			incorrect++;
		}
		
		printf(" equality: %i -> %s\n", i, s1==s2 ? "true" : "false");
		if (ByteWriter::VERSION == 2) {
// 			s1.cmp(s2);
		}
	}
	}
	
	std::cout << "\n\n\n\n";
	
	if constexpr (ByteReader::VERSION == 1) {
		COMPARE(true, float, 73, RC<float>((uint32_t)(0x4291E1E2)), s.op(v,0,200.f,1));
		COMPARE(true, float, 500, RC<float>((uint32_t)(0x43F9FFEC)), s.op(v,480.f,-20,60.0,2));
		COMPARE(true, float, 123.23456, RC<float>((uint32_t)(0x42F67818)), s.op(v));

		COMPARE(true, float, -1, -1, s.op(v,-1,1,1));
		COMPARE(true, float, 1, 1, s.op(v,-1,1,1));
	} else {
		COMPARE(true, float, 73.0f, 73.0f, s.op(v));
		COMPARE(true, float, 500.0f, 500.0f, s.op(v));
		COMPARE(true, float, 123.23456f, 123.23456f, s.op(v));

		COMPARE(true, float, -1, -1, s.op_half(v));
		COMPARE(true, float, 1, 1, s.op_half(v));
	}
// 	COMPARE(float, 0, 0, s.op(v,-1,1,1));
// 	COMPARE(float, -0.5, -0.5, s.op(v,-1,1,1));
// 	COMPARE(float, 0.5, 0.5, s.op(v,-1,1,1));
// 	COMPARE(float, -0.25, -0.25, s.op(v,-1,1,1));
// 	COMPARE(float, 0.25, 0.25, s.op(v,-1,1,1));
	
	COMPARE(false, std::string, "123456", "123456", s.op(v));
	{
		std::vector<std::string> vs = {"asdf","323","mleko"};
		COMPARE(false, decltype(vs), vs, vs, s.op(v));
	}
	
	{
		std::vector<uint8_t> vs = {41,31,211,21,33,123,53,223,135,235,25,243,77};
		COMPARE(false, decltype(vs), vs, vs, s.op(v));
	}
	
	{
		std::vector<int> vs = {4,3,2,1,24,532,53,2423,435,2345,4325,243,54235};
		COMPARE(false, decltype(vs), vs, vs, s.op(v));
	}
	
	{
		std::vector<int> vs = {13,123,12345};
		COMPARE(false, decltype(vs), vs, vs, s.op(v));
	}
	
	{
		std::vector<std::set<int>> vs = {{13,123},{12345}};
		COMPARE(false, decltype(vs), vs, vs, s.op(v));
	}
	
	{
		std::vector<int> vs = {13,123,12345};
		std::vector<int> vs2 = {13,123,12345};
		COMPARE(false, decltype(vs), vs, vs2, s.op(v));
	}
	
	{
		std::vector<std::set<int>> vs = {{13,123},{12345}};
		std::vector<std::set<int>> vs2 = {{13,123},{12345}};
		COMPARE(false, decltype(vs), vs, vs2, s.op(v));
	}
	
	printf("\n\n correct %i / %i\n", correct, correct + incorrect);
	
	totalErrors += incorrect;
	
	return 0;
}
};

void TestNetworkOrder() {
	auto t = [](const uint64_t value) {
		constexpr uint64_t sizes[9] = {0x0lu, 0xFFlu, 0xFFFFlu, 0xFFFFFFlu, 0xFFFFFFFFlu, 0xFFFFFFFFFFlu, 0xFFFFFFFFFFFFlu, 0xFFFFFFFFFFFFFFlu, 0xFFFFFFFFFFFFFFFFlu};
		for (int bytes=1; bytes<=8; ++bytes) {
			if (value <= sizes[bytes]) {
				uint8_t buffer[64] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
				bitscpp::WriteBytesInNetworkOrder(buffer, value, bytes);
				const uint64_t v = bitscpp::ReadBytesInNetworkOrder(buffer, bytes);
				printf(" [bytes=%i]  %16lX == %16lX . . .", bytes, value, v);
				if (v != value) {
					printf("              FAILED ! ! !\n");
					totalErrors++;
				} else {
					printf(" SUCCESS\n");
				}
			}
		}
	};
	t(0x10);
	t(0x2010);
	t(0x302010);
	t(0x40302010);
	t(0x5040302010);
	t(0x605040302010);
	t(0x70605040302010);
	t(0x8070605040302010);
}

template<typename ByteReader, typename ByteWriter>
void TestVarInt() {
	int correct = 0, incorrect = 0;
	std::cout << "Test var_int\n";
	std::random_device rd;
	std::mt19937_64 mt(rd());
	std::vector<int64_t> values;
	
	for (int64_t bits=0; bits!=64; ++bits) {
		for (int64_t i = -2; i != 3; ++i) {
			int64_t v = (1llu << bits) + i;
			values.push_back(v);
			values.push_back(-v);
		}
	}

	for (int64_t bits=0; bits!=64; ++bits) {
		for (int64_t i = -2; i != 3; ++i) {
			int64_t v = mt() | (1llu << 63);
			v = (v >> (64-bits)) + i;
			values.push_back(v);
		}
	}

	for (auto v : values) {
		for (int i=0; i<2; ++i) {
			uint64_t v2 = mt();

			_BufferStorage<ByteWriter> buf;
			{
				ByteWriter s(buf.InitWriter());
				if (i) {
					s.op(v);
				} else {
					s.op_untyped_var_uint(v);
				}
			}

			{
				ByteReader s(buf.buf.data(), buf.buf.size());
				if (i) {
					s.op(v2);
				} else {
					s.op_untyped_var_uint(v2);
				}

				if (s.is_valid()) {
					correct++;
				} else {
					std::cout << " READING BUFFER ERROR ";
					totalErrors++;
					incorrect++;
				}

				if((uint64_t)v == v2 && s.is_valid()) {
// 					std::cout << " TRUE       ";
// 					std::cout << v << "  ==  " << v2 << "\n";
					correct++;
				} else {
					std::cout << " FALSE (" << (i?"V1":"V2") << ")   ";
					std::cout << v << "  ==  " << v2 << "\n";
					totalErrors++;
					incorrect++;
				}
			}
		}
	}
	printf("\n\n correct %i / %i\n\n", correct, correct + incorrect);
}

int main() {
	printf("bitscpp::network order:\n");
	TestNetworkOrder();

	{
		printf("\n\n");
		printf("bitscpp::v1:\n");
		Test<bitscpp::v1::deprecated::ByteReader<true>, bitscpp::v1::deprecated::ByteWriter<bitscpp::VectorWrapper>>{}.main();
	}

	{
		printf("\n\n");
		printf("bitscpp::v2:\n");
		TestVarInt<bitscpp::v2::ByteReader, bitscpp::v2::deprecated::ByteWriter<bitscpp::VectorWrapper>>();
		printf("\n\n");
		printf("bitscpp::v2:\n");
		Test<bitscpp::v2::ByteReader, bitscpp::v2::deprecated::ByteWriter<bitscpp::VectorWrapper>>{}.main();
	}

	{
		printf("\n\n");
		printf("bitscpp::v2_2:\n");
		TestVarInt<bitscpp::v2::ByteReader, bitscpp::v2_2::ByteWriter>();
		printf("\n\n");
		printf("bitscpp::v2_2:\n");
		Test<bitscpp::v2::ByteReader, bitscpp::v2_2::ByteWriter>{}.main();
	}

	printf("Total errors: %lu\n", totalErrors);

	return totalErrors ? 1 : 0;
}

