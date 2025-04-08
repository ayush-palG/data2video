#ifndef AES_H_
#define AES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


uint8_t sbox[256] = {63, 7c, 77, 7b, f2, 6b, 6f, c5, 30, 01, 67, 2b, fe, d7, ab, 76,
		     ca, 82, c9, 7d, fa, 59, 47, f0, ad, d4, a2, af, 9c, a4, 72, c0,
		     b7, fd, 93, 26, 36, 3f, f7, cc, 34, a5, e5, f1, 71, d8, 31, 15,
		     04, c7, 23, c3, 18, 96, 05, 9a, 07, 12, 80, e2, eb, 27, b2, 75,
		     09, 83, 2c, 1a, 1b, 6e, 5a, a0, 52, 3b, d6, b3, 29, e3, 2f, 84,
		     53, d1, 00, ed, 20, fc, b1, 5b, 6a, cb, be, 39, 4a, 4c, 58, cf,
		     d0, ef, aa, fb, 43, 4d, 33, 85, 45, f9, 02, 7f, 50, 3c, 9f, a8,
		     51, a3, 40, 8f, 92, 9d, 38, f5, bc, b6, da, 21, 10, ff, f3, d2,
		     cd, 0c, 13, ec, 5f, 97, 44, 17, c4, a7, 7e, 3d, 64, 5d, 19, 73,
		     60, 81, 4f, dc, 22, 2a, 90, 88, 46, ee, b8, 14, de, 5e, 0b, db,
		     e0, 32, 3a, 0a, 49, 06, 24, 5c, c2, d3, ac, 62, 91, 95, e4, 79,
		     e7, c8, 37, 6d, 8d, d5, 4e, a9, 6c, 56, f4, ea, 65, 7a, ae, 08,
		     ba, 78, 25, 2e, 1c, a6, b4, c6, e8, dd, 74, 1f, 4b, bd, 8b, 8a,
		     70, 3e, b5, 66, 48, 03, f6, 0e, 61, 35, 57, b9, 86, c1, 1d, 9e,
		     e1, f8, 98, 11, 69, d9, 8e, 94, 9b, 1e, 87, e9, ce, 55, 28, df,
		     8c, a1, 89, 0d, bf, e6, 42, 68, 41, 99, 2d, 0f, b0, 54, bb, 16};

// multiplication in GF(2^8)
uint8_t galois_mul(uint8_t a, uint8_t b);
uint8_t galois_mul2(uint8_t a);
uint8_t galois_mul3(uint8_t a);

void initialize_aes_sbox(uint8_t sbox[256]);
void initialize_aes_inverse_sbox(uint8_t sbox[256], uint8_t inverse_sbox[256]);

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

void initialize_aes_inverse_sbox(uint8_t sbox[256], uint8_t inverse_sbox[256])
{
  for (size_t i = 0; i < 256; ++i) {
    for (size_t j = 0; j < 256; ++j) {
      if (sbox[j] == i) {
	inverse_sbox[i] = j;
	break;
      }
    }
  }
}

#endif // AES_IMPLEMENTATION
