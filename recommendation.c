#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "recommendation.h"

#define TOTALITEMS 1682

//Helper functions for recommendation algorithms
UserNode* FindUser(Graph *graph, int userID){
    UserNode *user = graph->users;
    while(user != NULL && user->userID != userID){
        user = user->next;
    }
    return user;
}

void MarkUserInteractions(int *visited, UserNode *user, Edge **firstEdge){
    *firstEdge = user->edges;
    Edge *edge = *firstEdge;
    while(edge != NULL){
        visited[edge->itemID] = 1; //Mark item as visited
        edge = edge->next;
    }
}


//Generate random recommendations for user
int *RandomRecommendation(Graph *graph, int userID, int maxRecommendations, int *size){
    srand(time(NULL));
    //Initilaize size and recommendation array
    *size = 0;
    int *recommendedItems = NULL;

    //Find the target user in the graph
    UserNode *user = FindUser(graph, userID);
    if(user == NULL){
        printf("User %d not found.\n", userID);
        return NULL;
    }

    //Create an array to track which items the user has already seen
    int *itemVisited = (int*)calloc(TOTALITEMS+1, sizeof(int)); //As we're going to return an array

    //Mark the items the user has already interacted with
    Edge *edge = NULL;
    MarkUserInteractions(itemVisited, user, &edge);

    //Allocate memory for recommended items
    recommendedItems = (int*)malloc(maxRecommendations*sizeof(int));
    int count = 0;

    //Randomly select unvisisted items
    while(count < maxRecommendations){
        //Generate a random item id between 1 and total item number
        int randomItem = (rand() % TOTALITEMS) + 1;

        //If the item hasn't been visited before, add it to recommendations
        if(!itemVisited[randomItem]){
            recommendedItems[count++] = randomItem;
            itemVisited[randomItem] = 1; //Mark item as visited to avoid recommend again
        }
    }

    //Set size of recommendations and clean up
    *size = count;
    free(itemVisited);
    return recommendedItems;
}

//Recommend top-rated items for user
int *TopRatedRecommendation(Graph *graph, int userID, int maxRecommendations, int *size) {
    *size = 0;

    //Find the target user
    UserNode *user = FindUser(graph, userID);
    if(user == NULL) {
        printf("User %d not found.\n", userID);
        return NULL;
    }

    //Mark the items that user already interacted with
    int *itemVisited = (int*)calloc(TOTALITEMS + 1, sizeof(int));
    Edge *firstEdge = NULL;
    MarkUserInteractions(itemVisited, user, &firstEdge);

    //Create structure to track item watch counts
    typedef struct {
        int itemID;
        int watchCount;
    } ItemWatchCount;

    //Allocate memory for watch count tracking
    ItemWatchCount *itemWatchCounts = (ItemWatchCount*)malloc((TOTALITEMS+1) * sizeof(ItemWatchCount));

    //Initialize watch counts
    for(int i=0 ; i<TOTALITEMS ; i++){
        itemWatchCounts[i].itemID = i;
        itemWatchCounts[i].watchCount = 0;
    }

    //Aggregate watch counts across all users
    UserNode *currentUser = graph->users;
    while(currentUser != NULL){
        Edge *currentEdge = currentUser->edges;
        while(currentEdge != NULL){
            //Increment watch count for this item
            itemWatchCounts[currentEdge->itemID].watchCount++;
            currentEdge = currentEdge->next;
        }
        currentUser = currentUser->next;
    }

    //Sort items by watch count (descending order)
    for(int i=0 ; i<TOTALITEMS-1 ; i++){
        for(int j=i+1 ; j<TOTALITEMS ; j++){
            if(itemWatchCounts[i].watchCount < itemWatchCounts[j].watchCount){
                ItemWatchCount temp = itemWatchCounts[i];
                itemWatchCounts[i] = itemWatchCounts[j];
                itemWatchCounts[j] = temp;
            }
        }
    }

    // Select top items user hasn't interacted with
    int *recommendedItems = (int*)malloc(maxRecommendations*sizeof(int));
    int count = 0;

    for(int i=0 ; i<TOTALITEMS&&count<maxRecommendations ; i++){
        int itemID = itemWatchCounts[i].itemID;
        
        //Check if the user has not already interacted with this item
        if(!itemVisited[itemID]) {
            recommendedItems[count++] = itemID;
        }
    }

    *size = count;
    
    // Free allocated memory
    free(itemVisited);
    free(itemWatchCounts);

    return recommendedItems;
}

