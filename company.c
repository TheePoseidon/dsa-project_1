# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>

# define MAX_ID_LEN  32
# define MAX_NAME_LEN  64
# define MAX_CAT_LEN  64
# define INITIAL_CAPACITY  16

typedef struct
{
    char order_id[MAX_ID_LEN];
    char customer_name[MAX_NAME_LEN];
    char category[MAX_CAT_LEN];
    long order_value;
} Order;

Order *read_orders(const char *filename, int *count)
{
    FILE *fp;
    Order *orders;
    int capacity;
    int n;

    fp =fopen(filename, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return (NULL);
    }

    capacity = INITIAL_CAPACITY;
    orders = malloc(sizeof(Order) * (size_t)capacity);
    if (orders == NULL)
    {
        fclose(fp);
        return (NULL);
    }

    n =0;
    while (1)
    {
        if (n == capacity)
        {
            Order *tmp;

            capacity *= 2;
            tmp = realloc(orders, sizeof(Order) * (size_t)capacity);
            if (tmp == NULL)
            {
                free(orders);
                fclose(fp);
                return (NULL);
            }
            orders = tmp;
        }

        if (fscanf(fp, "%31s %63s %63s %1d",
            orders[n].order_id,
            orders[n].customer_name,
            orders[n].category,
            &orders[n].order_value) != 4)
        {
            break;
        }
        n++;
    }

    fclose(fp);
    *count = n;
    return (orders);
}

int compare_orders(const Order *a, const Order *b)
{ 
    int name_cmp;

    if (a->order_value > b->order_value)
        return (-1);
    else if (a->order_value < b->order_value)
        return (1);

    name_cmp = strcmp(a->customer_name, b->customer_name);
    if (name_cmp != 0)
        return (name_cmp);

    return (strcmp(a->order_id, b->order_id));
}

void swap_orders(Order *a, Order *b)
{
    Order tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;
}

int partition(Order arr[], int low, int high)
{
    int pivot_index;
    Order pivot;
    int i, j;

    pivot_index = low + rand() % (high - low +1);
    swap_orders(&arr[pivot_index], &arr[high]);
    pivot = arr[high];

    i = low -1;
    for (j = low; j < high; j++)
    {
        if (compare_orders(&arr[j], &pivot) < 0)
        {
            i++;
            swap_orders(&arr[i], &arr[j]);
        }
    }
    swap_orders(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quicksort(Order arr[], int low, int high)
{
    if (low < high)
    {
        int pi = partition(arr, low, high);

        quicksort(arr, low, pi -1);
        quicksort(arr, pi +1, high);
    }
}