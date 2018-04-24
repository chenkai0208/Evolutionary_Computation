#include <stdio.h>  
#include <stdlib.h>  
#include <time.h>  

//this program is aimed at calculating the maximum of the function 'f=ax^2+by^2'.
//'a' and 'b' are input by users
//developed by Chen Kai (2016.11.26)

//The definiton of user data//
#define Cmax 100   //certain maximal value
#define Cmin 0   //certain minimum value

#define LENGHT1  3  //the chromosome length of 1st variable
#define LENGHT2  3  //the chromosome length of 2nd variable

#define CHROMLENGTH LENGHT1+LENGHT2
const int PopSize = 10;  //the size of sample population
int a, b;
int MaxGeneration; //the maximum of generation could be input by users
double Pc, Pm; //the possibility of crossover and mutation could be input by users


//the defination of data structure//
struct Individual{
    char chrom[CHROMLENGTH + 1];  //chromosome for one individual
    double value; 
    double fitness; 
};

int generation; //the pointer of generation  
int bestIndex;  
int worstIndex;  

struct Individual bestIndividual ;  
struct Individual worstIndividual ; 
struct Individual currentBest ;// current best individual
struct Individual  population [PopSize]  ;//chromosome sample


//the declaration of functions//
void generateInitialPopulation();
void generateNextPopulation();
void evalutePopulation();
long decodeChromosome(char*, int, int);
void calculateObjectValue();
void calculateFitnessValue();
void findBestAndWorstIndividual();
void performEvolution();
void selectionOperator();
void crossoverOperator();
void mutationOperator();
void outputTextReport();
/////////////////////////////////

int main(){
    generation = 0;
    printf("Please input the values for 'a', and 'b' in the equation: 'f=ax^2+by^2'.\n");
    scanf("%d%d",&a,&b);
    printf("Please input the possibility of crossover\n");
    scanf("%lf",&Pc);
    printf("Please input the possibility of mutation\n");
    scanf("%lf",&Pm);
    printf("Please input the maximum of generation\n");
    scanf("%d",&MaxGeneration);
    generateInitialPopulation();
    evalutePopulation();
    while (generation < MaxGeneration) {
        generation++;
        generateNextPopulation();
        evalutePopulation();
        performEvolution();
        outputTextReport();
    }
    return 0;
}


//////////////////////////////////

//Initialization//
void generateInitialPopulation() {
    int i, j;
    srand((unsigned)time(NULL));
    for (i = 0; i < PopSize; i++) {
        for (j = 0; j < CHROMLENGTH; j++) {
            population[i].chrom[j] = ((rand() % 10) < 5) ? '0' : '1';
        }
        population[i].chrom[CHROMLENGTH] = '\0';
    }
    
}


//Genetic Operators//
void generateNextPopulation() {
    selectionOperator();
    crossoverOperator();
    mutationOperator();
}

//Mutation Operator//
void mutationOperator() {
    int i, j;
    double p;
    // bit mutation
    for (i = 0; i < PopSize; i++) {
        for (j = 0; j < CHROMLENGTH; j++) {
            p = rand() % 1000 / 1000.0;
            if (p < Pm) {
                population[i].chrom[j] = (population[i].chrom[j] == '0') ? '1': '0';
            }
        }
    }
    
}

//Crossover Operator///
void crossoverOperator() {
    int i, j;
    int index[PopSize];
    int point, temp;
    double p;
    char ch;
    for (i = 0; i < PopSize; i++) {
        index[i] = i;
    }
    for (i = 0; i < PopSize; i++) {
        
        point = rand() %(PopSize - i);
        temp = index[i];
        index[i] = index[point + i];
        index[point + i] = temp;
    }
    for (i = 0; i < PopSize - 1; i+=2) {
        p = rand() % 1000 / 1000.0;
        if (p < Pc) {
            point = rand()% (CHROMLENGTH - 1) + 1;
            for (j = point; j < CHROMLENGTH; j++) {
                ch = population[index[i]].chrom[j];
                population[index[i]].chrom[j] = population[index[i + 1]].chrom[j];
                population[index[i + 1]].chrom[j] = ch;
            }
        }
    }
}

