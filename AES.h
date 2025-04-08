#ifndef AES_H_
#define AES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


// multiplication in GF(2^8)
uint8_t galois_mul(uint8_t a, uint8_t b);
uint8_t galois_mul2(uint8_t a);
uint8_t galois_mul3(uint8_t a);

#endif // AES_H_

#ifdef AES_IMPLEMENTATION

uint8_t galois_mul(uint8_t a, uint8_t b)
{
  uint8_t p = 0x00;
  
  for (size_t i = 0; i < 8; ++i) {
    if ((b & 1) != 0) {
      p = p ^ a;
    }

    int bool = (a & 0x80) != 0;
    a = a << 1; //shift left
    
    if (bool) {
      a = a ^ 0x1B;
    }
    b = b >> 1; //right shift
  }

  return p;
}

uint8_t galois_mul2(uint8_t a)
{
  if ((a & 0x80) == 0x80) {
    return (a << 1) ^ 0x1b;
  }
  return (a << 1); 
}

uint8_t galois_mul3(uint8_t a)
{
  return a ^ gmul2(a);
}

#endif // AES_IMPLEMENTATION
