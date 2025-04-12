#define FFMPEG_IMPLEMENTATION
#include "./ffmpeg.h"

int main(void)
{
  write_header_info_to_file("./input", "output");
  write_solid_color_to_file("raw_pixels", 0xFF00FF00, 240);
  file_to_video("raw_pixels", "out.mp4");

  return 0;
}
