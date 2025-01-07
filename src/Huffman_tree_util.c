#include "Huffman_tree_util.h"

// 트리 자체의 자료구조를 만들기 보다 그냥 root node로 유지한다.
Huffman_node* Huffman_tree_generate(PriorityQueue* pq) {
    int n = pq->size;
    
    for (int i = 0; i < n - 1; i++) { // n-1번 병합
        Huffman_node* z = (Huffman_node*) malloc(sizeof(Huffman_node));
        if (z == NULL) {
            fprintf(stderr, "Error: Memory allocation failed.\n");
            return NULL;
        }

        z->ch = '\0'; // 내부 노드는 문자를 가지지 않음
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

    return Pq_pop(pq); // 루트 노드 반환
}




// 허프만 트리를 dfs로 순회하면서 백트랙킹하여 codeword를 만든다.
void Huffman_tree_fill_codewords(Huffman_node* node, uint8_t* code, int depth, ByteTable* bt) {
    // code는 일종의 시뮬레이션 변수.

    if (node == NULL) return;

    // 리프 노드인 경우
    if (node->l == NULL && node->r == NULL) {
        code[depth] = '\0'; // 코드 문자열 종료
        ByteTable_set_codeword(bt, (uint8_t)node->ch, code); // ByteTable에 코드워드 저장
        return;
    }

    // 왼쪽 자식 (0)
    code[depth] = '0';
    Huffman_tree_fill_codewords(node->l, code, depth + 1, bt);

    // 오른쪽 자식 (1)
    code[depth] = '1';
    Huffman_tree_fill_codewords(node->r, code, depth + 1, bt);
}