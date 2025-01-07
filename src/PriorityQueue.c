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
    
};

void Pq_percolateUp(Huffman_node** arr, int index) {
    
    if (index == 0) return; 

    int parentIndex = (index - 1) / 2;

    
    if (arr[parentIndex]->cnt > arr[index]->cnt) {
        
        Huffman_node* temp = arr[parentIndex];
        arr[parentIndex] = arr[index];
        arr[index] = temp;

        
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
    return pq->arr[0]; 
}

void Pq_percolateDown(Huffman_node** arr, int size, int idx){
    int smallest = idx;
    int leftChild = 2 * idx + 1;
    int rightChild = 2 * idx + 2;

    
    if (leftChild < size && arr[leftChild]->cnt < arr[smallest]->cnt) {
        smallest = leftChild;
    }

    
    if (rightChild < size && arr[rightChild]->cnt < arr[smallest]->cnt) {
        smallest = rightChild;
    }
    if(smallest == idx) return;
    Huffman_node* temp = arr[idx];
    arr[idx] = arr[smallest];
    arr[smallest] = temp;

        
    Pq_percolateDown(arr, size, smallest);

    
}


Huffman_node* Pq_pop(PriorityQueue* pq) {
    if (pq->size == 0) {
        fprintf(stderr, "Error: PriorityQueue is empty.\n");
        return NULL;
    }

    
    Huffman_node* minNode = pq->arr[0];

    
    pq->arr[0] = pq->arr[pq->size - 1];
    pq->size--; 

    
    Pq_percolateDown(pq->arr, pq->size, 0);

    return minNode; 
}