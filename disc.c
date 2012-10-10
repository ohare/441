#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "disc.h"
#include "tools.h"

void *disc_start(void *args){
    int work_id = 0;
    //int disc_id = (*(int *) args);
    int disc_id = 0;
    info thread_info = *((info *)(args));

    //printf("(disc_start) info addr:%d\n",&thread_info);

    disc_id = get_id(&thread_info, pthread_self());
    if(disc_id < 0){
        fprintf(stderr,"Error fetching disc_id\n");
    }

    printf("I am disc:%d\n", disc_id);

    for(;;){
        if(!(is_circ_empty(&thread_info.read_queues[disc_id]))){    
            //printf("Stuff to read\n");
            pthread_mutex_lock(&thread_info.read_mons[disc_id].lock);
            work_id = read_circ_buf(&thread_info.read_queues[disc_id]);

            read(&thread_info.read_mons[work_id], &thread_info,disc_id);
            pthread_mutex_unlock(&thread_info.read_mons[disc_id].lock);
        }
        if(!(is_circ_empty(&thread_info.write_queues[disc_id]))){    
            //printf("Stuff to write\n");
            work_id = read_circ_buf(&thread_info.write_queues[disc_id]);
            //Lock mutex, write, release
            pthread_mutex_lock(&thread_info.write_mons[disc_id].lock);
            write(&thread_info.write_mons[work_id], &thread_info,disc_id);
            pthread_mutex_unlock(&thread_info.write_mons[disc_id].lock);
        }
        if(is_circ_empty(&thread_info.write_queues[disc_id]) &&
            is_circ_empty(&thread_info.read_queues[disc_id]) &&
            thread_info.disc_kill[disc_id] == 1){
            printf("Disc:%d finished disc clock:%d\n",disc_id,thread_info.disc_times[disc_id]);
            break;
        }
        //printf("Thread:%u, Num:%d\n",pthread_self(),(*(int *) args));
    }

    return 0;
}

//'Read' from disc
int read(rm *rmon, info *i, int disc_id){

    if(rmon->request_time > i->disc_times[disc_id]){
        i->disc_times[disc_id] = rmon->request_time;
    }

    //printf("Read: disc clock is now:%d\n",my_clock);

    rmon->receipt_time = i->disc_times[disc_id];

    //Pretend to read in memory to buffer

    i->disc_times[disc_id] += 10 + 12 * drand48();

    rmon->completion_time = i->disc_times[disc_id];

    //update monitor with receipt time and completion time
    rmon->completion_time = i->disc_times[disc_id];

    //do anything else it needs to let requestor know to proceed

    return 0;
}

//'Write' to buffer
int write(wm *wmon, info *i, int disc_id){

    if(wmon->request_time > i->disc_times[disc_id]){
        i->disc_times[disc_id] = wmon->request_time;
    }

    //printf("Write: Disk clock is now:%d\n",my_clock);

    wmon->receipt_time = i->disc_times[disc_id];

    //Pretend to write to buffer

    printf("Disk time:%d\n",i->disc_times[disc_id]);
    i->disc_times[disc_id] += 10 + 12 * drand48();
    printf("Disk time:%d\n",i->disc_times[disc_id]);

    wmon->completion_time = i->disc_times[disc_id];
    printf("Disk time:%d\n",i->disc_times[disc_id]);

    //do anything else it needs to let requestor know to proceed

    return 0;
}

int quit(){
    //Exit thread
    return 0;
}
