#include <arpa/inet.h>  // Для структуры sockaddr_in
#include <sys/socket.h>  // Для создания сокетов, привязки и т.д.

#include <iostream>

#include "/opt/goinfre/janiecee/homebrew/Cellar/libevent/2.1.12_1/include/event2/event.h"  // Заголовочный файл libevent

void read_callback(evutil_socket_t fd, short events, void* arg) {
  char buffer[4096];
  int recv_len = recv(fd, buffer, sizeof(buffer), 0);
  if (recv_len > 0) {
    // Обработка полученных данных
  } else {
    // Произошла ошибка или клиент закрыл соединение
    event* ev = static_cast<event*>(arg);
    event_del(ev);
    delete ev;
    close(fd);
  }
}

int main() {
  int server_socket =
      socket(AF_INET, SOCK_STREAM, 0);  // Получить дескриптор сокета сервера
  if (server_socket < 0) {
    std::cerr << "Failed to create server socket." << std::endl;
    return 1;
  }

  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(8000);

  if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    std::cerr << "Failed to bind server socket." << std::endl;
    close(server_socket);
    return 1;
  }

  listen(server_socket, 10);

  event_base* base = event_base_new();  // Создать основу для событий libevent
  if (!base) {
    std::cerr << "Failed to create event base." << std::endl;
    close(server_socket);
    return 1;
  }

  event* ev = event_new(base, server_socket, EV_READ | EV_PERSIST,
                        read_callback, event_self_cbarg());
  if (!ev) {
    std::cerr << "Failed to create event." << std::endl;
    event_base_free(base);
    close(server_socket);
    return 1;
  }

  event_add(ev, nullptr);  // Добавить событие на обработку

  event_base_dispatch(base);  // Начать обработку событий

  event_free(ev);
  event_base_free(base);
  close(server_socket);

  return 0;
}