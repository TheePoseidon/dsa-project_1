# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# define MATCH_ID_LEN 16

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

static int g_match_counter = 0;

Node *create_leaf(int participant_id)
{
	Node *node;
 
	node = malloc(sizeof(Node));
	if (node == NULL)
	{
		fprintf(stderr, "Error: memory allocation failed.\n");
		return (NULL);
	}
 
	node->is_leaf = 1;
	node->participant_id = participant_id;
	node->match_id[0] = '\0';
	node->left = NULL;
	node->right = NULL;
	node->parent = NULL;
 
	return (node);
}

Node *create_match(Node *left, Node *right)
{
	Node *node;
 
	node = malloc(sizeof(Node));
	if (node == NULL)
	{
		fprintf(stderr, "Error: memory allocation failed.\n");
		return (NULL);
	}
 
	g_match_counter++;
	node->is_leaf = 0;
	node->participant_id = 0;
	snprintf(node->match_id, MATCH_ID_LEN, "M%d", g_match_counter);
	node->left = left;
	node->right = right;
	node->parent = NULL;
 
	if (left != NULL)
		left->parent = node;
	if (right != NULL)
		right->parent = node;
 
	return (node);
}

Node *build_tournament_tree(const int *ids, int n)
{
	Node **level;
	Node **next_level;
	int size, next_size;
	int i, j;
 
	if (n <= 0)
		return (NULL);
 
	level = malloc(sizeof(Node *) * (size_t)n);
	if (level == NULL)
		return (NULL);
 
	for (i = 0; i < n; i++)
		level[i] = create_leaf(ids[i]);
	size = n;
 
	while (size > 1)
	{
		next_size = (size % 2 == 0) ? size / 2 : size / 2 + 1;
		next_level = malloc(sizeof(Node *) * (size_t)next_size);
		if (next_level == NULL)
		{
			free(level);
			return (NULL);
		}
 
		i = 0;
		j = 0;
		while (i < size)
		{
			if (i + 1 < size)
			{
				next_level[j] = create_match(level[i], level[i + 1]);
				i += 2;
			}
			else
			{
				next_level[j] = level[i];
				i += 1;
			}
			j++;
		}
 
		free(level);
		level = next_level;
		size = next_size;
	}
 
	{
		Node *root = level[0];
 
		free(level);
		return (root);
	}
}

void collect_leaves(const Node *node, int *out, int *count)
{
	if (node == NULL)
		return;
 
	if (node->is_leaf)
	{
		out[*count] = node->participant_id;
		(*count)++;
		return;
	}
 
	collect_leaves(node->left, out, count);
	collect_leaves(node->right, out, count);
}

int count_leaves(const Node *node)
{
	if (node == NULL)
		return (0);
	if (node->is_leaf)
		return (1);
	return (count_leaves(node->left) + count_leaves(node->right));
}

Node *search_participant(Node *node, int target_id)
{
	Node *found;
 
	if (node == NULL)
		return (NULL);
 
	if (node->is_leaf)
		return (node->participant_id == target_id ? node : NULL);
 
	found = search_participant(node->left, target_id);
	if (found != NULL)
		return (found);
 
	return (search_participant(node->right, target_id));
}

Node *get_parent(const Node *node)
{
	return (node == NULL ? NULL : node->parent);
}

Node *get_sibling(const Node *node)
{
	if (node == NULL || node->parent == NULL)
		return (NULL);
 
	if (node->parent->left == node)
		return (node->parent->right);
 
	return (node->parent->left);
}

int get_grandchildren(const Node *node, Node *out[4])
{
	int count = 0;
 
	if (node == NULL)
		return (0);
 
	if (node->left != NULL)
	{
		if (node->left->left != NULL)
			out[count++] = node->left->left;
		if (node->left->right != NULL)
			out[count++] = node->left->right;
	}
	if (node->right != NULL)
	{
		if (node->right->left != NULL)
			out[count++] = node->right->left;
		if (node->right->right != NULL)
			out[count++] = node->right->right;
	}
 
	return (count);
}

