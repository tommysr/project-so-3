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
    perror("cant create sending thread\n");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&receiving_thread, NULL, receiving_message, NULL) != 0)
  {
    perror("cant create receiving thread\n");
    exit(EXIT_FAILURE);
  }

  if (pthread_join(sending_thread, NULL) != 0)
  {
    perror("cant join sending thread");
    exit(EXIT_FAILURE);
  }

  if (pthread_join(receiving_thread, NULL) != 0)
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
      pthread_exit((void *)EXIT_FAILURE);
    }

    printf("message returned: %s \n", message_buff.m_text_with_source.text);
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

    printf("Enter message: ");
    int new_line_pos = 0;

    do
    {
      memset(message_buff.m_text_with_source.text, 0, MAX);

      char *res = fgets(message_buff.m_text_with_source.text, MAX, stdin);

      if (res == NULL)
      {
        printf("read string error\n");
        pthread_exit((void *)EXIT_FAILURE);
      }
      else
      {
        new_line_pos = strcspn(message_buff.m_text_with_source.text, "\n");
        printf("%d", new_line_pos);
        message_buff.m_text_with_source.text[new_line_pos] = '\0';
      }

      printf("Message: \"%s\" \n", message_buff.m_text_with_source.text);

      if (msgsnd(queue_id, (struct Message *)&message_buff, sizeof(struct TextWithSource), 0) == -1)
      {
        perror("sending error\n");
        pthread_exit((void *)EXIT_FAILURE);
      }

    } while (new_line_pos != 0);
  }

  pthread_exit((void *)EXIT_SUCCESS);
}
