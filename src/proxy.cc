#include </opt/goinfre/janiecee/homebrew/Cellar/libevent/2.1.12_1/include/event2/event.h>  // Заголовочный файл libevent
#include <arpa/inet.h>  // Для структуры sockaddr_in
#include <libpq-fe.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>  // Для создания сокетов, привязки и т.д.
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <vector>

//   - Функция считывает данные из сокета (recv()) в буфер.
//   - Если количество считанных байтов (recv_len) больше 0, выполняется
//   обработка полученных данных (здесь нужно вставить соответствующий код).
//   - Если recv_len равно 0, это означает, что клиент закрыл соединение или
//   произошла ошибка. Функция закрывает сокет (close(fd)).

// Глобальная переменная для хранения подключения к базе данных
PGconn* connection = nullptr;

// Функция для выполнения SQL запросов и логирования
void executeSQLQuery(const std::string& query) {
  PGresult* result = PQexec(connection, query.c_str());
  if (PQresultStatus(result) == PGRES_TUPLES_OK) {
    // Обработка и вывод результатов запроса
    int rows = PQntuples(result);
    int cols = PQnfields(result);
    for (int i = 0; i < rows; ++i) {
      for (int j = 0; j < cols; ++j) {
        std::cout << PQgetvalue(result, i, j) << "\t";
      }
      std::cout << std::endl;
    }
  } else {
    std::cerr << "Query execution failed: " << PQerrorMessage(connection)
              << std::endl;
  }
  PQclear(result);
}

void getListDataBase() {
  connection = PQconnectdb("user=janiecee hostaddr=127.0.0.1 port=5432");
  if (PQstatus(connection) == CONNECTION_OK) {
    PGresult* result =
        PQexec(connection,
               "SELECT datname FROM pg_database WHERE datistemplate = false;");
    if (PQresultStatus(result) == PGRES_TUPLES_OK) {
      int rows = PQntuples(result);
      std::cout << "List of databases:" << std::endl;
      for (int i = 0; i < rows; ++i) {
        std::cout << PQgetvalue(result, i, 0) << std::endl;
      }
    }
    PQclear(result);
  } else {
    std::cerr << "Failed to connect to PostgreSQL: "
              << PQerrorMessage(connection) << std::endl;
  }
  PQfinish(connection);
}

void read_callback(evutil_socket_t fd, short events, void* arg) {
  char buffer[4096];
  int recv_len = recv(fd, buffer, sizeof(buffer), 0);
  if (recv_len > 0) {
    getListDataBase();
    // Обрабатываем полученные данные
    std::string user, dbname;
    std::cout << "Enter user=";
    std::cin >> user;

    std::cout << "Select a database from the list above (dbname=): ";
    std::cin >> dbname;

    std::string connectionString =
        "user=" + user + " dbname=" + dbname + " hostaddr=127.0.0.1 port=5432";
    // Подключение к PostgreSQL
    connection = PQconnectdb(connectionString.c_str());
    if (PQstatus(connection) == CONNECTION_OK) {
      std::cout << "Connected to the database." << std::endl;

      // Настройка логирования SQL запросов в файл
      std::ofstream logFile("queries.log");
      if (!logFile.is_open()) {
        std::cerr << "Failed to open log file." << std::endl;
        PQfinish(connection);
      }

      // Redirect stdout to the log file
      std::streambuf* origStdout = std::cout.rdbuf();
      std::cout.rdbuf(logFile.rdbuf());

      // Enable tracing (stdout is redirected to the log file)
      PQtrace(connection, stdout);

      // Ввод и выполнение SQL запросов
      while (true) {
        std::string query;
        std::cout << "Enter SQL query (or 'exit' to quit): ";
        std::getline(std::cin, query);

        if (query == "exit") {
          break;
        }

        executeSQLQuery(query);
      }

      // Закрытие файла и подключения
      PQuntrace(connection);
      logFile.close();
      PQfinish(connection);
    } else {
      std::cerr << "Failed to connect to PostgreSQL: "
                << PQerrorMessage(connection) << std::endl;
    }
  } else {
    // Произошла ошибка или клиент закрыл соединение
    close(fd);
  }
}

