#ifndef SRC_MY_QUEUE_H_
# define SRC_MY_QUEUE_H_

# define Q_SIZE 10

typedef struct
{
	int queue[Q_SIZE];
	int front;
	int rear;
	int size;
} Queue ;

void initQueue(Queue *q);
bool isEmpty(Queue *q);
bool isFull(Queue *q);
int  enQueue(Queue *q, int value);
int  deQueue(Queue *q, int *value);

#endif /* SRC_MY_QUEUE_H_ */
