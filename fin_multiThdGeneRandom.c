//生成随机数，可根据计算机的核数，生成等量线程，并把线程绑定到计算机内核上，实现多核并行运算

//尽量不在变量定义的时候，给变量赋值
//也不要在代码中间，定义变量，
//变量定义都放在程序头（或者函数头），程序体中不要在定义变量
//变量定义区与代码区中间加一个空行

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include<pthread.h> 
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>
cpu_set_t mask;                  //创建CPU集合 

pthread_t * pPids;       
FILE * pFile;
int * dwCount;                  //声明计数器的指针
int ref;                        //一个中间变量用来存方法的返回值
int dwSum;                      //生成记录的条目数
int dwCPUNum;

struct record                   //结构体的声明 12个字节
{
  unsigned int dwID;
  unsigned int dwTime;
  float fTemp;
  unsigned long int ddwThreadID;      //线程ID
};
struct record * recd;                              //结构体类型的指针
//=============================================================================================
struct tran                     //用作中间参数在创建线程的时候传递参数
{ 
  int dwTran;                   //通过该结构体向pthread_create()中的函数传递参数
  int dwInput; 
  // ...可以添加其他成员
};
//=============================================================================================
void *myfun(void*);
float funcRandomTemp();         //随机生成温度的函数
unsigned int funcRandomTime();
int funcRandom_fin(int ,int );

//---------------------------------------声明与赋值分离--------------------------------------//

void main(int argc,char *argv[])       
{ 
  

  dwSum = atoi(argv[1]);
  //生成一整块内存
  recd = ( struct record * ) malloc(sizeof(struct record)*dwSum); //在主函数中申请一整块内存

  struct tran pTran;                                 //pTran是结构体

  dwCount = (int *)malloc(sizeof(int) * 100);        //为计数器申请空间

  dwCPUNum = sysconf(_SC_NPROCESSORS_CONF);          //获取CPU核心数

  pPids = (pthread_t *)malloc(sizeof(pthread_t)*dwCPUNum);  

  //循环创建线程 在创建线程时调用线程绑定函数
  for (int ii = 0; ii < dwCPUNum; ii++)                    
  { //-----------------------------------------------------------------------------
    pTran.dwTran = ii;                               //给结构体内的成员赋值
    printf("初始ii值：%d\n",ii);
    pTran.dwInput = atoi(argv[1]);
    //sleep(2);
    //-----------------------------------------------------------------------------
    ref = pthread_create(&pPids[ii], NULL,  (void *)myfun, &(pTran) );//第一个参数是指向线程标识符的指针  最后传递一个结构体指针
    if (ref !=0)
    {
      perror("Thread creation failed");
    }
    sleep(2);
  }
  
  for (int ii = 0; ii < dwCPUNum; ii++)              
  {
    pthread_join(pPids[ii], NULL);
  }

  free(pPids);
  free(dwCount); //释放计数器

  //使用fwrite一次性向文件写入

  pFile = fopen( "/myProject/output/GeneRandomNum/tst.txt" , "w" );

  fwrite(recd , sizeof(struct record), dwSum, pFile );

  fclose(pFile);
  //================================================================================
  //                    这段代码是用来测试结果的，所以保留了；
  pFile = fopen("/myProject/output/GeneRandomNum/tst1.txt", "ab");//"ab+追加 二进制"
  if (!pFile)
  {
    printf("cannot open file\n");
  }

  printf("向文件写入记录：\n");
  fseek(pFile, 0, SEEK_END);
  for (int ii=0;ii<dwSum;ii++)
  {
    int dwflag;

    dwflag = fprintf (pFile, "ID %-3u time %-6u temp %2.2f  ThreadID : %u\n",
        recd[ii].dwID, recd[ii].dwTime, recd[ii].fTemp, recd[ii].ddwThreadID);
    if (!dwflag)
    {
      printf("file write error\n");
    }
  }
  //===================================================================================
  fclose(pFile);

  free(recd);

}

//创建进程时调用的函数
void *myfun(void *pTran)    //pTran用来传递参数
{  
	//在函数头定义变量
	int dwii ;                //创建进程时的大循环数

	int dwinput;              //部输入的参数

	unsigned long int threadIDself ;
	
	struct tran * ptran;               //ptran是结构体指针

	threadIDself = pthread_self(); //获取自身线程ID

  //int *ptr =(int*)p;      //void型指针可以强制转换成任何类型的指针，可以充当接口的作用! 
  
  ptran = (struct tran *) pTran;     //将void指针转化为结构体
  
  dwii= ptran->dwTran;
  
  dwinput = ptran->dwInput;

  //transf[dwii].dwId = dwii;
  //transf[dwii].id = pthread_self();

  CPU_ZERO(&mask);

  CPU_SET(dwii, &mask);              //add CPU to set

  ref = pthread_setaffinity_np(threadIDself, sizeof(mask), &mask);

  if(ref != 0)
  {
    fprintf(stderr, "set thread  0 affinity failed\n");
  }

  CPU_ISSET(dwii, &mask);

  printf("INFO : The thread-%u started ....\n", threadIDself);
  //==============================================================================================//
  funcRandom_fin(dwinput,dwii);  //调用生成函数

  // transf[dwii].dwId = dwii;
  // transf[dwii].id = threadIDself;

  //==============================================================================================//
  printf("INFO : The thread-%u ending !!!\n", threadIDself);

  

}

int funcRandom_fin(int dwinput,int dwii)
{ 
  
  int dwPerSum;
  int dwPtr;

  dwPerSum = dwinput / dwCPUNum;

  printf("开始生成记录：\n");

  srand((unsigned)time(NULL));    //以时间为种子

  dwPtr = dwii * dwPerSum;

  for (int jj = dwPtr ; jj < dwPtr + dwPerSum;jj++)               
  { 
    unsigned int dwRanID;         //声明ID变量
    float fRanTemp;               //声明温度变量

    dwRanID = (rand() % 100);     //生成随机ID
    //---------------------------------------------------------------------------------
    recd[jj].dwID = dwRanID;
    recd[jj].fTemp = funcRandomTemp();             //单独做一个随机温度的函数
    recd[jj].dwTime = funcRandomTime(dwRanID);     //生成time需要一个计数器
    recd[jj].ddwThreadID = pthread_self();
    //---------------------------------------------------------------------------------
  }

return 0;
}

float funcRandomTemp()
{
  float dwTemp,dwTemp1,dwTemp2;
  dwTemp1 = (rand()% 1000);
  dwTemp2 = dwTemp1 + 2500;
  dwTemp = dwTemp2 * 0.01;
  return dwTemp;
}

unsigned int funcRandomTime(unsigned int dwID)
{
  dwCount[dwID]+=3;
  return dwCount[dwID];
}




