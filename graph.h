#ifndef GRAPH_H
#define GRAPH_H

typedef struct Edge{
    int itemID;         //Item ID
    int rating;         //Rating by user
    struct Edge *next;  
} Edge;

typedef struct UserNode{
    int userID;         //User ID
    Edge *edges;        
    struct UserNode *next;
} UserNode;

typedef struct Graph{
    UserNode *users;
} Graph;

//Function prototypes
Graph *CreateGraph();
void AddEdge(Graph *graph, int userID, int itemID, int rating);
void PrintGraph(const Graph *graph);
void FreeGraph(Graph *graph);

#endif