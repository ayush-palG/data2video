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
#include <stdbool.h>

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

typedef struct {
  Node *arr;
  size_t size;
} Node_List;

// Frequency
void print_freq(const Freq *freq);
void print_freq_list(const Freq_List *fl);
void get_useful_freq_arr(Freq_List *fl);
void sort_freq(Freq_List *fl, size_t index);
void sort_freq_list(Freq_List *fl);
void get_freq_from_file(const char *file_path, Freq_List *fl);

// Node
void print_node(Node *node, int level);
void freq_to_node(Freq_List *fl, Node_List *nl);
void sort_node(Node_List *nl, size_t index);
void sort_node_list(Node_List *nl);
bool node_eq(const Node *node1, const Node *node2);
int find_node_in_node_list(const Node_List *nl, const Node *node);
void add_node_to_tree(Node_List *char_nl, Node_List *temp_nl);

Node get_huffman_tree(const char *file_path);

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

void print_node(Node *node, int level)
{
  if (node == NULL)
    return;
  for (int i = 0; i < level; i++)
    printf(i == level - 1 ? "|-" : "  ");
  printf("%02x %c: %u\n", node->freq.byte, node->freq.byte, node->freq.count);
  print_node(node->left, level + 1);
  print_node(node->right, level + 1);
}

void freq_to_node(Freq_List *fl, Node_List *nl)
{
  nl->arr = (Node *) malloc(sizeof(Node) * fl->size * 2);
  nl->size = fl->size;
  
  for (size_t i = 0; i < fl->size; ++i) {
    nl->arr[i] = (Node) {
      .freq = fl->arr[i],
      .left = NULL,
      .right = NULL,
    };
  }

  for (size_t i = fl->size; i < fl->size * 2; ++i) {
    nl->arr[i] = (Node) {
      .freq = (Freq) {0},
      .left = NULL,
      .right = NULL,
    };
  }
}

void sort_node(Node_List *nl, size_t index)
{
  assert(index < nl->size);
  
  for (size_t j = index; j > 0; --j) {
    if (nl->arr[j].freq.count < nl->arr[j-1].freq.count) {
      break;
    } else {
      Node temp_node = nl->arr[j];
      nl->arr[j] = nl->arr[j-1];
      nl->arr[j-1] = temp_node;
    }
  }
}

void sort_node_list(Node_List *nl)
{
  for (size_t i = 0; i < nl->size; ++i) {
    sort_node(nl, i);
  }
}

bool node_eq(const Node *node1, const Node *node2)
{
  return (bool) (node1->freq.count == node2->freq.count && node1->freq.byte == node2->freq.byte);
}

int find_node_in_node_list(const Node_List *nl, const Node *node)
{
  for (size_t i = 0; i < nl->size; ++i) {
    if (node_eq(&nl->arr[i], node)) return i;
  }
  return -1;
}

void add_node_to_tree(Node_List *char_nl, Node_List *temp_nl)
{
  assert(temp_nl->size > 1);

  Node *node1 = &temp_nl->arr[temp_nl->size-1];
  Node *node2 = &temp_nl->arr[temp_nl->size-2];

  Node new_node = {
    .freq = (Freq) {.byte = 0, .count = node1->freq.count + node2->freq.count},
    .left = &char_nl->arr[find_node_in_node_list(char_nl, node1)],
    .right = &char_nl->arr[find_node_in_node_list(char_nl, node2)],
  };

  temp_nl->arr[temp_nl->size++] = new_node;
  char_nl->arr[char_nl->size++] = new_node;
  sort_node(temp_nl, temp_nl->size-1);
  temp_nl->size -= 2;
}

Node get_huffman_tree(const char *file_path)
{
  Freq_List fl = {0};
  get_freq_from_file(file_path, &fl);

  Node_List char_nl = {0};
  Node_List temp_nl = {0};
  freq_to_node(&fl, &char_nl);
  freq_to_node(&fl, &temp_nl);
  free(fl.arr);

  while (temp_nl.size > 1) {
    add_node_to_tree(&char_nl, &temp_nl);
  }

  print_node(&temp_nl.arr[0], 0);

  free(char_nl.arr);
  free(temp_nl.arr);
  
  return (Node) {0};
}

#endif // HUFFMAN_IMPLEMENTATION
