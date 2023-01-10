#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "common.h"

void sigint_handler();
int queue_id;

int main(int argc, char **argv)
{
  key_t queue_key = create_key(2115);
  struct Message message_buff;

  queue_id = create_msg_queue(queue_key);
  signal(SIGINT, sigint_handler);

  while (1)
  {
    message_buff.m_destination = SERVER;

    if (msgrcv(queue_id, (struct Message *)&message_buff, sizeof(struct TextWithSource), message_buff.m_destination, 0) == -1)
    {
      perror("failed to receive message\n");
      exit(EXIT_FAILURE);
    }

    printf("\tReceived from %ld: %s -> %ld to \n", message_buff.m_text_with_source.source, message_buff.m_text_with_source.text, message_buff.m_destination);

    size_t message_size = strlen(message_buff.m_text_with_source.text);

    for (int i = 0; i < message_size; i++)
      message_buff.m_text_with_source.text[i] = toupper(message_buff.m_text_with_source.text[i]);

    message_buff.m_destination = message_buff.m_text_with_source.source;
    message_buff.m_text_with_source.source = SERVER;

    if (msgsnd(queue_id, (struct Message *)&message_buff, sizeof(struct TextWithSource), 0) == -1)
    {
      printf("cant send back processed message \n");
      exit(EXIT_FAILURE);
    }
  }

  exit(EXIT_SUCCESS);
}

void sigint_handler()
{
  remove_msg_queue(queue_id);
}
