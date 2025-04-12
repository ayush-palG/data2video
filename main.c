#define FFMPEG_IMPLEMENTATION
#include "./ffmpeg.h"

int main(void)
{
  file_to_video("input", "out.mp4");

  return 0;
}
