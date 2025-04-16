/*
  Huffman Coding Specifications
  * Header Information:
    X total number of bits storing the topology of the Huffman coding tree, as a 4-byte unsigned integer.
    - total number of distinct characters in the uncompressed file, as a 1-byte unsigned integer.
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
#define SV_CAPACITY (U8_CAPACITY * 2)

// Frequency
typedef struct {
  uint8_t byte;
  uint32_t count;
} Freq;

typedef struct {
  Freq *arr;
  size_t size;
} Freq_List;

void get_useful_freq_arr(Freq_List *fl);
void get_freq_from_file(const char *file_path, Freq_List *fl);

// Node
typedef struct node {
  Freq freq;
  struct node* left;
  struct node* right;
} Node;

typedef struct {
  Node **node_ptrs;
  size_t size;
} Node_List;

void print_node(Node *node, int level);
Node *node_alloc(Freq freq, Node *left, Node *right);
void node_list_add(Node_List *nl, Node *node_ptr);
Node_List *freq_list_to_node_list(Freq_List *fl);
void sort_node(Node_List *nl, size_t index);
void get_huffman_tree_from_file(const char *file_path, Node *tree);

// String View
typedef struct {
  char *str;
  size_t size;
} String_View;

// TODO: maybe introduce sv_from_cstr and sv_to_cstr (especially for value in Table_Item)
void sv_concat_cstr(String_View *sv, const char *cstr);
void sv_from_byte(String_View *sv, uint8_t byte);
bool sv_to_byte(const String_View *sv, uint8_t *byte);

// Huffman Table
typedef struct {
  uint8_t byte;
  char *value;
} Table_Item;

typedef struct {
  Table_Item *items;
  size_t size;
} Table;

void print_table(const Table *table);
void free_table(Table *table);
void in_order(String_View *sv, Node *node, Table *table);
void node_to_table(Node *node, Table *table);
void get_leaf_node_count_in_tree(Node *tree, size_t *counter);
void get_huffman_table_from_tree(Node *tree, Table *table);

uint64_t get_file_size(const char *file_path);
void post_order(String_View *sv, Node *tree);
String_View get_header_info_from_tree(Node *tree);

void huffman_encode(const char *plain_file_path, const char *encoded_file_path);

#endif // HUFFMAN_H_

#ifdef HUFFMAN_IMPLEMENTATION

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
}

void print_node(Node *node, int level)
{
  if (node == NULL) {
    return;
  }
  
  for (int i = 0; i < level; i++) {
    printf(i == level - 1 ? "|-" : "  ");
  }
  printf("%02x %c: %u\n", node->freq.byte, node->freq.byte, node->freq.count);
  print_node(node->left, level + 1);
  print_node(node->right, level + 1);
}

void sort_node(Node_List *nl, size_t index)
{
  assert(index < nl->size);
  
  for (size_t j = index; j > 0; --j) {
    if (nl->node_ptrs[j]->freq.count < nl->node_ptrs[j-1]->freq.count) {
      break;
    } else {
      Node *temp_node_ptr = nl->node_ptrs[j];
      nl->node_ptrs[j] = nl->node_ptrs[j-1];
      nl->node_ptrs[j-1] = temp_node_ptr;
    }
  }
}

Node *node_alloc(Freq freq, Node *left, Node *right)
{
  Node *node = (Node *) malloc(sizeof(Node));
  *node = (Node) {
    .freq = freq,
    .left = left,
    .right = right
  };
  return node;
}

void node_list_add(Node_List *nl, Node *node_ptr)
{
  nl->node_ptrs[nl->size++] = node_ptr;
  sort_node(nl, nl->size - 1);
}

Node_List *freq_list_to_node_list(Freq_List *fl)
{
  Node_List *nl = (Node_List *) malloc(sizeof(Node_List));
  nl->node_ptrs = (Node **) malloc(sizeof(Node *) * (fl->size + 1));
  nl->size = 0;

  for (size_t i = 0; i < fl->size; ++i) {
    node_list_add(nl, node_alloc(fl->arr[i], NULL, NULL));
  }
  return nl;
}

void get_huffman_tree_from_file(const char *file_path, Node *tree)
{
  Freq_List fl = {0};
  get_freq_from_file(file_path, &fl);

  Node_List *nl = freq_list_to_node_list(&fl);
  free(fl.arr);

  while (nl->size > 1) {
    Freq freq = {
      .byte = 0,
      .count = nl->node_ptrs[nl->size - 1]->freq.count + nl->node_ptrs[nl->size - 2]->freq.count,
    };
    Node *node = node_alloc(freq, nl->node_ptrs[nl->size-1], nl->node_ptrs[nl->size-2]);
    node_list_add(nl, node);
    nl->size -= 2;
  }

  *tree = *(nl->node_ptrs[0]);  
  free(nl->node_ptrs[0]);
  free(nl->node_ptrs);
  free(nl);
}

void sv_concat(String_View *dst, String_View *src)
{
  assert(dst->size + src->size < SV_CAPACITY);
  for (size_t i = 0; i < src->size; ++i) {
    dst->str[dst->size++] = src->str[i];
  }
}

void sv_from_byte(String_View *sv, uint8_t byte)
{
  for (size_t i = 0; i < 8; ++i) {
    sv->str[sv->size++] = (((byte << i) & 0x80) >> 7) + '0';
  }
}

bool sv_to_byte(const String_View *sv, uint8_t *byte)
{
  if (sv->size >= 8) {
    *byte = 0;
    for (size_t i = 0; i < 8; ++i) {
      *byte = (*byte << 1) | (sv->str[i] - '0');
    }
    return true;
  }

  return false;
}

void print_table(const Table *table)
{
  for (size_t i = 0; i < table->size; ++i) {
    printf("%02x %c: %8s\n", table->items[i].byte, table->items[i].byte, table->items[i].value);
  }
  printf("\n");
}

void free_table(Table *table)
{
  for (size_t i = 0; i < table->size; ++i) {
    free(table->items[i].value);
  }
  free(table->items);
}

// TODO: Introduce in-order, post-order general traversals and then use them
void in_order(String_View *sv, Node *node, Table *table)
{
  if (node->left == NULL && node->right == NULL) {
    char *value = (char *) malloc(sizeof(char) * sv->size + 1);
    value[sv->size] = '\0';
    memcpy(value, sv->str, sv->size);
    table->items[table->size++] = (Table_Item) {
      .byte = node->freq.byte,
      .value = value,
    };
    return;
  }

  sv->str[sv->size++] = '0';
  in_order(sv, node->left, table);
  sv->size -= 1;
  
  sv->str[sv->size++] = '1';
  in_order(sv, node->right, table);
  sv->size -= 1;
}

void node_to_table(Node *node, Table *table)
{
  char *str = (char *) malloc(sizeof(char) * U8_CAPACITY);
  String_View sv = {.str = str, .size = 0};

  in_order(&sv, node, table);

  free(str);
}

void get_leaf_node_count_in_tree(Node *tree, size_t *counter)
{
  if (tree->left == NULL && tree->right == NULL) {
    *counter += 1;
    return;
  }
  get_leaf_node_count_in_tree(tree->left, counter);
  get_leaf_node_count_in_tree(tree->right, counter);
}

void get_huffman_table_from_tree(Node *tree, Table *table)
{
  size_t leaf_node_count = 0;
  get_leaf_node_count_in_tree(tree, &leaf_node_count);
  
  table->items = (Table_Item *) malloc(sizeof(Table_Item) * leaf_node_count);
  node_to_table(tree, table);
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

  return file_size;
}

void post_order(String_View *sv, Node *tree)
{
  if (tree->left == NULL && tree->right == NULL) {
    sv->str[sv->size++] = '1';
    sv->str[sv->size++] = tree->freq.byte;
    return;
  }

  post_order(sv, tree->left);  
  post_order(sv, tree->right);
  sv->str[sv->size++] = '0';
}

// TODO: Introduce a method to devise the tree back from header file
String_View get_header_info_from_tree(Node *tree)
{
  char *str = (char *) malloc(sizeof(char) * U8_CAPACITY * 10);
  String_View sv = {.str = str, .size = 0};

  post_order(&sv, tree);
  sv.str[sv.size++] = '0';
  
  return sv;
}

// Complete the huffman encode using sv functions sv_to_byte, sv_from_byte, sv_concat
void huffman_encode(const char *plain_file_path, const char *encoded_file_path)
{
  FILE *plain_file = fopen(plain_file_path, "rb");
  if (plain_file == NULL) {
    fprintf(stderr, "ERROR: could not open file %s: %s\n", plain_file_path, strerror(errno));
    exit(1);
  }

  FILE *encoded_file = fopen(encoded_file_path, "wb");
  if (encoded_file == NULL) {
    fprintf(stderr, "ERROR: could not open file %s: %s\n", encoded_file_path, strerror(errno));
    exit(1);
  }

  Node tree = {0};
  get_huffman_tree_from_file(plain_file_path, &tree);

  Table table = {0};
  get_huffman_table_from_tree(&tree, &table);

  String_View header_info = get_header_info_from_tree(&tree);
  printf("%zu: %.*s\n", header_info.size, (int) header_info.size, header_info.str);

  fprintf(encoded_file, "%zu%lu", header_info.size, get_file_size(plain_file_path));
  // Write the header_info into encoded_file
  free(header_info.str);
}

#endif // HUFFMAN_IMPLEMENTATION
