/**
 * @file graphgenerator.c
 * @author Tushar Mahat (A00429666)
 * @author Nabin Bhandari (A00430201)
 * @brief This program generate graph with edge weight
 * @version 0.1
 * @date 2022-03-31
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * @brief main method of the program
 *
 * @return int
 */
int main()
{
    // seeding for random generator
    srand(getpid());
    // number of edges
    int n;
    // get and hold the edges
    scanf("%i", &n);

    // input u(node 1),v(node 2) and w(weight)
    for (int i = 0; i < n; i++)
    {
        // start node
        int u = i + 1;
        // end node
        int v = i + 2;
        // generate weight randomly
        int w = (rand() % n) + 1;
        // for the last starting node, connect it with first node.
        if (i == n - 1)
        {
            printf("%i %i %i\n", u, 1, w);
        }
        // else connect consecutive nodes sequentially
        else
        {
            printf("%i %i %i\n", u, v, w);
        }
    }
}