int *SimilarUserRecommendation(Graph *graph, int userID, int maxRecommendations, int *size){
    *size = 0;

    //Find the target user
    UserNode *targetUser = FindUser(graph, userID);
    if(targetUser == NULL){
        printf("User %d not found.\n", userID);
        return NULL;
    }

    //Find most similar user based on shared items
    UserNode *otherUser = graph->users;
    int maxShared = 0;
    UserNode *mostSimilarUser = NULL;

    while(otherUser != NULL){
        if(otherUser->userID != userID){
            //Count shared items between target and other users
            int sharedItems = 0;
            Edge *targetEdge = targetUser->edges;

            while(targetEdge != NULL){
                Edge *otherEdge = otherUser->edges;
                while(otherEdge != NULL){
                    if(targetEdge->itemID == otherEdge->itemID){
                        sharedItems++;
                        break;
                    }
                    otherEdge = otherEdge->next;
                }
                targetEdge = targetEdge->next;
            }
            //Update most similar user
            if(sharedItems > maxShared){
                maxShared = sharedItems;
                mostSimilarUser = otherUser;
            }
        }
        otherUser = otherUser->next;
    }
    //If no similar user found, return null
    if(mostSimilarUser == NULL){
        printf("No similar users found.\n");
        return NULL;
    }

    //Recommend items liked by the most similar user
    int *recommendedItems = (int*)malloc(maxRecommendations*sizeof(int));
    int count = 0;

    Edge *mostSimilarEdge = mostSimilarUser->edges;
    while(mostSimilarEdge != NULL && count < maxRecommendations){
        Edge *targetEdge = targetUser->edges;
        int alreadySeen = 0;
        //Check if target user has already seen this item
        while(targetEdge != NULL){
            if(targetEdge->itemID == mostSimilarEdge->itemID){
                alreadySeen = 1;
                break;
            }
            targetEdge = targetEdge->next;
        }
        //Add unseen items to recommendations
        if(!alreadySeen){
            recommendedItems[count++] = mostSimilarEdge->itemID;
        }
        mostSimilarEdge = mostSimilarEdge->next;
    }
    *size = count;
    return recommendedItems;
}

int *WeightedDistanceRecommendation(Graph *graph, int userID, int maxRecommendations, int *size) {
    *size = 0;

    //Find the target user
    UserNode *targetUser = FindUser(graph, userID);
    if(targetUser == NULL){
        printf("Error: User %d not found in the graph.\n", userID);
        return NULL;
    }
    
    //Intilialize tracking arrays
    double *itemScores = (double*)malloc((TOTALITEMS + 1) * sizeof(double));
    int *visited = (int*)calloc(TOTALITEMS + 1, sizeof(int));
    int *itemInteractionCounts = (int*)calloc(TOTALITEMS + 1, sizeof(int));

    //Mark items already interacted by target user
    Edge *userEdge = targetUser->edges;
    while(userEdge != NULL){
        visited[userEdge->itemID] = 1;
        userEdge = userEdge->next;
    }

    //Calculate recommendations by exploring other users
    UserNode *currentUser = graph->users;
    while(currentUser != NULL){
        if(currentUser->userID != userID) {
            //Calculate user similarity
            double userSimilarity = 0.0;
            int commonItemCount = 0;

            //Find common items and calculate similarity
            Edge *targetEdge = targetUser->edges;
            while(targetEdge != NULL){
                Edge *currentEdge = currentUser->edges;
                while(currentEdge != NULL){
                    if(targetEdge->itemID == currentEdge->itemID){
                        //Calculate rating proximity
                        double ratingDifference = fabs(targetEdge->rating - currentEdge->rating);
                        double ratingProximity = 1.0 / (1.0 + ratingDifference);
                        
                        //Boost similarity based on rating magnitude
                        double ratingMagnitudeBoost = 1.0 + 
                            (fmin(targetEdge->rating, currentEdge->rating) / 
                             fmax(targetEdge->rating, currentEdge->rating));
                        
                        userSimilarity += ratingProximity * ratingMagnitudeBoost;
                        commonItemCount++;
                        break;
                    }
                    currentEdge = currentEdge->next;
                }
                targetEdge = targetEdge->next;
            }

            //Normalize similarity
            if(commonItemCount > 0){
                userSimilarity /= commonItemCount;
            }

            //Score items based on similarity and ratings
            Edge *currentEdge = currentUser->edges;
            while(currentEdge != NULL){
                if(!visited[currentEdge->itemID]){
                    double itemScore = userSimilarity * currentEdge->rating * (1.0 / (1.0 + itemInteractionCounts[currentEdge->itemID]));
                    itemScores[currentEdge->itemID] += itemScore;
                    itemInteractionCounts[currentEdge->itemID]++;
                }
                currentEdge = currentEdge->next;
            }
        }
        currentUser = currentUser->next;
    }

    //Generate recommendations
    int *recommendedItems = (int*)malloc(maxRecommendations*sizeof(int));
    int recommendCount = 0;

    //Find top recommendations
    while(recommendCount < maxRecommendations){
        double highestScore = -1.0;
        int bestItem = -1;

        //Find the item with the highest score
        for(int i=1 ; i<=TOTALITEMS ; i++){
            if(!visited[i] && itemScores[i] > highestScore){
                highestScore = itemScores[i];
                bestItem = i;
            }
        }

        //No more items to recommend
        if (bestItem == -1) break;

        recommendedItems[recommendCount++] = bestItem;
        visited[bestItem] = 1;
    }

    //Clean up
    free(itemScores);
    free(visited);
    free(itemInteractionCounts);

    *size = recommendCount;
    return recommendCount > 0 ? recommendedItems : NULL;
}


