#define COMPRESSION_IMPLEMENTATION
#include "./compression.h"

int main(void)
{
  Node tree = {0};
  get_huffman_tree_from_file("input", &tree);
  print_node(&tree, 0);
  
  return 0;
}
