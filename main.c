// #define COMPRESSION_IMPLEMENTATION
// #include "./compression.h"

#define SV_IMPLEMENTATION
#include "./compression_algorithms/SV.h"

int main(void)
{

  String_View sv = sv_alloc(10);
  for (size_t i = 0; i < 10; ++i) {
    sv.str[sv.size++] = i + 'a';
  }

  String_View sv_bytes = sv_bytes_to_bits(&sv);
  for (size_t i = 0; i < sv_bytes.size; i += U8_SIZE) {
    printf("%.*s ", U8_SIZE, sv_bytes.str+i);
  }
  printf("\n");

  free(sv.str);
  free(sv_bytes.str);
  
  // huffman_encode("input", "output");
  //huffman_decode("output", "input2");
  
  return 0;
}
