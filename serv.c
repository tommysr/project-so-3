#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#define MAX 20
#define SERVER 1

key_t create_key(int id);

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

void sigint_handler();
int create_msg_queue(key_t key);
int queue_id;

int main(int argc, char **argv)
{
  key_t queue_key = create_key(2115);
  queue_id = create_msg_queue(queue_key);
  signal(SIGINT, sigint_handler);

  struct Message message_buff;

  int i;

  while (1)
  {
    message_buff.m_destination = SERVER;

    if (msgrcv(queue_id, (struct Message *)&message_buff, sizeof(struct TextWithSource), message_buff.m_destination, 0) == -1)
    {
      perror("failed to receive message\n");
      exit(EXIT_FAILURE);
    }

    printf("\tReceived from %d -> %s to \n", message_buff.m_text_with_source.source, message_buff.m_text_with_source.text, message_buff.m_destination);

    size_t message_size = strlen(message_buff.m_text_with_source.text);

    int i;
    for (i = 0; i < message_size; i++)
      message_buff.m_text_with_source.text[i] = to_upper(message_buff.m_text_with_source.text[i]);

    message_buff.m_destination = (long)message_buff.m_text_with_source.source;
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

void remove_msg_queue(int queue_id)
{
  int msg_rem = msgctl(queue_id, IPC_RMID, 0);

  if (msg_rem == -1)
  {
    perror("cant delete msg queue");
    exit(EXIT_FAILURE);
  }
  else
  {
    printf("queue deleted");
    exit(EXIT_SUCCESS);
  }
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
    printf("removed queue %d\n", queue_id);
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
