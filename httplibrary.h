#ifndef HTTP_HEADER_INCLUDED
#define HTTP_HEADER_INCLUDED

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <mswsock.h>

#define http_close_socket(socket) closesocket(socket)
#else
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <ctype.h>

#define SOCKET int
#define SD_BOTH SHUT_RDWR
#define http_close_socket(socket) close(socket)
#endif

#include "string_lib.h"

typedef struct {
    char* val;
    size_t len;
} http_string;

typedef struct {
    SOCKET socket;

    // Parsing result
    http_string method;
    http_string path;
    char* query_pointer;
    size_t query_list_length;
    http_string version;
    char* headers_pointer;
    size_t headers_length;
    char* cookie_pointer;
    http_string body;
} http_client;

typedef void (*http_callback)(http_client*);

char* http_get_query(http_client* client, const char* key); // HTTP Get Query
char* http_get_header(http_client* client, const char* key); // HTTP Get Header
char* http_get_cookie(http_client* client, const char* key); // HTTP Get Cookie
char http_write(SOCKET s, const char* data, unsigned long long int size); // HTTP Write
char http_send_file(SOCKET s, const char* name_file, char manual_code); // HTTP Send File

SOCKET http_init_socket(const char* ip, unsigned short port); // initilaize socket
void http_start(SOCKET s_socket, http_callback callback); // Start HTTP

#endif // HTTP_HEADER_INCLUDED
