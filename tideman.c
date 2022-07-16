#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


#define MAX 9 // Max number of candidates


int preferences[MAX][MAX]; // preferences[i][j] is number of voters who prefer i over j

bool locked[MAX][MAX]; // locked[i][j] means i is locked in over j

typedef struct // Each pair has a winner, loser
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
bool is_cyclical(int steps, int loser, int query);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
void merge_sort (pair* pair0, int* array, int elements);

int main(int argc, string argv[])
{

    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    candidate_count = argc - 1;    // Populate array of candidates
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }


    for (int i = 0; i < candidate_count; i++)    // Clear graph of locked in pairs
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");


    for (int i = 0; i < voter_count; i++)    // Query for votes
    {

        int ranks[candidate_count];        // ranks[i] is voter's ith preference

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }

    return false;

}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]]++;
        }
    }

    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            if (preferences[i][j] != preferences[j][i])
            {
                pairs[pair_count].winner = (preferences[i][j] > preferences[j][i]) ? i : j;
                pairs[pair_count].loser = (preferences[i][j] < preferences[j][i]) ? i : j;
                pair_count++;
            }
        }
    }

    return;
}


void sort_pairs(void) // Sort pairs in decreasing order by strength of victory
{
    int pair_differences[pair_count];
    pair pair_buffer[pair_count];

    for (int i = 0; i < pair_count; i++)
    {
        pair_differences[i] = preferences[pairs[i].winner][pairs[i].loser] - preferences[pairs[i].loser][pairs[i].winner];
        pair_buffer[i] = pairs[i];
    }

    merge_sort(pair_buffer, pair_differences, pair_count);

    for (int i = 0; i < pair_count; i++)
    {
        pairs[i] = pair_buffer[i];
    }

    for (int i = 0; i < pair_count; i++){
        printf("winner: %s  loser: %s   diff: %d\n",candidates[pairs[i].winner],candidates[pairs[i].loser],pair_differences[i]);
    }
    printf("\n");

    return;
}


void merge_sort (pair* pair0, int* array, int elements)
{
    int array1_size = ceil(elements/2.0), array2_size = floor(elements/2.0);
    int array1[array1_size], array2[array2_size];
    pair pair1[array1_size], pair2[array2_size];
    int array_pointer = 0, array1_pointer = 0, array2_pointer = 0;

    for (int i = 0; i < array1_size; i++)
    {
        array1[i] = array[i];
        pair1[i] = pair0[i];
    }

    for (int i = 0; i < array2_size; i++)
    {
        array2[i] = array[i + array1_size];
        pair2[i] = pair0[i + array1_size];
    }

    if (array1_size >= 2)
    {
            merge_sort(pair1, array1, array1_size);
    }
    if (array2_size >= 2)
    {
            merge_sort(pair2, array2, array2_size);
    }

    while (array1_pointer < array1_size && array2_pointer < array2_size)
    {
        if (array1[array1_pointer] >= array2[array2_pointer])
        {
            array[array_pointer] = array1[array1_pointer];
            pair0[array_pointer++] = pair1[array1_pointer++];
        }
        else
        {
            array[array_pointer] = array2[array2_pointer];
            pair0[array_pointer++] = pair2[array2_pointer++];
        }
    }

    while (array1_pointer < array1_size)
    {
        array[array_pointer] = array1[array1_pointer];
        pair0[array_pointer++] = pair1[array1_pointer++];
    }

    while (array2_pointer < array2_size)
    {
        array[array_pointer] = array2[array2_pointer];
        pair0[array_pointer++] = pair2[array2_pointer++];
    }
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int pair_seq = 0; pair_seq < pair_count; pair_seq++)       //pair_seq also used to determine the path length is_cyclical() takes
    {
        if (!(is_cyclical(pair_seq, pairs[pair_seq].loser, pairs[pair_seq].winner)))    //if not cyclical, lock
        {
            locked[pairs[pair_seq].winner][pairs[pair_seq].loser] = true;
            printf("%s > %s\n",candidates[pairs[pair_seq].winner],candidates[pairs[pair_seq].loser]);
        }
        else                                                                            //otherwise dont lock
        {
            printf("%s !! %s\n",candidates[pairs[pair_seq].winner],candidates[pairs[pair_seq].loser]);
        }
    }

    return;
}


bool is_cyclical (int loser, int query)
/*
this function is coded based on my shallow understanding of the graph theory.
Basically it checks if the winner in the ith pair (pair[pair_seq].winner, aka THE winner)'s 
loser (pair[pair_seq].loser)'s loser's loser's etc is THE winner. 

How many "loser's" there is is determined by # of steps.

steps: decreases as more recursion occurs.
	if THE winner comes first in the sorted pairs[], the more 'likely' it can be locked,
	since the function won't look too deep into the graph, less likely the loser's loser's
	etc is THE winner.

loser: THE winner (not necessarily a loser) / THE winner's loser etc;

query: THE winner
*/
{
    if (loser == query)     //if the loser is THE winner, it's cyclical
    {
        return true;
    }
    // check all candidates 
    for (int i = 0; i < candidate_count; i++)
    {
	// if "loser" over candidate i is already locked in the graph
        if (locked[loser][i])
        {
	    // check if candidate i has a cyclic link from "query"
            if (is_cyclical(i, query))
            {
                return true;
            }
        }
    }
    return false;
}

// Print the winner of the election
void print_winner(void)
{
    // TODO
    return;
}
