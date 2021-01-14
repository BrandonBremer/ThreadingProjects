/*Discussed with George Taliaferro wgt7xp*/
#include "life.h"
#include <pthread.h>
typedef struct 
{
	LifeBoard *state;
	LifeBoard *next_state;
	int steps;
	int width;
	int height;
	int start;
	pthread_barrier_t *b;
	pthread_barrier_t *b2;
	int id;
	int threads;
}arg_struct;
void simulate_life_parallel_draw(void* arg)
{
	arg_struct *args = (arg_struct *)arg;
	if(args->id ==0)
	{
		args->start +=1;
	}
	if(args->id == args->threads-1)
	{
		args->height-=1;
	}
	for (int step = 0; step < args->steps; step += 1) {
        /* We use the range [1, width - 1) here instead of
         * [0, width) because we fix the edges to be all 0s.
         */			
	for (int y = args->start; y < args->height; y += 1) {
            for (int x = 1; x < args->width - 1; x += 1) {
				
                /* For each cell, examine a 3x3 "window" of cells around it,
                 * and count the number of live (true) cells in the window. */
                int live_in_window = 0;
                for (int y_offset = -1; y_offset <= 1; y_offset += 1)
                    for (int x_offset = -1; x_offset <= 1; x_offset += 1)
					{
                        if (LB_get(args->state, x + x_offset, y + y_offset))
                            live_in_window += 1;
					}

                /* Cells with 3 live neighbors remain or become live.
                   Live cells with 2 live neighbors remain live. */
                LB_set(args->next_state, x, y,live_in_window == 3 ||(live_in_window == 4 && LB_get(args->state, x, y)));

            }
	}
	/* now that we computed next_state, make it the current state */
		pthread_barrier_wait(args->b);
		if(args->id==0)
		{
        LB_swap(args->state, args->next_state);
		}
		pthread_barrier_wait(args->b2);
    }
	if(args->id==0)
    	LB_del(args->next_state);
    
}
void simulate_life_parallel(int threads, LifeBoard *state, int steps) {
LifeBoard *next_state = LB_new(state->width, state->height);
	pthread_t *thread; 
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	int width = state->width;
	int height = state->height;
	arg_struct *args = calloc(threads,sizeof(arg_struct));
	pthread_barrier_t b;
    pthread_barrier_init(&b,NULL,threads);
	pthread_barrier_t b2;
    pthread_barrier_init(&b2,NULL,threads);
	thread = calloc(threads,sizeof(pthread_t));
	for(int i = 0; i < threads; i++)
	{
		args[i].state= state;
		args[i].next_state = next_state;
		args[i].steps = steps;
		args[i].width = width;
		args[i].height = (int)((double)(height)*(((double)i+1)/(double)threads));
		if(i==0)
		{
			args[i].start = 0;
		}
		else
		{
			args[i].start = args[i-1].height;
		}
		args[i].b = &b;
		args[i].b2 = &b2;
		args[i].id = i;
		args[i].threads = threads;
		pthread_create(&thread[i], &attr, simulate_life_parallel_draw, (void*)&args[i]);
	}
	for(int i = 0; i< threads; i++)
	{
		pthread_join(thread[i],NULL);
	}
	pthread_barrier_destroy(&b);
	pthread_barrier_destroy(&b2);
	free(args);
	free(thread);
}

