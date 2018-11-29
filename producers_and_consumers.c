/*auuthor:何智强
 *class:计算机3班
 *studentId:201641903312
 *QQ:157412086
 */

#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
 
#define TRUE  1
#define FALSE 0
#define SIZE 11
 
typedef struct _queue    //队列结构体,用作公用缓冲区
{
	int data[SIZE];
	int front;       //指向队头的下标
	int rear;        //指向队尾的下标
}Queue;
 
struct Sem              //信号量结构体
{
	sem_t empty;	//记录公用缓冲区中空缓冲区个数
	sem_t full;	//记录公用缓冲区中满缓冲区个数
	Queue q;
};
 
pthread_mutex_t mutex;	//声明互斥信号量
 
int num = 0;
 
struct Sem sem;
 
//队列初始化
int InitQueue (Queue *q)
{
	if (q == NULL)
	{
		return FALSE;
	}
	
	q->front = 0;
	q->rear  = 0;
	
	return TRUE;
}
 
//判断空对情况
int QueueEmpty (Queue *q)       
{
	if (q == NULL)
	{
		return FALSE;
	}
	
	return q->front == q->rear;
}
 
//判断队满的情况
int QueueFull (Queue *q)      
{
	if (q == NULL)
	{
		return FALSE;
	}
	
	
	return q->front == (q->rear+1)%SIZE;
} 
 
//出队函数
int DeQueue (Queue *q, int *x)    
{
	if (q == NULL)
	{
		return FALSE;
	}
	
	if (QueueEmpty(q))
	{
		return FALSE;
	}
	q->front = (q->front + 1) % SIZE;
	*x = q->data[q->front];
	
	return TRUE;
}
 
//进队函数
int EnQueue (Queue *q, int x)    
{
	if (q == NULL)
	{
		return FALSE;
	}
	
	if (QueueFull(q))
	{
		return FALSE;
	}
	
	q->rear = (q->rear+1) % SIZE;
	q->data[q->rear] = x;
	
	return TRUE;
}
 
//生产者线程
void *Producer()
{
	puts("生产者线程创建成功！");
	printf("线程ID为：%lu\n\n",pthread_self());
	while(1)
	{
		int time = rand() % 10 + 1;           //随机使程序睡眠0点几秒
		usleep(time * 100000);                 
		                                      
		sem_wait(&sem.empty);                  //信号量的P操作
		pthread_mutex_lock(&mutex);            //互斥锁上锁
		                                      
		num++;                                
		EnQueue (&(sem.q), num);               //消息进队
		printf("生产了一条消息：%d\n", num);  
		                                      
		pthread_mutex_unlock(&mutex);          //互斥锁解锁
		sem_post(&sem.full);                   //信号量的V操作
	}
}
 
//消费者线程
void *Consumer()
{
	puts("消费者线程创建成功！");
	printf("线程ID为：%lu\n\n",pthread_self());
	while(1)
	{
		int time = rand() % 10 + 1;    //随机使程序睡眠0点几秒
		usleep(time * 100000);
		
		sem_wait(&sem.full);            //信号量的P操作
		pthread_mutex_lock(&mutex);     //互斥锁上锁
		
		num--;
		DeQueue (&sem.q, &num);        //消息出队
		printf("消费了一条消息：%d\n",num);
		
		pthread_mutex_unlock(&mutex);   //互斥锁解锁
		sem_post(&sem.empty);          //信号量的V操作
	}
}

/*void kill_pthrad()
{	
	pthread_t producid;
	pthread_t consumid;
	char exit;
	while(1)
	{i
		scanf("%c", &exit);
		if(exit == '\n')
		{
			pthread_kill(producid, 9);
			pthread_kill(consumid, 9);
			puts("已杀死进程！");
		}
	}
}*/

int main()
{
	srand((unsigned int)time(NULL));	//播种子
	
	sem_init(&sem.empty, 0, 10);     //信号量初始化（做多容纳10条消息，容纳了10条生产者将不会生产消息）
	sem_init(&sem.full, 0, 0);
	
	pthread_mutex_init(&mutex, NULL);   //互斥锁初始化
	
	InitQueue(&(sem.q));    //队列初始化
	
	pthread_t producid;	//声明线程ID
	pthread_t consumid;
	
	//创建生产者线程
	if(pthread_create(&producid, NULL, Producer, NULL) == -1)	//返回0表示成功，返回-1表示线程创建出错
	{
        	puts("生产者线程创建失败！");
        	exit(1);
    	}		
	//创建消费者线程
	if(pthread_create(&consumid, NULL, Consumer, NULL) == -1)
	{
        	puts("消费者线程创建失败！");
        	exit(1);
	}

	//输入回车杀死线程
	puts("****按回车可结束线程****\n");
	char exit;
        while(1)
        {
                scanf("%c", &exit);
                if(exit == '\n')
                {
                        puts("正在杀死线程：");
                        pthread_kill(producid, 9);
                        pthread_kill(consumid, 9);
                }
	}       
	

	pthread_join(consumid, NULL);     //线程等待，如果没有这一步，主程序会直接结束，导致线程也直接退出。
	pthread_join(producid, NULL);	

	sem_destroy(&sem.empty);          //信号量的销毁
	sem_destroy(&sem.full);    
	
	pthread_mutex_destroy(&mutex);    //互斥锁的销毁
	
	return 0; 
}