//Selection Operator//
void selectionOperator() {
    int i, index;
    double p, sum = 0.0;
    double cfitness[PopSize];
    struct Individual newpopulation[PopSize];
    for (i = 0; i < PopSize; i++) {
        sum += population[i].fitness;
    }
    for (i = 0; i < PopSize; i++) {
        cfitness[i] = population[i].fitness / sum;
    }
    // calculate cumulative fitness
    for (i = 1; i < PopSize; i++) {
        cfitness[i] = cfitness[i] + cfitness[i - 1];
    }
    for (i = 0; i < PopSize; i++) {
        p = rand() % 1000 / 1000.0;
        index = 0;
        while (p > cfitness[index]) {
            index++;
        }
        newpopulation[i] = population[index];
    }
    for (i = 0; i < PopSize; i++) {
        population[i] = newpopulation[i];
    }
}


//Evaluation//
void evalutePopulation() {
    calculateObjectValue();
    calculateFitnessValue();
    findBestAndWorstIndividual();
}



//find the best and the worst individual//
void findBestAndWorstIndividual() {
    
    int i;
    double sum = 0.0;
    bestIndividual = population[0];
    worstIndividual = population[0];
    for (i = 0; i < PopSize; i++) {
        if (population[i].fitness > bestIndividual.fitness) {
            bestIndividual = population[i];
            bestIndex = i;
        } else if (population[i].fitness < worstIndividual.fitness) {
            worstIndividual = population[i];
            worstIndex = i;
        }
        sum += population[i].fitness;
    }
    if (generation == 0) {
        currentBest = bestIndividual;
    } else {
        if (bestIndividual.fitness > currentBest.fitness) {
            currentBest = bestIndividual;
        }
    }
    
}

//fitness calculation//
void calculateFitnessValue() {
    int i;
    long temp1, temp2;
    double x1, x2;
    for (i = 0; i < PopSize; i++) {
        temp1 = decodeChromosome(population[i].chrom, 0, LENGHT1);
        temp2 = decodeChromosome(population[i].chrom, LENGHT1, LENGHT2);
        x1 = temp1 * temp1;
        x2 = temp2 * temp2;
        population[i].fitness = a * x1 + b * x2;
    }
    
}

//Objective Value Calculation
//the objective function is f=ax^2+by^2
void calculateObjectValue() {
    int i;
    long temp1, temp2;
    double x1, x2;
    for (i = 0; i < PopSize; i++) {
        temp1 = decodeChromosome(population[i].chrom, 0, LENGHT1);
        temp2 = decodeChromosome(population[i].chrom, LENGHT1, LENGHT2);
        x1 = temp1 * temp1;
        x2 = temp2 * temp2;
        population[i].value = a * x1 + b * x2;
    }
    
}

//decode chromosome into decimal format
long decodeChromosome(char* string, int point, int length) {
    int i;
    long decimal = 0L;
    char * pointer;
    for(i = 0, pointer=string+point; i < length;i++,pointer++){
        decimal += (*pointer - '0') << (length - 1 - i);
    }
    return decimal;
}


//Execute Evolution//
void performEvolution() {
    if (bestIndividual.fitness > currentBest.fitness) {
        currentBest = population[bestIndex];
    } else {
        population[worstIndex] = currentBest;
    }
}


//Output the results//
void outputTextReport() {
    int i;
    double sum;
    double average;
    
    sum = 0.0;
    for (i = 0; i < PopSize; i++) {
        sum += population[i].value;
    }
    average = sum / PopSize;
    printf("gen=%d, avg=%f, best=%f",generation, average,currentBest.value);
    printf(" chromosome=");
    for(  i = 0; i < CHROMLENGTH; i++){
        printf("%c", currentBest.chrom[i]);
    }
    printf("\n");
    
}