#include <stdio.h>
#include <stdlib.h>
#include "Huffman_node.h"

// --- 기본 생성 및 소멸 함수 ---

/**
 * @brief 메모리 할당만 수행하는 함수 (초기화 X)
 * @return 할당된 Huffman_node 포인터
 */
Huffman_node* Huffman_node_allocate() {
    Huffman_node* node = (Huffman_node*) malloc(sizeof(Huffman_node));
    if (node == NULL) {
        perror("Error: Memory allocation failed for Huffman_node.");
        exit(EXIT_FAILURE);
    }
    node->l = NULL; // 왼쪽 자식 노드 초기화
    node->r = NULL; // 오른쪽 자식 노드 초기화
    return node;
}

/**
 * @brief 기존 노드를 초기화하는 함수
 * @param target 초기화할 대상 노드
 * @param ch 문자
 * @param cnt 빈도 수 또는 가중치
 * @param l 왼쪽 자식 노드
 * @param r 오른쪽 자식 노드
 */
void Huffman_node_initialize(Huffman_node* target, uint8_t ch, uint64_t cnt, Huffman_node* l, Huffman_node* r) {
    if (target == NULL) {
        fprintf(stderr, "Error: Cannot initialize a NULL node.\n");
        return;
    }
    target->ch = ch;
    target->cnt = cnt;
    target->l = l;
    target->r = r;
}

/**
 * @brief 메모리를 할당하고 초기화까지 수행하는 함수
 * @param ch 문자
 * @param cnt 빈도 수 또는 가중치
 * @param l 왼쪽 자식 노드
 * @param r 오른쪽 자식 노드
 * @return 생성된 Huffman_node 포인터
 */
Huffman_node* Huffman_node_create(uint8_t ch, uint64_t cnt, Huffman_node* l, Huffman_node* r) {
    Huffman_node* node = (Huffman_node*) malloc(sizeof(Huffman_node));
    if (node == NULL) {
        perror("Error: Memory allocation failed for Huffman_node.");
        exit(EXIT_FAILURE);
    }
    node->ch = ch;
    node->cnt = cnt;
    node->l = l;
    node->r = r;
    return node;
}

/**
 * @brief 메모리 해제 함수 (필드 데이터는 해제하지 않음)
 * @param self 해제할 대상 노드
 */
void Huffman_node_deallcoate(Huffman_node* self) {
    if (self == NULL) {
        fprintf(stderr, "Error: Cannot deallocate a NULL node.\n");
        return;
    }
    free(self);
}

// --- 유틸리티 함수 ---

/**
 * @brief 두 노드를 비교하는 함수
 * @param a 비교할 첫 번째 노드
 * @param b 비교할 두 번째 노드
 * @return -1 (a < b), 0 (a == b), 1 (a > b)
 */
int Huffman_node_compare(const Huffman_node* a, const Huffman_node* b) {
    if (a == NULL || b == NULL) {
        fprintf(stderr, "Error: Cannot compare NULL nodes.\n");
        return 0; // 기본적으로 같다고 반환
    }

    if (a->cnt < b->cnt) return -1; // a가 b보다 작음
    if (a->cnt > b->cnt) return 1;  // a가 b보다 큼
    return 0; // a와 b가 같음
}

/**
 * @brief 특정 노드가 리프 노드인지 확인하는 함수
 * @param node 확인할 노드
 * @return 1 (리프 노드), 0 (리프 노드 아님)
 */
int Huffman_node_is_leaf(const Huffman_node* node) {
    if (node == NULL) {
        fprintf(stderr, "Error: Cannot check leaf status of a NULL node.\n");
        return 0; // NULL 노드는 리프 노드가 아님
    }

    return (node->l == NULL && node->r == NULL); // 리프 노드 조건
}

/**
 * @brief 노드의 정보를 출력하는 디버깅 함수
 * @param node 출력할 노드
 */
void Huffman_node_print(const Huffman_node* node) {
    if (node == NULL) {
        printf("Node: NULL\n");
        return;
    }

    // 리프 노드 여부 확인
    char is_leaf = Huffman_node_is_leaf(node) ? 'Y' : 'N';

    printf("Node Info:\n");
    printf("  Char: '%c'\n", (node->ch != '\0') ? node->ch : '-'); // 문자 출력, 내부 노드는 '-'
    printf("  Count: %lu\n", node->cnt); // 빈도 수
    printf("  Is Leaf: %c\n", is_leaf); // 리프 노드 여부
    printf("  Left Child: %s\n", (node->l != NULL) ? "Exists" : "NULL");
    printf("  Right Child: %s\n", (node->r != NULL) ? "Exists" : "NULL");
}