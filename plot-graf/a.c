#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAX_HEAP_SIZE 1000
//queue com heap

typedef struct priorityQueueHeap{
    int size;
    int items[MAX_HEAP_SIZE];
}priorityQueueHeap;


priorityQueueHeap *createPriorityQueueHeap(){
    priorityQueueHeap *new_queue = (priorityQueueHeap*) malloc(sizeof(priorityQueueHeap)); 
    new_queue->size = 0;
    return new_queue;
}


int get_parent_index(int index){
    return index/2;
}

int get_left_index(int index){
    return index*2;
}

int get_right_index(int index){
    return index*2+1;
}

void swap(int *a,int *b){
    int c = *a;
    *a = *b;
    *b = c;
    return;

}

int enqueueHeap(priorityQueueHeap *queue,int data){
    int comparcoes = 0;

    if(queue->size >= MAX_HEAP_SIZE){
        printf("Heap overflow\n");
        return 0;
    }else
    {
        queue->items[++queue->size] = data;
        int key_index = queue->size;
        int parent_index= get_parent_index(queue->size);

        //equanto nao chegar a raiz e  equanto este ser maior que o pai
        while (parent_index >= 1 && 
                queue->items[key_index] > queue->items[parent_index])
        {
            swap(&queue->items[key_index],&queue->items[parent_index]);

            //atualizar os "status" do antigo nó pai agora trocado como filho;
            key_index = parent_index;
            parent_index = get_parent_index(key_index);

            comparcoes++;
        }
        
        return comparcoes;
    }
    
}

void print_heap_tree_like(priorityQueueHeap *heap){
    int size_heap = heap->size;
    int i,level = 1;
    int level_size = 1;
    for(i = 1; i <= size_heap; i++){
        if(i == level_size){
            printf("\nLevel %d:",level);
            level_size = level_size*2;
            level++;
        }
        printf("%d ",heap->items[i]);
    }

    return;
}

//queue sem heap
typedef struct queueNode{
    int priority;
    struct queueNode *next;
}queueNode;

typedef struct priorityQueue{
    queueNode *head;
}priorityQueue;

priorityQueue* createPriorityQueue(){
    priorityQueue *new_node = (priorityQueue*) malloc(sizeof(priorityQueue));

    new_node->head = NULL;
    return new_node;
}

int isEmpty(priorityQueue* pq){
    return (pq->head == NULL);
}   

int enqueue(priorityQueue *pq, int prio){
    int comparacoes = 0;
    queueNode *new_node = (queueNode*) malloc(sizeof(queueNode));
    new_node->priority = prio;

    if(isEmpty(pq) || prio > pq->head->priority){
        new_node->next = pq->head;
        pq->head = new_node;
        return 0;
    }else
    {
        queueNode* current = pq->head;
        while(current->next != NULL && current->next->priority > prio){
            current = current->next;

            comparacoes++;
        }
        new_node->next = current->next;
        current->next = new_node;

        return comparacoes;
    }
    

}

void print_pq(priorityQueue *pq){
    queueNode *current = pq->head;
    while (current != NULL)
    {
        printf("(%d);\n",current->priority);
        current = current->next;
    }   
}



int main(){
    priorityQueue *pq_sem_heap = createPriorityQueue();
    priorityQueueHeap *pq_heap = createPriorityQueueHeap();

    FILE *arq_in; //pointer para o arquivo input
    FILE *arq_out; 

    char buffer[20];

    arq_in = fopen("input.txt","r");
    arq_out = fopen("output.txt","w");
    

    if(arq_in == NULL || arq_out == NULL){
        printf("Erro - Arquivo(s) nao encontrado(s)\n");
    }else{
        //leia o texto do arquivo e coloque em buffer
        while(fgets(buffer,sizeof(buffer),arq_in)){

            int prioridade = atoi(buffer);
            int comparacoes_sem_heap;
            int comparacoes_com_heap;
            //printf("%d\n",input);

            
            comparacoes_sem_heap = enqueue(pq_sem_heap,prioridade);
            //printf("%d -> %d\n",prioridade,comparacoes_sem_heap);
            
            comparacoes_com_heap = enqueueHeap(pq_heap,prioridade);
            //printf("%d -> %d\n",prioridade,comparacoes_com_heap);

            fprintf(arq_out,"%d %d %d\n",prioridade,comparacoes_sem_heap,comparacoes_com_heap);

        }
    }
    fclose(arq_in);
    fclose(arq_out);
    //print_heap_tree_like(pq_heap);
}