#include <stdio.h>
#include <assert.h>
#include <stdatomic.h>
#include <pthread.h> 

#define A 1
#define B 2

#define BARRIER __sync_synchronize()

int nested = 0;
long count = 0;

void critical_section() {
  long cnt = atomic_fetch_add(&count, 1);
  int i = atomic_fetch_add(&nested, 1) + 1;
  if (i != 1) {
    printf("%d threads in the critical section @ count=%ld", i, cnt);
    assert(0);
  }
  atomic_fetch_add(&nested, -1);
}

int x = 0, y = 0, turn;

void *TA(void *arg) {
  while (1) {
    x = 1;                   BARRIER;
    turn = B;                BARRIER;
    while (1) {
      if (!y) break;         BARRIER;
      if (turn != B) break;  BARRIER;
    }
    critical_section();
    x = 0;                   BARRIER;
  }
}

void *TB(void *arg) {
  while (1) {
    y = 1;                   BARRIER;
    turn = A;                BARRIER;
    while (1) {
      if (!x) break;         BARRIER;
      if (turn != A) break;  BARRIER;
    }
    critical_section();
    y = 0;                   BARRIER;
  }
}

int main() {
  pthread_t tid_TA, tid_TB;
  
  pthread_create(&tid_TA, NULL, TA, NULL);
  pthread_create(&tid_TB, NULL, TB, NULL);
  
  pthread_join(tid_TA, NULL);
  pthread_join(tid_TB, NULL);
  
  return 0;
}
