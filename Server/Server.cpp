

#define WIN32_LEAN_AND_MEAN // Убирает ненужные заголовки из Windows.h для повышения производительности
#include<Windows.h> // Заголовок для WinAPI функций
#include <iostream> // Заголовок для работы с потоками ввода-вывода
#include <WinSock2.h> // Заголовок для Winsock 2, используемого для сетевого программирования
#include <WS2tcpip.h> // Заголовок для дополнительных функций Winsock 2


using namespace std;
int main()
{
    WSADATA wsaData; // Структура для Winsock для создания сетевыйх приложений
    ADDRINFO hints; // Структура с параметрами для getaddrinfo
    ADDRINFO* addrResult; // Указатель на структуру с информацией об адресе
    SOCKET ConnectSocket = INVALID_SOCKET; // Дескриптор сокета для соединения с клиентом, по умолчанию не валидный
    SOCKET ListenSocket = INVALID_SOCKET; // Дескриптор сокета для прослушивания подключений, по умолчанию не валидный
    const char* sendBuffer = "Hello from server."; // Текст сообщения для клиента
    char recvBuffer[512]; // Буфер для приема данных

    // Создание Winsock для создания сетевых приложений
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); // Инициализируем Winsock с версией 2.2
    if (result != 0) {
        cout << "WSAStartup fauled result" << endl; // Выводим сообщение об ошибке инициализации Winsock
        WSACleanup(); // Очищаем Winsock
        return 1; // Выход из программы с ошибкой
    }

    // Заполняем структуру hints для определения параметров поиска адресса getaddrinfo
    ZeroMemory(&hints, sizeof(hints)); // Обнуляем структуру hints 0
    hints.ai_family = AF_INET; // Семейство протоколов IPv4
    hints.ai_socktype = SOCK_STREAM; // Тип сокета TCP
    hints.ai_protocol = IPPROTO_TCP; // Протокол TCP
    hints.ai_flags = AI_PASSIVE; // Флаг AI_PASSIVE для прослушивания подключений на всех доступных адресах

    // Получаем информацию об адресе
    result = getaddrinfo(NULL, "666", &hints, &addrResult); // Получаем адрес по порту 666 для прослушивания
    if (result != 0) {
        cout << "Getabddrinfo fauled error" << endl; // Выводим сообщение об ошибке 
        freeaddrinfo(addrResult); // Освобождаем память,которая выделенная для структуры адреса
        WSACleanup(); // Очищаем Winsock
        return 1; // Выход из программы с ошибкой
    }

    // Создаем сокет для прослушивания
    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol); // Создаем сокет с параметрами из структуры адреса
    if (ListenSocket == INVALID_SOCKET) {
        cout << "Socket creation with" << endl; // Выводим сообщение об ошибке
        WSACleanup(); // Очищаем Winsock
        freeaddrinfo(addrResult); // Освобождаем память,которая выделенная для структуры адреса
        return 1; // Выход из программы
    }

    // Привязываем сокет к адресу
    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen); // Привязываем сокет к полученному адресу
    if (result == SOCKET_ERROR) {
        cout << "error " << endl; // Выводим сообщение об ошибке 
        closesocket(ListenSocket); // Закрываем сокет
        ListenSocket = INVALID_SOCKET; // Сбрасываем дескриптор сокета
        freeaddrinfo(addrResult); // Освобождаем память,которая выделенная для структуры адреса
        WSACleanup(); // Очищаем Winsock
        return 1; // Выход из программы 
    }

    // Переводим сокет в режим прослушивания
    result = listen(ListenSocket, SOMAXCONN); // Переводим сокет в режим прослушивания с максимальным количеством подключений
    if (result == SOCKET_ERROR) {
        cout << "Listening failed" << endl; // Выводим сообщение об ошибке
        closesocket(ListenSocket); // Закрываем сокет
        ListenSocket = INVALID_SOCKET; // Сбрасываем дескриптор сокета
        freeaddrinfo(addrResult); // Освобождаем память,которая выделенная для структуры адреса
        WSACleanup(); // Очищаем Winsock
        return 1; // Выход из программы
    }

    // Принимаем соединение
    ConnectSocket = accept(ListenSocket, NULL, NULL); // Принимаем соединение от клиента
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Accepting SOCKET failed" << endl; // Выводим сообщение об ошибке
        closesocket(ListenSocket); // Закрываем сокет для прослушивания
        freeaddrinfo(addrResult); // Освобождаем память,которая выделенная для структуры адреса
        WSACleanup(); // Очищаем Winsock
        return 1; // Выход из программы
    }

    
    closesocket(ListenSocket); // Закрываем сокет для прослушивания

    // Обработка соединения с клиентом
    do {
        ZeroMemory(recvBuffer, 512); // Обнуляем буфер приема данных
        result = recv(ConnectSocket, recvBuffer, 512, 0); // Получаем данные от клиента
        if (result > 0) {
            cout << "Received: " << result << " bytes" << endl; // Выводим количество полученных байт
            cout << "Received data: " << recvBuffer << endl; // Выводим полученные данные

            
            result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0); // Отправляем сообщение клиенту
            if (result == SOCKET_ERROR) {
                cout << "Failed to send data back" << endl; // Выводим сообщение об ошибке
                closesocket(ConnectSocket); // Закрываем сокет
                freeaddrinfo(addrResult); // Освобождаем память,которая выделенная для структуры адреса
                WSACleanup(); // Очищаем Winsock
            }
        }
        else if (result == 0) {
            cout << "Connection closing..." << endl; // Выводим сообщение о закрытии соединения
        }
        else {
            cout << "recv failed with error" << endl; // Выводим сообщение об ошибке
            closesocket(ConnectSocket); // Закрываем сокет
            freeaddrinfo(addrResult); // Освобождаем память,которая выделенная для структуры адреса
            WSACleanup(); // Очищаем Winsock
            return 1; // Выход 
        }
    } while (result > 0); // Повторяем цикл, пока получаем данные от клиента

  
    result = shutdown(ConnectSocket, SD_SEND); // Запрещаем отправку данных на сокет
    if (result == SOCKET_ERROR) {
        closesocket(ConnectSocket); // Закрываем сокет
        freeaddrinfo(addrResult); // Освобождаем память,которая выделенная для структуры адреса 
        WSACleanup(); // Очищаем Winsock
        return 1; // Выход из программы
    }

    
    closesocket(ConnectSocket); // Закрываем сокет
    freeaddrinfo(addrResult); //Освобождаем память,которая выделенная для структуры адреса
    WSACleanup(); // Очищаем Winsock
    return 0; // Выход из программы 
}



