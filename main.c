#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#define WIDTH  1280
#define HEIGHT 720
#define COLOR_CHANNELS 3

typedef struct {
  uint8_t *data;
  size_t size;
} Memory;

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

  fclose(file);
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

void memory_save_as_ppm_file(Memory *memory, const char *file_name)
{
  FILE *file = fopen(file_name, "wb");
  if (file == NULL) {
    fprintf(stderr, "ERROR: could not open file `%s`: %s\n", file_name, strerror(errno));
    exit(1);
  }

  fprintf(file, "P6\n%zu %zu 255\n", (size_t)WIDTH, (size_t)HEIGHT);
  if (ferror(file)) {
    fprintf(stderr, "ERROR: could not write to file `%s`: %s\n", file_name, strerror(errno));
    exit(1);
  }
  
  for(size_t i = 0; i < HEIGHT*WIDTH; ++i) {
    for (size_t j = 0; j < COLOR_CHANNELS; ++j) {
      if (i < memory->size) {
	fprintf(file, "%c", memory->data[i]);
      } else {
	fprintf(file, "%c", (char) 0);
      }
    }
  }

  fclose(file);
}

void box_pattern(Memory *memory, size_t box_size)
{
  Memory new_mem = {0};
  size_t height = (memory->size * box_size) / WIDTH;
  height += !!((memory->size * box_size) % WIDTH != 0);
  new_mem.size = box_size * box_size * WIDTH * height;
  new_mem.data = calloc(WIDTH*HEIGHT*COLOR_CHANNELS, sizeof(char));
  
  size_t index = 0;
  for (size_t h = 0; h < height; ++h) {
    for (size_t i = 0; i < WIDTH/box_size; ++i) {
      for (size_t j = 0; j < box_size; ++j) {
	for (size_t k = 0; k < box_size; ++k) {
	  if (index >= memory->size) goto done;
	  new_mem.data[j*WIDTH+k + box_size*i + box_size*(WIDTH*h)] = memory->data[index];
	}
      }
      index += 1;
    }
  }

 done:
  free(memory->data);
  memory->data = new_mem.data;
  memory->size = new_mem.size;
}

int main(void)
{
  Memory memory = {0};
  file_to_memory("./input", &memory);
  box_pattern(&memory, 4);
  memory_save_as_ppm_file(&memory, "./output.ppm");
  
  return 0;
}
