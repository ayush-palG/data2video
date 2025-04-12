#define COMPRESSION_IMPLEMENTATION
#include "./compression.h"

int main(void)
{
  String_View sv = {0};
  sv.str = (char *) malloc(sizeof(char) * U8_CAPACITY);

  byte_to_sv(&sv, 'a');
  printf("%.*s\n", (int) sv.size, sv.str);

  uint8_t byte = 0;
  if (!sv_to_byte(&sv, &byte)) {
    printf("sv have less than 8 bits to work with!\n");
  }
  printf("%c\n", byte);
  
  return 0;
}
