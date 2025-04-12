#ifndef FFMPEG_H_
#define FFMPEG_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#define GRID_SIZE 20
#define WIDTH     1280
#define HEIGHT    720

/*
  CMD for raw data to video -> ffmpeg -f rawvideo -pix_fmt rgb24 -s:v 1280x720 -r 24 -i input -c:v libx264 out.mp4
  CMD to copy video to windows -> cp out.mp4 /mnt/p/ && rm out.mp4

  Overall CMD : cc -o temp temp.c && ./temp && ffmpeg -f rawvideo -pix_fmt rgb24 -s:v 1280x720 -r 24 -i raw_pixels -c:v libx264 out.mp4 && cp out.mp4 /mnt/p/ && rm out.mp4
*/

void write_solid_color_to_file(const char *file_path, uint32_t color, uint32_t frames);
void box_grid_from_file(const char *input_file_path, const char *output_file_path);
uint64_t get_file_size(const char *file_path);
char *get_file_name_from_path(const char *file_path);
void write_header_info_to_file(const char *input_file_path, const char *output_file_path);
void add_padding_to_file(const char *file_path);
void file_to_video(const char *input_path, const char *video_path);

#endif // FFMPEG_H_

#ifdef FFMPEG_IMPLEMENTATION

void write_solid_color_to_file(const char *file_path, uint32_t color, uint32_t frames)
{
  FILE *file = fopen(file_path, "wb");

  // uint32_t color = 0xXXRRGGBB
  // Each frame would cost us 1280*720*3 bytes of storage
  for (size_t frame = 0; frame < frames; ++frame) {
    for (size_t i = 0; i < 720; ++i) {
      for (size_t j = 0; j < 1280; ++j) {
	uint8_t bytes[3] = {(color >> (8*2)) & 0xFF,
			    (color >> (8*1)) & 0xFF,
			    (color >> (8*0)) & 0xFF};
	fwrite(&bytes, sizeof(bytes), 1, file);
      }
    }
  }

  fclose(file);
}

char *byte_to_cstr(uint8_t byte)
{
  char *cbyte = (char *) malloc(sizeof(char) * 8);
  for (size_t i = 0; i < 8; ++i) {
    cbyte[i] = (((byte << i) & 0x80) >> 7) + '0';
  }
  return cbyte;
}

void box_grid_from_file(const char *input_file_path, const char *output_file_path)
{
  // TODO: make a function or macro which takes file_path and file_mode
  FILE *input_file = fopen(input_file_path, "rb");
  if (input_file == NULL) {
    fprintf(stderr, "ERROR: could not open file %s: %s\n", input_file_path, strerror(errno));
    exit(1);
  }
  
  FILE *output_file = fopen(output_file_path, "wb");
  if (output_file == NULL) {
    fprintf(stderr, "ERROR: could not open file %s: %s\n", output_file_path, strerror(errno));
    exit(1);
  }
  
  size_t file_size = get_file_size(input_file_path);
  size_t height = file_size / 16;
  size_t width  = file_size % 16;

  uint8_t arr[16] = {0};
  
  for (size_t i = 0; i < height; ++i) {
    for (size_t j = 0; j < 16; ++j) {
      arr[j] = fgetc(input_file);
    }

    for (size_t j = 0; j < GRID_SIZE; ++j) {
      for (size_t h = 0; h < 16; ++h) {
	char *cbyte = byte_to_cstr(arr[h]);
	for (size_t g = 0; g < 8; ++g) {
	  for (size_t k = 0; k < GRID_SIZE; ++k) {
	    for (size_t l = 0; l < 3; ++l) {
	      if (cbyte[g] == '0') {
		fputc(0x00, output_file);
	      } else if (cbyte[g] == '1') {
		fputc(0xFF, output_file);
	      }
	    }
	  }
	}
	free(cbyte);
      }
    }
  }
  
  for (size_t i = 0; i < width; ++i) {
    arr[i] = fgetc(input_file);
  }

  for (size_t j = 0; j < GRID_SIZE; ++j) {
    for (size_t h = 0; h < width; ++h) {
      char *cbyte = byte_to_cstr(arr[h]);
      for (size_t g = 0; g < 8; ++g) {
	for (size_t k = 0; k < GRID_SIZE; ++k) {
	  for (size_t l = 0; l < 3; ++l) {
	    if (cbyte[g] == '0') {
	      fputc(0x00, output_file);
	    } else if (cbyte[g] == '1') {
	      fputc(0xFF, output_file);
	    }
	  }
	}
      }
      free(cbyte);
    }

    for (size_t h = 0; h < 16-width; ++h) {
      for (size_t g = 0; g < 8; ++g) {
	for (size_t k = 0; k < GRID_SIZE; ++k) {
	  for (size_t l = 0; l < 3; ++l) {
	    fputc(0x00, output_file);
	  }
	}
      }
    }
  }
}

