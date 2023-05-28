#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <windows.h>
#include <iostream>

#define BUFFER_SIZE 1024//размер буфера для чтения и записи данных в канал
#define READ_END 0
#define WRITE_END 1
//константы для обозначения концов канала для чтения и записи соответственно

int main(int argc, char* argv[]) {
    char write_msg[BUFFER_SIZE] = "Hello, child process!";
    char read_msg[BUFFER_SIZE];
    //массивы символов для записи и чтения сообщений в канал
    int fd[2];//массив для хранения файловых дескрипторов канала
    pid_t pid;//идентификатор процесса-потомка
    HANDLE hReadPipe, hWritePipe;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    DWORD dwWritten, dwRead;
    BOOL bSuccess;
    SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES) };

    /* Создание анонимного канала */
    if (!CreatePipe(&hReadPipe, &hWritePipe, &saAttr, 0)) { //принимает указатели на дескриптор для чтнения и записи в канал
        fprintf(stderr, "Create Pipe failed");
        return 1;
    }

    /* Создание процесса-потомка */
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdInput = hReadPipe;
    si.dwFlags = STARTF_USESTDHANDLES;
    ZeroMemory(&pi, sizeof(pi));
    if (!CreateProcess(NULL, argv[1], NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {//передаются параметры командной строки для запуска нового процесса, а также дескрипторы для ввода/вывода, которые будут использоваться в процессе-потомке
        fprintf(stderr, "Create Process failed");
        return 1;
    }

    /* Закрытие ненужных концов канала */
    CloseHandle(hReadPipe);
    CloseHandle(hWritePipe);

    /* Запись данных в канал */
    bSuccess = WriteFile(hWritePipe, write_msg, strlen(write_msg) + 1, &dwWritten, NULL);
    if (!bSuccess) {
        fprintf(stderr, "Write to pipe failed");
        return 1;
    }

    /* Чтение данных из канала */
    bSuccess = ReadFile(hReadPipe, read_msg, BUFFER_SIZE, &dwRead, NULL);
    if (!bSuccess || dwRead == 0) {
        fprintf(stderr, "Read from pipe failed");
        return 1;
    }
    printf("Received message from child process: %s\n", read_msg);

    /* Закрытие читающего конца канала */
    CloseHandle(hReadPipe);

    return 0;
}