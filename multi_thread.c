/**
 * @file multi_thread.c
 * @author Tushar Mahat (A00429666)
 * @author Nabin Bhandari (A00430201)
 * @brief This is a multi-threaded verion of Floyd-Warshall All Pair
 * Shortest Path Algorithm
 *
 * @version 0.1
 * @date 2022-03-31
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>

#define INF 1000000 // large infinite value

int **graph; // adjacency matrix
int **dist;  // distance matrix

// structures to hold argument values
struct arg_s
{
    int n;
    int i;
    int k;
};

// structure for start and end time
struct timeval start, end;
// variable to hold total time taken for calculation
double totalTime = 0.0;

int readCount = 0;             // the readCount
pthread_mutex_t readCountLock; // the readCount mutex lock
sem_t dbLock;                  // semaphore for dbLock

/**
 * @brief Get the Node Edge Number object
 *
 * @param n, n is the number of nodes
 * @param m, m is the number of edges
 */
void getNodeEdgeNumber(int *n, int *m)
{
    // get # of nodes and edges from user
    printf("Enter # of Nodes and Edges:\n");
    // nodes
    scanf("%d", *&n);

    /*
    display error message if a negative node is entered and ask for the
    input again
    */
    while (*n < 0)
    {
        printf("Node count must be positive\nEnter again:\n");
        scanf("%i", *&n);
    }
    // edges
    scanf("%d", *&m);

    /*
    display error message if a negative edges is entered and ask for the
    input again
    */
    while (*m < 0)
    {
        printf("Edge count must be positive\nEnter again:\n");
        scanf("%i", *&m);
    }
}

/**
 * @brief Allocate memory for adjacent and distance matrix based on user
 * inputs(#nodes,#edges)
 *
 * @param args
 * @param n
 * @param m
 */
void initializeMatrix(struct arg_s *args, int n, int m)
{

    // dynamic memory allocation for 2D array
    graph = (int **)malloc(n * sizeof(int *));
    dist = (int **)malloc(n * sizeof(int *));

    // initialize adjacent and dist matrix
    for (int i = 0; i < n; i++)
    {
        args[i].n = n;
        graph[i] = (int *)malloc(n * sizeof(int *));
        dist[i] = (int *)malloc(n * sizeof(int *));
        for (int j = 0; j < n; j++)
        {
            if (i == j)
            // make all diagonal elements zero
            {
                graph[i][j] = 0;
                dist[i][j] = 0;
            }
            else
            /* fill zero in adjacent matrix and INF in distance matrix in
            non-diagonal elements */
            {
                graph[i][j] = 0;
                dist[i][j] = INF;
            }
        }
    }
}
/**
 * @brief Get u,v,w from the user and fill the matrix
 *
 * @param m, is the number of edges
 */
void fillMatrix(int n, int m)
{
    // input u(node 1),v(node 2) and w(weight)
    for (int i = 0; i < m; i++)
    {
        int u, v, w;

        // get input u(node 1),v(node 2) and w(weight)
        scanf("%i", &u);
        scanf("%i", &v);
        scanf("%i", &w);
        /*
        display error message if a invalid node or negative weight is entered and ask for the input again
       */
        while (u > n || v > n || w < 0)
        {
            if (u > n || v > n)
            {
                printf("Invalid node\nPlease enter three values again:\n");
            }
            else
            {
                printf("Edge weight cannot be negative\nPlease enter three values again:\n");
            }
            scanf("%i", &u);
            scanf("%i", &v);
            scanf("%i", &w);
        }

        // set value to undirect graph
        graph[u - 1][v - 1] = 1;
        graph[v - 1][u - 1] = 1;
        dist[u - 1][v - 1] = w;
        dist[v - 1][u - 1] = w;
    }
}
/**
 * @brief Calculate the total time elapsed to run threads
 *
 */
void calcTimeElapsed()
{
    double elapsed = (1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)) * 1e-6;

    // total time calculaed by adding time taken by each thread.
    totalTime += elapsed;
}

