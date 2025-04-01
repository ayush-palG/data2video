#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

size_t get_file_size(FILE *file)
{
  if (fseek(file, 0, SEEK_END) < 0) {
    fprintf(stderr, "ERROR: could not read file %s\n", strerror(errno));
    exit(1);
  }

  size_t file_size = ftell(file);
  if (fseek(file, 0, SEEK_SET) < 0) {
    fprintf(stderr, "ERROR: could not read file %s\n", strerror(errno));
    exit(1);
  }

  return file_size;
}

typedef struct {
  char *data;
  size_t size;
} Memory;

void file_to_memory(const char *file_name, Memory *memory)
{
  FILE *file = fopen(file_name, "rb");
  if (file == NULL) {
    fprintf(stderr, "ERROR: could not open file `%s`: %s\n", file_name, strerror(errno));
    exit(1);
  }

  size_t file_size = get_file_size(file);

  memory->data = malloc(file_size);
  memory->size = 0;
  char temp;
  while ((temp = getc(file)) != EOF) {
    memory->data[memory->size++] = temp;
  }

  if (memory->size != file_size) {
    fprintf(stderr, "ERROR: could not read full file(%zu) into memory(%zu)\n", file_size, memory->size);
    exit(1);
  }
}

void print_memory(const Memory *memory)
{
  for (size_t i = 0; i < memory->size; ++i) {
    printf("%c", memory->data[i]);
  }
}

int main(void)
{
  Memory memory = {0};
  file_to_memory("./input", &memory);
  print_memory(&memory);
  
  return 0;
}
