# Movie Recommendation System

A small recommendation engine written in C for a first-year data structures
course. Ratings are loaded into a user-item graph, and five different algorithms
generate movie recommendations from it. It runs on the
[MovieLens 100K](https://grouplens.org/datasets/movielens/100k/) dataset
(943 users, 1682 movies, 100,000 ratings) with no external libraries, just the
standard C library.

## How the data is modeled

The ratings are held in a bipartite user-to-item graph, stored as adjacency
lists:

```
Graph
  UserNode(userID)  ->  UserNode  ->  ...        (linked list of users)
      Edge(itemID, rating)  ->  Edge  ->  ...     (each user's ratings)
```

Each user is a node in a linked list; every movie that user rated is an edge
carrying the score. Loading the dataset is a single linear pass
(`LoadGraphFromFile` in `main.c`), and each algorithm walks this structure
directly. See `graph.h` and `graph.c`.

## The five algorithms

They live in `recommendation.c` and all take
`(graph, userID, maxRecommendations, &size)` and return an array of recommended
item IDs.

1. **Random**: picks unseen movies uniformly at random. The baseline to beat.
2. **Top Rated**: ranks movies by their overall ratings across all users and
   returns the best ones, skipping anything the target already rated.
3. **Similar User**: finds the one user who shares the most rated items with the
   target and recommends what that user liked. Basic user-based collaborative
   filtering.
4. **Weighted Distance**: scores other users by rating proximity
   (`1 / (1 + |rating difference|)`, boosted when both ratings are high), then
   scores candidate items by `similarity * rating * 1/(1 + popularity)` so that
   agreement counts but over-popular items get damped.
5. **Discovery** (my own addition): deliberately flips the similarity signal so
   items are weighted by dissimilarity, nudging the user toward movies outside
   their usual taste instead of the safe, popular picks.

`main.c` times each run with `clock()` and prints the elapsed CPU time, which
made comparing the algorithms straightforward.

## Build and run

```sh
gcc -o recommendation_system graph.c recommendation.c main.c
./recommendation_system
```

It prompts for a user ID, how many recommendations you want, and which algorithm
to use, then prints the recommended item IDs and the execution time.

## Getting the dataset

The ratings file is not committed, because the MovieLens data may not be
redistributed. Download it yourself:

1. Get **MovieLens 100K** from <https://grouplens.org/datasets/movielens/100k/>.
2. From `u.data` (tab-separated `user  item  rating  timestamp`), keep the first
   three columns and save the result as `interactions.txt` in the project root,
   one rating per line:

   ```
   196 242 3
   186 302 3
   22 377 1
   ```

The hard-coded user/item counts (943 / 1682) match this dataset, so it works out
of the box.

## Files

| File | Contents |
|---|---|
| `graph.h`, `graph.c` | the user-item graph (adjacency lists) |
| `recommendation.h`, `recommendation.c` | the five recommendation algorithms |
| `main.c` | dataset loading, menu, timing |
