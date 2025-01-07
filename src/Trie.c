#include "Trie.h"

// TrieNode 생성 함수
TrieNode* TrieNode_create() {
    TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));
    if (!node) {
        perror("Failed to allocate TrieNode");
        exit(EXIT_FAILURE);
    }
    node->left = NULL;
    node->right = NULL;
    node->is_leaf = 0; // 기본적으로 리프 노드 아님
    node->character = 0; // 리프 노드가 아닌 경우 의미 없음
    return node;
}

// 메타데이터를 기반으로 트라이를 생성하는 함수
TrieNode* Trie_build_from_metadata(const uint8_t* metadata, size_t metadata_size) {
    TrieNode* root = TrieNode_create(); // 루트 노드 생성
    size_t offset = 0;

    while (offset < metadata_size) {
        uint8_t target_character = metadata[offset++]; // 문자
        uint8_t codeword_length = metadata[offset++];  // 코드워드 길이

        // 허프만 코드 추출 (비트 단위)
        TrieNode* current = root;
        for (int i = 0; i < codeword_length; i++) {
            // 현재 비트를 추출
            uint8_t bit = (metadata[offset + (i / 8)] >> (7 - (i % 8))) & 1;

            // 비트에 따라 트라이에 추가
            if (bit == 0) {
                if (!current->left) {
                    current->left = TrieNode_create();
                }
                current = current->left;
            } else {
                if (!current->right) {
                    current->right = TrieNode_create();
                }
                current = current->right;
            }
        }

        // 코드 끝에 도달하면 리프 노드에 문자 저장
        current->is_leaf = 1;
        current->character = target_character;

        // 코드워드가 끝나는 지점으로 이동
        offset += (codeword_length + 7) / 8;
    }

    return root;
}

// Trie 메모리 해제 함수
void Trie_destroy(TrieNode* root) {
    if (!root) return;

    Trie_destroy(root->left);
    Trie_destroy(root->right);
    free(root);
}

/**
 * @brief 허프만 압축 데이터를 디코드하여 원래 데이터를 복원하는 함수
 * @param root 트라이의 루트 노드
 * @param data 압축된 데이터 배열
 * @param total_bits 압축 데이터의 전체 비트 수
 * @param outputFile 디코드된 데이터를 저장할 출력 파일
 */
void decode(TrieNode* root, uint8_t* data, size_t total_bits, FILE* outputFile) {
    TrieNode* current = root; // 트라이 탐색을 위한 현재 노드
    size_t bit_offset = 0;    // 현재 읽고 있는 비트의 위치 (0부터 시작)

    // 압축 데이터의 모든 비트를 순회
    while (bit_offset < total_bits) {
        // 현재 비트 계산
        size_t byte_index = bit_offset / 8;    // 현재 비트가 속한 바이트
        size_t bit_index = bit_offset % 8;    // 바이트 내에서의 비트 위치
        uint8_t bit = (data[byte_index] >> (7 - bit_index)) & 1; // 현재 비트 추출

        // 트라이 탐색
        if (bit == 0) {
            current = current->left;
        } else {
            current = current->right;
        }

        // 리프 노드에 도달한 경우
        if (current->is_leaf) {
            // 복원된 문자를 출력 파일에 쓰기
            fwrite(&current->character, 1, 1, outputFile);
            // 트라이 탐색을 루트 노드로 초기화
            current = root;
        }

        // 다음 비트로 이동
        bit_offset++;
    }
}