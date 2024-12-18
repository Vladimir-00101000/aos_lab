#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_QUEUE_SIZE 100
#define PREVIEW_SIZE 16

typedef enum {
    MSG_TYPE_TEXT,
    MSG_TYPE_BINARY,
    MSG_TYPE_SYSTEM,
    MSG_TYPE_ALL
} MessageType;

typedef enum {
    PRIORITY_LOW,
    PRIORITY_MEDIUM,
    PRIORITY_HIGH
} MessagePriority;

typedef struct {
    char preview[PREVIEW_SIZE];
    size_t fullSize;
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

MessageType getMessageType(const char* typeStr) {
    if (strcmp(typeStr, "text") == 0) return MSG_TYPE_TEXT;
    if (strcmp(typeStr, "binary") == 0) return MSG_TYPE_BINARY;
    if (strcmp(typeStr, "system") == 0) return MSG_TYPE_SYSTEM;
    if (strcmp(typeStr, "all") == 0) return MSG_TYPE_ALL;

    printf("Неизвестный тип сообщения. Используется MSG_TYPE_ALL\n");
    return MSG_TYPE_ALL;
}

const char* messageTypeToString(MessageType type) {
    switch(type) {
        case MSG_TYPE_TEXT: return "TEXT";
        case MSG_TYPE_BINARY: return "BINARY";
        case MSG_TYPE_SYSTEM: return "SYSTEM";
        default: return "UNKNOWN";
    }
}

int isFull(MessageQueue* queue) {
    return queue->count >= MAX_QUEUE_SIZE;
}

int isEmpty(MessageQueue* queue) {
    return queue->count == 0;
}

int enqueue(MessageQueue* queue, const char* content, size_t contentSize, MessageType type, MessagePriority priority) {
    if (isFull(queue)) {
        printf("Ошибка: очередь заполнена\n");
        return 0;
    }

    queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;

    size_t copySize = contentSize < PREVIEW_SIZE ? contentSize : PREVIEW_SIZE;
    memcpy(queue->messages[queue->rear].preview, content, copySize);

    if (copySize < PREVIEW_SIZE) {
        queue->messages[queue->rear].preview[copySize] = '\0';
    }

    queue->messages[queue->rear].fullSize = contentSize;
    queue->messages[queue->rear].type = type;
    queue->messages[queue->rear].priority = priority;
    queue->messages[queue->rear].timestamp = time(NULL);

    queue->count++;
    queue->total_messages++;
    queue->messages_by_type[type]++;
    queue->messages_by_priority[priority]++;

    return 1;
}

const char* priorityToString(MessagePriority priority) {
    switch(priority) {
        case PRIORITY_LOW: return "LOW";
        case PRIORITY_MEDIUM: return "MEDIUM";
        case PRIORITY_HIGH: return "HIGH";
        default: return "UNKNOWN";
    }
}

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

void displayMessage(const Message* msg) {
    printf("\n=== Сообщение ===\n");
    printf("Тип: %s\n", messageTypeToString(msg->type));
    printf("Приоритет: %s\n", priorityToString(msg->priority));
    printf("Время: %s", ctime(&msg->timestamp));
    printf("Полный размер: %zu байт\n", msg->fullSize);
    printf("Превью (%d байт): ", PREVIEW_SIZE);

    if (msg->type == MSG_TYPE_BINARY) {
        for (int i = 0; i < PREVIEW_SIZE && i < msg->fullSize; i++) {
            printf("%02X ", (unsigned char)msg->preview[i]);
        }
    } else {
        printf("%.*s", PREVIEW_SIZE, msg->preview);
    }
    printf("\n===============\n");
}

void readMessages(MessageQueue* queue, MessageType requestedType) {
    if (isEmpty(queue)) {
        printf("Очередь пуста\n");
        return;
    }

    int currentIndex = queue->front;
    int messageFound = 0;

    while (!messageFound) {
        if (requestedType == MSG_TYPE_ALL || queue->messages[currentIndex].type == requestedType) {
            displayMessage(&queue->messages[currentIndex]);
            messageFound = 1;
        }
        currentIndex = (currentIndex + 1) % MAX_QUEUE_SIZE;

        if (currentIndex == queue->front) {
            printf("Ищем сообщение...\n");
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Использование: %s <id_очереди> <тип_сообщения>\n", argv[0]);
        printf("Типы сообщений: text, binary, system, all\n");
        return 1;
    }

    int queueId = atoi(argv[1]);
    MessageType requestedType = getMessageType(argv[2]);

    printf("Чтение сообщений из очереди %d\n", queueId);
    printf("Запрошенный тип сообщений: %s\n", requestedType == MSG_TYPE_ALL ? "ALL" : messageTypeToString(requestedType));

    MessageQueue queue;
    initQueue(&queue);

    char longMessage[] = "Это очень длинное тестовое сообщение, которое превышает размер превью и будет обрезано";
    char binaryData[] = "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13";

    enqueue(&queue, longMessage, strlen(longMessage), MSG_TYPE_TEXT, PRIORITY_LOW);
    enqueue(&queue, binaryData, sizeof(binaryData), MSG_TYPE_BINARY, PRIORITY_MEDIUM);
    enqueue(&queue, "Короткое системное уведомление", 28, MSG_TYPE_SYSTEM, PRIORITY_HIGH);

    readMessages(&queue, requestedType);

    return 0;
}
