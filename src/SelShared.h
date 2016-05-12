/* SelShared.h
 *
 * Everything related to shared variables
 *
 * 07/06/2015 LF : First version
 * 11/11/2015 LF : Add TaskOnce enum
 * 20/01/2016 LF : Rename as SelShared
 * 16/04/2016 LF : Add TTL for variables
 */

#ifndef SHAREDOBJ_H
#define SHAREDOBJ_H

#include "selene.h"
#include <pthread.h>

#define FUNCREFLOOKTBL "__SELENE_FUNCREF"	/* Function reference lookup table */

extern void init_shared( lua_State * );
extern void init_shared_Lua( lua_State * );	/* Init only Lua's object */

enum SharedObjType {
	SOT_UNKNOWN = 0,	/* Invalid variable */
	SOT_NUMBER,		/* Integers */
	SOT_STRING,		/* Dynamically allocated string (managed by sharedobj' functions) */
	SOT_XSTRING		/* Const char * managed externally (constant, allocated elsewhere ... */
};

struct SharedVar {
	struct SharedVar *prev, *succ;	/* link list */
	const char *name;
	int H;
	enum SharedObjType type;
	time_t death;	/* when this variable become invalid ? */
	union {
		double num;
		const char *str;
	} val;
	pthread_mutex_t mutex;	/*AF* As long their is only 2 threads, a simple mutex is enough */
};

extern struct _SharedStuffs {
		/* Shared variables */
	struct SharedVar *first_shvar, *last_shvar;
	pthread_mutex_t mutex_shvar;	/*AF* As long there is only 2 threads, a simple mutex is enough */

		/* pending tasks */
	int todo[SO_TASKSSTACK_LEN];	/* pending tasks list */
	unsigned int ctask;			/* current task index */
	unsigned int maxtask;		/* top of the task stack */
	pthread_mutex_t mutex_tl;	/* tasklist protection */
	int tlfd;			/* Task list file descriptor for eventfd */
} SharedStuffs;


	/* exposed API */
enum TaskOnce {
	TO_MULTIPLE = 0,	/* Allow multiple run */
	TO_ONCE,			/* Push a task only if it isn't already queued */
	TO_LAST				/* Only one run but put at the end of the queue */
};

extern int pushtask( int, enum TaskOnce );
extern int findFuncRef(lua_State *, int );
extern void soc_sets( const char *, const char * );

#endif
