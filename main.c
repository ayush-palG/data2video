// #define COMPRESSION_IMPLEMENTATION
// #include "./compression.h"

#define SV_IMPLEMENTATION
#include "./compression_algorithms/SV.h"

int main(void)
{

  String_View sv = sv_alloc(10, BYTES);
  for (size_t i = 0; i < 10; ++i) {
    sv.str[sv.size++] = i + 'a';
  }

  sv_bits_from_sv_bytes(&sv);
  for (size_t i = 0; i < sv.size; i += U8_SIZE) {
    printf("%.*s ", U8_SIZE, sv.str+i);
  }
  printf("\n");

  free(sv.str);
  
  // huffman_encode("input", "output");
  //huffman_decode("output", "input2");
  
  return 0;
}
