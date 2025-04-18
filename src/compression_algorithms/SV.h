#ifndef SV_H_
#define SV_H_

typedef struct {
  char *str;
  size_t size;
  size_t capacity;
} String_View;

// TODO: sv_alloc would return String_View*
// TODO: If sv_alloc returns String_View*, then create a func to free the allocated memory
void sv_alloc(String_View *sv, size_t size);

void sv_rev(String_View *sv, size_t start_pos, size_t end_pos);
void sv_rev_bytes(String_View *sv);

void sv_concat_sv(String_View *dst, String_View *src);
void sv_concat_file(FILE *file, String_View *src);
void sv_concat_file_path(const char *file_path, String_View *src);

void sv_byte_to_bits(String_View *sv, uint8_t byte);
bool sv_bits_to_byte(String_View *sv, uint8_t *byte);
void sv_to_bytes(String_View *sv);
void sv_print_bytes(String_View *sv);

#endif // SV_H_


#ifdef SV_IMPLEMENTATION

void sv_alloc(String_View *sv, size_t size)
{
  *sv = (String_View) {
    .str = (char *) malloc(sizeof(char) * size),
    .size = 0,
    .capacity = size,
  };
}

void sv_rev(String_View *sv, size_t start_pos, size_t end_pos)
{
  assert(start_pos <= end_pos);
  for(size_t i = 0; i < (end_pos - start_pos + 1) / 2; ++i) {
    char ch = sv->str[start_pos + i];
    sv->str[start_pos + i] = sv->str[end_pos - i];
    sv->str[end_pos - i] = ch;
  }
}

void sv_rev_bytes(String_View *sv)
{
  sv_rev(sv, 0, sv->size - 1);
  for (size_t i = sv->size - 1; i >= U8_SIZE-1 && i < sv->size; i -= U8_SIZE) {
    sv_rev(sv, i+1 - U8_SIZE, i);
  }
  sv_rev(sv, 0, sv->size % U8_SIZE - 1);
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
  sv_rev_bytes(src);
  uint8_t byte = 0;
  while (src->size >= U8_SIZE) {
    assert(src->size + U8_SIZE <= src->capacity);
    sv_bits_to_byte(src, &byte);
    fwrite(&byte, sizeof(byte), 1, file);
  }
}

// TODO: Instead of adding the bits at the end of sv, alloc a new sv and add to it them.
void sv_byte_to_bits(String_View *sv, uint8_t byte)
{
  for (size_t i = 0; i < U8_SIZE; ++i) {
    sv->str[sv->size++] = (((byte << i) & 0x80) >> 7) + '0';
  }
}

// TODO: there is no need to spit out bool here
bool sv_bits_to_byte(String_View *sv, uint8_t *byte)
{
  if (sv->size >= U8_SIZE) {
    *byte = 0;
    for (size_t i = sv->size-U8_SIZE; i < sv->size; ++i) {
      assert(sv->str[i] == '0' || sv->str[i] == '1');
      *byte = (*byte << 1) | (sv->str[i] - '0');
    }
    // TODO: To make it more general, remove the following line and add it to where it needed
    sv->size -= U8_SIZE;
    return true;
  }

  return false;
}

void sv_bytes_to_bits(String_View *sv)
{
  sv_rev_bytes(sv);
  String_View sv_byte = {0};
  sv_alloc(&sv_byte, U8_SIZE);
  for (size_t i = sv->size - 1; i < sv->size; --i) {
    sv_byte_to_bits(&sv_byte, sv->str[i]);
    for (size_t j = 0; j < U8_SIZE; ++j) {
      sv->str[U8_SIZE*(i-1) + j] = sv_byte.str[j];
    }
    // sv->size += 7;
    sv_byte.size = 0;
  }
  free(sv_byte.str);
}

void sv_print_bytes(String_View *sv)
{
  String_View sv_byte = {0};
  sv_alloc(&sv_byte, U8_SIZE);
  for (size_t i = 0; i < sv->size / U8_SIZE; ++i) {
    uint8_t byte = sv->str[i];
    sv_byte_to_bits(&sv_byte, byte);
    printf("%02x %.*s\n", byte, (int) sv_byte.size, sv_byte.str);
    sv_byte.size = 0;
  }
  printf("\n");
  free(sv_byte.str);
}

#endif // SV_IMPLEMENTATION
