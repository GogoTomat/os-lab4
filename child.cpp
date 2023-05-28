#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <iostream>

#define BUFFER_SIZE 1024//размер буфера для чтения и записи данных в канал

int main(void) {
    char read_msg[BUFFER_SIZE];//массив символов для чтения данных из стандартного потока ввода
    DWORD dwRead;//количество байт, прочитанных из стандартного потока ввода
    BOOL bSuccess;//флаг, указывающий на успешность операции чтения и записи данных

    DWORD fileType = GetFileType(GetStdHandle(STD_INPUT_HANDLE));//тип файла дескриптора
if (fileType == FILE_TYPE_PIPE) {
    // Читаем данные из канала
    bSuccess = ReadFile(GetStdHandle(STD_INPUT_HANDLE), read_msg, BUFFER_SIZE, &dwRead, NULL);
    if (!bSuccess || dwRead == 0) {
        fprintf(stderr, "Read from pipe failed");
        return 1;
    }
} else if (fileType == FILE_TYPE_CHAR) {
    // Читаем данные с клавиатуры
    printf("Please enter a message: ");
    fgets(read_msg, BUFFER_SIZE, stdin);
} else {
    // Ошибка: неизвестный тип файла
    fprintf(stderr, "Unknown file type for standard input");
    return 1;
}

// Форматированный вывод полученных данных в стандартный поток вывода
printf("Formatted message from child process: %s", read_msg);

// Запись данных в стандартный поток вывода
char write_msg[BUFFER_SIZE] = "Hello, parent process!";
DWORD dwWritten;
bSuccess = WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), write_msg, strlen(write_msg) + 1, &dwWritten, NULL);
if (!bSuccess) {
    fprintf(stderr, "Write to pipe failed");
    return 1;
}
}