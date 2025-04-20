#ifndef SV_H_
#define SV_H_

#define U8_SIZE 8

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

typedef enum {
  BITS,
  BYTES,
} String_Type;

typedef struct {
  char *str;
  size_t size;
  size_t capacity;
  String_Type type;
} String_View;

String_View sv_alloc(size_t size, String_Type type);

// TODO: Once the String_Type enum introduced, change reverse functions accordingly
void sv_rev_within_bound(String_View *sv, size_t start_pos, size_t end_pos);
void sv_rev(String_View *sv);

void sv_concat_sv(String_View *dst, String_View *src);
void sv_concat_file(FILE *file, String_View *src);

void sv_bits_from_byte(String_View *sv, uint8_t byte);
void sv_bits_to_byte(String_View *sv, uint8_t *byte);
void sv_bits_from_sv_bytes(String_View *sv);
void sv_bytes_from_sv_bits(String_View *sv);

void sv_print(String_View *sv);

#endif // SV_H_


#ifdef SV_IMPLEMENTATION

String_View sv_alloc(size_t size, String_Type type)
{
  return (String_View) {
    .str = (char *) malloc(sizeof(char) * size),
    .size = 0,
    .capacity = size,
    .type = type,
  };
}

void sv_rev_within_bound(String_View *sv, size_t start_pos, size_t end_pos)
{
  assert(start_pos <= end_pos);
  for(size_t i = 0; i < (end_pos - start_pos + 1) / 2; ++i) {
    char ch = sv->str[start_pos + i];
    sv->str[start_pos + i] = sv->str[end_pos - i];
    sv->str[end_pos - i] = ch;
  }
}

void sv_rev(String_View *sv)
{
  switch (sv->type) {
  case BITS: {
    sv_rev_within_bound(sv, 0, sv->size - 1);
    for (size_t i = sv->size - 1; i >= U8_SIZE-1 && i < sv->size; i -= U8_SIZE) {
      sv_rev_within_bound(sv, i+1 - U8_SIZE, i);
    }
    sv_rev_within_bound(sv, 0, sv->size % U8_SIZE - 1);
  } break;
      
  case BYTES: {
    sv_rev_within_bound(sv, 0, sv->size - 1);
  } break;
    
  default:
    printf("unreachable: sv_rev");
  }
}

void sv_concat_sv(String_View *dst, String_View *src)
{
  assert(dst->size + src->size <= dst->capacity);
  for (size_t i = 0; i < src->size; ++i) {
    dst->str[dst->size++] = src->str[i];
  }
}

void sv_concat_file(FILE *file, String_View *src)
{
  sv_rev(src);
  uint8_t byte = 0;
  while (src->size >= U8_SIZE) {
    assert(src->size + U8_SIZE <= src->capacity);
    sv_bits_to_byte(src, &byte);
    fwrite(&byte, sizeof(byte), 1, file);
  }
}

void sv_bits_from_byte(String_View *sv, uint8_t byte)
{
  assert(sv->type == BITS);
  for (size_t i = 0; i < U8_SIZE; ++i) {
    sv->str[sv->size++] = (((byte << i) & 0x80) >> 7) + '0';
  }
}

void sv_bits_to_byte(String_View *sv, uint8_t *byte)
{
  assert(sv->type == BITS);
  if (sv->size >= U8_SIZE) {
    *byte = 0;
    for (size_t i = sv->size-U8_SIZE; i < sv->size; ++i) {
      *byte = (*byte << 1) | (sv->str[i] - '0');
    }
    // TODO: To make it more general, remove the following line and add it to only where it needed
    sv->size -= U8_SIZE;
  }
}

void sv_bits_from_sv_bytes(String_View *sv)
{
  assert(sv->type == BYTES);
  String_View sv_bits = sv_alloc(sv->size * U8_SIZE, BITS);
  
  for (size_t i = 0; i < sv->size; ++i) {
    sv_bits_from_byte(&sv_bits, sv->str[i]);
  }

  free(sv->str);
  *sv = (String_View) {
    .str = sv_bits.str,
    .size = sv_bits.capacity,
    .capacity = sv_bits.capacity,
    .type = sv_bits.type
  };
}

void sv_bytes_from_sv_bits(String_View *sv)
{
  assert(sv->type == BITS);
  assert(sv->size % U8_SIZE == 0);
  String_View sv_bytes = sv_alloc(sv->size / U8_SIZE, BYTES);
  uint8_t byte = 0;

  for (size_t i = 0; i < sv_bytes.capacity; ++i) {
    sv_bits_to_byte(sv, &byte);
    sv_bytes.str[sv_bytes.size++] = byte;
  }

  free(sv->str);
  *sv = (String_View) {
    .str = sv_bytes.str,
    .size = sv_bytes.size,
    .capacity = sv_bytes.capacity,
    .type = sv_bytes.type
  };
}

void sv_print(String_View *sv)
{
  switch (sv->type) {
  case BITS: {
    for (size_t i = 0; i < sv->size; i += U8_SIZE) {
      printf("%.*s ", U8_SIZE, sv->str+(i*U8_SIZE));
    }
    printf("\n");
  } break;
    
  case BYTES: {
    for (size_t i = 0; i < sv->size; ++i) {
      printf("%c ", sv->str[i]);
    }
    printf("\n");
  } break;
    
  default:
    printf("unreachable: sv_print\n");
  }
}

#endif // SV_IMPLEMENTATION