uint64_t get_file_size(const char *file_path)
{
  FILE *file = fopen(file_path, "rb");
  if (file == NULL) {
    fprintf(stderr, "ERROR: could not open file %s: %s\n", file_path, strerror(errno));
    exit(1);
  }
  
  if (fseek(file, 0, SEEK_END) < 0) {
    fprintf(stderr, "ERROR: could not read file %s\n", strerror(errno));
    exit(1);
  }

  uint64_t file_size = ftell(file);
  if (fseek(file, 0, SEEK_SET) < 0) {
    fprintf(stderr, "ERROR: could not read file %s\n", strerror(errno));
    exit(1);
  }

  fclose(file);

  return file_size;
}

char *get_file_name_from_path(const char *file_path)
{
  size_t slash_index = strlen(file_path);
  for (size_t i = strlen(file_path) - 1; i < strlen(file_path); --i) {
    if (file_path[i] == '/') {
      slash_index = i+1;
      break;
    }
  }

  char buffer[1024] = {0};
  uint16_t buffer_length;
  if (slash_index < strlen(file_path)) {
    buffer_length = sprintf(buffer, "%s", file_path+slash_index);
  } else {
    buffer_length = sprintf(buffer, "%s", file_path);
  }

  char *file_name = (char *) malloc(sizeof(char) * buffer_length + 1);
  file_name[buffer_length] = '\0';
  memcpy(file_name, buffer, buffer_length);

  return file_name;
}

void write_header_info_to_file(const char *input_file_path, const char *output_file_path)
{
  char *file_name = get_file_name_from_path(input_file_path);
  uint16_t file_name_length = (uint16_t) strlen(file_name);
  uint64_t file_size = get_file_size(input_file_path);

  FILE *input_file = fopen(input_file_path, "rb");
  if (input_file == NULL) {
    fprintf(stderr, "ERROR: could not open file %s: %s\n", input_file_path, strerror(errno));
    exit(1);
  }
  
  FILE *output_file = fopen(output_file_path, "wb");
  if (output_file == NULL) {
    fprintf(stderr, "ERROR: could not open file %s: %s\n", output_file_path, strerror(errno));
    exit(1);
  }

  fwrite(&file_name_length, sizeof(file_name_length), 1, output_file);
  fprintf(output_file, "%s", file_name);
  fwrite(&file_size, sizeof(file_size), 1, output_file);
  free(file_name);

  for (size_t i = 0; i < file_size; ++i) {
    fputc(fgetc(input_file), output_file);
  }

  fclose(input_file);
  fclose(output_file);
}

void add_padding_to_file(const char *file_path)
{
  FILE *file = fopen(file_path, "ab");
  if (file == NULL) {
    fprintf(stderr, "ERROR: could not open file %s: %s\n", file_path, strerror(errno));
    exit(1);
  }

#define PIXELS_IN_ONE_FRAME (1280*720*3)
  
  uint64_t file_size = get_file_size(file_path);
  size_t padding_size = (PIXELS_IN_ONE_FRAME - (file_size % PIXELS_IN_ONE_FRAME)) % PIXELS_IN_ONE_FRAME;

  for (size_t i = 0; i < padding_size; ++i) {
    fputc(0, file);
  }
  
  fclose(file);
}

void file_to_video(const char *input_path, const char *video_path)
{
  const char *temp_path1 = "./output1.bin";
  const char *temp_path2 = "./output2.bin";
  box_grid_from_file(input_path, temp_path1);
  write_header_info_to_file(temp_path1, temp_path2);
  add_padding_to_file(temp_path2);

  char buffer[512];
  sprintf(buffer, "ffmpeg -f rawvideo -pix_fmt rgb24 -s:v 1280x720 -r 24 -i %s -c:v libx264 %s", temp_path2, video_path);
  system(buffer);

  sprintf(buffer, "cp %s /mnt/p/ && rm %s", video_path, video_path);
  system(buffer);
}

#endif // FFMPEG_IMPLEMENTATION
