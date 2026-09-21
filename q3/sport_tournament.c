# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# define NAME_LEN 16

static const int PARTICIPANTS[] = {
	42, 17, 68, 9, 23, 55, 81, 4, 13, 20, 31, 49, 61, 75, 90, 2, 7, 11,
	15, 19, 21, 27, 35, 45, 52, 58, 64, 72, 78, 85, 95, 1, 3, 5, 6, 8,
	10, 12, 14, 16, 18, 22, 24, 26, 29, 33, 37, 41, 44, 47, 50, 54, 57,
	60, 63, 66, 70, 74, 77, 80, 83, 87, 92, 97, 25, 28, 30, 34, 39
};

# define PARTICIPANT_COUNT ((int)(sizeof(PARTICIPANTS) / sizeof(PARTICIPANTS[0])))

typedef struct node
{
	int is_leaf;
	int participant_id;
	char match_id[MATCH_ID_LEN];
	struct node *left;
	struct node *right;
	struct node *parent;
} Node;