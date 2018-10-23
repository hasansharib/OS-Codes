#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>

#define item_in 10
#define item_out 100

int item_in_array[item_in], item_out_array[item_out], item_in_count = 0, item_out_count = 0, num = 0;
pthread_t p_tid, c_tid[3], disp_tid;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t full, empty;

void add_item(int item) 
{
        int i = 0;
        if(num==0)
        {
                item_out_array[0] = item;
                num++;
        }
        else
        {
                i = num;
                item_out_array[i] = item;
                num++;
        }
}

void * producer(void * var)
{
        int item; 
        while(1)
        {
                item = (rand() % 100) + 1;                
                sem_wait(&empty);
                pthread_mutex_lock(&mutex);
                if(item_in_count < item_in)
                {
                        item_in_array[item_in_count] = item;
                        printf("\nproduced item: %d",item_in_array[item_in_count]);
                        item_in_count++;
                }
                pthread_mutex_unlock(&mutex);
                sem_post(&full);
                sleep(1);
        }
        return 0;
}

void * consumer(void * var)
{
        int item; 
        while(1)
        {
                sem_wait(&full);
                pthread_mutex_lock(&mutex);
                if(item_in_count > 0)
                {
                        item = item_in_array[--item_in_count];
                        printf("\nconsumed item: %d",item);
                        add_item(item);
                }
                pthread_mutex_unlock(&mutex);
                sem_post(&empty);
                sleep(1);
        }
        return 0;
}

void bubble_sort()
{
        int i, j, temp;
        
        for(i=0; i<num; i++)
        {
                for(j=0; j<(num-1); j++)
                {
                     if(item_out_array[j] >= item_out_array[j+1])
                     {
                        temp = item_out_array[j+1];
                        item_out_array[j+1] = item_out_array[j];
                        item_out_array[j] = temp;
                     }   
                }
        }
}

void * display(void * var)
{
        int i;
        while(1)
        {
                bubble_sort();
                printf("\nnumbers: ");
                for(i=0; i<num; i++)
                {
                        printf("%d ", item_out_array[i]);
                }
        }
        sleep(10);
}

int main()
{
        int i;
        
        sem_init(&full, 0, 0);
        sem_init(&empty, 0, 10);
        
        pthread_create(&p_tid, NULL, producer, NULL);
        
        for(i=0; i<3; i++)
        {
                pthread_create(&c_tid[i], NULL, consumer, NULL);
        } 
        
        pthread_create(&disp_tid, NULL, display, NULL);
        
        pthread_join(disp_tid, NULL);
        
        return 0;
}

