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

// Frequency
typedef struct {
  uint8_t byte;
  uint32_t count;
} Freq;

typedef struct {
  Freq *arr;
  size_t size;
} Freq_List;

void print_freq(const Freq *freq);
void print_freq_list(const Freq_List *fl);
void get_useful_freq_arr(Freq_List *fl);
void sort_freq(Freq_List *fl, size_t index);
void sort_freq_list(Freq_List *fl);
void get_freq_from_file(const char *file_path, Freq_List *fl);

// Node
typedef struct node {
  Freq freq;
  struct node* left;
  struct node* right;
} Node;

typedef struct {
  Node *arr;
  size_t size;
} Node_List;

void print_node(Node *node, int level);
void freq_to_node(Freq_List *fl, Node_List *nl);
void sort_node(Node_List *nl, size_t index);
void sort_node_list(Node_List *nl);
bool node_eq(const Node *node1, const Node *node2);
int find_node_in_node_list(const Node_List *nl, const Node *node);
void add_node_to_tree(Node_List *char_nl, Node_List *temp_nl);

// Huffman-Table
/*
  Approach #1
  - Introduce a new structure (Bits) of members uint64_t word and size_t count,
    here word would store the data and count would tell us how many initial bits are of our use.
  - To create the huffman table from huffman tree, maybe we have to use this Bits structure.

  Approach #2
  - {(uint8_t) byte, const char *binary_value}
  - For Huffman Table use the above structure, for byte we have the character in our input file and
    for the binary value we would use '<' for 0 and '>' for 1.
  - I'm hoping that this approach would be easier as compared to bit-manipulation for both table and compression
*/
typedef struct {
  char *str;
  size_t size;
} String_View;

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
void get_huffman_tree_from_file(const char *file_path, Node *tree);
void get_leaf_node_count_in_tree(Node *tree, size_t *counter);
void get_huffman_table_from_tree(Node *tree, Table *table);

void post_order(String_View *sv, Node *tree);
String_View get_header_info_from_tree(Node *tree);


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

  // TODO: add the sort_node() here, so that we could remove the sort_freq()
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
  return (bool) ((node1->freq.count == node2->freq.count && node1->freq.byte == node2->freq.byte) &&
		 (node1->left == node2->left && node1->right == node2->right));
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

void print_table(const Table *table)
{
  for (size_t i = 0; i < table->size; ++i) {
    printf("%02x %c %8s: ", table->items[i].byte, table->items[i].byte, table->items[i].value);
    for (size_t j = 0; table->items[i].value[j] != '\0'; ++j) {
      if (table->items[i].value[j] == '<') {
	printf("0");
      } else if (table->items[i].value[j] == '>') {
	printf("1");
      }
    }
    printf("\n");
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

  sv->str[sv->size++] = '<';
  in_order(sv, node->left, table);
  sv->size -= 1;
  
  sv->str[sv->size++] = '>';
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

void get_huffman_tree_from_file(const char *file_path, Node *tree)
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

  *tree = temp_nl.arr[0];
  free(temp_nl.arr);
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

void post_order(String_View *sv, Node *tree)
{
  if (tree->left == NULL && tree->right == NULL) {
    sv->str[sv->size++] = '1';
    sv->str[sv->size++] = tree->freq.byte;
    return;
  }

  post_order(sv, tree->left);
  sv->str[sv->size++] = '0';
  sv->size -= 1;
  
  post_order(sv, tree->right);
  sv->str[sv->size++] = '0';
}

String_View get_header_info_from_tree(Node *tree)
{
  char *str = (char *) malloc(sizeof(char) * U8_CAPACITY * 10);
  String_View sv = {.str = str, .size = 0};

  post_order(&sv, tree);
  sv.str[sv.size++] = '0';
  
  return sv;
}

}

#endif // HUFFMAN_IMPLEMENTATION
