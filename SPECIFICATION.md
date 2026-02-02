
# bitscpp data storage format specification

## format V1: untyped

[[ CURRENTLY IMPLEMENTED ]]

User is fully responsible for checking if stored data has correct type.

## Format V2

[[ CURRENTLY NOT IMPLEMENTED ]]

In case of detecting incorrect type during deserialization, error flag is set
and further deserialization should be stopped. If error flag is set then next
attempts of deserialization of the same object are Undefined Behavior.

In case of deserializing integer with value that does not fit in supplied value
than overflow flag is set and value is undefined.

All multi-byte structures are stored in little-endian order.

```
In the following subsections: "H" is used to describe header byte.
```

### Integers

Uses all values from 00000000b to 11000111b

```
Header byte is H

H=<0x00, 0x9F> -> integer in range <-31, 128> (stores integer modified by +31)
H=<0xA0, 0xAF> + BYTE[1] -> integer in range <-2^11, 2^11-1>
H=<0xB0, 0xB6> + BYTE[n = (H-0xB0+2)] -> integer in range <-2^(8n-1), 2^(8n-1)>

Values of integers before storing in more than single byte are transformed in
the following way:

new_value = value < 0 ? ((~value)<<1) | 1 : value << 1
```

### Floats

```
H=0xB7 -> IEEE 754 half-precision binary floating-point format: binary16
H=0xB8 -> IEEE 754 single-precision binary floating-point format: binary32
H=0xB9 -> IEEE 754 double-precision binary floating-point format: binary64
H=0xBA -> bfloat16
```

### Miscelaneus types

```
H=0xBB -> false
H=0xBC -> true
H=0xBD -> begin object
H=0xBE -> end object
```

### Map type

```
H=0xBF -> empty map
H=0xC0 + VAR_UINT -> header for map of size (VAR_UINT+1) pairs of elements, both key
                     and value can be of any type
```

### Array of any type/size elements

```
H=<0xC1, 0xD2> -> array of size H-0xC1 in <0, 17>
H=0xD3 + VAR_UINT -> array of size VAR_UINT+18
```

### Strings / byte arrays

```
H=<0xD4, 0xF8> -> string of size H-0xD4 in <0, 36>
H=0xF9 + VAR_UINT -> string of size VAR_UINT+37
```

### C-String

```
H=0xFA -> null terminated c-string
```

### Reserved for future use

```
H=<0xFB, 0xFF> -> reserved for future use
```

### Internal VAR\_UINT type

Integer uses little endian.

To implement VAR\_UINT, std::countl\_one can be used with array of offsets and
masks.

```
H=<0x00, 0x7F> -> integer in range <0, 2^7-1>
H=<0x80, 0xBF> + BYTE[1] -> integer ((H&0x3F)|(BYTE[1]<<6)) in <0, 2^14-1>
H=<0xC0, 0xDF> + BYTE[2] -> integer ((H&0x1F)|(BYTE[2]<<5)) in <0, 2^21-1>
H=<0xE0, 0xEF> + BYTE[3] -> integer ((H&0x0F)|(BYTE[3]<<4)) in <0, 2^28-1>
H=<0xF0, 0xF7> + BYTE[4] -> integer ((H&0x07)|(BYTE[4]<<3)) in <0, 2^35-1>
H=<0xF8, 0xFB> + BYTE[5] -> integer ((H&0x03)|(BYTE[5]<<2)) in <0, 2^42-1>
H=<0xFC, 0xFD> + BYTE[6] -> integer ((H&0x01)|(BYTE[6]<<1)) in <0, 2^49-1>
H=0xFE         + BYTE[7] -> integer (BYTE[7]) in <0, 2^56-1>
H=0xFF         + BYTE[8] -> integer (BYTE[8]) in <0, 2^64-1>
```

### Internal VAR\_INT type

It is saved using VAR\_UINT function but firstly modifying it's value:
```
new_value = value < 0 ? ((~value)<<1) | 1 : value << 1
```

