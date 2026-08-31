#ifndef RECOMMENDATION_H
#define RECOMMENDATION_H

#include "graph.h"

//Recommendation function prototypes
int *RandomRecommendation(Graph *graph, int userID, int maxRecommendations, int *size);
int *TopRatedRecommendation(Graph *graph, int userID, int maxRecommendations, int *size);
int *SimilarUserRecommendation(Graph *graph, int userID, int maxRecommendations, int *size);
int *WeightedDistanceRecommendation(Graph *graph, int userID, int maxRecommendations, int *size);
int *DiscoveryRecommendation(Graph *graph, int userID, int maxRecommendations, int *size); //Custom

#endif