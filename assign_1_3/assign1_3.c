#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "assign1_3.h"



queue_t* queue_init (void) {
    queue_t *q = (queue_t *) malloc(sizeof(queue_t));
    if(q == NULL) {
        perror("Malloc failed.");
        exit(1);
    }
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

    return n;
}

struct node_t* put_node (queue_t *q, struct node_t* n) {
    if (q->end) {
        q->end->next = n;
    } else {
        q->start = n;
    }

    q->end = n;

    return n;
}

int get (queue_t *q) {
    struct node_t *n = q->start;
    int num = n->data;
    q->start = q->start->next;
    free(n);

    return num;
}

struct node_t* get_node (queue_t *q) {
    struct node_t *n = q->start;
    q->start = q->start->next;

    return n;
}

int isEmpty (queue_t *q) {
    if (q->start == q->end) {
        return 1;
    } else {
        return 0;
    }
}

void *thread (void *args) {
    args_t *old_args = (args_t *) args;
    queue_t *inbound = old_args->queue;
    int count = old_args->count;
    pthread_cond_t old_cond = old_args->cond;
    pthread_mutex_t old_cond_lock = old_args->cond_lock;
    pthread_mutex_t old_lock = old_args->lock;

    pthread_t id;
    if (count == 0) {
        queue_t *outbound = queue_init();
        args_t *new_args = (args_t *) malloc(sizeof(args_t));
        if (new_args == NULL) {
            perror("Malloc failed.");
            exit(1);
        }

        pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
        pthread_mutex_t cond_lock = PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

        new_args->lock = lock;
        new_args->cond = cond;
        new_args->cond_lock = cond_lock;
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
            put(outbound, i);
            i++;
            // pthread_mutex_lock(&cond_lock);
            pthread_cond_signal(&cond);
            // pthread_mutex_unlock(&cond_lock);
        }

        // return outbound;
    } else {
        while (1){
            if (!isEmpty(inbound)){
                count = get_node(inbound)->data;
                break;
            }
        }
        printf("%d\n", count);

        queue_t *outbound = queue_init();
        args_t *new_args = (args_t *) malloc(sizeof(args_t));
        if (new_args == NULL) {
            perror("Malloc failed.");
            exit(1);
        }

        pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
        pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_t cond_lock = PTHREAD_MUTEX_INITIALIZER;


        new_args->lock = lock;
        new_args->cond_lock = cond_lock;
        new_args->cond = cond;
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
            // pthread_mutex_lock(&old_cond_lock);
            pthread_cond_wait(&old_cond, &old_lock);
            // pthread_mutex_unlock(&old_cond_lock);

            if (!isEmpty(inbound)) {
                struct node_t *node = get_node(inbound);
                if (! (node->data % count) == 0){
                    if(!outbound) {
                        queue_t *outbound = queue_init();
                        new_args->queue = outbound;
                    }

                    put_node (outbound, node);
                    // pthread_mutex_lock(&cond_lock);
                    pthread_cond_signal(&cond);
                    // pthread_mutex_unlock(&cond_lock);
                }
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
