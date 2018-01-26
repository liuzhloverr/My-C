#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int *pCodeList;     //代码列表
int *pTimeList;     //时间列表
float *pPriceList;  //成交价
long int *pAmount;    //成交额
int *pVolume;       //成交量

float * openPrice;
float * closePrice;
float * highPrice;
float * lowPrice;
float * amountPrice;
int * volumePrice;
float * MA;
int * time;


int dwcount;

FILE * pFile;

FILE * pFileOpenPrice;
FILE * pFileClosePrice;
FILE * pFileHighPrice;
FILE * pFileLowPrice;
FILE * pFileAmountPrice;
FILE * pFileVolumePrice;
FILE * pFileMA;
FILE * pfileTime;


int dwSize;

int ddwPositionOfStart;//记录每一支股票的起始位置
int ddwPositionOfEnd;  //记录每一支股票的起始位置
int ddwPositionOfStartTime;//根据起止时间来确定的范围
int ddwPositionOfEndTime;//根据起止时间来确定的范围
int ddwFinUpperBound;
int ddwFinLowerBound;


int funcGetLine();
bool funcputIntToMem();		//读code文件到内存
bool funcputPriceToMem();    //读Price文件到内存
bool funcputAmountToMem();    //读Amount文件到内存

//最终确认的范围是根据code确定的范围与根据Time确定的范围取交集

void funcMapFromCode();       //根据code来映射出一个范围的函数
void funcMapFromTime();       //根据起始终止时间来映射一个范围
void funcMapFin();            //根据code与起止时间确定的范围
void funcTiemLine();          //生成时间线