void print_label(const Node *node)
{
	if (node == NULL)
	{
		printf("(none)");
		return;
	}
 
	if (node->is_leaf)
		printf("Participant %d", node->participant_id);
	else
		printf("Match %s", node->match_id);
}
 
void print_label_line(const Node *node)
{
	print_label(node);
	printf("\n");
}

typedef struct
{
	Node **items;
	int count;
	int capacity;
	int head;
} PtrQueue;
 
void queue_init(PtrQueue *q)
{
	q->capacity = 8;
	q->items = malloc(sizeof(Node *) * (size_t)q->capacity);
	q->count = 0;
	q->head = 0;
}
 
void queue_push(PtrQueue *q, Node *node)
{
	if (q->head + q->count == q->capacity)
	{
		q->capacity *= 2;
		q->items = realloc(q->items, sizeof(Node *) * (size_t)q->capacity);
	}
	q->items[q->head + q->count] = node;
	q->count++;
}
 
Node *queue_pop(PtrQueue *q)
{
	Node *node = q->items[q->head];
 
	q->head++;
	q->count--;
	return (node);
}

int insert_participant(Node **root, int new_id)
{
	PtrQueue q;
	Node *current;
	Node *new_leaf;
	Node *old_leaf_copy;
 
	new_leaf = create_leaf(new_id);
	if (new_leaf == NULL)
		return (0);
 
	if (*root == NULL)
	{
		*root = new_leaf;
		return (1);
	}
 
	queue_init(&q);
	queue_push(&q, *root);
 
	while (q.count > 0)
	{
		current = queue_pop(&q);
 
		if (current->is_leaf)
		{
			old_leaf_copy = create_leaf(current->participant_id);
			if (old_leaf_copy == NULL)
			{
				free(new_leaf);
				free(q.items);
				return (0);
			}
 
			g_match_counter++;
			current->is_leaf = 0;
			snprintf(current->match_id, MATCH_ID_LEN, "M%d", g_match_counter);
			current->left = old_leaf_copy;
			current->right = new_leaf;
			old_leaf_copy->parent = current;
			new_leaf->parent = current;
 
			free(q.items);
			return (1);
		}
 
		queue_push(&q, current->left);
		queue_push(&q, current->right);
	}

	free(new_leaf);
	free(q.items);
	return (0);
}

void free_tree(Node *node)
{
	if (node == NULL)
		return;
 
	free_tree(node->left);
	free_tree(node->right);
	free(node);
}
 
void read_line(char *buffer, size_t size)
{
	if (fgets(buffer, (int)size, stdin) != NULL)
	{
		size_t len = strlen(buffer);
 
		if (len > 0 && buffer[len - 1] == '\n')
			buffer[len - 1] = '\0';
		else
		{
			int c;
 
			while ((c = getchar()) != '\n' && c != EOF)
				;
		}
	}
	else
	{
		buffer[0] = '\0';
	}
}
 
int read_int(const char *prompt, int min, int max)
{
	char buf[64];
	int value;
	int ok;
 
	do
	{
		ok = 0;
		printf("%s", prompt);
		read_line(buf, sizeof(buf));
		if (sscanf(buf, "%d", &value) == 1 && value >= min && value <= max)
			ok = 1;
		else
			printf("Invalid input. Please enter a number between %d and %d.\n",
			       min, max);
	} while (!ok);
 
	return (value);
}

 
void action_display_root(const Node *root)
{
	if (root == NULL)
	{
		printf("The tournament tree is empty.\n");
		return;
	}
 
	printf("Root: ");
	print_label(root);
	printf(" (the final match) - the root has no parent.\n");
}
 
