#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  void **buffer;
  size_t capacity;
  size_t size;
  size_t head;
  size_t tail;
  bool is_closed;
  pthread_mutex_t mutex;
  pthread_cond_t can_send;
  pthread_cond_t can_receive;
} ChannelInternal;

typedef struct {
  void *data;
  bool ok;
} ChannelResultInternal;

typedef void *ThreadInternalVoidPointer;

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct Channel {
  ChannelInternal channel;
};

struct ChannelResult {
  ChannelResultInternal result;
};

struct IntChannelResult {
  int value;
  bool ok;
};

struct Thread {
  pthread_t id;
  bool joined;
};

bool ChannelResultis_ok(struct ChannelResult *this);
bool IntChannelResultis_ok(struct IntChannelResult *this);
int IntChannelResultget_value(struct IntChannelResult *this);
void Channel__init__(struct Channel *this, int capacity);
ThreadInternalVoidPointer ChannelToThreadVoidPointer(struct Channel *this);
bool Channelsend_ptr(struct Channel *this, ThreadInternalVoidPointer data);
bool Channelsend(struct Channel *this, int data);
struct ChannelResult Channelreceive(struct Channel *this);
struct IntChannelResult Channelreceive_int(struct Channel *this);
void Channelclose(struct Channel *this);
void Channel__del__(struct Channel *this);
void Thread__init__(
    struct Thread *this,
    ThreadInternalVoidPointer (*start_routine)(ThreadInternalVoidPointer),
    ThreadInternalVoidPointer arg);
void Threadjoin(struct Thread *this);
void Thread__del__(struct Thread *this);
void Task();
ThreadInternalVoidPointer WorkerThread(ThreadInternalVoidPointer arg);
int main();

bool ChannelResultis_ok(struct ChannelResult *this) { return this->result.ok; }

bool IntChannelResultis_ok(struct IntChannelResult *this) { return this->ok; }

int IntChannelResultget_value(struct IntChannelResult *this) {
  return this->value;
}

void Channel__init__(struct Channel *this, int capacity) {
  if (capacity <= 0) {
    fprintf(stderr, "Error: Channel capacity must be positive.\n");
    exit(-1);
  }

  ChannelInternal *ch = &this->channel;

  ch->buffer = (void **)malloc(capacity * sizeof(void *));
  if (!ch->buffer) {
    fprintf(stderr, "Error: Memory allocation failed for Channel buffer.\n");
    exit(-1);
  }

  ch->capacity = capacity;
  ch->size = 0;
  ch->head = 0;
  ch->tail = 0;
  ch->is_closed = false;

  pthread_mutex_init(&ch->mutex, NULL);
  pthread_cond_init(&ch->can_send, NULL);
  pthread_cond_init(&ch->can_receive, NULL);
}

ThreadInternalVoidPointer ChannelToThreadVoidPointer(struct Channel *this) {
  ThreadInternalVoidPointer ptr = (ThreadInternalVoidPointer)this;
  return ptr;
}

bool Channelsend_ptr(struct Channel *this, ThreadInternalVoidPointer data) {
  ChannelInternal *ch = &this->channel;
  pthread_mutex_lock(&ch->mutex);

  while (ch->size == ch->capacity) {
    if (ch->is_closed) {
      pthread_mutex_unlock(&ch->mutex);
      return false;
    }
    pthread_cond_wait(&ch->can_send, &ch->mutex);
  }

  if (ch->is_closed) {
    pthread_mutex_unlock(&ch->mutex);
    return false;
  }

  ch->buffer[ch->tail] = data;
  ch->tail = (ch->tail + 1) % ch->capacity;
  ch->size++;

  pthread_cond_signal(&ch->can_receive);
  pthread_mutex_unlock(&ch->mutex);

  return true;
}

bool Channelsend(struct Channel *this, int data) {
  int *value = (int *)malloc(sizeof(int));
  if (!value) {
    fprintf(
        stderr,
        "Error: Threads::send() Memory allocation failed for sending data.\n");
    return false;
  }
  *value = data;
  return Channelsend_ptr(this, (ThreadInternalVoidPointer)value);
}

struct ChannelResult Channelreceive(struct Channel *this) {
  ChannelInternal *ch = &this->channel;
  pthread_mutex_lock(&ch->mutex);
  while (ch->size == 0) {
    if (ch->is_closed) {
      pthread_mutex_unlock(&ch->mutex);
      ChannelResultInternal result = {NULL, false};
      struct ChannelResult res;
      res.result = result;
      return res;
    }
    pthread_cond_wait(&ch->can_receive, &ch->mutex);
  }