int main(int argc,char *argv[])  //argv[]参数列表(时间间隔，起点时间，终点时间，code)
     //arg[1]是时间间隔以秒为单位，起始终止时间是以天为单位例如20100101，code是六位数字代码
{   //argv[1] 是code argv[2]是起始时间，argv[3]是终止时间，argv[4]是时间间隔
	//先把文件读到内存中
	int dwIputCode;
	int dwIputStartTime;
	int dwIputEndTime;
	int dwIputSecond;
	int dwIputMA;//为移动平均值传入参数


	//计算移动平均值的相关的变量
	int ii;
	int jj;
	float dwsum;
	float dwAve;  //平均值

	int ddwNumOfRec; //记录数目

	pFile = fopen("/Users/liuzh/Desktop/reptile/output/data/binary/timeLine.txt", "ab+");

	pFileOpenPrice = fopen("/Users/liuzh/Desktop/reptile/output/data/binary/openPrice.txt", "ab+");
	pFileClosePrice = fopen("/Users/liuzh/Desktop/reptile/output/data/binary/closePrice.txt", "ab+");
	pFileHighPrice = fopen("/Users/liuzh/Desktop/reptile/output/data/binary/highPrice.txt", "ab+");
	pFileLowPrice = fopen("/Users/liuzh/Desktop/reptile/output/data/binary/lowPrice.txt", "ab+");
	pFileAmountPrice = fopen("/Users/liuzh/Desktop/reptile/output/data/binary/amountPrice.txt", "ab+");
	pFileVolumePrice = fopen("/Users/liuzh/Desktop/reptile/output/data/binary/volumePrice.txt", "ab+");
	pFileMA = fopen("/Users/liuzh/Desktop/reptile/output/data/binary/MA.txt", "ab+");
	pfileTime = fopen("/Users/liuzh/Desktop/reptile/output/data/binary/Time.txt", "ab+");


	ddwNumOfRec = funcGetLine("/Users/liuzh/Desktop/reptile/output/data/codelist.txt"); //先获取文件的包含的记录数目
	
	pCodeList = (int *) malloc (sizeof(int) * ddwNumOfRec);                               //为codelist申请内存
	pTimeList = (int *) malloc (sizeof(int) * ddwNumOfRec);
	pPriceList = (float *) malloc (sizeof(float) * ddwNumOfRec);
	pAmount = (long int *) malloc (sizeof(long int) * ddwNumOfRec);
	pVolume = (int *) malloc (sizeof(int) * ddwNumOfRec);


	openPrice = (float * )malloc(sizeof(float ) * ddwNumOfRec);
	closePrice = (float * )malloc(sizeof(float ) *ddwNumOfRec);
	highPrice = (float * )malloc(sizeof(float)* ddwNumOfRec);
	lowPrice = (float * )malloc(sizeof(float)* ddwNumOfRec);
	amountPrice = (float * )malloc(sizeof(float)* ddwNumOfRec);
	volumePrice = (int * )malloc(sizeof(int)* ddwNumOfRec);
	MA = (float * )malloc(sizeof(float)* ddwNumOfRec);
	time = (int * )malloc(sizeof(int)* ddwNumOfRec);

	dwcount = 0; //初始化计数器
//把列式文件写入内存

	funcputIntToMem("/Users/liuzh/Desktop/reptile/output/data/codelist.txt",pCodeList);//传文被读文件的地址
	funcputIntToMem("/Users/liuzh/Desktop/reptile/output/data/timelist.txt",pTimeList);
	funcputPriceToMem("/Users/liuzh/Desktop/reptile/output/data/pricelist.txt",pPriceList);
	funcputAmountToMem("/Users/liuzh/Desktop/reptile/output/data/sumprice.txt",pAmount);
	funcputIntToMem("/Users/liuzh/Desktop/reptile/output/data/numlist.txt",pVolume);

//根据输入的参数code来确定一个范围，然后在这个确定的范围内再根据起点终点时间确定更小的范围
	dwIputCode = atoi(argv[1]);
	dwIputStartTime = atoi(argv[2]);
	dwIputEndTime = atoi(argv[3]);
	dwIputSecond = atoi(argv[4]);
	dwIputMA = atoi(argv[5]);    //移动平均值参数
	dwSize = (dwIputEndTime - dwIputStartTime) / dwIputSecond;

	funcMapFromCode(pCodeList,dwIputCode);
	funcMapFromTime(pTimeList,dwIputStartTime,dwIputEndTime);
	funcMapFin();      //获取最终的范围
	funcTiemLine(dwIputSecond);


//在此之前生成的时间线信息已经写到内存中了，计算移动平均数可以直接使用之前写在内存中的数据

	ii = 0;

	while(ii < dwcount - dwIputMA +1)
	{
		jj = 0;

		dwsum = 0;

		while (jj < dwIputMA)
		{	
			dwsum = dwsum + closePrice[ii + jj];

			jj++;
		}
		dwAve = dwsum / dwIputMA;

		MA[ii] = dwAve;

		ii++;
		
		//printf(" %s %f \n","移动平均值",dwAve );
		
		//printf("%f\n",MA[ii-1] );
	}

//二进制写文件
	fwrite(openPrice, sizeof(float) ,  dwcount  ,pFileOpenPrice);
	fwrite(closePrice, sizeof(float) ,  dwcount ,pFileClosePrice);
	fwrite(highPrice, sizeof(float) ,  dwcount ,pFileHighPrice);
	fwrite(lowPrice, sizeof(float) ,  dwcount ,pFileLowPrice);
	fwrite(amountPrice, sizeof(float) ,  dwcount ,pFileAmountPrice);
	fwrite(volumePrice, sizeof(int) ,  dwcount ,pFileVolumePrice);
	fwrite(MA, sizeof(float) ,  dwcount - dwIputMA + 1  ,pFileMA);
	fwrite(time, sizeof(float) ,  dwcount - dwIputMA + 1  ,pfileTime);


  fclose(pFileOpenPrice);
  fclose(pFileClosePrice);
  fclose(pFileHighPrice);
  fclose(pFileLowPrice);
  fclose(pFileAmountPrice);
  fclose(pFileVolumePrice);
  fclose(pFileMA);
//释放资源
	free(pCodeList);
	free(pTimeList);
	free(pPriceList);
	free(pAmount);
	free(pVolume);

	return 0;
}

