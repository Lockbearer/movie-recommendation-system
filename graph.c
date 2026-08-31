#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

//Create an empty graph
Graph *CreateGraph(){
    Graph *graph = (Graph*)malloc(sizeof(Graph));
    graph->users = NULL;
    return graph;
}

//Add an edge between user and item
void AddEdge(Graph *graph, int userID, int itemID, int rating){
    //Find or create the user node
    UserNode *user = graph->users;
    UserNode *prev = NULL;
    while(user != NULL && user->userID != userID){
        prev = user;
        user = user->next;
    }

    if(user == NULL){
        //Create a new user node
        user = (UserNode*)malloc(sizeof(UserNode));
        user->userID = userID;
        user->edges = NULL;
        user->next = NULL;

        if(prev == NULL){
            graph->users = user;
        } else{
            prev->next = user;
        }
    }
    
    //Add the edge to the user's list
    Edge *newEdge = (Edge*)malloc(sizeof(Edge));
    newEdge->itemID = itemID;
    newEdge->rating = rating;
    newEdge->next = user->edges;
    user->edges = newEdge;
}

//Print the graph (for debugging purposes)
void PrintGraph(const Graph *graph){
    UserNode *user = graph->users;
    while(user != NULL){
        printf("User %d:\n", user->userID);
        Edge *edge = user->edges;
        while(edge != NULL){
            printf("  -> Item %d (Rating: %d)\n", edge->itemID, edge->rating);
            edge = edge->next;
        }
        user = user->next;
    }
}

//Free all memory allocated for the graph
void FreeGraph(Graph *graph){
    UserNode* user = graph->users;
    while(user != NULL){
        Edge *edge = user->edges;
        while(edge != NULL){
            Edge *temp = edge;
            edge = edge->next;
            free(temp);
        }
        UserNode *temp = user;
        user = user->next;
        free(temp);
    }
    free(graph);
}