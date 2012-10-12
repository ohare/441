#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "disc.h"
#include "tools.h"

void *disc_start(void *args){
    mon *temp;
    int work_id = 0;
    //int disc_id = (*(int *) args);
    int disc_id = 0;
    //info thread_info = *((info *)(args));
    info thread_info = *((info *)(args));

    //printf("(disc_start) info addr:%d\n",&thread_info);

    disc_id = get_id(&thread_info, pthread_self());
    if(disc_id < 0){
        fprintf(stderr,"Error fetching disc_id\n");
    }

    printf("I am disc:%d\n", disc_id);

    for(;;){
        //pthread_mutex_lock(&thread_info.read_mons[disc_id]);
        /* If requests in the read queue to process */
        if(!(is_circ_empty(&thread_info.read_queues[disc_id]))){    
            //printf("Stuff to read\n");
            /* Should this be being locked? */
        //pthread_mutex_lock(&thread_info.read_mons[disc_id]);
        //pthread_cond_wait(&thread_info.read_ready[disc_id],
        //    &thread_info.read_mons[disc_id]);
            temp = read_circ_buf(&thread_info.read_queues[disc_id]);

            read(temp, &thread_info,disc_id);
        //pthread_mutex_unlock(&thread_info.read_mons[disc_id]);
        //printf("Read\n");
        }
        //pthread_mutex_unlock(&thread_info.read_mons[disc_id]);
        
        /* If requests in the write queue to process */
        if(!(is_circ_empty(&thread_info.write_queues[disc_id]))){    
            //printf("Stuff to write\n");
            temp = read_circ_buf(&thread_info.write_queues[disc_id]);
            //Lock mutex, write, release
            //pthread_mutex_lock(&thread_info.write_mons[disc_id]);
            write(temp, &thread_info,disc_id);
            //pthread_mutex_unlock(&thread_info.write_mons[disc_id]);
        }
        /*
        pthread_mutex_lock(&thread_info.write_mons[disc_id]);
        pthread_cond_wait(&thread_info.write_ready[disc_id],
            &thread_info.write_mons[disc_id]);
        temp = read_circ_buf(&thread_info.write_queues[disc_id]);
        write(temp, &thread_info,disc_id);
        pthread_mutex_unlock(&thread_info.write_mons[disc_id]);
        printf("Write\n");
        */
        /* If both queues are empty and the kill flag has been set, exit */
        if(is_circ_empty(&thread_info.write_queues[disc_id]) &&
            is_circ_empty(&thread_info.read_queues[disc_id]) &&
            thread_info.disc_kill[disc_id] == 1){
            printf("Disc:%d finished disc clock:%d\n",disc_id,
                thread_info.disc_times[disc_id]);
            break;
        }
        //printf("Thread:%u, Num:%d\n",pthread_self(),(*(int *) args));
    }

    return 0;
}

//'Read' from disc
int read(mon *rmon, info *i, int disc_id){
    mon temp;
    int work_id = rmon->work_id;

    /* Lock read response queue */
    pthread_mutex_lock(&(i->read_resp_lock[work_id]));

    if(rmon->request_time > i->disc_times[disc_id]){
        i->disc_times[disc_id] = rmon->request_time;
    }

    //printf("Read: disc clock is now:%d\n",i->disc_times[disc_id]);

    /* Set the receipt time */
    //rmon->receipt_time = i->disc_times[disc_id];
    temp.receipt_time = i->disc_times[disc_id];

    //Pretend to read in memory to buffer

    /* Update the disc time */
    i->disc_times[disc_id] += 10 + 12 * drand48();
    //printf("Read: disc clock is now:%d\n",i->disc_times[disc_id]);

    /* Set the completion time */
    //rmon->completion_time = i->disc_times[disc_id];
    temp.completion_time = i->disc_times[disc_id];

    //Let the requestor know to proceed

    /* Set the monitor with the completion time to requestor */
    i->read_response[work_id] = temp;

    /* Signal the finished condition so requestor knows to proceed */
    pthread_cond_signal(&(i->read_resp_fin[work_id]));

    /* Unlock write response queue */
    pthread_mutex_unlock(&(i->read_resp_lock[work_id]));

    return 0;
}

//'Write' to buffer
int write(mon *wmon, info *i, int disc_id){
    mon temp;
    int work_id = wmon->work_id;

    /* Lock read response queue */
    pthread_mutex_lock(&(i->write_resp_lock[work_id]));

    if(wmon->request_time > i->disc_times[disc_id]){
        i->disc_times[disc_id] = wmon->request_time;
    }

    //printf("Write: disc clock is now:%d\n",i->disc_times[disc_id]);

    /* Set the receipt time */
    //wmon->receipt_time = i->disc_times[disc_id];
    temp.receipt_time = i->disc_times[disc_id];

    //Pretend to read in memory to buffer

    /* Update the disc time */
    i->disc_times[disc_id] += 10 + 12 * drand48();
    //printf("Read: disc clock is now:%d\n",i->disc_times[disc_id]);

    /* Set the completion time */
    //wmon->completion_time = i->disc_times[disc_id];
    temp.completion_time = i->disc_times[disc_id];

    //Let the requestor know to proceed

    /* Set the monitor with the completion time to requestor */
    i->write_response[work_id] = temp;

    /* Signal the finished condition so requestor knows to proceed */
    pthread_cond_signal(&(i->write_resp_fin[work_id]));

    /* Unlock write response queue */
    pthread_mutex_unlock(&(i->write_resp_lock[work_id]));

    return 0;
}

int quit(){
    //Exit thread
    return 0;
}
