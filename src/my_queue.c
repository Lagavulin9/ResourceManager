#include "my_queue.h"

void initQueue(Queue *q)
{
	memset(q, 0, sizeof(Queue));
}

bool isEmpty(Queue *q)
{
	return (q->size == 0);
}

bool isFull(Queue *q)
{
	return (q->size == Q_SIZE);
}

int enQueue(Queue *q, int value)
{
	if (isFull(q))
		return -1;

	q->queue[q->rear] = value;
	q->rear = (q->rear + 1) % Q_SIZE;
	q->size++;
	return 0;
}

int deQueue(Queue *q, int *value)
{
	if (isEmpty(q))
		return -1;

	*value = q->queue[q->front];
	q->front = (q->front + 1) % Q_SIZE;
	q->size--;
	return 0;
}
