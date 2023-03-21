#ifndef THREAD_QUEUE_H
#define THREAD_QUEUE_H

struct node {
    int* client_socket;
    struct node *next;
};

typedef struct node node_t;

void enqueue(int* client_socket);
int* dequeue();

#endif // THREAD_QUEUE_H