
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

### Integers

Uses all values from 00000000b to 11000111b

```
 MSB    LSB 
  V      V
+----------+
| 00000000 |
+----------+
```

```
+----------+
| 0XXXXXXX | -> integer XXXXXXX in range <0, 127>
+----------+

+----------+
| 100XXXXX | -> integer XXXXX in range <-32, -1>
+----------+

+----------+
| 1010XXXX | -> integer XXXX in range <-48, -33>
+----------+

+----------+---------------+
| 10110ZZZ | VAR_TYPED_INT | -> variadic integer, sizeof(VAR_TYPED_INT) = ZZZ+1
+----------+---------------+

stores integer in format:
absolute value multiplied by 2, and sign at LSB (1 - negative, 0 - positive)
negative values are first incremented by 1.

  ZZZ (little endian):
    = 0 -> VAR_TYPED_INT in range <-176, -49> u <128, 255>
    = 1 -> VAR_TYPED_INT in range <-2^15, 2^15-1>
    = 2 -> VAR_TYPED_INT in range <-2^23, 2^23-1>
    = 3 -> VAR_TYPED_INT in range <-2^31, 2^31-1>
    = 4 -> VAR_TYPED_INT in range <-2^39, 2^39-1>
    = 5 -> VAR_TYPED_INT in range <-2^47, 2^47-1>
    = 6 -> VAR_TYPED_INT in range <-2^55, 2^55-1>
    = 7 -> VAR_TYPED_INT in range <-2^63, 2^63-1>
```

### Miscelaneus types

```
+----------+
| 10111UUU | -> UNDEFINED - FOR FUTURE USE
+----------+
+----------+
| 11000000 | -> UNDEFINED - FOR FUTURE USE
+----------+

+----------+---------+
| 11000001 | 2 BYTES | -> float16
+----------+---------+

+----------+---------+
| 11000010 | 4 BYTES | -> float32
+----------+---------+

+----------+---------+
| 11000011 | 8 BYTES | -> float64
+----------+---------+

+----------+
| 11000100 | -> false
+----------+

+----------+
| 11000101 | -> true
+----------+

+----------+
| 11000110 | -> NULL -> can be interpreted as int=0, float=0, array sized=0,
+----------+    string sized=0, map sized=0, false, begin EMPTY object (no end
                object is placed afterwards), cannot be interpreded only as end
                object

+----------+
| 11000111 | -> begin object
+----------+

+----------+
| 11001000 | -> end object
+----------+

+----------+
| 11001001 | -> empty map
+----------+

+----------+----------+------------------------------+
| 11001010 | VAR_UINT | VAR_UINT+1 pairs of elements | -> map of any type to any
+----------+----------+------------------------------+    type
```

### Array of any type/size elements

```
+----------+
| 11001011 | -> empty array
+----------+

+----------+----------+----------------------+
| 11001100 | VAR_UINT | VAR_UINT+17 elements | -> array of size VAR_UINT+17
+----------+----------+----------------------+

+----------+-----------------+
| 1101AAAA | AAAA+1 elements | -> array of size AAAA+1
+----------+-----------------+
```

### Strings / byte arrays

```
+----------+
| 11001101 | -> empty string / byte array
+----------+

+----------+--------+----------+
| 11001110 | string | 00000000 | -> null terminated string (c-string)
+----------+--------+----------+

+----------+----------+----------------+
| 11001111 | VAR_UINT | byte[VAR_UINT] | -> byte array of size VAR_UINT+33
+----------+----------+----------------+

+----------+---------------+
| 111WWWWW | byte[WWWWW+1] | -> byte array of size WWWWW+1
+----------+---------------+

```

### Internal VAR\_UINT type

Integer uses little endian.

To implement VAR\_UINT, std::countl\_one can be used with array of offsets and
masks.

```
+----------+
| 0XXXXXXX | -> integer in range <0, 127>
+----------+

+----------+---------+
| 10XXXXXX | byte[1] | -> integer in range <2^7, 2^7+2^14>
+----------+---------+

+----------+---------+
| 110XXXXX | byte[2] | -> integer in range <0, 2^21>
+----------+---------+

+----------+---------+
| 1110XXXX | byte[3] | -> integer in range <0, 2^28>
+----------+---------+

+----------+---------+
| 11110XXX | byte[4] | -> integer in range <0, 2^35>
+----------+---------+

+----------+---------+
| 111110XX | byte[5] | -> integer in range <0, 2^42>
+----------+---------+

+----------+---------+
| 1111110X | byte[6] | -> integer in range <0, 2^49>
+----------+---------+

+----------+---------+
| 11111110 | byte[7] | -> integer in range <0, 2^56>
+----------+---------+

+----------+---------+
| 11111111 | byte[8] | -> integer in range <0, 2^64-1>
+----------+---------+
```

### Internal VAR\_INT type

It is saved using VAR\_UINT function with modifying integer storage format.
There are 3 variants:

 - value == 0:
   store(0)
 - value > 0:
   store((abs(value) - 1) * 2)
 - value < 0
   store(value * 2 + 1)

