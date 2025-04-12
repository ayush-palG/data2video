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


#endif // FFMPEG_IMPLEMENTATION
