#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_LEN 128
#define PROJECT_ID 123

typedef struct {
    long type;
    int sender_id;
    int receiver_id;
    char data[MSG_LEN];
} Message;

int main() {
    key_t server_key = ftok(".", PROJECT_ID);
    if (server_key == -1) {
        perror("Ошибка создания ключа сервера");
        return 1;
    }

    int server_queue = msgget(server_key, 0666);
    if (server_queue == -1) {
        perror("Ошибка получения очереди сервера");
        return 1;
    }

    int client_queue = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    if (client_queue == -1) {
        perror("Ошибка создания очереди клиента");
        return 1;
    }

    printf("Ваш ID: %d\n", client_queue);
    printf("Доступные типы сообщений:\n");
    printf("1 - Публичное сообщение\n");
    printf("2 - Личное сообщение\n");
    printf("3 - Важное уведомление\n");

    Message request, response;
    char input[MSG_LEN];
    int msg_type, receiver_id;

    while(1) {
        printf("\nВыберите тип сообщения (1-3): ");
        scanf("%d", &msg_type);
        getchar(); // Очистка буфера

        printf("Введите ID получателя (0 для всех): ");
        scanf("%d", &receiver_id);
        getchar(); // Очистка буфера

        printf("Введите сообщение (или 'exit' для выхода): ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "exit") == 0) {
            break;
        }

        request.type = msg_type;
        request.sender_id = client_queue;
        request.receiver_id = receiver_id;
        strncpy(request.data, input, MSG_LEN - 1);
        request.data[MSG_LEN - 1] = '\0';

        if (msgsnd(server_queue, &request, sizeof(Message) - sizeof(long), 0) == -1) {
            perror("Ошибка отправки запроса");
            break;
        }

        printf("Отправлено сообщение типа %d: %s\n", msg_type, request.data);

        if (msgrcv(client_queue, &response, sizeof(Message) - sizeof(long), 0, 0) == -1) {
            perror("Ошибка получения ответа");
            break;
        }

        printf("Получено сообщение типа %ld от %d: %s\n",
               response.type, response.sender_id, response.data);
    }

    msgctl(client_queue, IPC_RMID, NULL);
    printf("Клиент завершил работу\n");
    return 0;
}
