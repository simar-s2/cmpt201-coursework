#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 64
#define PORT 8000
#define LISTEN_BACKLOG 32

#define handle_error(msg)                                                      \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)
// Shared counters for: total # messages, and counter of clients (used for
// assigning client IDs)
int total_message_count = 0;
int client_id_counter = 1;

// Mutexs to protect above global state.
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t client_id_mutex = PTHREAD_MUTEX_INITIALIZER;

struct client_info {
  int cfd;
  int client_id;
};
void *handle_client(void *arg) {
  char buf[BUF_SIZE];
  ssize_t num_read = 0;

  struct client_info *client = (struct client_info *)arg;

  while ((num_read = read(client->cfd, buf, BUF_SIZE)) > 0) {
    printf("[Client %d]: ", client->client_id);
    write(STDOUT_FILENO, buf, num_read);
    pthread_mutex_lock(&count_mutex);
    total_message_count++;
    printf("Total messages received: %d\n", total_message_count);
    pthread_mutex_unlock(&count_mutex);
  }
  return NULL;
}

int main() {
  struct sockaddr_in addr;
  int sfd;
  pthread_t thread_id;

  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1) {
    handle_error("socket");
  }
  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) {
    handle_error("bind");
  }

  if (listen(sfd, LISTEN_BACKLOG) == -1) {
    handle_error("listen");
  }
  for (;;) {
    int cfd = accept(sfd, NULL, NULL);
    struct client_info *client = malloc(sizeof(struct client_info));
    pthread_mutex_lock(&client_id_mutex);
    client->client_id = client_id_counter++;
    pthread_mutex_unlock(&client_id_mutex);

    client->cfd = cfd;

    pthread_create(&thread_id, NULL, handle_client, client);
    pthread_detach(thread_id);
  }
  if (close(sfd) == -1) {
    handle_error("close");
  }

  return 0;
}

/*
1. What is the address of the server it is trying to connect to (IP address and
port number)? Answer: 127.0.0.1 (localhost) on port 8000

2. Is it UDP or TCP? How do you know?
   Answer: TCP. The code uses SOCK_STREAM which indicates TCP.
   UDP would use SOCK_DGRAM.

3. The client is going to send some data to the
server. Where does it get this data from? How can you tell in the code? Answer:
From standard input (STDIN_FILENO). The read() function reads from STDIN_FILENO
in the while loop.
4. How does the client program end? How can you tell that in the code?
   Answer: The client ends when read() returns <= 1 (either EOF from Ctrl+D,
   an empty line with just newline, or an error). The while loop condition
   is (num_read > 1), so it exits when this becomes false.
*/
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8000
#define BUF_SIZE 64
#define ADDR                                                                   \
  "127.0.0.1" #define handle_error(msg) do {                                   \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  }                                                                            \
  while (0)

int main() {
  struct sockaddr_in addr;
  int sfd;
  ssize_t num_read;
  char buf[BUF_SIZE];
  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1) {
    handle_error("socket");
  }

  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  if (inet_pton(AF_INET, ADDR, &addr.sin_addr) <= 0) {
    handle_error("inet_pton");
  }
  int res = connect(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
  if (res == -1) {
    handle_error("connect");
  }

  while ((num_read = read(STDIN_FILENO, buf, BUF_SIZE)) > 1) {
    if (write(sfd, buf, num_read) != num_read) {
      handle_error("write");
    }
    printf("Just sent %zd bytes.\n", num_read);
  }
  if (num_read == -1) {
    handle_error("read");
  }

  close(sfd);
  exit(EXIT_SUCCESS);
}
