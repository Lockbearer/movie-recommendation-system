#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "graph.h"
#include "recommendation.h"

//Function to display algorithm choices
void DisplayMenu() {
    printf("Choose a recommendation algorithm:\n");
    printf("1. Random Recommendation\n");
    printf("2. Top Rated Recommendation\n");
    printf("3. Similar User Recommendation\n");
    printf("4. Weighted Distance Recommendation\n");
    printf("5. Discovery Recommendation\n");
    printf("Enter your choice: ");
}

//Function to load data from file
void LoadGraphFromFile(Graph *graph, const char *filename){
    FILE *file = fopen(filename, "r");
    if(file == NULL){
        perror("Error opening file");
        return;
    }

    int userID, itemID, rating;
    while(fscanf(file, "%d %d %d", &userID, &itemID, &rating) == 3){
        AddEdge(graph, userID, itemID, rating);
    }
    fclose(file);
}

int main(){
    Graph *graph = CreateGraph();
    //Load graph from "interactions.txt"
    LoadGraphFromFile(graph, "interactions.txt");

    int userID, algorithmChoice, maxRecommendations;
    printf("Enter user ID for recommendations: ");
    scanf("%d", &userID);

    printf("Enter maximum number of recommendations: ");
    scanf("%d", &maxRecommendations);

    DisplayMenu();
    scanf("%d", &algorithmChoice);

    int size=0;
    int *recommendedItems = NULL;

    clock_t start, end;
    double cpu_time_used;

    start = clock(); //Record start time

    switch(algorithmChoice){
        case 1:
            recommendedItems = RandomRecommendation(graph, userID, maxRecommendations, &size);
            break;
        case 2:
            recommendedItems = TopRatedRecommendation(graph, userID, maxRecommendations, &size);
            break;
        case 3:
            recommendedItems = SimilarUserRecommendation(graph, userID, maxRecommendations, &size);
            break;
        case 4:
            recommendedItems = WeightedDistanceRecommendation(graph, userID, maxRecommendations, &size);
            break;
        case 5:
            recommendedItems = DiscoveryRecommendation(graph, userID, maxRecommendations, &size);
            break;
        default:
            printf("Invalid choice.\n");
            FreeGraph(graph);
            return 1;
    }

    //End timing
    end = clock();

    //Calculate cpu time used
    cpu_time_used = ((double)(end-start)) / CLOCKS_PER_SEC;

    if(recommendedItems != NULL){
        printf("Recommendations for user %d:\n", userID);
        for(int i = 0; i < size; i++){
            printf("  Item ID: %d\n", recommendedItems[i]);
        }

        //Print execution time
        printf("Execution time: %f seconds\n",cpu_time_used);
        
        free(recommendedItems);
    }

    FreeGraph(graph);
    return 0;
}