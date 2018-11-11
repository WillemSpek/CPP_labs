struct node_t {
    struct node_t *next;
    int data;
};

typedef struct {
    struct node_t *start;
    struct node_t *end;
} queue_t;

typedef struct {
    queue_t *queue;
    int count;
} args_t;
