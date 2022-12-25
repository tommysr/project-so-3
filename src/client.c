#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/sem.h>
#include <pthread.h>

#define MAX 20
#define SERVER 1

struct TextWithSource
{
  int source;
  char text[MAX];
};

struct Message
{
  long m_destination;
  struct TextWithSource m_text_with_source;
};

int create_msg_queue(key_t key);
int queue_id;
int my_pid;
void *sending_message();
void *receiving_message();

int main()
{
  key_t queue_key = create_key(2115);
  queue_id = create_msg_queue(queue_key);
  my_pid = getpid();
  pthread_t sending_thread;
  pthread_t receiving_thread;

  if (pthread_create(&sending_thread, NULL, sending_message, NULL) != 0)
  {
    perror("cant create sending thread\n");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&receiving_thread, NULL, receiving_message, NULL) != 0)
  {
    perror("cant create receiving thread\n");
    exit(EXIT_FAILURE);
  }

  if (pthread_join(sending_thread, NULL))
  {
    perror("cant join sending thread");
    exit(EXIT_FAILURE);
  }

  if (pthread_join(receiving_thread, NULL))
  {
    perror("cant join receiving thread");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}

void *receiving_message()
{
  struct Message message_buff;

  while (1)
  {
    if (msgrcv(queue_id, (struct Message *)&message_buff, sizeof(struct TextWithSource), my_pid, 0) == -1)
    {
      perror("cant receive message \n");
      exit(EXIT_FAILURE);
    }

    printf("\tmessage: %s \n", message_buff.m_text_with_source.text);
  }

  pthread_exit((void *)0);
}

void *sending_message()
{
  struct Message message_buff;
  message_buff.m_destination = SERVER;
  message_buff.m_text_with_source.source = my_pid;

  while (1)
  {
    memset(message_buff.m_text_with_source.text, 0, MAX);

    printf("Enter message: ");
    scanf("%s", message_buff.m_text_with_source.text);

    printf("Message: \"%s\" \n", message_buff.m_text_with_source.text);

    if (msgsnd(queue_id, (struct Message *)&message_buff, sizeof(struct TextWithSource), 0) == -1)
    {
      perror("sending error\n");
      exit(EXIT_FAILURE);
    }
  }

  pthread_exit((void *)0);
}

int create_msg_queue(key_t key)
{
  int queue_id = msgget(key, 0600 | IPC_CREAT);

  if (queue_id == -1)
  {
    perror("Message queue creation failed\n");
    exit(EXIT_FAILURE);
  }
  else
  {
    printf("queue id %d\n", queue_id);
  }

  return queue_id;
}

key_t create_key(int id)
{
  key_t key = ftok(".", id);

  if (key == -1)
  {
    printf("key creation failed\n");
    exit(EXIT_FAILURE);
  }

  return key;
}
