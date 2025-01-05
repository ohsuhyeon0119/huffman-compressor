#include <stdio.h>
#include "Huffman_node.h" // Huffman_node 관련 함수 포함

int main() {
    // 1. 왼쪽, 오른쪽 자식 노드 생성
    Huffman_node* left_child = Huffman_node_create('L', 10, NULL, NULL); // 왼쪽 자식
    Huffman_node* right_child = Huffman_node_create('R', 20, NULL, NULL); // 오른쪽 자식

    // 2. 루트 노드 생성 및 자식 연결
    Huffman_node* root = Huffman_node_create('-', 30, left_child, right_child); // 루트 노드

    // 3. 각 노드의 정보를 출력하여 연결 관계 확인
    printf("\n--- Node Details ---\n");
    printf("Left Child:\n");
    Huffman_node_print(left_child);

    printf("\nRight Child:\n");
    Huffman_node_print(right_child);

    printf("\nRoot Node:\n");
    Huffman_node_print(root);

    // 4. 루트 노드의 연결 관계 검증
    printf("\n--- Connection Verification ---\n");
    if (root->l == left_child) {
        printf("Root's left child is correctly connected.\n");
    } else {
        printf("Root's left child connection is incorrect.\n");
    }

    if (root->r == right_child) {
        printf("Root's right child is correctly connected.\n");
    } else {
        printf("Root's right child connection is incorrect.\n");
    }

    // 5. 리프 노드 확인
    printf("\n--- Leaf Node Verification ---\n");
    if (Huffman_node_is_leaf(left_child)) {
        printf("Left child is a leaf node.\n");
    } else {
        printf("Left child is NOT a leaf node.\n");
    }

    if (Huffman_node_is_leaf(right_child)) {
        printf("Right child is a leaf node.\n");
    } else {
        printf("Right child is NOT a leaf node.\n");
    }

    if (Huffman_node_is_leaf(root)) {
        printf("Root is a leaf node.\n");
    } else {
        printf("Root is NOT a leaf node.\n");
    }

    // 6. 메모리 해제
    Huffman_node_deallcoate(left_child);
    Huffman_node_deallcoate(right_child);
    Huffman_node_deallcoate(root);

    printf("\nAll nodes successfully deallocated.\n");

    return 0;
}
