#ifndef RWLOCK_H
#define RWLOCK_H

typedef struct {
	int n_waiting_readers;	
	int n_waiting_writers;	
	int n_writers;	
	int n_readers;	
	pthread_mutex_t	mu;	
	pthread_cond_t cond;	
	//Your code here
}rwl;

void rwl_init(rwl *l);
int rwl_nwaiters(rwl *l);
int rwl_rlock(rwl *l, const struct timespec *expire);
void rwl_runlock(rwl *l);
int rwl_wlock(rwl *l, const struct timespec *expire);
void rwl_wunlock(rwl *l);

#endif
