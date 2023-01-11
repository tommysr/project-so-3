#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <sys/sem.h>
#include <pthread.h>
#include "common.h"

int queue_id;
long my_pid;
void *sending_message();
void *receiving_message();

int main()
{
  key_t queue_key = create_key(2115);
  queue_id = create_msg_queue(queue_key);
  my_pid = (long)getpid();
  pthread_t sending_thread;
  pthread_t receiving_thread;

  if (pthread_create(&sending_thread, NULL, sending_message, NULL) != 0)
  {
    perror("[C] cant create sending thread\n");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&receiving_thread, NULL, receiving_message, NULL) != 0)
  {
    perror("[C] cant create receiving thread\n");
    exit(EXIT_FAILURE);
  }

  if (pthread_join(sending_thread, NULL) != 0)
  {
    perror("[C] cant join sending thread");
    exit(EXIT_FAILURE);
  }

  if (pthread_join(receiving_thread, NULL) != 0)
  {
    perror("[C] cant join receiving thread");
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
      if (errno == EIDRM)
      {
        printf("[C] message queue has been deleted\n");
        exit(EXIT_SUCCESS);
      }
      else
      {
        perror("[C] cant receive message from queue\n");
        exit(EXIT_FAILURE);
      }
    }

    printf("[C] message received: %s \n", message_buff.m_text_with_source.text);
  }

  pthread_exit((void *)EXIT_SUCCESS);
}

void *sending_message()
{
  struct Message message_buff;
  message_buff.m_destination = SERVER;
  message_buff.m_text_with_source.source = my_pid;

  while (1)
  {
    // // memset(message_buff.m_text_with_source.text, 0, MAX);

    char *res = fgets(message_buff.m_text_with_source.text, MAX, stdin);

    if (res == NULL)
    {
      printf("[C] read string from stdin error, incorrect input\n");
      exit(EXIT_FAILURE);
    }

    message_buff.m_text_with_source.text[strcspn(message_buff.m_text_with_source.text, "\n")] = '\0';

    printf("[C] Sending message or message part: %s \n", message_buff.m_text_with_source.text);

    if (msgsnd(queue_id, (struct Message *)&message_buff, sizeof(struct TextWithSource), IPC_NOWAIT) == -1)
    {
      if (errno == EAGAIN)
      {
        printf("[C] Message queue is full, not sending message\n");
      }
      else
      {
        perror("[C] error in sending message to the queue\n");
        exit(EXIT_FAILURE);
      }
    }
  }

  pthread_exit((void *)EXIT_SUCCESS);
}
