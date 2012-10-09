#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "disc.h"
#include "tools.h"

int my_clock = 0;

void *disc_start(void *args){
    int work_id = 0;
    //int disc_id = (*(int *) args);
    int disc_id = 0;
    info thread_info = *((info *)(args));

    //printf("(disc_start) info addr:%d\n",&thread_info);

    disc_id = get_id(thread_info, pthread_self());
    if(disc_id < 0){
        fprintf(stderr,"Error fetching disc_id\n");
    }

    printf("I am disc:%d\n", disc_id);

    for(;;){
        //Debugging?
        if(disc_id == 0){
            //printf("Checking read_queues[%d]\n",disc_id);
        }
        if(!(is_circ_empty(&thread_info.read_queues[disc_id]))){    
            printf("Stuff to read\n");
            work_id = read_circ_buf(&thread_info.read_queues[disc_id]);
            read(&thread_info.read_mons[work_id]);
        }
        if(!(is_circ_empty(&thread_info.write_queues[disc_id]))){    
            printf("Stuff to write\n");
            work_id = read_circ_buf(&thread_info.write_queues[disc_id]);
            write(&thread_info.write_mons[work_id]);
        }
        //printf("Thread:%u, Num:%d\n",pthread_self(),(*(int *) args));
    }
}

int read(rm *rmon){
    //PTHREAD_MUTEX_ERRORCHECKER

    if(rmon->request_time > my_clock){
        my_clock = rmon->request_time;
    }

    printf("Read: disc clock is now:%d\n",my_clock);

    rmon->receipt_time = my_clock;

    //Pretend to read in memory to buffer

    my_clock = 10 + 12 * drand48();

    rmon->completion_time = my_clock;

    //update monitor with receipt time and completion time
    rmon->completion_time = my_clock;

    //do anything else it needs to let requestor know to proceed

    return 0;
}

int write(wm *wmon){

    if(wmon->request_time > my_clock){
        my_clock = wmon->request_time;
    }

    printf("Write: Disk clock is now:%d\n",my_clock);

    wmon->receipt_time = my_clock;

    //Pretend to write to buffer

    my_clock = 10 + 12 * drand48();

    wmon->completion_time = my_clock;

    //do anything else it needs to let requestor know to proceed

    return 0;
}

int quit(){
    //Exit thread
    return 0;
}
