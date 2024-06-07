#include <stdio.h>
#include <stdlib.h>
#include "../include/queue.h"
#include <string.h>

// for Queues

Queue* createQueue() {
    Queue* myqueue = (Queue*)malloc(sizeof(Queue));
    myqueue->size = 0;
    myqueue->first_node = NULL;
    myqueue->last_node = NULL;
}

void deleteQueue(Queue* myqueue) {
    Node* temp_node = myqueue->first_node;
    for (int i = 0 ; i < myqueue->size ; i++) {
        Node* old_node = temp_node;
        temp_node = temp_node->child;
        free(old_node);
    }
    free(myqueue);
}

void set_node(Node* new_node, void* value, Node* parent) {
    new_node->value = value;
    new_node->parent = parent;
    new_node->child = NULL;
    if (parent != NULL) {
        parent->child = new_node;
    }
}

int enqueue(Queue* myqueue, void* value) {

    if (myqueue->size == 0) {   // if queue is empty
        Node* first_node = (Node*)malloc(sizeof(Node));
        set_node(first_node, value, NULL);
        myqueue->first_node = first_node;
        myqueue->last_node = first_node;
        myqueue->size += 1;
        return 1;
    }
    else {  // if queue is not empty
        Node* new_node = (Node*)malloc(sizeof(Node));
        set_node(new_node, value, myqueue->last_node);
        myqueue->last_node = new_node;
        myqueue->size++;
        return 1;
    }
}

void* dequeue(Queue* myqueue) {
    if (myqueue->size == 0) {   // if queue is empty
        printf("Queue is empty, cannot remove other items\n");
        return NULL;
    }
    else if (myqueue->size == 1) {  // if queue is of size 1
        void* value = myqueue->first_node->value;
        free(myqueue->first_node);
        myqueue->first_node = NULL;
        myqueue->last_node = NULL;
        myqueue->size--;
        return value;
    }
    else {  // if queue has more than one items
        void* value = myqueue->first_node->value;
        Node* child =  myqueue->first_node->child;
        child->parent = NULL;
        free(myqueue->first_node);
        myqueue->first_node = child;
        myqueue->size--;
        return value;
    }
}

Node* front(Queue* myqueue) {
    return myqueue->first_node;
}

Node* rear(Queue* myqueue) {
    return myqueue->last_node;
}

int queue_empty(Queue myqueue) {
    if (myqueue.size == 0)
        return 1;
    else
        return 0;
}

int queue_size(Queue myqueue) {
    return myqueue.size;
}

// for Triplets

Triplet* init_triplet(char* jobID, char* job, int commander_socket) {
    Triplet* mytriplet = (Triplet*)malloc(sizeof(Triplet));
    mytriplet->jobID = (char*)malloc(strlen(jobID) + 1);
    strcpy(mytriplet->jobID, jobID);
    mytriplet->job = (char*)malloc(strlen(job) + 1);
    strcpy(mytriplet->job, job);
    mytriplet->commander_socket = commander_socket;
    return mytriplet;
}

void delete_triplet(Triplet* mytriplet) {
    free(mytriplet->jobID);
    free(mytriplet->job);
    free(mytriplet);
}

char* format_triplet_basic(Triplet* mytriplet) {

    int safe_amount_of_chars = (strlen(mytriplet->job) + strlen(mytriplet->jobID) + 7)*2;
    char* buffer = (char*)malloc(sizeof(char)*(safe_amount_of_chars));
    sprintf(buffer, "<%s,%s>", mytriplet->jobID, mytriplet->job);
    return buffer;
}

char* format_triplet(Triplet* mytriplet) {

    int safe_amount_of_chars = (strlen(mytriplet->job) + strlen(mytriplet->jobID) + 20)*2;
    char* buffer = (char*)malloc(sizeof(char)*(safe_amount_of_chars));
    sprintf(buffer, "JOB <%s,%s> SUBMITTED", mytriplet->jobID, mytriplet->job);
    return buffer;
}

void print_queue_and_stats(Queue* myqueue) {
    printf("\n");
    if (queue_empty(*myqueue)) {
        printf("The queue is empty!\n");
        return;
    }
    Triplet* first_node = myqueue->first_node->value;
    Triplet* last_node = myqueue->last_node->value;
    printf("myqueue: size = %d\n", myqueue->size);

    Node* temp_node = myqueue->first_node;
    for (int i = 0 ; i < myqueue->size ; i++) {
        Triplet* temp_triplet = temp_node->value;
        printf("myqueue[%d] jobID = %s\n", i, temp_triplet->jobID);
        printf("myqueue[%d] job = %s\n", i, temp_triplet->job);
        temp_node = temp_node->child;
    }
    printf("\n");
}