/**
 * @brief Displays the result matrix
 *
 * @param args
 */
void displayMatrix(struct arg_s *args)
{
    int n = args[0].n;

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (dist[i][j] == INF)
                printf("INF ");
            else
                printf("%i ", dist[i][j]);
        }
        printf("\n");
    }
}

/**
 * @brief Worker function
 *
 * @param args
 * @return void*
 */
void *floydWarshall(void *args)
{
    struct arg_s *arguments = (struct arg_s *)args;

    // get n,i,k from args
    int n = arguments->n;
    int i = arguments->i;
    int k = arguments->k;

    for (int j = 0; j < n; j++)
    {
        // acquire read lock
        pthread_mutex_lock(&readCountLock);
        readCount++;
        if (readCount == 1)
        {
            sem_wait(&dbLock);
        }
        pthread_mutex_unlock(&readCountLock);

        if ((dist[i][k] + dist[k][j]) < dist[i][j])
        {
            // release readCount lock
            pthread_mutex_lock(&readCountLock);
            readCount--;
            if (readCount == 0)
            {
                sem_post(&dbLock);
            }
            pthread_mutex_unlock(&readCountLock);

            // acquire write lock
            sem_wait(&dbLock);
            dist[i][j] = dist[i][k] + dist[k][j];
            // release write lock
            sem_post(&dbLock);
        }
        else
        {
            // release readCount lock
            pthread_mutex_lock(&readCountLock);
            readCount--;
            if (readCount == 0)
            {
                sem_post(&dbLock);
            }
            pthread_mutex_unlock(&readCountLock);
        }
    }
    pthread_exit(NULL);
}

/**
 * @brief Run Floyd-Warsall Algorithm in the matrix
 *
 * @param n
 * @param args
 * @param threads
 */
void FloydWarshallAlg(int n, struct arg_s *args, pthread_t *threads)
{
    struct arg_s *arguments = (struct arg_s *)args;
    //-------------Floyd-Warshall (FW) All-Pairs-Shortest-Path algorithm--------------//
    for (int k = 0; k < n; k++)
    {
        gettimeofday(&start, 0); // get thread start time
        // create threads
        for (int i = 0; i < n; i++)
        {
            // set up arguement to be passed to, n,k,i
            arguments[i].n = n;
            arguments[i].k = k;
            arguments[i].i = i;

            // create threads -> pthread_create
            pthread_create(threads + i, NULL, (void *)floydWarshall, (void *)&(arguments[i]));
        }

        for (int i = 0; i < n; i++)
        {
            // join threads -> pthread_join
            pthread_join(*(threads + i), NULL);
        }

        gettimeofday(&end, 0); // get thread end time

        calcTimeElapsed(); // calculate total elapsed time
    }
    printf("Multi Threading took %f seconds.\n", totalTime);
}

/**
 * @brief main method of the program
 *
 * @return int
 */
int main()
{
    struct arg_s *arguments;

    int n; //# of nodes
    int m; //# of edges

    // get node and edge number
    getNodeEdgeNumber(&n, &m);

    // allocate memory for argument
    arguments = (struct arg_s *)malloc(n * sizeof(struct arg_s));

    // allocate memory for 2D matrices and fill them using inputs
    initializeMatrix(arguments, n, m);

    // get inputs of start node , end node and weight of the edge
    fillMatrix(n, m);

    // initialize lock
    pthread_mutex_init(&readCountLock, NULL);

    // initialize semaphore
    sem_init(&dbLock, 0, 1);

    int numOfVertex = arguments[0].n; // number of vertex

    // allocate memory for threads
    pthread_t *threads = (pthread_t *)malloc(numOfVertex * sizeof(pthread_t *));

    // run Floyd-Warshall Algorithm
    FloydWarshallAlg(n, arguments, threads);

    // display result
    displayMatrix(arguments);

    // destroy lock
    sem_destroy(&dbLock);
    pthread_mutex_destroy(&readCountLock);

    return 0;
}
