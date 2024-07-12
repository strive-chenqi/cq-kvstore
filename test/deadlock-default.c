

#define _GNU_SOURCE
#include <dlfcn.h>


#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#define MAX		100
typedef unsigned long int uint64;



struct rela_node_s {
	pthread_mutex_t *mtx;
	pthread_t thid;
};

struct rela_node_s rela_table[MAX] = {0};

pthread_t search_rela_table(pthread_mutex_t *mtx) {

	int i = 0;

	for (i = 0;i < MAX;i ++) {
		if (mtx == rela_table[i].mtx) {
			return rela_table[i].thid;
		}
	}
	return 0;
} 

int del_rela_table(pthread_mutex_t *mtx, pthread_t tid) {

	int i = 0;

	for (i = 0;i < MAX;i ++) {

		if ((mtx == rela_table[i].mtx) && (tid == rela_table[i].thid)) {
			rela_table[i].mtx = NULL;
			rela_table[i].thid = 0;
			return 0;
		}
	
	}

	return -1;
}

int add_rela_table(pthread_mutex_t *mtx, pthread_t tid) {

	int i = 0;

	for (i = 0;i < MAX;i ++) {

		if ((rela_table[i].mtx == NULL) && (rela_table[i].thid == 0)) {
			rela_table[i].mtx = mtx;
			rela_table[i].thid = tid;

			return 0;
		}
	}
	return -1;
}




#if 1




enum Type {PROCESS, RESOURCE};

struct source_type {

	uint64 id;
	enum Type type;

	uint64 lock_id;
	int degress;
};

struct vertex {

	struct source_type s;
	struct vertex *next;

};

struct task_graph {

	struct vertex list[MAX];
	int num;

	struct source_type locklist[MAX];
	int lockidx;

	pthread_mutex_t mutex;
};

struct task_graph *tg = NULL;
int path[MAX+1];
int visited[MAX];
int k = 0;
int deadlock = 0;

struct vertex *create_vertex(struct source_type type) {

	struct vertex *tex = (struct vertex *)malloc(sizeof(struct vertex ));

	tex->s = type;
	tex->next = NULL;

	return tex;

}


int search_vertex(struct source_type type) {

	int i = 0;

	for (i = 0;i < tg->num;i ++) {

		if (tg->list[i].s.type == type.type && tg->list[i].s.id == type.id) {
			return i;
		}

	}

	return -1;
}

void add_vertex(struct source_type type) {

	if (search_vertex(type) == -1) {

		tg->list[tg->num].s = type;
		tg->list[tg->num].next = NULL;
		tg->num ++;

	}

}


int add_edge(struct source_type from, struct source_type to) {

	add_vertex(from);
	add_vertex(to);

	struct vertex *v = &(tg->list[search_vertex(from)]);

	while (v->next != NULL) {
		v = v->next;
	}

	v->next = create_vertex(to);

}


int verify_edge(struct source_type i, struct source_type j) {

	if (tg->num == 0) return 0;

	int idx = search_vertex(i);
	if (idx == -1) {
		return 0;
	}

	struct vertex *v = &(tg->list[idx]);

	while (v != NULL) {

		if (v->s.id == j.id) return 1;

		v = v->next;
		
	}

	return 0;

}


int remove_edge(struct source_type from, struct source_type to) {

	int idxi = search_vertex(from);
	int idxj = search_vertex(to);

	if (idxi != -1 && idxj != -1) {

		struct vertex *v = &tg->list[idxi];
		struct vertex *remove;

		while (v->next != NULL) {

			if (v->next->s.id == to.id) {

				remove = v->next;
				v->next = v->next->next;

				free(remove);
				break;

			}

			v = v->next;
		}

	}

}


void print_deadlock(void) {

	int i = 0;

	printf("cycle : ");
	for (i = 0;i < k-1;i ++) {

		printf("%ld --> ", tg->list[path[i]].s.id);

	}

	printf("%ld\n", tg->list[path[i]].s.id);

}

int DFS(int idx) {

	struct vertex *ver = &tg->list[idx];
	if (visited[idx] == 1) {

		path[k++] = idx;
		print_deadlock();
		deadlock = 1;
		
		return 0;
	}

	visited[idx] = 1;
	path[k++] = idx;

	while (ver->next != NULL) {

		DFS(search_vertex(ver->next->s));
		k --;
		
		ver = ver->next;

	}

	
	return 1;

}


int search_for_cycle(int idx) {

	

	struct vertex *ver = &tg->list[idx];
	visited[idx] = 1;
	k = 0;
	path[k++] = idx;

	while (ver->next != NULL) {

		int i = 0;
		for (i = 0;i < tg->num;i ++) {
			if (i == idx) continue;
			
			visited[i] = 0;
		}

		for (i = 1;i <= MAX;i ++) {
			path[i] = -1;
		}
		k = 1;

		DFS(search_vertex(ver->next->s));
		ver = ver->next;
	}

}

int init_graph(void) {

	tg = (struct task_graph*)malloc(sizeof(struct task_graph));
	tg->num = 0;

}

#endif


void before_lock(pthread_t tid, pthread_mutex_t *mtx) {

	pthread_t otherid = search_rela_table(mtx);

	if (otherid != 0) {

		struct source_type from;
		from.id = tid;
		from.type = PROCESS;

		struct source_type to;
		to.id = otherid;
		to.type = PROCESS;
		
		add_edge(tid, otherid);
	}
	

}