void action_display_leaves(const Node *root)
{
	int *ids;
	int count = 0;
	int i;
 
	if (root == NULL)
	{
		printf("The tournament tree is empty.\n");
		return;
	}
 
	ids = malloc(sizeof(int) * (size_t)count_leaves(root));
	if (ids == NULL)
	{
		fprintf(stderr, "Error: memory allocation failed.\n");
		return;
	}
 
	collect_leaves(root, ids, &count);
 
	printf("All %d leaf participants (left to right):\n", count);
	for (i = 0; i < count; i++)
	{
		printf("%4d", ids[i]);
		if ((i + 1) % 10 == 0)
			printf("\n");
	}
	if (count % 10 != 0)
		printf("\n");
 
	free(ids);
}
 
void action_lookup_participant(Node *root)
{
	int target;
	Node *node;
	Node *parent;
	Node *sibling;
	Node *grandchildren[4];
	int gc_count;
	int i;
 
	if (root == NULL)
	{
		printf("The tournament tree is empty.\n");
		return;
	}
 
	target = read_int("Enter the participant identifier to look up: ", -1000000, 1000000);
	node = search_participant(root, target);
 
	if (node == NULL)
	{
		printf("No participant with identifier %d exists in the tournament.\n", target);
		return;
	}
 
	printf("\nFound: ");
	print_label_line(node);
 
	parent = get_parent(node);
	printf("Parent: ");
	if (parent == NULL)
		printf("(none - this participant IS the root, it has no parent)\n");
	else
		print_label_line(parent);
 
	sibling = get_sibling(node);
	printf("Sibling: ");
	if (sibling == NULL)
		printf("(none - this participant has no sibling)\n");
	else
		print_label_line(sibling);
 
	gc_count = get_grandchildren(node, grandchildren);
	printf("Grandchildren: ");
	if (gc_count == 0)
	{
		printf("(none)");
		if (node->is_leaf)
			printf(" - participants are leaves, so they never have children or grandchildren.");
		printf("\n");
	}
	else
	{
		printf("\n");
		for (i = 0; i < gc_count; i++)
		{
			printf("  - ");
			print_label_line(grandchildren[i]);
		}
	}
}
 
void action_add_participant(Node **root)
{
	int new_id;
	int already_exists;
 
	new_id = read_int("Enter the new participant's identifier: ", -1000000, 1000000);
 
	already_exists = (*root != NULL && search_participant(*root, new_id) != NULL);
	if (already_exists)
	{
		printf("Participant %d is already registered. No changes made.\n", new_id);
		return;
	}
 
	if (insert_participant(root, new_id))
		printf("Participant %d added to the tournament.\n", new_id);
	else
		printf("Failed to add participant %d (memory allocation error).\n", new_id);
}
 
void print_menu(void)
{
	printf("\n===== E-Sports Tournament Bracket =====\n");
	printf("1. Display root match/participant\n");
	printf("2. Display all leaf participants\n");
	printf("3. Look up a participant (parent / sibling / grandchildren)\n");
	printf("4. Add a new participant to the tournament\n");
	printf("5. Exit\n");
}
 
int main(void)
{
	Node *root;
	int choice;
	int running;
 
	printf("Building the tournament bracket from %d participants...\n", PARTICIPANT_COUNT);
	root = build_tournament_tree(PARTICIPANTS, PARTICIPANT_COUNT);
	if (root == NULL)
	{
		fprintf(stderr, "Failed to build the tournament tree.\n");
		return (1);
	}
	printf("Tree built successfully. %d matches created; root is Match M%d.\n\n",
	       g_match_counter, g_match_counter);
 
	running = 1;
	while (running)
	{
		print_menu();
		choice = read_int("Choose an option (1-5): ", 1, 5);
 
		switch (choice)
		{
			case 1:
				action_display_root(root);
				break;
			case 2:
				action_display_leaves(root);
				break;
			case 3:
				action_lookup_participant(root);
				break;
			case 4:
				action_add_participant(&root);
				break;
			case 5:
				running = 0;
				break;
			default:
				break;
		}
	}
 
	free_tree(root);
	printf("\nTree memory released. Goodbye.\n");
 
	return (0);
}