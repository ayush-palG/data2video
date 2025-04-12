#ifndef FFMPEG_H_
#define FFMPEG_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

/*
  CMD for raw data to video -> ffmpeg -f rawvideo -pix_fmt rgb24 -s:v 1280x720 -r 24 -i input -c:v libx264 out.mp4
  CMD to copy video to windows -> cp out.mp4 /mnt/p/ && rm out.mp4

  Overall CMD : cc -o temp temp.c && ./temp && ffmpeg -f rawvideo -pix_fmt rgb24 -s:v 1280x720 -r 24 -i raw_pixels -c:v libx264 out.mp4 && cp out.mp4 /mnt/p/ && rm out.mp4
*/

void write_solid_color_to_file(const char *file_path, uint32_t color, uint32_t frames);
uint64_t get_file_size(const char *file_path);
const char *get_file_name_from_path(const char *file_path);
void write_header_info_to_file(const char *input_file_path, const char *output_file_path);
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

const char *get_file_name_from_path(const char *file_path)
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
  const char *file_name = get_file_name_from_path(input_file_path);
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

  char ch;
  while ((ch = fgetc(input_file)) != EOF) {
    fputc(ch, output_file);
  }

  fclose(input_file);
  fclose(output_file);
}

void file_to_video(const char *input_path, const char *video_path)
{
  const char *temp_path = "./output.bin";
  write_header_info_to_file(input_path, temp_path);
  
  char buffer[512];
  sprintf(buffer, "ffmpeg -f rawvideo -pix_fmt rgb24 -s:v 1280x720 -r 24 -i %s -c:v libx264 %s", temp_path, video_path);
  system(buffer);

  sprintf(buffer, "cp %s /mnt/p/ && rm %s", video_path, video_path);
  system(buffer);
}

#endif // FFMPEG_IMPLEMENTATION
