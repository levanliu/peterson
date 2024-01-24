#include <stdio.h>
#include <stdatomic.h>
#include <assert.h>
#include <pthread.h>

#define A 1
#define B 2

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
atomic_int nested;
atomic_long count;

void critical_section() {
  long cnt = atomic_fetch_add(&count, 1);
  assert(atomic_fetch_add(&nested, 1) == 0);
  atomic_fetch_add(&nested, -1);
}

volatile int x = 0, y = 0, turn = A;

void *TA(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        x = 1;
        turn = B;
        while (y && turn == B);
        pthread_mutex_unlock(&mutex);
        
        critical_section();
        
        pthread_mutex_lock(&mutex);
        x = 0;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *TB(void *arg) {
  while (1) {
    pthread_mutex_lock(&mutex);
    y = 1;
    turn = A;
    while (x && turn == A);
    pthread_mutex_unlock(&mutex);
    
    critical_section();
    
    pthread_mutex_lock(&mutex);
    y = 0;
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

int main() {
  pthread_t tid_TA, tid_TB;
  
  pthread_create(&tid_TA, NULL, TA, NULL);
  pthread_create(&tid_TB, NULL, TB, NULL);
  
  pthread_join(tid_TA, NULL);
  pthread_join(tid_TB, NULL);
  
  return 0;
}
