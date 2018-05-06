#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

//this program is aimed at solving the TSP (Traveling Salesman Problem) by Ant Colony Optimization
//the locations of cities are given in a txt file
//the best tour and the shortest length can be calculated by this program
//developed by Chen Kai (2016.12.20)

#define CITY_COUNT 51 //the number of cities
#define AntCount 30   //the number of artificial ants
#define NCMAX 300 
#define alfa 1.0   //the parameter to control the influence of pheromone
#define beta 2.0   //the parameter to control the influence of desirability of state transition
#define rou 0.5    //pheromone evaporation coefficient
#define Q 100.0    //the total amount of pheromone
double distance[CITY_COUNT][CITY_COUNT];  //distance matrix of cities
double tao[CITY_COUNT][CITY_COUNT];   //pheromone matrix
int bestTour[CITY_COUNT]; 

//functions for randomly selecting
int rnd(int nLow,int nUpper)
{
	 return nLow+(nUpper-nLow)*rand()/(RAND_MAX+1);
}
double rnd_d(double Low,double Upper)
{
	double Temp=rand()/((double)RAND_MAX+1.0);
    return Low+Temp*(Upper-Low);
}

//the defination of data structure//
typedef struct
{
	double x;
	double y;
}Location;
typedef struct
{
	Location *city;
}City;
typedef struct 
{
	int Path[CITY_COUNT];
	double PathLength;
	int AllowedCity[CITY_COUNT];
	int CurCityN;
	int MovedCityCount;
}Ant;
typedef struct
{
	Ant * Ants;
	double   bestLength;
}TSP;

//the declaration of functions//
void Init_City();
void InitList_TSP();
void Init_S();
void CreateAnts();
void GetLoc();
void GetDistance();
void updatetrial();
void startsearch();
////////////////////////////////

///////////////////////////////
int main()
{
	int i,j;
	TSP tsp;
	City Ci;
	
	Init_S();
	Init_City(&Ci);
	InitList_TSP(&tsp);
	CreateAnts(&tsp);
	GetLoc(&Ci);
	GetDistance(&Ci);

	startsearch(&tsp);
	printf("The shortest length:%.2f\n",tsp.bestLength);
	printf("The best tour:");
	for(i=0;i<CITY_COUNT-1;i++)
	{
		if (i % 20 == 0)
        {
            printf("\n\n");
        }
		printf("%d->",bestTour[i]+1);
    }
    printf("%d",bestTour[CITY_COUNT-1]+1);    
    printf("\n");
	return 0;
}
/////////////////////////////////


//the initialization of the problem//
void Init_City(City *Ci)
{
	Ci->city=(Location*)malloc(CITY_COUNT*sizeof(Location));
	if(!Ci->city)  exit(OVERFLOW);
}
void InitList_TSP(TSP *tsp)                         
{
	tsp->Ants=(Ant*)malloc(AntCount*sizeof(Ant));
	if(!tsp->Ants) exit(OVERFLOW);                     
	tsp->bestLength=1000000000;    //set the intial tour length
}
void Init_S()
{
	int i,j;
	for(i=0;i<CITY_COUNT;i++)
	    for(j=0;j<CITY_COUNT;j++)
	    {
	    	tao[i][j]=1.0;      //set 1 as initial tao value 
	    }
}
void CreateAnts(TSP *tsp)
{

    int i,j;
    for(i=0;i<AntCount;i++)
    {
    	for(j=0;j<CITY_COUNT;j++)
    	{
    	    tsp->Ants[i].AllowedCity[j]=1;
    	    tsp->Ants[i].Path[j]=0;
    	}
        tsp->Ants[i].PathLength=0.0;
        tsp->Ants[i].CurCityN=rnd(0,CITY_COUNT);   //randomly select a city 
        tsp->Ants[i].Path[0]=tsp->Ants[i].CurCityN;
        tsp->Ants[i].AllowedCity[tsp->Ants[i].CurCityN]=0;
        tsp->Ants[i].MovedCityCount=1;
    }
}

//extract the locations of cities from given txt file//
void GetLoc(City *Ci)
{
	int i,number;
	FILE *fp;
	
	fp=fopen("/Users/chenkai/Downloads/eil51.txt","r");
	if(fp==NULL)
	{
		printf("Unable to open the file.\n");
		exit(0);
	}
	for(i=0;i<CITY_COUNT;i++)
	{
		fscanf(fp,"%d%lf%lf",&number,&Ci->city[i].x,&Ci->city[i].y);
    }
	fclose(fp);
}

