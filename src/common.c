#include "common.h"

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
    printf("created queue %d\n", queue_id);
  }

  return queue_id;
}

void remove_msg_queue(int queue_id)
{
  int msg_rem = msgctl(queue_id, IPC_RMID, 0);

  if (msg_rem == -1)
  {
    perror("cant delete msg queue\n");
    exit(EXIT_FAILURE);
  }
  else
  {
    printf("queue deleted\n");
    exit(EXIT_SUCCESS);
  }
}
