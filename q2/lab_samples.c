# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# define ID_LEN 32
# define TYPE_LEN 32

typedef struct sample
{
    char id [ID_LEN];
    char type [TYPE_LEN];
    int priority;
    struct sample *next;
    struct sample *prev;
    
} Sample;

typedef struct
{
    Sample *head;
    Sample *tail;
    Sample *current;
    int count;
} Queue;

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
    char buf[32];
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
            printf("Invalid input. Please enter an integer between %d and %d.\n", min, max);
    } while (!ok);
    return value;
}

const char *priority_label(int priority)
{
    if (priority == 1)
        return("Urgent");
    if (priority == 2)
        return("Normal");
    if (priority == 3)
        return("Routine");
    return("Unknown");
}

Sample *create_sample(const char *id, const char *type, int priority)
{
	Sample *node;
 
	node = malloc(sizeof(Sample));
	if (node == NULL)
	{
		fprintf(stderr, "Error: memory allocation failed.\n");
		return (NULL);
	}
 
	strncpy(node->id, id, ID_LEN - 1);
	node->id[ID_LEN - 1] = '\0';
	strncpy(node->type, type, TYPE_LEN - 1);
	node->type[TYPE_LEN - 1] = '\0';
	node->priority = priority;
	node->next = NULL;
	node->prev = NULL;
 
	return (node);
}

void append_sample(Queue *q, Sample *node)
{
	if (node == NULL)
		return;
 
	if (q->head == NULL)
	{
		q->head = node;
		q->tail = node;
		q->current = node;
	}
	else
	{
		node->prev = q->tail;
		q->tail->next = node;
		q->tail = node;
	}
 
	q->count++;
}

void prompt_new_sample(Queue *q)
{
	char id[ID_LEN];
	char type[TYPE_LEN];
	int priority;
	Sample *node;
 
	printf("Enter sample ID: ");
	read_line(id, sizeof(id));
	printf("Enter sample type: ");
	read_line(type, sizeof(type));
	priority = read_int("Enter priority (1=Urgent, 2=Normal, 3=Routine): ", 1, 3);
 
	node = create_sample(id, type, priority);
	if (node == NULL)
		return;
 
	append_sample(q, node);
	printf("Sample '%s' added to the end of the queue.\n", id);
}

void display_sample(const Sample *s)
{
	if (s == NULL)
		return;
 
	printf("  ID: %-10s | Type: %-15s | Priority: %s\n",
	       s->id, s->type, priority_label(s->priority));
}

void display_current(const Queue *q)
{
	if (q->head == NULL)
	{
		printf("Queue is empty. No sample to display.\n");
		return;
	}
 
	printf("Current sample:\n");
	display_sample(q->current);
}

void move_next(Queue *q)
{
	if (q->head == NULL)
	{
		printf("Queue is empty. Nothing to move to.\n");
		return;
	}
 
	if (q->current->next == NULL)
	{
		printf("Already at the last sample in the queue.\n");
		return;
	}
 
	q->current = q->current->next;
	printf("Moved to next sample.\n");
	display_sample(q->current);
}

void move_previous(Queue *q)
{
	if (q->head == NULL)
	{
		printf("Queue is empty. Nothing to move to.\n");
		return;
	}
 
	if (q->current->prev == NULL)
	{
		printf("Already at the first sample in the queue.\n");
		return;
	}
 
	q->current = q->current->prev;
	printf("Moved to previous sample.\n");
	display_sample(q->current);
}

void review_forward(Queue *q)
{
	Sample *node;
 
	if (q->head == NULL)
	{
		printf("Queue is empty. Nothing to review.\n");
		return;
	}
 
	printf("--- Reviewing queue forward (oldest to newest) ---\n");
	node = q->head;
	while (node != NULL)
	{
		display_sample(node);
		q->current = node;
		if (node->next == NULL)
			break;
		node = node->next;
	}
	printf("--- Reached the end of the queue ---\n");
}

void review_backward(Queue *q)
{
	Sample *node;
 
	if (q->head == NULL)
	{
		printf("Queue is empty. Nothing to review.\n");
		return;
	}
 
	printf("--- Reviewing queue backward (newest to oldest) ---\n");
	node = q->tail;
	while (node != NULL)
	{
		display_sample(node);
		q->current = node;
		if (node->prev == NULL)
			break;
		node = node->prev;
	}
	printf("--- Reached the beginning of the queue ---\n");
}
 
void load_initial_samples(Queue *q)
{
	int n;
	int i;
	char id[ID_LEN];
	char type[TYPE_LEN];
	int priority;
	Sample *node;
 
	n = read_int("How many initial samples will you enter? ", 0, 10000);
 
	for (i = 0; i < n; i++)
	{
		printf("\nSample %d of %d:\n", i + 1, n);
		printf("Enter sample ID: ");
		read_line(id, sizeof(id));
		printf("Enter sample type: ");
		read_line(type, sizeof(type));
		priority = read_int("Enter priority (1=Urgent, 2=Normal, 3=Routine): ", 1, 3);
 
		node = create_sample(id, type, priority);
		append_sample(q, node);
	}
}
 
void free_queue(Queue *q)
{
	Sample *node;
	Sample *next_node;
 
	node = q->head;
	while (node != NULL)
	{
		next_node = node->next;
		free(node);
		node = next_node;
	}
 
	q->head = NULL;
	q->tail = NULL;
	q->current = NULL;
	q->count = 0;
}
 
 
void print_menu(void)
{
	printf("\n===== Laboratory Sample Queue =====\n");
	printf("1. Move to next sample\n");
	printf("2. Move to previous sample\n");
	printf("3. Display current sample\n");
	printf("4. Review queue forward (start to end)\n");
	printf("5. Review queue backward (end to start)\n");
	printf("6. Add new sample to end of queue\n");
	printf("7. Exit\n");
}
 
int main(void)
{
	Queue q = {NULL, NULL, NULL, 0};
	int choice;
	int running;
 
	printf("Laboratory Sample Queue System\n");
	load_initial_samples(&q);
 
	running = 1;
	while (running)
	{
		print_menu();
		choice = read_int("Choose an option (1-7): ", 1, 7);
 
		switch (choice)
		{
			case 1:
				move_next(&q);
				break;
			case 2:
				move_previous(&q);
				break;
			case 3:
				display_current(&q);
				break;
			case 4:
				review_forward(&q);
				break;
			case 5:
				review_backward(&q);
				break;
			case 6:
				prompt_new_sample(&q);
				break;
			case 7:
				running = 0;
				break;
			default:
				break;
		}
	}
 
	printf("\nTotal samples in queue at exit: %d\n", q.count);
	free_queue(&q);
	printf("Queue memory released. Goodbye.\n");
 
	return (0);
}