  void *data = ch->buffer[ch->head];
  ch->head = (ch->head + 1) % ch->capacity;
  ch->size--;
  pthread_cond_signal(&ch->can_send);
  pthread_mutex_unlock(&ch->mutex);

  ChannelResultInternal result = {data, true};
  struct ChannelResult res;
  res.result = result;
  return res;
}

struct IntChannelResult Channelreceive_int(struct Channel *this) {
  ChannelInternal *ch = &this->channel;
  pthread_mutex_lock(&ch->mutex);
  while (ch->size == 0) {
    if (ch->is_closed) {
      pthread_mutex_unlock(&ch->mutex);
      struct IntChannelResult res = {0, false};
      return res;
    }
    pthread_cond_wait(&ch->can_receive, &ch->mutex);
  }

  int *data = (int *)ch->buffer[ch->head];
  ch->head = (ch->head + 1) % ch->capacity;
  ch->size--;
  pthread_cond_signal(&ch->can_send);
  pthread_mutex_unlock(&ch->mutex);

  struct IntChannelResult res = {*data, true};
  free(data);
  return res;
}

void Channelclose(struct Channel *this) {
  ChannelInternal *ch = &this->channel;
  pthread_mutex_lock(&ch->mutex);
  if (ch->is_closed) {
    pthread_mutex_unlock(&ch->mutex);
    return;
  }

  ch->is_closed = true;
  pthread_cond_broadcast(&ch->can_receive);
  pthread_cond_broadcast(&ch->can_send);
  pthread_mutex_unlock(&ch->mutex);
}

void Channel__del__(struct Channel *this) {
  ChannelInternal *ch = &this->channel;

  pthread_mutex_lock(&ch->mutex);

  for (size_t i = 0; i < ch->size; i++) {
    size_t index = (ch->head + i) % ch->capacity;
    free(ch->buffer[index]);
  }

  free(ch->buffer);
  ch->buffer = NULL;
  ch->size = 0;
  ch->capacity = 0;
  ch->head = 0;
  ch->tail = 0;
  ch->is_closed = true;

  pthread_mutex_unlock(&ch->mutex);

  pthread_mutex_destroy(&ch->mutex);
  pthread_cond_destroy(&ch->can_send);
  pthread_cond_destroy(&ch->can_receive);
}

void Thread__init__(
    struct Thread *this,
    ThreadInternalVoidPointer (*start_routine)(ThreadInternalVoidPointer),
    ThreadInternalVoidPointer arg) {
  if (pthread_create(&this->id, NULL, start_routine, arg) != 0) {
    fprintf(stderr, "Error: Failed to create thread.\n");
    exit(-1);
  }
  this->joined = false;
}

void Threadjoin(struct Thread *this) {
  pthread_join(this->id, NULL);
  this->joined = true;
}

void Thread__del__(struct Thread *this) {
  if (!this->joined) {
    fprintf(stderr, "Warning: Thread was not joined before deletion.\n");
    pthread_detach(this->id);
  }
}

struct Channel ch;

void Task() {

  while (true) {
    struct IntChannelResult res = Channelreceive_int(&ch);

    if (IntChannelResultis_ok(&res)) {
      int value = IntChannelResultget_value(&res);
      printf("Worker: Received %d \n", value);
    } else {
      printf("Worker: Channel is closed and empty. Exiting. \n");
      break;
    }
  }
}

ThreadInternalVoidPointer WorkerThread(ThreadInternalVoidPointer arg) {
  Task();
  return NULL;
}

int main() {
  // Global Variables Initialization.
  Channel__init__(&ch, 10);

  printf("Main: Created a channel with capacity 10. \n");

  printf("Main: Starting worker thread. \n");

  ThreadInternalVoidPointer channelAsVoidPointer =
      ChannelToThreadVoidPointer(&ch);
  // the above void* casting can be done by Thread itself.
  struct Thread worker;
  Thread__init__(&worker, WorkerThread, channelAsVoidPointer);

  for (size_t i = 1; i < 50; i++) {
    printf("Main: Sending %llu \n", i);

    bool res = Channelsend(&ch, i);

    if (res == false) {
      printf("Main: Failed to send data, channel is closed.\n");
      break;
    }
  }

  printf("Main: All items sent. Closing channel.\n");
  Channelclose(&ch);

  printf("Main: Waiting for worker to finish...\n");
  Threadjoin(&worker);

  printf("Main: Program finished successfully.\n");

  Thread__del__(&worker);

  Channel__del__(&ch);
  return 0;
}
