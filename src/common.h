#ifndef COMMON_H
#define COMMON_H

#define MAX 8184 // min is 2, because 1 is reserved for NULL TERMINATOR
#define SERVER 1

#include "stdio.h"
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct TextWithSource
{
  long source;
  char text[MAX];
};

struct Message
{
  long m_destination;
  struct TextWithSource m_text_with_source;
};

key_t create_key(int id);
int create_msg_queue(key_t key);
void remove_msg_queue(int queue_id);

#endif