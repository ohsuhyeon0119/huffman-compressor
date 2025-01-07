#include "Huffman_tree_util.h"


Huffman_node* Huffman_tree_generate(PriorityQueue* pq) {
    int n = pq->size;
    
    for (int i = 0; i < n - 1; i++) { 
        Huffman_node* z = (Huffman_node*) malloc(sizeof(Huffman_node));
        if (z == NULL) {
            fprintf(stderr, "Error: Memory allocation failed.\n");
            return NULL;
        }

        z->ch = '\0'; 
        z->l = Pq_pop(pq);
        z->r = Pq_pop(pq);

        if (z->l == NULL || z->r == NULL) {
            fprintf(stderr, "Error: Insufficient nodes in the priority queue.\n");
            free(z);
            return NULL;
        }

        z->cnt = z->l->cnt + z->r->cnt;
        Pq_pushNode(pq, z);
    }

    return Pq_pop(pq); 
}





void Huffman_tree_fill_codewords(Huffman_node* node, uint8_t* code, int depth, ByteTable* bt) {
    

    if (node == NULL) return;

    
    if (node->l == NULL && node->r == NULL) {
        code[depth] = '\0'; 
        ByteTable_set_codeword(bt, (uint8_t)node->ch, code); 
        return;
    }

    
    code[depth] = '0';
    Huffman_tree_fill_codewords(node->l, code, depth + 1, bt);

    
    code[depth] = '1';
    Huffman_tree_fill_codewords(node->r, code, depth + 1, bt);
}