#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "assign1_3.h"

#define MAX_ITEMS 5000



queue_t* queue_init (void) {
    queue_t *q = (queue_t *) malloc(sizeof(queue_t));
    if(q == NULL) {
        perror("Malloc failed.");
        exit(1);
    }

    q->num_items = 0;
    q->start = NULL;
    q->end = NULL;
    return q;
}

struct node_t* put (queue_t *q, int num) {
    struct node_t* n = malloc(sizeof(struct node_t));
    if(n == NULL) {
        perror("Malloc failed.");
        exit(1);
    }


    n->data = num;

    if (q->end) {
        q->end->next = n;
    } else {
        q->start = n;
    }

    q->end = n;

    q->num_items++;

    return n;
}

struct node_t* put_node (queue_t *q, struct node_t* n) {
    if (q->end) {
        q->end->next = n;
    } else {
        q->start = n;
    }

    q->num_items++;
    q->end = n;

    return n;
}

int get (queue_t *q) {
    struct node_t *n = q->start;
    int num = n->data;
    q->start = q->start->next;
    free(n);

    q->num_items--;

    return num;
}

struct node_t* get_node (queue_t *q) {
    struct node_t *n = q->start;
    q->start = q->start->next;
    q->num_items--;

    return n;
}

int isEmpty (queue_t *q) {
    return q->num_items == 0;
}

int isFull (queue_t *q) {
    return q->num_items >= MAX_ITEMS;
}

void *thread (void *args) {
    args_t *old_args = (args_t *) args;
    queue_t *inbound = old_args->queue;
    int count = old_args->count;
    pthread_cond_t inbound_cond = old_args->outbound_cond;
    pthread_mutex_t inbound_cond_lock = old_args->outbound_cond_lock;

    pthread_t id;
    if (count == 0) {
        queue_t *outbound = queue_init();
        args_t *new_args = (args_t *) malloc(sizeof(args_t));
        if (new_args == NULL) {
            perror("Malloc failed.");
            exit(1);
        }

        pthread_cond_t outbound_cond;
        pthread_cond_init(&outbound_cond, NULL);
        pthread_mutex_t outbound_cond_lock;
        pthread_mutex_init(&outbound_cond_lock, NULL);


        new_args->outbound_cond = outbound_cond;
        new_args->outbound_cond_lock = outbound_cond_lock;

        new_args->queue = outbound;
        new_args->count = 2;


        if (pthread_create (&id,
                        NULL,
                        &thread,
                        (void *)new_args) != 0) {
            perror("Failed to create new thread.");
            exit(1);
        }

        int i = 2;
        while (1) {
            pthread_mutex_lock(&outbound_cond_lock);
            if (isFull(outbound)) {
                printf("FULL\n");
                pthread_cond_wait(&outbound_cond, &outbound_cond_lock);
            }

            put(outbound, i);

            pthread_cond_signal(&outbound_cond);
            i++;
            pthread_mutex_unlock(&outbound_cond_lock);

        }
    } else {
        pthread_mutex_lock(&inbound_cond_lock);
        if (isEmpty(inbound)){
            pthread_cond_wait(&inbound_cond, &inbound_cond_lock);
        }
        count = get_node(inbound)->data;
        pthread_cond_signal(&inbound_cond);
        pthread_mutex_unlock(&inbound_cond_lock);

        printf("%d\n", count);

        queue_t *outbound = queue_init();
        args_t *new_args = (args_t *) malloc(sizeof(args_t));
        if (new_args == NULL) {
            perror("Malloc failed.");
            exit(1);
        }

        pthread_mutex_t outbound_cond_lock = PTHREAD_MUTEX_INITIALIZER;
        pthread_cond_t outbound_cond = PTHREAD_COND_INITIALIZER;


        new_args->outbound_cond = outbound_cond;
        new_args->outbound_cond_lock = outbound_cond_lock;
        new_args->count = count;
        new_args->queue = outbound;


        if (pthread_create (&id,
                        NULL,
                        &thread,
                        (void *)new_args) != 0) {
            perror("Failed to create new thread.");
            exit(1);
        }

        while (1) {
            pthread_mutex_lock(&inbound_cond_lock);
            if (isEmpty(inbound)) {
                pthread_cond_wait(&inbound_cond, &inbound_cond_lock);
            }

            struct node_t *node = get_node(inbound);
            pthread_cond_signal(&inbound_cond);
            pthread_mutex_unlock(&inbound_cond_lock);


            if (! (node->data % count) == 0){

                pthread_mutex_lock(&outbound_cond_lock);
                if (isFull(outbound)) {
                    pthread_cond_wait(&outbound_cond, &outbound_cond_lock);
                }
                put_node (outbound, node);
                pthread_cond_signal(&outbound_cond);
                pthread_mutex_unlock(&outbound_cond_lock);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    args_t *new_args;
    pthread_t id;
    new_args = malloc(sizeof(args_t));

    if (new_args == NULL) {
        perror("Malloc failed.");
        exit(1);
    }

    new_args->queue = NULL;
    new_args->count = 0;


    if (pthread_create (&id,
                    NULL,
                    &thread,
                    (void *)new_args) != 0) {
        perror("Failed to create new thread.");
        exit(1);
    }

    pthread_join(id, NULL);

    return 0;
}
