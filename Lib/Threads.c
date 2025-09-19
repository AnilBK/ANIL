// clang-format off
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    void** buffer;
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
    void* data;
    bool ok;
} ChannelResultInternal;

typedef void* ThreadInternalVoidPointer;

///*///

struct Channel {ChannelInternal channel};

struct ChannelResult{ChannelResultInternal result};

namespace ChannelResult

c_function is_ok() -> bool:
  return this->result.ok;
endc_function

endnamespace

struct IntChannelResult{int value, bool ok};

namespace IntChannelResult
c_function is_ok() -> bool:
  return this->ok;
endc_function

c_function get_value() -> int:
  return this->value;
endc_function
endnamespace

namespace Channel

c_function __init__(capacity : int)
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
endc_function

c_function ToThreadVoidPointer() -> ThreadInternalVoidPointer:
    ThreadInternalVoidPointer ptr = (ThreadInternalVoidPointer)this;
    return ptr;
endc_function

c_function send_ptr(data : ThreadInternalVoidPointer) -> bool:
    ChannelInternal* ch = &this->channel;
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
endc_function

c_function send(data : int) -> bool:
    int *value = (int*)malloc(sizeof(int));
    if (!value) {
        fprintf(stderr, "Error: Threads::send() Memory allocation failed for sending data.\n");
        return false;
    }
    *value = data;
    return Channelsend_ptr(this, (ThreadInternalVoidPointer)value);
endc_function

c_function receive() -> ChannelResult:
    ChannelInternal* ch = &this->channel;
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

    void* data = ch->buffer[ch->head];
    ch->head = (ch->head + 1) % ch->capacity;
    ch->size--;
    pthread_cond_signal(&ch->can_send);
    pthread_mutex_unlock(&ch->mutex);

    ChannelResultInternal result = {data, true};
    struct ChannelResult res;
    res.result = result;
    return res;
endc_function

c_function receive_int() -> IntChannelResult:
    ChannelInternal* ch = &this->channel;
    pthread_mutex_lock(&ch->mutex);
    while (ch->size == 0) {
        if (ch->is_closed) {
            pthread_mutex_unlock(&ch->mutex);
            struct IntChannelResult res = {0, false};
            return res;
        }
        pthread_cond_wait(&ch->can_receive, &ch->mutex);
    }

    int* data = (int*)ch->buffer[ch->head];
    ch->head = (ch->head + 1) % ch->capacity;
    ch->size--;
    pthread_cond_signal(&ch->can_send);
    pthread_mutex_unlock(&ch->mutex);

    struct IntChannelResult res = {*data, true};
    free(data);
    return res;
endc_function

c_function close()
    ChannelInternal* ch = &this->channel;
    pthread_mutex_lock(&ch->mutex);
    if (ch->is_closed) {
        pthread_mutex_unlock(&ch->mutex);
        return;
    }

    ch->is_closed = true;
    pthread_cond_broadcast(&ch->can_receive);
    pthread_cond_broadcast(&ch->can_send);
    pthread_mutex_unlock(&ch->mutex);
endc_function

c_function __del__()
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
endc_function

endnamespace

struct Thread{pthread_t id, bool joined};

namespace Thread

c_function __init__(start_routine: Fn(ThreadInternalVoidPointer) -> ThreadInternalVoidPointer, arg : ThreadInternalVoidPointer)
    if (pthread_create(&this->id, NULL, start_routine, arg) != 0) {
        fprintf(stderr, "Error: Failed to create thread.\n");
        exit(-1);
    }
    this->joined = false;
endc_function

c_function join()
    pthread_join(this->id, NULL);
    this->joined = true;
endc_function

c_function __del__()
    if (!this->joined) {
        fprintf(stderr, "Warning: Thread was not joined before deletion.\n");
        pthread_detach(this->id);
    }
endc_function
endnamespace
///*///

