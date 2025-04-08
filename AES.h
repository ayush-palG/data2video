#ifndef AES_H_
#define AES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


// multiplication in GF(2^8)
uint8_t galois_mul(uint8_t a, uint8_t b);
uint8_t galois_mul2(uint8_t a);
uint8_t galois_mul3(uint8_t a);

void initialize_aes_sbox(uint8_t sbox[256]);

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
  return a ^ galois_mul2(a);
}

void initialize_aes_sbox(uint8_t sbox[256])
{
  uint8_t p = 1, q = 1;
	
  /* loop invariant: p * q == 1 in the Galois field */
  do {
    /* multiply p by 3 */
    p = p ^ (p << 1) ^ (p & 0x80 ? 0x1B : 0);

    /* divide q by 3 (equals multiplication by 0xf6) */
    q ^= q << 1;
    q ^= q << 2;
    q ^= q << 4;
    q ^= q & 0x80 ? 0x09 : 0;

    /* compute the affine transformation */
    uint8_t xformed = q ^ ROTL8(q, 1) ^ ROTL8(q, 2) ^ ROTL8(q, 3) ^ ROTL8(q, 4);

    sbox[p] = xformed ^ 0x63;
  } while (p != 1);

  /* 0 is a special case since it has no inverse */
  sbox[0] = 0x63;
}


#endif // AES_IMPLEMENTATION
