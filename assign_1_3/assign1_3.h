struct node_t {
    struct node_t *next;
    int data;
};

typedef struct {
    struct node_t *start;
    struct node_t *end;
    int num_items;
} queue_t;

typedef struct {
    queue_t *queue;
    int count;
    // pthread_mutex_t outbound_lock;
    pthread_cond_t outbound_cond;
    pthread_mutex_t outbound_cond_lock;
} args_t;