//Custom recommendation algorithm 
int *DiscoveryRecommendation(Graph *graph, int userID, int maxRecommendations, int *size){
    *size = 0;
    //Find the target user
    UserNode *user = FindUser(graph, userID);
    if(user == NULL){
        printf("User %d not found.\n", userID);
        return NULL;
    }
    //Arrays to track exploration and visited items
    double *explorationScores = (double*)calloc(TOTALITEMS+1, sizeof(double));
    int *itemVisited = (int*)calloc(TOTALITEMS+1, sizeof(int));

    //Mark the items the user has already interacted with
    Edge *edge = NULL;
    MarkUserInteractions(itemVisited, user, &edge);

    //Calculate exploration scores by examining other users
    UserNode *otherUser = graph->users;
    while(otherUser != NULL){
        if(otherUser->userID != userID){
            //Calculate dissimilarity
            double similarity = 0.0;
            Edge *targetEdge = user->edges;
            while(targetEdge != NULL){
                Edge *otherEdge = otherUser->edges;
                while(otherEdge != NULL){
                    if(targetEdge->itemID == otherEdge->itemID){
                        //Inverse of rating difference 
                        similarity += 1.0/fabs(targetEdge->rating - otherEdge->rating);
                    }
                    otherEdge = otherEdge->next;
                }
                targetEdge = targetEdge->next;
            }
            //Convert to a dissimilarity score
            similarity = 1.0 / (1.0 + similarity); 

            //Update exploration scores
            Edge *otherEdge = otherUser->edges;
            while(otherEdge != NULL){
                if(!itemVisited[otherEdge->itemID]){
                    explorationScores[otherEdge->itemID] += similarity *(1.0/otherEdge->rating);
                }
                otherEdge = otherEdge->next;
            }
        }
        otherUser = otherUser->next;
    }
    //Select top recommendations based on exploration scores
    int *recommendItems = (int*)malloc(maxRecommendations*sizeof(int));
    int count = 0;

    while(count<maxRecommendations){
        int bestItem = -1;
        double highestScore = -1.0;

        //Find item with highest exploration score
        for(int i=1 ; i<=TOTALITEMS ; i++){
            if(!itemVisited[i] && explorationScores[i] > highestScore){
                bestItem = i;
                highestScore = explorationScores[i];
            }
        }
        if(bestItem == -1){
            break; //No more valid items
        }
        recommendItems[count++] = bestItem;
        itemVisited[bestItem] = 1; //Mark as visited
    }

    *size = count;
    free(explorationScores);
    free(itemVisited);

    if(count == 0){
        free(recommendItems);
        return NULL;
    }
    return recommendItems;
}