void funcTiemLine(int dwSeconds)    //传入参数的单位为秒
{
	//ddwFinUpperBound  ddwFinLowerBound   合法的区间范围，在词范围内按时间分片
	int dwStartTime;
	int dwEndTime;
	int ddwFinUpBndNxt;
	int ddwFinUpBndNxt1;
	int dwEnd;
	int dwAdjustStart;
	int dwAdjustEnd;

	float dwOpenPrice;//开盘价格
	float dwClosePrice;//收盘价格
	
	float dwTemp;    //找最值用到的临时变量

	dwStartTime = pTimeList[ddwFinUpperBound]; 
	dwEndTime = pTimeList[ddwFinLowerBound];//确定时间上下界

	//printf("%d\n",ddwFinUpperBound );
	//printf("%d\n", dwStartTime);
	//printf("%d\n", dwEndTime);
	//printf("%d\n", ddwFinUpperBound);
  dwEnd = dwStartTime + dwSeconds;  //每一个时间分片的末尾

  

  while (dwEnd < dwEndTime)//时间累加，每累加一次做一次统计
  {    //第一个时间间隔，要在Timelist中找最后一个比dwStartTime小的值
  	float dwHighPrice;//最高价格
		float dwLowPrice;//最低价格
		float dwAmount;  //成交额
		int dwVolume;
		int dwtmp1;
		int dwtmp2;

		dwHighPrice = 0;//初始化
		dwAmount = 0;  //初始化
		dwVolume = 0;  //初始化

		dwStartTime += dwSeconds;   
		dwEnd = dwStartTime + dwSeconds;
		dwAdjustStart = dwStartTime - dwSeconds;
		dwAdjustEnd = dwEnd - dwSeconds;
    //printf("%d %d\n",dwAdjustStart, dwAdjustEnd ); //确定了时间分片的时间间隔

  	//printf("%d\n",ddwFinUpperBound);              //逻辑位置
		dwtmp1 = ddwFinUpperBound;

    dwOpenPrice = pPriceList[ddwFinUpperBound];     //开盘价
		dwLowPrice = dwOpenPrice;//初始化
    //printf("%s %f\n","开盘价",dwOpenPrice );
		//printf("%d\n",pTimeList[ddwFinUpperBound] );
		while (pTimeList[ddwFinUpperBound] < dwAdjustEnd) 
		{
    	
			if (dwHighPrice < pPriceList[ddwFinUpperBound])
			{
				dwHighPrice = pPriceList[ddwFinUpperBound];
			}
			if (dwLowPrice > pPriceList[ddwFinUpperBound])
			{
				dwLowPrice = pPriceList[ddwFinUpperBound];
			}
			dwAmount = dwAmount + pAmount[ddwFinUpperBound];

			dwVolume = dwVolume + pVolume[ddwFinUpperBound];

			ddwFinUpperBound += 1;  

		} 
			//printf("%d\n", ddwFinUpperBound - 1);
		dwtmp2 = ddwFinUpperBound;

		if (dwtmp1 != dwtmp2)
		{
			// printf("%s %f\n","开盘价",dwOpenPrice );	
			openPrice[dwcount] = dwOpenPrice;
	    // printf("%f\n",openPrice[dwcount]);

			// printf("%s %d\n","成交量",dwVolume );
			volumePrice[dwcount] = dwVolume;
			// printf("%d\n",volumePrice[dwcount]);
			
			// printf("%s %f\n","成交额",dwAmount );
			amountPrice[dwcount] = dwAmount;
			// printf("%f\n",amountPrice[dwcount]);

			// printf("%s %f\n","最高价",dwHighPrice );
			highPrice[dwcount] = dwHighPrice;
			// printf("%f\n",highPrice[dwcount]);

			// printf("%s %f\n","最低价",dwLowPrice );
			lowPrice[dwcount] = dwLowPrice;
			// printf("%f\n",lowPrice[dwcount]);

		    //printf("%d\n", ddwFinUpperBound - 1);           //逻辑位置
	    dwClosePrice = pPriceList[ddwFinUpperBound-1];   //收盘价

	    // printf("%s %f\n","收盘价",dwClosePrice);
	    closePrice[dwcount] = dwClosePrice;
	    // printf("%f\n",closePrice[dwcount]);

	    // printf("%s\n","===================" );
	    //	向文件中写时间线信息
	    fseek(pFile, 0, SEEK_END);

	    time[dwcount] = dwAdjustStart;

	    fprintf (pFile, "%d\t %f\t %f\t %f\t %f\t %f\t %d\n",dwAdjustStart,dwOpenPrice,dwClosePrice,dwHighPrice,dwLowPrice,dwAmount,dwVolume);

	    dwcount = dwcount + 1;

	    //fseek(pFileOpenPrice, 0, SEEK_END);
		  }

    //while(pTimeList[ii] < dwStartTime)
    //{
    	//printf("%d\n",pTimeList[ii] );
    	//if(pPriceList[ii]>pPriceList[ii])
    	//{
    	// 	dwHighPrice = pPriceList[ii+1];
    	//}
    	//if(pPriceList[ii+1]<pPriceList[ii])
    	//{
    	// 	dwLowPrice = pPriceList[ii+1];
    	//}
    //}
		}

		
	}
void funcMapFin()               //取funcMapFromCode与funcMapFromTime的交集
{	
	if (ddwPositionOfStart <= ddwPositionOfStartTime)
	{
		ddwFinUpperBound = ddwPositionOfStartTime;
	}
	else
	{
		ddwFinUpperBound = ddwPositionOfStart;
	} 

	if (ddwPositionOfEnd < ddwPositionOfEndTime)
	{
		ddwFinLowerBound = ddwPositionOfEnd;
	}
	else
	{
		ddwFinLowerBound = ddwPositionOfEndTime;
	}

	if (ddwFinUpperBound < ddwFinLowerBound)
	{
  	//printf("%d %d\n",ddwFinUpperBound,ddwFinLowerBound);  //根据code与起止时间确定的范围
	}
	else
	{
		printf("%s\n","非法的范围！" );
	}
}

