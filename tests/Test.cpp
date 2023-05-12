
#include <bitscpp/Writer.hpp>
#include <bitscpp/Reader.hpp>

#include <iostream>

#include <cstdio>

// #pragma pack(push, 1)
struct Struct {
	Struct() {
		clear();
	}
	
	Struct& operator= (const Struct& other) {
		memcpy(this, &other, sizeof(*this));
		return *this;
	}
	
	bool operator==(const Struct& other) {
		return !memcmp(this, &other, sizeof(*this));
	}
	
	uint32_t bytes16;
	uint32_t bytes32;
	uint32_t bytes64;
	
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
	}
	
	template<typename S>
	S& __ByteStream_op(S& s) {
		s.op(bytes16, 1);
		s.op(bytes32, 1);
		s.op(bytes64, 1);
		
		s.op(a8);
		s.op(ua8);
		s.op(a16, bytes16);
		s.op(ua16, bytes16);
		s.op(a32, bytes32);
		s.op(ua32, bytes32);
		s.op(a64, bytes64);
		s.op(ua64, bytes64);
		
		s.op(f2);
		
		s.op(b8);
		s.op(ub8);
		s.op(b16);
		s.op(ub16);
		s.op(b32);
		s.op(ub32);
		s.op(b64);
		s.op(ub64);
		
		s.op(f1);
		return s;
	}
	
	void clear() {
		memset(this, 0, sizeof(*this));
	}
};
// #pragma pack(pop)

static uint64_t I = 1;
void Random(void* ptr, uint32_t bytes) {
	static uint64_t I = 1;
	for(int i=0; i<bytes; ++i) {
		((uint8_t*)ptr)[i] = ++I;
	}
}

template<typename T>
void Random(T& v) {
	v = 0;
	for(int i=0; i<sizeof(T); ++i) {
		v |= ((T)(++I)&255)<<(i<<3);
	}
}

void Random(Struct& s) {
	static int _b16 = -1;
	static int _b32 = -1;
	static int _b64 = -1;
	
	s.clear();
	
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
	s.bytes32 = ((++_b32)&2) + 1;
	s.bytes64 = ((++_b64)&3) + 1;
		
	s.a16 &=  (1llu<<(s.bytes16<<3)) - 1;
	s.ua16 &= (1llu<<(s.bytes16<<3)) - 1;
	s.a32 &=  (1llu<<(s.bytes32<<3)) - 1;
	s.ua32 &= (1llu<<(s.bytes32<<3)) - 1;
	s.a64 &=  (1llu<<(s.bytes64<<3)) - 1;
	s.ua64 &= (1llu<<(s.bytes64<<3)) - 1;
}

int main() {
	
	std::vector<uint8_t> buffer;
	
	for(int i=0; i<16; ++i) {
		buffer.resize(10000);
		Random((void*)buffer.data(), buffer.size());
		buffer.clear();
		
		Struct s1, s2;
		s1.clear();
		s2.clear();
		Random(s1);
		
		bitscpp::ByteWriter writer(buffer);
		writer.op(s1);
		
		bitscpp::ByteReader reader(buffer.data(), buffer.size());
		reader.op(s2);
		
		printf(" equality: %i -> %s\n", i, s1==s2 ? "true" : "false");
// 		s1.cmp(s2);
	}
	return 0;
}

