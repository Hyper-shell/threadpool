#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

//	仓库容量
#define BUF_MAX 50 
//	仓库
char buf[BUF_MAX];
//	数据的数量
int cnt = 0;
//	访问仓库的互斥量
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//	仓库满的条件
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
//	仓库空的条件
pthread_cond_t null = PTHREAD_COND_INITIALIZER;

//	显示仓库
void show_buf(const char* who,const char* opt,char data)
{
	for(int i=0;i<cnt;i++)
	{
		printf("%c",buf[i]);
	}
	printf("%s[%c]%s\n",opt,data,who);
}
//	生产数据的线程
void* production(void* arg)
{
	const char* who = arg;
	for(;;)
	{
		//	加锁
		pthread_mutex_lock(&mutex);
		while(cnt >= BUF_MAX)
		{
			printf("满仓\n");
			//	睡入满仓的条件，醒来时自动加锁
			pthread_cond_wait(&full,&mutex);
		}
		//	生产数据
		char data = 'A' + rand()%26;
		//	把生产的数据放入仓库
		buf[cnt++] = data;
		//	显示仓库
		show_buf(who,"<-",data);
		//	解锁
		pthread_mutex_unlock(&mutex);
		//	叫醒空条件中的线程
		pthread_cond_signal(&null);
		//	休眠模拟耗时
		usleep(rand()%10*10000);
	}
}
//	消费数据的线程
void* consumption(void* arg)
{
	const char* who = arg;
	for(;;)
	{
		//	加锁
		pthread_mutex_lock(&mutex);
		while(0 == cnt)
		{
			printf("空仓\n");
			//	睡入空仓条件变量,并解锁
			pthread_cond_wait(&null,&mutex);
		}
		//	消费数据
		char data = buf[--cnt]; 
		//	显示仓库
		show_buf(who,"->",data);
		//	解锁
		pthread_mutex_unlock(&mutex);
		//	叫醒一个满仓
		pthread_cond_signal(&full);
		//	模拟耗时
		usleep(rand()%10*10000);
	}
}

int main(int argc,const char* argv[])
{
	srand(time(NULL));
	pthread_t tid[10];
	//	生产者
	for(int i=0;i<5;i++)
	{
		pthread_create(&tid[i],NULL,production,"生产者");
	}
	//	消费者
	for(int i=5;i<10;i++)
	{
		pthread_create(&tid[i],NULL,consumption,"消费者");
	}

	for(int i=0;i<10;i++)
	{
		pthread_join(tid[i],NULL);
	}

}
