#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include "common.h"

void clear_abort();
void sigint_handler(int sig);
int queue_id;

int main(int argc, char **argv)
{
  key_t queue_key = create_key(2115);
  struct Message message_buff;
  signal(SIGINT, sigint_handler);
  queue_id = create_msg_queue(queue_key);

  while (1)
  {
    printf("[S] Waiting for messages\n");

    message_buff.m_destination = SERVER;

    if (msgrcv(queue_id, (struct Message *)&message_buff, sizeof(struct TextWithSource), message_buff.m_destination, 0) == -1)
    {
      perror("[S] failed to receive message\n");
      clear_abort();
    }

    printf("[S] Received: %s [%ld: -> %ld]\n", message_buff.m_text_with_source.text, message_buff.m_text_with_source.source, message_buff.m_destination);

    size_t message_size = strlen(message_buff.m_text_with_source.text);

    for (int i = 0; i < message_size; i++)
      message_buff.m_text_with_source.text[i] = toupper(message_buff.m_text_with_source.text[i]);

    message_buff.m_destination = message_buff.m_text_with_source.source;
    message_buff.m_text_with_source.source = SERVER;

    printf("[S] Sending: %s [%ld: -> %ld]\n", message_buff.m_text_with_source.text, message_buff.m_text_with_source.source, message_buff.m_destination);

    if (msgsnd(queue_id, (struct Message *)&message_buff, sizeof(struct TextWithSource), 0) == -1)
    {
      perror("[S] Error in sending message to the queue\n");
      clear_abort();
    }
  }

  remove_msg_queue(queue_id);
  exit(EXIT_SUCCESS);
}

void sigint_handler(int sig)
{
  printf("Signal SIGINT!\n");
  remove_msg_queue(queue_id);
  return;
}

void clear_abort()
{
  remove_msg_queue(queue_id);
  exit(EXIT_FAILURE);
}
