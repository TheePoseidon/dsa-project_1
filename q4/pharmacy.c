#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CODE_LEN 16
#define NAME_LEN 80
#define LINE_LEN 300


typedef struct medicine_node
{
	char code[CODE_LEN];
	char name[NAME_LEN];
	int quantity;
	double unit_price;
	struct medicine_node *left;
	struct medicine_node *right;
} MedicineNode;

typedef struct
{
	int lines_read;
	int inserted;
	int updated;
	int skipped;
} LoadStats;

void trim(char *s)
{
	char *start = s;
	char *end;

	while (*start == ' ' || *start == '\t')
		start++;
	if (start != s)
		memmove(s, start, strlen(start) + 1);

	if (strlen(s) == 0)
		return;

	end = s + strlen(s) - 1;
	while (end >= s && (*end == ' ' || *end == '\t' ||
			     *end == '\n' || *end == '\r'))
	{
		*end = '\0';
		end--;
	}
}

int read_file_line(FILE *fp, char *buffer, size_t size, int *overflowed)
{
	*overflowed = 0;

	if (fgets(buffer, (int)size, fp) == NULL)
		return (0);

	if (strchr(buffer, '\n') == NULL && !feof(fp))
	{
		int c;

		*overflowed = 1;
		while ((c = fgetc(fp)) != '\n' && c != EOF)
			;
	}

	return (1);
}

int next_field(char **cursor, char *field, size_t field_size)
{
	char *start;
	char *pipe;
	size_t len;

	if (*cursor == NULL)
		return (0);

	start = *cursor;
	pipe = strchr(start, '|');

	if (pipe != NULL)
	{
		len = (size_t)(pipe - start);
		if (len >= field_size)
			len = field_size - 1;
		memcpy(field, start, len);
		field[len] = '\0';
		*cursor = pipe + 1;
	}
	else
	{
		strncpy(field, start, field_size - 1);
		field[field_size - 1] = '\0';
		*cursor = NULL;
	}

	trim(field);
	return (1);
}

int parse_record(char *line, char *code_out, char *name_out,
		  int *qty_out, double *price_out, const char **reason)
{
	char *cursor = line;
	char field[LINE_LEN];
	char *endptr;
	long qty;
	double price;

	if (!next_field(&cursor, field, CODE_LEN) || strlen(field) == 0)
	{
		*reason = "missing or empty medicine code";
		return (0);
	}
	strncpy(code_out, field, CODE_LEN - 1);
	code_out[CODE_LEN - 1] = '\0';

	if (!next_field(&cursor, field, NAME_LEN) || strlen(field) == 0)
	{
		*reason = "missing or empty medicine name";
		return (0);
	}
	strncpy(name_out, field, NAME_LEN - 1);
	name_out[NAME_LEN - 1] = '\0';

	if (!next_field(&cursor, field, sizeof(field)) || strlen(field) == 0)
	{
		*reason = "missing or empty quantity";
		return (0);
	}
	qty = strtol(field, &endptr, 10);
	if (*endptr != '\0')
	{
		*reason = "quantity is not a valid integer";
		return (0);
	}
	if (qty < 0)
	{
		*reason = "quantity cannot be negative";
		return (0);
	}
	*qty_out = (int)qty;

	if (!next_field(&cursor, field, sizeof(field)) || strlen(field) == 0)
	{
		*reason = "missing or empty unit price";
		return (0);
	}
	price = strtod(field, &endptr);
	if (*endptr != '\0')
	{
		*reason = "unit price is not a valid number";
		return (0);
	}
	if (price < 0)
	{
		*reason = "unit price cannot be negative";
		return (0);
	}
	*price_out = price;

	if (cursor != NULL)
	{
		next_field(&cursor, field, sizeof(field));
		if (strlen(field) != 0 || cursor != NULL)
		{
			*reason = "too many fields on the line";
			return (0);
		}
	}

	return (1);
}

MedicineNode *create_medicine_node(const char *code, const char *name,
				    int quantity, double unit_price)
{
	MedicineNode *node;

	node = malloc(sizeof(MedicineNode));
	if (node == NULL)
	{
		fprintf(stderr, "Error: memory allocation failed.\n");
		return (NULL);
	}

	strncpy(node->code, code, CODE_LEN - 1);
	node->code[CODE_LEN - 1] = '\0';
	strncpy(node->name, name, NAME_LEN - 1);
	node->name[NAME_LEN - 1] = '\0';
	node->quantity = quantity;
	node->unit_price = unit_price;
	node->left = NULL;
	node->right = NULL;

	return (node);
}

MedicineNode *bst_insert_or_update(MedicineNode *root, const char *code,
				    const char *name, int quantity,
				    double unit_price, LoadStats *stats)
{
	int cmp;

	if (root == NULL)
	{
		stats->inserted++;
		return (create_medicine_node(code, name, quantity, unit_price));
	}

	cmp = strcmp(code, root->code);
	if (cmp == 0)
	{
		root->quantity = quantity;
		stats->updated++;
	}
	else if (cmp < 0)
	{
		root->left = bst_insert_or_update(root->left, code, name,
						   quantity, unit_price, stats);
	}
	else
	{
		root->right = bst_insert_or_update(root->right, code, name,
						    quantity, unit_price, stats);
	}

	return (root);
}

