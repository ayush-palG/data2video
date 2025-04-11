/*
  Huffman Coding Specifications
  * Header Information:
    - total number of bytes storing the header, i.e., the topology of the Huffman coding tree, as a 4-byte unsigned integer.
    - total number of characters in the uncompressed file, as a 8-byte unsigned integer.
    - Huffman tree structure
      . To store the tree at the beginning of the file, we use a post-order traversal, writing each node visited.
        When you encounter a leaf node, you write a 1 followed by the ASCII character of the leaf node.
	When you encounter a non-leaf node, you write a 0. To indicate the end of the Huffman
	coding tree, we write another 0.
      . Size of Huffman tree structure should be of 8*n bits(n bytes), fill the remaining (if any) bits with zero
  * Then add the compressed text
  * At last add the pseudo-EOF, we have number of characters in original file , we could only decode that many
    characters and ignore any more bits (if any).
*/

#ifndef HUFFMAN_H_
#define HUFFMAN_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#define U8_CAPACITY 258

typedef struct {
  uint8_t byte;
  uint32_t count;
} Freq;

typedef struct {
  Freq *arr;
  size_t size;
} Freq_List;

typedef struct node {
  Freq freq;
  struct node* left;
  struct node* right;
} Node;


// Frequency
void print_freq(const Freq *freq);
void print_freq_list(const Freq_List *fl);
void get_useful_freq_arr(Freq_List *fl);
void sort_freq(Freq_List *fl, size_t index);
void sort_freq_list(Freq_List *fl);
void get_freq_from_file(const char *file_path, Freq_List *fl);


#endif // HUFFMAN_H_

#ifdef HUFFMAN_IMPLEMENTATION

void print_freq(const Freq *freq)
{
  printf("%02x %c: %u\n", freq->byte, freq->byte, freq->count);
}

void print_freq_list(const Freq_List *fl)
{
  for (size_t i = 0; i < fl->size; ++i) {
    print_freq(&fl->arr[i]);
  }
  printf("\n");
}

void get_useful_freq_arr(Freq_List *fl)
{
  size_t useful_freq_count = 0;

  for (size_t i = 0; i < fl->size; ++i) {
    if (fl->arr[i].count > 0) {
      useful_freq_count += 1;
    }
  }

  Freq *useful_freq_arr = (Freq *) malloc(sizeof(Freq) * useful_freq_count);
  
  for (size_t i = 0, index = 0; i < fl->size; ++i) {
    if (fl->arr[i].count > 0) {
      useful_freq_arr[index].byte = fl->arr[i].byte;
      useful_freq_arr[index++].count = fl->arr[i].count;
    }
  }

  free(fl->arr);
  fl->arr = useful_freq_arr;
  fl->size = useful_freq_count;
}

void sort_freq(Freq_List *fl, size_t index)
{
  assert(index < fl->size);
  
  for (size_t j = index; j > 0; --j) {
    if (fl->arr[j].count < fl->arr[j-1].count) {
      break;
    } else {
      Freq temp_freq = fl->arr[j];
      fl->arr[j] = fl->arr[j-1];
      fl->arr[j-1] = temp_freq;
    }
  }
}

void sort_freq_list(Freq_List *fl)
{
  for (size_t i = 0; i < fl->size; ++i) {
    sort_freq(fl, i);
  }
}

void get_freq_from_file(const char *file_path, Freq_List *fl)
{
  FILE *file = fopen(file_path, "rb");
  if (file == NULL) {
    fprintf(stderr, "ERROR: could not open file %s: %s\n", file_path, strerror(errno));
    exit(1);
  }
  
  fl->arr = (Freq *) malloc(sizeof(Freq) * U8_CAPACITY);
  fl->size = U8_CAPACITY;

  for (size_t i = 0; i < fl->size; ++i) {
    fl->arr[i].byte = (uint8_t) i;
    fl->arr[i].count = 0;
  }

  char ch;
  while ((ch = fgetc(file)) != EOF) {
    fl->arr[(uint8_t) ch].count += 1;
  }

  get_useful_freq_arr(fl);

  sort_freq_list(fl);
}

  }

  get_useful_freq_arr(&fl);

  for (size_t i = 0; i < fl.size; ++i) {
    sort_freq(&fl, i);
  }
  print_freq_list(&fl);
}

#endif // HUFFMAN_IMPLEMENTATION
