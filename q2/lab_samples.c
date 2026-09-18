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

/* Function to create a new sample node with the given id, type, and priority. */