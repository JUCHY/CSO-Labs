#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <assert.h>
#include "rwlock.h"

/* rwl implements a reader-writer lock.
 * A reader-write lock can be acquired in two different modes, 
 * the "read" (also referred to as "shared") mode,
 * and the "write" (also referred to as "exclusive") mode.
 * Many threads can grab the lock in the "read" mode.  
 * By contrast, if one thread has acquired the lock in "write" mode, no other 
 * threads can acquire the lock in either "read" or "write" mode.
 */

//helper function
static inline int
cond_timedwait(pthread_cond_t *c, pthread_mutex_t *m, const struct timespec *expire)
{
	int r; 
	if (expire != NULL)  {
		r = pthread_cond_timedwait(c, m, expire);
	} else
		r = pthread_cond_wait(c, m);
	assert(r == 0 || r == ETIMEDOUT);
       	return r;
}

//rwl_init initializes the reader-writer lock 
void
rwl_init(rwl *l)
{
	l->n_waiting_readers= 0;
	l->n_waiting_writers = 0;
	l->n_writers = 0;
	l->n_readers = 0;
	pthread_mutex_init(&(l->mu),NULL);
	pthread_cond_init(&(l->cond),NULL);
	//Your code here
}

//rwl_nwaiters returns the number of threads *waiting* to acquire the lock
//Note: it should not include any thread who has already grabbed the lock
int
rwl_nwaiters(rwl *l) 
{
	//Your code here
	return l->n_waiting_readers+l->n_waiting_writers;

}

//rwl_rlock attempts to grab the lock in "read" mode
//if lock is not grabbed before absolute time "expire", it returns ETIMEDOUT
//else it returns 0 (when successfully grabbing the lock)
int
rwl_rlock(rwl *l, const struct timespec *expire)
{
	pthread_mutex_lock(&(l->mu));
	l->n_waiting_readers++;
	//if(l->n_waiting_writers>0){
	//	pthread_cond_signal()
	//}
	while(l->n_writers>0 || l->n_waiting_writers>0){
		if(cond_timedwait(&l->cond,&(l->mu), expire)==ETIMEDOUT){
			l->n_waiting_readers--;
			pthread_mutex_unlock(&(l->mu));
			return ETIMEDOUT;
		}
	}
	l->n_waiting_readers--;
	l->n_readers++;
	pthread_mutex_unlock(&(l->mu));
	//Your code here
	return 0;
}

//rwl_runlock unlocks the lock held in the "read" mode
void
rwl_runlock(rwl *l)
{
	pthread_mutex_lock(&(l->mu));
	l->n_readers--;
	if(l->n_waiting_writers>0){
		pthread_cond_signal(&l->cond);
	}
	pthread_mutex_unlock(&(l->mu));
	//Your code here
}

//rwl_wlock attempts to grab the lock in "write" mode
//if lock is not grabbed before absolute time "expire", it returns ETIMEDOUT
//else it returns 0 (when successfully grabbing the lock)
int
rwl_wlock(rwl *l, const struct timespec *expire)
{
	pthread_mutex_lock(&(l->mu));
	l->n_waiting_writers++;
	while(l->n_writers>0 ||l->n_readers>0){
		if(cond_timedwait(&l->cond,&(l->mu), expire)==ETIMEDOUT){
			l->n_waiting_writers--;
			pthread_mutex_unlock(&(l->mu));
			return ETIMEDOUT;
		}

	}
	l->n_waiting_writers--;
	l->n_writers++;
	pthread_mutex_unlock(&(l->mu));
	//Your code here
	return 0;
}

//rwl_wunlock unlocks the lock held in the "write" mode
void
rwl_wunlock(rwl *l)
{
	pthread_mutex_lock(&(l->mu));
	l->n_writers--;
	pthread_cond_broadcast(&(l->cond));
	pthread_mutex_unlock(&(l->mu));

	//Your code here
}
