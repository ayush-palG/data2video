#ifndef AES_H_
#define AES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BLOCK_SIZE 16
#define BLOCK_GRID_SIZE 4
#define KEY_SIZE 16
#define KEY_ROUNDS 11

uint8_t sbox[256] = {0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
		     0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
		     0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
		     0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
		     0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
		     0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
		     0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
		     0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
		     0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
		     0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
		     0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
		     0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
		     0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
		     0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
		     0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
		     0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16};

uint8_t inverse_sbox[256] = {0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
			     0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
			     0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
			     0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
			     0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
			     0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
			     0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
			     0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
			     0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
			     0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
			     0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
			     0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
			     0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
			     0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
			     0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
			     0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d};

uint8_t round_constants[10] = {0x01, 0x02, 0x04, 0x08, 0x10,
			       0x20, 0x40, 0x80, 0x1b, 0x36};

void aes_block_encrypt(uint8_t *block, uint8_t *key);

// multiplication in GF(2^8)
uint8_t galois_mul2(uint8_t a);
uint8_t galois_mul3(uint8_t a);

void transpose_block(uint8_t *block);
void print_block(const uint8_t *block);
uint8_t *get_round_keys(uint8_t *key);

void sub_bytes(uint8_t *block);
void shift_rows(uint8_t *block);
void mix_columns(uint8_t *block);
void add_round_key(uint8_t *block, uint8_t *key);

#endif // AES_H_

#ifdef AES_IMPLEMENTATION

void aes_block_encrypt(uint8_t *block, uint8_t *key)
{
  uint8_t *round_keys = get_round_keys(key);
  transpose_block(block);

  //first round
  add_round_key(block, round_keys);
  
  //intermediate rounds
  for (size_t i = 1; i < KEY_ROUNDS-1; ++i) {
    sub_bytes(block);
    shift_rows(block);
    mix_columns(block);
    add_round_key(block, round_keys+(i*BLOCK_SIZE));
  }
  
  //last round
  sub_bytes(block);
  shift_rows(block);
  add_round_key(block, round_keys+(BLOCK_SIZE*(KEY_ROUNDS - 1)));

  print_block(block);

  free(round_keys);
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

void transpose_block(uint8_t *block)
{
  /*
    0 1 2 3        0 4 8 c
    4 5 6 7   ==\  1 5 9 d
    8 9 a b   ==/  2 6 a e
    c d e f        3 7 b f
  */
  for (size_t i = 0; i < BLOCK_GRID_SIZE; ++i) {
    for (size_t j = i+1; j < BLOCK_GRID_SIZE; ++j) {
      uint8_t t = block[i + 4*j];
      block[i + 4*j] = block[4*i + j];
      block[4*i + j] = t;
    }
  }
}

void print_block(const uint8_t *block)
{
  for (size_t i = 0; i < 4; ++i) {
    for (size_t j = 0; j < 4; ++j) {
      printf("%02x ", block[4*i + j]);
    }
    printf("\n");
  }
  printf("\n");
}

void sub_bytes(uint8_t *block)
{
  for (size_t i = 0; i < BLOCK_SIZE; ++i) {
    block[i] = sbox[block[i]];
  }
}

void shift_rows(uint8_t *block)
{
  /*
     1  5  9 13          1  5  9 13
     2  6 10 14    ==\   6 10 14  2
     3  7 11 15    ==/  11 15  3  7
     4  8 12 16         16  4  8 12
  */

  for (size_t i = 0; i < BLOCK_GRID_SIZE; ++i) {
    for (size_t k = 0; k < i; ++k) {
      for (size_t j = 0; j < 3; ++j) {
	uint8_t t = block[BLOCK_GRID_SIZE*i+j+1];
	block[BLOCK_GRID_SIZE*i+j+1] = block[BLOCK_GRID_SIZE*i+j];
	block[BLOCK_GRID_SIZE*i+j] = t;
      }
    }
  }
}

void mix_columns(uint8_t *block)
{
  /*
     1  5  9 13          1`  5`  9` 13`
     2  6 10 14    ==\   2`  6` 10` 14`
     3  7 11 15    ==/   3`  7` 11` 15`
     4  8 12 16          4`  8` 12` 16`
  */
  
  uint8_t arr[BLOCK_GRID_SIZE] = {2,3,1,1};
  uint8_t *ith_col = malloc(sizeof(uint8_t) * BLOCK_GRID_SIZE);
  
  for (size_t i = 0; i < BLOCK_GRID_SIZE; ++i) {
    for (size_t j = 0; j < BLOCK_GRID_SIZE; ++j) {
      ith_col[j] = 0;
    }
    
    for (size_t j = 0;  j < BLOCK_GRID_SIZE; ++j) {
      for (size_t k = 0; k < BLOCK_GRID_SIZE; ++k) {
	if (arr[(k-j)%BLOCK_GRID_SIZE] == 3) {
	  ith_col[j] ^= galois_mul3(block[i + BLOCK_GRID_SIZE*k]);
	} else if (arr[(k-j)%BLOCK_GRID_SIZE] == 2) {
	  ith_col[j] ^= galois_mul2(block[i + BLOCK_GRID_SIZE*k]);
	} else if (arr[(k-j)%BLOCK_GRID_SIZE] == 1) {
	  ith_col[j] ^= block[i + BLOCK_GRID_SIZE*k];
	}
      }
    }
    
    for (size_t j = 0; j < BLOCK_GRID_SIZE; ++j) {
      block[i + BLOCK_GRID_SIZE*j] = ith_col[j];
    }
  }
  free(ith_col);
}

void add_round_key(uint8_t *block, uint8_t *key)
{
  for (size_t i = 0; i < BLOCK_SIZE; ++i) {
    block[i] ^= key[i];
  }
}

uint8_t *get_round_keys(uint8_t *key)
{
  uint8_t *round_keys = malloc(sizeof(uint8_t) * KEY_SIZE * KEY_ROUNDS);
  
  for (size_t i = 0; i < KEY_SIZE * KEY_ROUNDS; i += 4) {
    if (i < KEY_SIZE) {
      for (size_t j = 0; j < 4; ++j) {
	round_keys[i+j] = key[i+j];
      }
    } else if (i >= KEY_SIZE && (i % KEY_SIZE) == 0) {
      for (size_t j = 0; j < 4; ++j) {
	round_keys[i+j] = round_keys[i+j - KEY_SIZE];

	if (j == 3) {
	  round_keys[i+j] ^= sbox[round_keys[i - 4]];
	} else {
	  round_keys[i+j] ^= sbox[round_keys[i - (4-j) + 1]];
	}
	
	if (j == 0) {
	  round_keys[i+j] ^= round_constants[(i / KEY_SIZE) - 1];
	}
      }
    } else {
      for (size_t j = 0; j < 4; ++j) {
	round_keys[i+j] = round_keys[i+j - KEY_SIZE] ^ round_keys[i+j - 4];
      }
    }
  }

  for (size_t i = 0; i < KEY_ROUNDS; ++i) {
    transpose_block(round_keys+(i*BLOCK_SIZE));
  }

  return round_keys;
}

#endif // AES_IMPLEMENTATION
