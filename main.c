#define FFMPEG_IMPLEMENTATION
#include "./ffmpeg.h"

int main(void)
{
  write_solid_color_to_file("raw_pixels", 0xFF00FF00, 240);
  return 0;
}