//get the distance matrix among cities//
void GetDistance(City *Ci)
{
	int i,j;
	double temp=0.0;
	for(i=0;i<CITY_COUNT;i++)
	{
	    for(j=0;j<CITY_COUNT;j++)
	    {
	    	temp=(Ci->city[i].x-Ci->city[j].x)*(Ci->city[i].x-Ci->city[j].x)+(Ci->city[i].y-Ci->city[j].y)*(Ci->city[i].y-Ci->city[j].y);
	    	temp=pow(temp,0.5);
	    	distance[i][j]=temp;
	    }
        distance[i][i]=0.0000000001;
	} 
}

//update tao value//
void updatetrial(TSP *tsp)
{
	double Temptao[CITY_COUNT][CITY_COUNT];
	int m=0;
	int n=0,i,j;
	memset(Temptao,0,sizeof(Temptao));
	
	
	for(i=0;i<AntCount;i++)
	{
		for(j=1;j<CITY_COUNT;j++)
		{
			m=tsp->Ants[i].Path[j];
			n=tsp->Ants[i].Path[j-1];
			Temptao[n][m]=Temptao[n][m]+Q/tsp->Ants[i].PathLength;
			Temptao[m][n]=Temptao[n][m];
		}
		
		n=tsp->Ants[i].Path[0];
		Temptao[n][m]=Temptao[n][m]+Q/tsp->Ants[i].PathLength;
		Temptao[m][n]=Temptao[n][m];
		
	}
	
	for(i=0;i<CITY_COUNT;i++)
	{
	    for(j=0;j<CITY_COUNT;j++)
	    {
	    	tao[i][j]=tao[i][j]*rou+Temptao[i][j];
	    }
    }
}

//search for the best tour//
void startsearch(TSP *tsp)
{
	int i,j,k,curcity,to,Nc=0,Se=-1; 
	double partsum,p[CITY_COUNT];
	double Temp=0.0;double temp;
	int bestAnt=0;
    srand((unsigned)time(NULL));
	while(Nc<NCMAX)
	{
		for(i=0;i<CITY_COUNT-1;i++)
		    for(j=0;j<AntCount;j++)
		    {
		    	partsum=0;
		    	curcity=tsp->Ants[j].Path[tsp->Ants[j].MovedCityCount-1];
		    	for(k=0;k<CITY_COUNT;k++)
		    	    if(tsp->Ants[j].AllowedCity[k]==1)
		    	   { 
				        p[k]=pow(tao[curcity][k],alfa)*pow(1.0/distance[curcity][k],beta);   
				        partsum=partsum+p[k];
		           }
		    	Temp=rnd_d(0.0,partsum);
		    	for(to=0;to<CITY_COUNT;to++)
		    	{
		    		if(tsp->Ants[j].AllowedCity[to]==1)
		    		{
		    		     Temp=Temp-p[to];         //roulette, the core of the code
		    		     if(Temp<0.0)
		    		      {
		    		      	Se=1;
		    		      	break;
		    		      }
		    		}
		    		
		    	}
		    	if(Se==-1)
		    	{
		    		for(i=0;i<CITY_COUNT;i++)
		    		if(tsp->Ants[j].AllowedCity[i]==1)
		    		{
		    			to=i;
		    			break;
		    		}
		    	}
		    	
		    	tsp->Ants[j].Path[tsp->Ants[j].MovedCityCount]=to;  
				tsp->Ants[j].MovedCityCount++;
				tsp->Ants[j].AllowedCity[to]=0; 
			}
		for(j=0;j<AntCount;j++)
		{
			for(i=0;i<CITY_COUNT-1;i++)  //calculate and update the distance of every path
			 tsp->Ants[j].PathLength+=distance[tsp->Ants[j].Path[i]][tsp->Ants[j].Path[i+1]];
			 tsp->Ants[j].PathLength+=distance[tsp->Ants[j].Path[CITY_COUNT-1]][tsp->Ants[j].Path[0]];
		} 
		//record the best tour
		
		temp=tsp->Ants[0].PathLength;
		for(j=1;j<AntCount;j++)
		    if(temp>tsp->Ants[j].PathLength) 
		    {
		    	temp=tsp->Ants[j].PathLength;
				bestAnt=j; 
		    }
		    if(temp<tsp->bestLength)
		    {
		    	tsp->bestLength=temp;
		    	for(i=0;i<CITY_COUNT;i++)
		    	    bestTour[i]=tsp->Ants[bestAnt].Path[i];
		    }
		    printf("[%d]:%.2f\n",Nc+1,tsp->bestLength);

		    updatetrial(tsp);//update pheromone
		    CreateAnts(tsp); //update ant data 
		    Nc++;
	}	
}