evutil_socket_t create_new_connection() {
  // Создание сокета
  evutil_socket_t conn_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (conn_socket < 0) {
    std::cerr << "Failed to create connection socket." << std::endl;
    // Обработка ошибки
    // ...

    // Возвращение недействительного сокета
    return -1;
  }

  // Заполнение информации о сетевом адресе (sockaddr_in) для удаленного хоста
  sockaddr_in remote_addr{};
  remote_addr.sin_family = AF_INET;
  // remote_addr.sin_addr.s_addr = INADDR_ANY;
  remote_addr.sin_port = htons(8080);
  inet_pton(AF_INET, "127.0.0.1", &(remote_addr.sin_addr));

  // Подключение к удаленному хосту
  if (connect(conn_socket, reinterpret_cast<struct sockaddr*>(&remote_addr),
              sizeof(remote_addr)) < 0) {
    std::cerr << "Failed to connect to the remote host." << std::endl;
    // Обработка ошибки
    // ...

    // Закрытие сокета
    close(conn_socket);

    // Возвращение недействительного сокета
    return -1;
  }

  // Возврат сокета для установленного соединения
  return conn_socket;
}

int main() {
  int num_connections = 100;  // Желаемое число соединений

  // Создаем экземпляр event_base
  struct event_base* base = event_base_new();
  if (!base) {
    std::cerr << "Failed to create event base." << std::endl;
    return 1;
  }

  std::vector<evutil_socket_t> conn_sockets(
      num_connections);  // Вектор сокетов соединений
  std::vector<struct event*> events(num_connections);  // Вектор событий

  // Создаем и добавляем события для каждого соединения
  for (int i = 0; i < num_connections; ++i) {
    evutil_socket_t conn_socket =
        create_new_connection();  // Создание нового соединения
    conn_sockets[i] = conn_socket;

    struct event* ev =
        event_new(base, conn_socket, EV_READ | EV_PERSIST, read_callback, NULL);
    event_add(ev, NULL);
    events[i] = ev;
  }

  // Запускаем цикл обработки событий
  event_base_dispatch(base);

  // Освобождаем ресурсы
  for (int i = 0; i < num_connections; ++i) {
    evutil_closesocket(conn_sockets[i]);
    event_free(events[i]);
  }
  event_base_free(base);

  return 0;
}

// void read_callback(evutil_socket_t fd, short events, void* arg) {
//   char buffer[4096];
//   int recv_len = recv(fd, buffer, sizeof(buffer), 0);
//   if (recv_len > 0) {
//     // Process the received data
//   } else {
//     // An error occurred or the client closed the connection
//     close(fd);
//   }
// }

// int main() {
//   // Создаёт серверный сокет с помощью socket().
//   int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//   if (server_socket < 0) {
//     std::cerr << "Failed to create server socket." << std::endl;
//     return 1;
//   }
//   // Заполняет информацию о сетевом адресе (sockaddr_in) для привязки сокета
//   к
//   // конкретному адресу и порту.
//   sockaddr_in server_addr{};
//   server_addr.sin_family = AF_INET;
//   server_addr.sin_addr.s_addr = INADDR_ANY;
//   server_addr.sin_port = htons(8000);
//   // Вызывает bind() для привязки сокета к указанному адресу и порту.
//   if (bind(server_socket, reinterpret_cast<struct sockaddr*>(&server_addr),
//            sizeof(server_addr)) < 0) {
//     std::cerr << "Failed to bind server socket." << std::endl;
//     close(server_socket);
//     return 1;
//   }
//   // Вызывает listen() для установки сокета в режим прослушивания входящих
//   // подключений.
//   listen(server_socket, 10);
//   // Создаёт базу событий (event_base) с помощью event_base_new().
//   event_base* base = event_base_new();
//   if (!base) {
//     std::cerr << "Failed to create event base." << std::endl;
//     close(server_socket);
//     return 1;
//   }
//   // Создаёт событие чтения (event) для серверного сокета с помощью
//   event_new(). event* ev = event_new(base, server_socket, EV_READ |
//   EV_PERSIST,
//                         read_callback, nullptr);
//   if (!ev) {
//     std::cerr << "Failed to create event." << std::endl;
//     event_base_free(base);
//     close(server_socket);
//     return 1;
//   }
//   // Добавляет событие к базе событий с помощью event_add().
//   event_add(ev, nullptr);
//   // Запускает цикл обработки событий с помощью event_base_dispatch().
//   event_base_dispatch(base);
//   // По завершении цикла освобождает память, закрывает сокеты.
//   event_free(ev);
//   event_base_free(base);
//   close(server_socket);

//   return 0;
// }