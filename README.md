
# bitscpp

This project is under MIT License. Please see LICENSE file.

Requires C++23 to compile.

Library provides serialization and deserialization to binary buffers/arrays.
Able to use with `std::vector<uint8_t>` or other classes/structures
with following interface as a serialization buffer:

```C++
class Buffer {
    uint8_t *data();
    size_t size();
    void resize(size_t newSize);
    size_t capacity();
    void reserve(size_t newCapacity);
};
```

Deserialization is made from plain C byte array.


## Benchmark rsults

Results of github.com/Drwalin/cpp\_serializers\_benchmark (fork of
github.com/fraillt/cpp\_serializers\_benchmark)


### gcc (GCC) 13.2.1

| library     | test case                      | bin size | data size | ser time | des time |
|-------------|--------------------------------|----------|-----------|----------|----------|
| bitscpp     | general                        | 48984B   | 7366B     | 692ms    | 734ms    |
| bitscpp     | compressed\_float              | 49144B   | 4666B     | 733ms    | 934ms    |
| bitscpp     | compressed\_float unsafe\_read | 49160B   | 4666B     | 769ms    | 778ms    |
| bitscpp     | general unsafe\_read           | 49048B   | 7366B     | 701ms    | 792ms    |
| bitsery     | general                        | 83800B   | 6913B     | 1245ms   | 956ms    |
| bitsery     | brief syntax                   | 83928B   | 6913B     | 1267ms   | 857ms    |
| bitsery     | compatibility                  | 92512B   | 7113B     | 1014ms   | 941ms    |
| bitsery     | compression                    | 96312B   | 4213B     | 1476ms   | 1128ms   |
| bitsery     | fixed buffer                   | 50360B   | 6913B     | 858ms    | 1064ms   |
| bitsery     | stream                         | 59792B   | 6913B     | 1175ms   | 4275ms   |
| bitsery     | unsafe read                    | 83136B   | 6913B     | 1207ms   | 509ms    |
| cereal      | general                        | 82816B   | 10413B    | 5987ms   | 4897ms   |
| handwritten | general                        | 48768B   | 10413B    | 798ms    | 885ms    |
| handwritten | unsafe                         | 44672B   | 10413B    | 844ms    | 550ms    |
| iostream    | general                        | 54592B   | 8413B     | 7628ms   | 9344ms   |
| msgpack     | general                        | 85296B   | 8857B     | 1097ms   | 8080ms   |
| yas         | general                        | 69840B   | 10463B    | 1990ms   | 1321ms   |
| yas         | compression                    | 74168B   | 7315B     | 2421ms   | 1473ms   |
| yas         | stream                         | 60280B   | 10463B    | 7253ms   | 7227ms   |
| zpp\_bits   | general                        | 57088B   | 8413B     | 604ms    | 593ms    |
| zpp\_bits   | fixed buffer                   | 44712B   | 8413B     | 552ms    | 612ms    |

