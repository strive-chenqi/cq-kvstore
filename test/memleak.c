
#define _GNU_SOURCE
#include <dlfcn.h>
#include <link.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


// kvstore -c kv.conf

int flag = 1;

#if 0

void *nMalloc(size_t size, const char *filename, int line) {
	void *p = malloc(size);

	if (flag) {
		
		char buff[128] = {0};
		snprintf(buff, 128, "./mem/%p.mem", p);

		FILE *fp = fopen(buff, "w");
		if (!fp) {
			free(p);
			return NULL;
		}

		fprintf(fp, "[+]%s:%d, addr: %p, size: %ld\n", filename, line, p, size);
		fflush(fp);
		fclose(fp);

	}
	
	//printf("nMalloc: %p, size: %ld\n", p, size);
	return p;
}

void nFree(void *ptr) {
	//printf("nFree: %p, \n", ptr);

	if (flag) {
		
		char buff[128] = {0};
		snprintf(buff, 128, "./mem/%p.mem", ptr);

		if (unlink(buff) < 0) {
			printf("double free: %p", ptr);
			return ;
		}

	}
	
	return free(ptr);
}



#define malloc(size) nMalloc(size, __FILE__, __LINE__)
#define free(ptr) nFree(ptr)


#else

// hook 

typedef void *(*malloc_t)(size_t size);
malloc_t malloc_f = NULL;

typedef void (*free_t)(void *ptr);
free_t free_f = NULL;

int enable_malloc = 1;
int enable_free = 1;



void *ConvertToELF(void *addr) {

	Dl_info info;
	struct link_map *link;
	
	dladdr1(addr, &info, (void **)&link, RTLD_DL_LINKMAP);

	return (void *)((size_t)addr - link->l_addr);
}

// main --> f1 --> f2 --> malloc

void *malloc(size_t size) {

	void *p = NULL;

	if (enable_malloc) {
		enable_malloc = 0;

		p = malloc_f(size);

		void *caller = __builtin_return_address(0);

		char buff[128] = {0};
		sprintf(buff, "./mem/%p.mem", p);

		FILE *fp = fopen(buff, "w");
		if (!fp) {
			free(p);
			return NULL;
		}

		//fprintf(fp, "[+]%p, addr: %p, size: %ld\n", caller, p, size);
		fprintf(fp, "[+]%p, addr: %p, size: %ld\n", ConvertToELF(caller), p, size);
		fflush(fp);
		
		enable_malloc = 1;
		
	} else {
		p = malloc_f(size);
	}


	return p;
}


// addr2line 
void free(void *ptr) {

	if (enable_free) {
		enable_free = 0;

		char buff[128] = {0};
		snprintf(buff, 128, "./mem/%p.mem", ptr);

		if (unlink(buff) < 0) {
			printf("double free: %p", ptr);
			return ;
		}

		free_f(ptr);

		enable_free = 1;
	} else {
		free_f(ptr);
	}

	return ;
}


void init_hook(void) {

	if (!malloc_f) {
		malloc_f = (malloc_t)dlsym(RTLD_NEXT, "malloc");
	}
	if (!free_f) {
		free_f = (free_t)dlsym(RTLD_NEXT, "free");
	}

}



#endif


// __FILE__, __LINE__, __func__

int main() {

	init_hook();

#if 1
	void *p1 = malloc(5);
	void *p2 = malloc(10);  
	void *p3 = malloc(35);
	void *p4 = malloc(10);

	free(p1);
	free(p3);
	free(p4);
#else
	void *p1 = nMalloc(5);
	void *p2 = nMalloc(10);
	void *p3 = nMalloc(15);

	nFree(p1);
	nFree(p3);
#endif
	getchar();

} 