MedicineNode *load_inventory(const char *filename, LoadStats *stats)
{
	FILE *fp;
	MedicineNode *root;
	char line[LINE_LEN];
	char line_copy[LINE_LEN];
	char original_line[LINE_LEN];
	int overflowed;
	char code[CODE_LEN];
	char name[NAME_LEN];
	int quantity;
	double price;
	const char *reason;

	stats->lines_read = 0;
	stats->inserted = 0;
	stats->updated = 0;
	stats->skipped = 0;
	root = NULL;

	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		fprintf(stderr, "Error: could not open '%s'.\n", filename);
		return (NULL);
	}

	while (read_file_line(fp, line, sizeof(line), &overflowed))
	{
		stats->lines_read++;

		strncpy(line_copy, line, LINE_LEN - 1);
		line_copy[LINE_LEN - 1] = '\0';
		trim(line_copy);

		if (strlen(line_copy) == 0)
			continue;

		strncpy(original_line, line_copy, LINE_LEN - 1);
		original_line[LINE_LEN - 1] = '\0';

		if (overflowed)
		{
			printf("Warning: line %d skipped (line too long).\n",
			       stats->lines_read);
			stats->skipped++;
			continue;
		}

		if (!parse_record(line_copy, code, name, &quantity, &price, &reason))
		{
			printf("Warning: line %d skipped (%s): \"%s\"\n",
			       stats->lines_read, reason, original_line);
			stats->skipped++;
			continue;
		}

		root = bst_insert_or_update(root, code, name, quantity, price, stats);
	}

	fclose(fp);

	if (stats->lines_read == 0)
		printf("Warning: '%s' is empty. Starting with an empty inventory.\n",
		       filename);

	return (root);
}


MedicineNode *bst_search(MedicineNode *root, const char *code)
{
	int cmp;

	if (root == NULL)
		return (NULL);

	cmp = strcmp(code, root->code);
	if (cmp == 0)
		return (root);
	if (cmp < 0)
		return (bst_search(root->left, code));

	return (bst_search(root->right, code));
}


void print_record(const MedicineNode *node)
{
	printf("  %-8s | %-33s | Qty: %-5d | Price: %.2f RWF\n",
	       node->code, node->name, node->quantity, node->unit_price);
}


void inorder_display(const MedicineNode *root, int *count)
{
	if (root == NULL)
		return;

	inorder_display(root->left, count);
	print_record(root);
	(*count)++;
	inorder_display(root->right, count);
}

void free_tree(MedicineNode *root)
{
	if (root == NULL)
		return;

	free_tree(root->left);
	free_tree(root->right);
	free(root);
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
			printf("Invalid input. Please enter a number between %d and %d.\n",
			       min, max);
	} while (!ok);

	return (value);
}


void action_search(MedicineNode *root)
{
	char code[CODE_LEN];
	MedicineNode *found;

	printf("Enter medicine code to search: ");
	read_line(code, sizeof(code));
	trim(code);

	if (strlen(code) == 0)
	{
		printf("No code entered.\n");
		return;
	}

	found = bst_search(root, code);
	if (found == NULL)
	{
		printf("Medicine code '%s' was not found in the inventory.\n", code);
		return;
	}

	printf("\nRecord found:\n");
	print_record(found);
}

void action_display_all(MedicineNode *root)
{
	int count = 0;

	if (root == NULL)
	{
		printf("The inventory is currently empty.\n");
		return;
	}

	printf("\nFull inventory, ascending by medicine code:\n");
	inorder_display(root, &count);
	printf("(%d record(s) listed)\n", count);
}

void print_menu(void)
{
	printf("\n===== Pharmacy Inventory (BST) =====\n");
	printf("1. Search for a medicine by code\n");
	printf("2. Display full inventory (sorted by code)\n");
	printf("3. Exit\n");
}

int main(void)
{
	char filename[256];
	MedicineNode *root;
	LoadStats stats;
	int choice;
	int running;

	printf("Pharmacy Inventory System\n");
	printf("Enter the inventory file path (blank = medicines.txt): ");
	read_line(filename, sizeof(filename));
	if (strlen(filename) == 0)
		strcpy(filename, "medicines.txt");

	root = load_inventory(filename, &stats);
	if (stats.lines_read == 0 && root == NULL)
	{
		printf("No valid records were found in the inventory file.\n");
		return (1);
	}

	printf("\nLoad summary for '%s':\n", filename);
	printf("  Lines read:        %d\n", stats.lines_read);
	printf("  Records inserted:  %d\n", stats.inserted);
	printf("  Duplicates updated:%d\n", stats.updated);
	printf("  Records skipped:   %d\n", stats.skipped);

	running = 1;
	while (running)
	{
		print_menu();
		choice = read_int("Choose an option (1-3): ", 1, 3);

		switch (choice)
		{
			case 1:
				action_search(root);
				break;
			case 2:
				action_display_all(root);
				break;
			case 3:
				running = 0;
				break;
			default:
				break;
		}
	}

	free_tree(root);
	printf("\nInventory memory released. Goodbye.\n");

	return (0);
}