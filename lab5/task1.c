#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_QUEUE_SIZE 100
#define MAX_MESSAGE_SIZE 256

typedef enum {
    MSG_TYPE_TEXT,
    MSG_TYPE_BINARY,
    MSG_TYPE_SYSTEM
} MessageType;

typedef enum {
    PRIORITY_LOW,
    PRIORITY_MEDIUM,
    PRIORITY_HIGH
} MessagePriority;

typedef struct {
    char content[MAX_MESSAGE_SIZE];
    MessageType type;
    MessagePriority priority;
    time_t timestamp;
} Message;

typedef struct {
    Message messages[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int count;
    int total_messages;
    int messages_by_type[3];
    int messages_by_priority[3];
} MessageQueue;

void initQueue(MessageQueue* queue) {
    queue->front = 0;
    queue->rear = -1;
    queue->count = 0;
    queue->total_messages = 0;

    for (int i = 0; i < 3; i++) {
        queue->messages_by_type[i] = 0;
        queue->messages_by_priority[i] = 0;
    }
}

int isFull(MessageQueue* queue) {
    return queue->count >= MAX_QUEUE_SIZE;
}

int isEmpty(MessageQueue* queue) {
    return queue->count == 0;
}

int enqueue(MessageQueue* queue, const char* content, MessageType type, MessagePriority priority) {
    if (isFull(queue)) {
        printf("Ошибка: очередь заполнена\n");
        return 0;
    }

    queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;

    strncpy(queue->messages[queue->rear].content, content, MAX_MESSAGE_SIZE - 1);
    queue->messages[queue->rear].content[MAX_MESSAGE_SIZE - 1] = '\0';
    queue->messages[queue->rear].type = type;
    queue->messages[queue->rear].priority = priority;
    queue->messages[queue->rear].timestamp = time(NULL);

    queue->count++;
    queue->total_messages++;
    queue->messages_by_type[type]++;
    queue->messages_by_priority[priority]++;

    return 1;
}

int dequeue(MessageQueue* queue, Message* msg) {
    if (isEmpty(queue)) {
        printf("Ошибка: очередь пуста\n");
        return 0;
    }

    *msg = queue->messages[queue->front];
    queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;

    queue->count--;
    queue->messages_by_type[msg->type]--;
    queue->messages_by_priority[msg->priority]--;

    return 1;
}

void displayStats(MessageQueue* queue) {
    printf("\nСтатистика очереди сообщений:\n");
    printf("Всего сообщений в очереди: %d\n", queue->count);

    printf("\nРаспределение по типам:\n");
    printf("TEXT: %d\n", queue->messages_by_type[MSG_TYPE_TEXT]);
    printf("BINARY: %d\n", queue->messages_by_type[MSG_TYPE_BINARY]);
    printf("SYSTEM: %d\n", queue->messages_by_type[MSG_TYPE_SYSTEM]);

    printf("\nРаспределение по приоритетам:\n");
    printf("LOW: %d\n", queue->messages_by_priority[PRIORITY_LOW]);
    printf("MEDIUM: %d\n", queue->messages_by_priority[PRIORITY_MEDIUM]);
    printf("HIGH: %d\n", queue->messages_by_priority[PRIORITY_HIGH]);
}

int main() {
    MessageQueue queue;
    Message msg;

    initQueue(&queue);

    enqueue(&queue, "Текстовое сообщение", MSG_TYPE_TEXT, PRIORITY_LOW);
    enqueue(&queue, "Системное уведомление", MSG_TYPE_SYSTEM, PRIORITY_HIGH);
    enqueue(&queue, "01001010", MSG_TYPE_BINARY, PRIORITY_MEDIUM);

    displayStats(&queue);

    if (dequeue(&queue, &msg)) {
        printf("\nИзвлечено сообщение:\n");
        printf("Содержимое: %s\n", msg.content);
        printf("Тип: %d\n", msg.type);
        printf("Приоритет: %d\n", msg.priority);
        printf("Временная метка: %s", ctime(&msg.timestamp));
    }

    displayStats(&queue);

    return 0;
}
