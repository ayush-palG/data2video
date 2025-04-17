#define COMPRESSION_IMPLEMENTATION
#include "./compression.h"

int main(void)
{
  huffman_encode("input", "output");
  huffman_decode("output", "input2");
  
  return 0;
}
