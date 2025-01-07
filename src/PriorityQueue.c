#include "PriorityQueue.h"

PriorityQueue* Pq_create(int capacity){
    PriorityQueue* pq=  (PriorityQueue*) malloc(sizeof(PriorityQueue));
    if(pq ==NULL) {
        fprintf(stderr, "Error: Cannot allocate createPrirityQueue.\n");
        return NULL;
    }
    
    Huffman_node** arr = (Huffman_node**) malloc(sizeof(Huffman_node*) * capacity);
    if(arr ==NULL){
        fprintf(stderr, "Error: Cannot allocate array , createPrirityQueue.\n");
        free(pq);
        return NULL;
    }

    pq->arr = arr;
    pq->capacity = capacity;
    pq->size = 0;
    return pq;
};

void Pq_destroy(PriorityQueue* pq){
    free(pq->arr);
    free(pq);
    // pq가 가진 노드들은 허프만 트리를 만드는데 이용되므로 free하지 않는다
};

void Pq_percolateUp(Huffman_node** arr, int index) {
    // 부모 노드 인덱스 계산
    if (index == 0) return; // 루트에 도달하면 종료

    int parentIndex = (index - 1) / 2;

    // 부모 노드와 현재 노드 비교
    if (arr[parentIndex]->cnt > arr[index]->cnt) {
        // 부모 노드가 더 크면 교환
        Huffman_node* temp = arr[parentIndex];
        arr[parentIndex] = arr[index];
        arr[index] = temp;

        // 부모 노드로 이동하여 재귀 호출
        Pq_percolateUp(arr, parentIndex);
    }
}



void Pq_pushNode(PriorityQueue* pq, Huffman_node* element){
    
    if (pq->size == pq->capacity) {
        fprintf(stderr, "Error: PriorityQueue capacity exceeded.\n");
        return;
    }
    pq->arr[pq->size] = element;
    int idx = pq->size;
    pq->size++;

    Pq_percolateUp(pq->arr,idx);

};




Huffman_node* Pq_top(PriorityQueue* pq) {
    if (pq->size == 0) {
        fprintf(stderr, "Error: PriorityQueue is empty.\n");
        return NULL;
    }
    return pq->arr[0]; // 루트 노드(최솟값) 반환
}

void Pq_percolateDown(Huffman_node** arr, int size, int idx){
    int smallest = idx;
    int leftChild = 2 * idx + 1;
    int rightChild = 2 * idx + 2;

    // 왼쪽 자식이 더 작으면 업데이트
    if (leftChild < size && arr[leftChild]->cnt < arr[smallest]->cnt) {
        smallest = leftChild;
    }

    // 오른쪽 자식이 더 작으면 업데이트
    if (rightChild < size && arr[rightChild]->cnt < arr[smallest]->cnt) {
        smallest = rightChild;
    }
    if(smallest == idx) return;
    Huffman_node* temp = arr[idx];
    arr[idx] = arr[smallest];
    arr[smallest] = temp;

        // 재귀적으로 자식 노드에서 힙 속성 복구
    Pq_percolateDown(arr, size, smallest);

    
}


Huffman_node* Pq_pop(PriorityQueue* pq) {
    if (pq->size == 0) {
        fprintf(stderr, "Error: PriorityQueue is empty.\n");
        return NULL;
    }

    // 루트 노드(최솟값) 저장
    Huffman_node* minNode = pq->arr[0];

    // 마지막 노드를 루트로 이동
    pq->arr[0] = pq->arr[pq->size - 1];
    pq->size--; // 큐 크기 감소

    // Heapify Down 호출: 루트에서 힙 속성 복구
    Pq_percolateDown(pq->arr, pq->size, 0);

    return minNode; // 최솟값 반환
}