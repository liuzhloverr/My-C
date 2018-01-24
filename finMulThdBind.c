

//探测计算机的内核数，然后根据内核数创建线程，每个线程调用绑定函数使线程固定到指定的内核上

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include<pthread.h>      //多线程库的引用
#include <string.h>
#include <unistd.h>
#include <sched.h>

int dwCPUNum;            //先分配内存空间 
int dwTid;


//-----------------------------------------------------------------------------------//
//                                 线程绑定到CPU                                     //
//-----------------------------------------------------------------------------------//
void *myfun(void* p)
{
  //printf("%u\n",pthread_self());
  //printf("%d\n",p);
  int *ptr =(int*)p;      //void型指针可以强制转换成任何类型的指针，可以充当接口的作用! 
  int ii = *ptr;          //把主函数中的cpuNum传进来了
  printf("调用了线程绑定函数%d\n",ii);    //ii的传值正确

  cpu_set_t mask;          // 创建一个CPU集合

  CPU_ZERO(&mask);

  CPU_SET(ii, &mask);

  if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0)
  {
    fprintf(stderr, "set thread %d affinity failed\n",ii);
  }
  if (CPU_ISSET(ii, &mask))
  {
   // printf("thread %d is running in CPU %d\n", (int)pthread_self(),ii);
  }
  // pthread_join(pthread_self(),NULL);
  //while(1){}
  //pthread_mutex_unlock(&lockid);
}
//--------------------------------------------------------------------------------------//
//                                 创建线程的函数                                       //
//--------------------------------------------------------------------------------------//
//struct myfuncTran
//{
  //int ii
//}

void myfunc(int ii,pthread_t mainTidp)
{ 
  void *p = &ii;

  printf("调用了线程创建函数%d\n",ii);
  dwTid = pthread_create (&mainTidp, NULL, (void*)myfun, p);//创建成功返回0

  if (dwTid != 0)
  {
    printf("创建进程失败！");
  }

//printf("%d\n",pthread_self());   //这个地方明明传入的线程ID不同，但是创建线程之后进程ID一样
//需要再研究。
//pthread_join(mainTidp,NULL);

}
//---------------------------------------------------------------------------------------//
//                                       主函数                                          //
//---------------------------------------------------------------------------------------//
void main(int argc,char *argv[])
{
  //在主函数中获取CPU的核数
  dwCPUNum = sysconf(_SC_NPROCESSORS_CONF);
  pthread_t * pPids = (pthread_t *)malloc(sizeof(pthread_t)*dwCPUNum);

  printf("该机器有%d个可用的CPU\n",dwCPUNum);

  for (int ii = 0; ii < dwCPUNum; ii++)
  {
    //pthread_t mainTidp; //声明线程ID
    //mainTidp++;
    //*(pPids+ii)
    //printf("%d\n",*(pPids+ii));
    myfunc(ii,*(pPids+ii));//调用函数创建线程，传入参数
    //pthread_join(*(pPids+ii),NULL);
  }
  //for (int ii = 0; ii < dwCPUNum; ii++)
  //{
  //  pthread_join(*(pPids+ii),NULL);
  //}

   pthread_join(*(pPids+0),NULL);
   pthread_join(*(pPids+1),NULL);
   pthread_join(*(pPids+2),NULL);
   pthread_join(*(pPids+3),NULL);
  free(pPids);
}