void funcMapFromCode(int * ptr ,int code)//参数是六位的股票代码  
{
	int ddwCount;
	
	ddwCount = 0;

	//printf("%s %d\n","检查点",code );

	while (ptr[ddwCount] )
	{	
		if (ptr[ddwCount] == code)
		{
			ddwPositionOfStart = ddwCount + 1;
			
			break;
		}
		ddwCount ++;
	}
	
	while (ptr[ddwCount])
	{ 
		ddwCount ++;

		if(ptr[ddwCount] != code)
		{
			ddwPositionOfEnd = ddwCount;
			break;
		}
	}
	ddwPositionOfStart = ddwPositionOfStart -1;  //数组下标是从零开始

	ddwPositionOfEnd = ddwPositionOfEnd - 1;     //这里向前偏移一位
	//printf("%d  %d\n",ddwPositionOfStart, ddwPositionOfEnd );
}

void funcMapFromTime(int *ptr ,int startTime,int endTime) //输入参数是起止时间与文件地址
{
	int ddwCount;
	
	if (startTime > endTime)
	{
		printf("%s\n","非法的时间范围 ！" );	
	}
	ddwCount = 0;

	while (ptr[ddwCount] )
	{	
		if (ptr[ddwCount] == startTime)
		{
			ddwPositionOfStartTime = ddwCount + 1;
			
			break;
		}
		ddwCount ++;
	}
	
	while (ptr[ddwCount] )
	{ 
		ddwCount ++;

		if(ptr[ddwCount] == endTime)
		{
			ddwPositionOfEndTime = ddwCount;  //到结束时间但是不包括结束时间
			break;
		}
	}
	ddwPositionOfStartTime = ddwPositionOfStartTime -1;

	ddwPositionOfEndTime = ddwPositionOfEndTime -1;

//printf("%d  %d\n",ddwPositionOfStartTime, ddwPositionOfEndTime );
}

//把code文件读到内存中的函数           
bool funcputIntToMem (char* filePath, int * ptr)
{
	int ddwCount;

	char tmpString[20];   //一个

	ddwCount = 0;

  FILE *fp = fopen(filePath,"r"); //打开文件    

  if (!fp)
  {
  	printf("can't open file\n");

  	return false;
  }
  while (!feof(fp))
  {
  	if (fgets(tmpString,20,fp) != NULL)
   	//puts(tmpString);
   	//printf("%s\n",tmpString );
   	ptr[ddwCount] = atoi(tmpString);  //把code写入int数组中
   	//printf("%d\n",ptr[ddwCount]);
   ddwCount ++;    
 }
 fclose(fp);

 return true;
}

//向内存中写单精度浮点数
bool funcputPriceToMem (char* filePath, float * ptr)   //读Price文件到内存
{
	int ddwCount;

	char tmpString[20];   //一个

	ddwCount = 0;

  FILE *fp = fopen(filePath,"r"); //打开文件    

  if (!fp)
  {
  	printf("can't open file\n");

  	return false;
  }
  while (!feof(fp))
  {
  	if (fgets(tmpString,20,fp) != NULL)

     	ptr[ddwCount] = atof(tmpString);  //把code写入int数组中
     	//printf("%f\n",ptr[ddwCount]); 
     ddwCount ++;   
   }
   fclose(fp);

   return true;
 }

//向内存中写长整型
bool funcputAmountToMem (char* filePath, long int * ptr)   //读Price文件到内存
{
	int ddwCount;

	char tmpString[20];   //一个

	ddwCount = 0;

  FILE *fp = fopen(filePath,"r"); //打开文件  

  if (!fp)
  {
  	printf("can't open file\n");

  	return false;
  }
  while (!feof(fp))
  {
  	if (fgets(tmpString,20,fp) != NULL)
     	//puts(tmpString);
     	ptr[ddwCount] = atol(tmpString);  //把code写入int数组中
     	//printf("%ld\n",ptr[ddwCount]); 
     ddwCount ++;   
   }
   fclose(fp);

   return true;
 }


//获取文件的行数，用以申请内存空间
 int funcGetLine (char strDir[])
 {
	FILE *pf = fopen(strDir, "r"); // 打开文件

	char buf[1000];

	int lineCnt = 0;

	if (!pf) // 判断是否打开成功

		return -1;

	while (fgets(buf, 1000, pf)) // fgets循环读取，直到文件最后，才会返回NULL

		lineCnt++; // 累计行数

	fclose(pf);
	//printf("file line count = %d\n", lineCnt);
	return lineCnt;
}