void after_lock(pthread_t tid, pthread_mutex_t *mtx) {

	pthread_t otherid = search_rela_table(mtx);

	if (otherid != 0) {

		struct source_type from;
		from.id = tid;
		from.type = PROCESS;

		struct source_type to;
		to.id = otherid;
		to.type = PROCESS;

		if (verify_edge(from, to)) {
			remove_edge(from, to);
		}
	}

	add_rela_table(mtx, tid);

}

void after_unlock(pthread_t tid, pthread_mutex_t *mtx) {

	del_rela_table(mtx, tid);
}


void check_dead_lock(void) {

	int i = 0;

	for (i = 0;i < tg->num;i ++) {

		
		search_for_cycle(i);

	}

	

}

static void *thread_routine(void *args) {

	while (1) {

		sleep(5);
		check_dead_lock();
		

	}

}


#if 1

typedef int (*pthread_mutex_lock_t)(pthread_mutex_t *mtx);
pthread_mutex_lock_t pthread_mutex_lock_f = NULL;

typedef int (*pthread_mutex_unlock_t)(pthread_mutex_t *mtx);
pthread_mutex_unlock_t pthread_mutex_unlock_f = NULL;

typedef int (*pthread_create_t)(pthread_t *restrict thread, const pthread_attr_t *restrict attr,
                          void *(*start_routine)(void *), void *restrict arg);
pthread_create_t pthread_create_f = NULL;


int pthread_mutex_lock(pthread_mutex_t *mtx) {

	printf("before pthread_mutex_lock %ld, %p\n", pthread_self(), mtx);

	pthread_mutex_lock_f(mtx);

	printf("after pthread_mutex_lock\n");

}

int pthread_mutex_unlock(pthread_mutex_t *mtx) {

	
	pthread_mutex_unlock_f(mtx);

	printf("after pthread_mutex_unlock %ld\n, %p", pthread_self(), mtx);

}


int pthread_create(pthread_t *restrict thread, const pthread_attr_t *restrict attr,
                          void *(*start_routine)(void *), void *restrict arg) {

	
	
	pthread_create_f(thread, attr, start_routine, arg);

	struct source_type v1;
	v1.id = *thread;
	v1.type = PROCESS;
	add_vertex(v1);

}


void init_hook(void) {

	if (!pthread_mutex_lock_f) {
		pthread_mutex_lock_f = dlsym(RTLD_NEXT, "pthread_mutex_lock");
	}

	if (!pthread_mutex_unlock_f) {
		pthread_mutex_unlock_f = dlsym(RTLD_NEXT, "pthread_mutex_unlock");
	}
	if (!pthread_create_f) {
		pthread_create_f = dlsym(RTLD_NEXT, "pthread_create");
	}
}
#endif

#if 1  // debug


pthread_mutex_t mtx1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx4 = PTHREAD_MUTEX_INITIALIZER;

void* t1_cb(void *arg) {

	pthread_mutex_lock(&mtx1);
	sleep(1);
	pthread_mutex_lock(&mtx2);

	printf("t1_cb\n");
	pthread_mutex_unlock(&mtx2);
	pthread_mutex_unlock(&mtx1);
}


void *t2_cb(void *arg) {

	pthread_mutex_lock(&mtx2);
	sleep(1);
	
	pthread_mutex_lock(&mtx3);

	printf("t2_cb\n");

	pthread_mutex_unlock(&mtx3);
	pthread_mutex_unlock(&mtx2);

}

void *t3_cb(void *arg) {

	pthread_mutex_lock(&mtx3);
	sleep(1);

	pthread_mutex_lock(&mtx4);

	printf("t3_cb\n");
	
	pthread_mutex_unlock(&mtx4);
	pthread_mutex_unlock(&mtx3);
}

void *t4_cb(void *arg) {

	pthread_mutex_lock(&mtx4);
	sleep(1);

	pthread_mutex_lock(&mtx1);
	printf("t4_cb\n");
	
	pthread_mutex_unlock(&mtx1);
	pthread_mutex_unlock(&mtx4);

}







int main() {

	init_graph();
	init_hook();


	pthread_t t1, t2, t3, t4;

	pthread_create(&t1, NULL, t1_cb, NULL);
	pthread_create(&t2, NULL, t2_cb, NULL);
	pthread_create(&t3, NULL, t3_cb, NULL);
	pthread_create(&t4, NULL, t4_cb, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	pthread_join(t3, NULL);
	pthread_join(t4, NULL);

	printf("complete\n");

	return 0;
}

#endif




#if 1
int main() {


	tg = (struct task_graph*)malloc(sizeof(struct task_graph));
	tg->num = 0;

	struct source_type v1;
	v1.id = 1;
	v1.type = PROCESS;
	add_vertex(v1);

	struct source_type v2;
	v2.id = 2;
	v2.type = PROCESS;
	add_vertex(v2);

	struct source_type v3;
	v3.id = 3;
	v3.type = PROCESS;
	add_vertex(v3);

	struct source_type v4;
	v4.id = 4;
	v4.type = PROCESS;
	add_vertex(v4);

	
	struct source_type v5;
	v5.id = 5;
	v5.type = PROCESS;
	add_vertex(v5);


	add_edge(v1, v2);
	add_edge(v2, v3);
	add_edge(v3, v4);
	add_edge(v4, v5);
	add_edge(v3, v1);
	
	search_for_cycle(search_vertex(v1));

}
#endif


