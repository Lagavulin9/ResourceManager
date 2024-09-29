#include <stdbool.h>
#include "my_queue.h"

void initQueue(Queue *q)
{
	q->front = 0;
	q->rear = 0;
}

bool isEmpty(Queue *q)
{
	return (q->front == q->rear);
}

bool isFull(Queue *q)
{
	return (q->front == (q->rear + 1) % Q_SIZE);
}

int enQueue(Queue *q, int value)
{
	if (isFull(q))
		return -1;

	q->rear = (q->rear + 1) % Q_SIZE;
	q->queue[q->rear] = value;
	return 0;
}

int deQueue(Queue *q, int *value)
{
	if (isEmpty(q))
		return -1;

	q->front = (q->front + 1) % Q_SIZE;
	*value = q->queue[q->front];
	return 0;
}
