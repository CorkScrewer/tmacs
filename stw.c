#include "stw.h"

/*                    STW_VECTOR                    */

static size_t vector_free_keeper = 0;

/* Instantiates the vector (dynamic array.) */
stw_vector *stw_vector_make(void) {
    stw_vector *vec = malloc(sizeof(stw_vector));
    vec->capacity = 2;
    vec->length = 0;
    vec->array = malloc(vec->capacity * sizeof(void*));
    assert(vec->array);

    vector_free_keeper++;

    return vec;
}

/* Pushes value `v` to the end of the vector. */
void stw_vector_push(stw_vector *vec, void *v) {
    vec->array[vec->length++] = v;

    if (vec->length >= vec->capacity) {
        vec->capacity *= 2;
        vec->array = realloc(vec->array, sizeof(void*) * vec->capacity);
        assert(vec->array);
    }
}

/* Adds value `v` to the vector at `index` */
void stw_vector_add(stw_vector *vec, size_t index, void *v) {
    assert(index < vec->length);
    vec->length++;
    
    if (vec->length >= vec->capacity) {
        vec->capacity *= 2;
        vec->array = realloc(vec->array, sizeof(void*) * vec->capacity);
        assert(vec->array);
    }
    
    /* Shift the entire array forward */
    {
        void *replaced = vec->array[index];
        size_t i;
        vec->array[index] = v;
        
        for (i = index+1; i < vec->length; ++i) {
            void *r = vec->array[i];
            vec->array[i] = replaced;
            replaced = r;
        }
    }
}

/* Removes the specified index from the vector. */
void stw_vector_remove(stw_vector *vec, size_t index) {
    size_t i;
    
    assert(index < vec->length);
    assert(vec->length > 0);

    /* Shift the entire array backward */
    for (i = index; i < vec->length; ++i) {
        vec->array[i] = vec->array[i+1];
    }
    vec->array[vec->length--] = NULL;
}

/* Returns the index of the first occurance of `v` in the vector.  Returns -1 if `v` does not exist. */
int stw_vector_find(stw_vector *vec, void *v) {
    size_t i;
    for (i = 0; i < vec->length; ++i) {
        if (vec->array[i] == v) return i;
    }
    return -1;
}

/* Removes and returns the value at the end of the vector. */
void *stw_vector_pop(stw_vector *vec) {
    void *ret;
    assert(vec->length > 0);
    
    ret = vec->array[vec->length - 1];
    vec->array[--vec->length] = NULL;
    return ret;
}

/* Frees the contents of the vector, if it was malloc'd */
void stw_vector_free_content(stw_vector *vec) {
    size_t i;
    for (i = 0; i < vec->length; ++i) {
        free(vec->array[i]);
    }
}

/* Frees the contents of the vector, via a supplied free function. */
void stw_vector_free_content_by(stw_vector *vec, void (*free_func)(void**)) {
    size_t i;
    for (i = 0; i < vec->length; ++i) {
        free_func(&vec->array[i]);
    }
}

/* Frees the vector. */
void stw_vector_free(stw_vector *vec, bool free_content) {
    if (free_content) stw_vector_free_content(vec);
    free(vec->array);
    free(vec);

    vector_free_keeper--;
}

void stw_vector_check_leaks() {
    printf("Number of Vector leaks: %d", vector_free_keeper);
}

/*                    LINKED LIST                    */

stw_list *stw_list_make(void) {
    stw_list *list = malloc(sizeof(stw_list));
    list->length = 0;
    list->start = NULL;
    list->curr = NULL;

    return list;
}

void stw_list_free(stw_list *list, bool free_elements) {
    stw_list_node *node = NULL;
    assert(list);
    
    while (list->start) {
        node = list->start->next;
        if (free_elements) {
            free(list->start->value);
        }
        free(list->start);
        list->start = node;
    }
    free(list);
}

void *stw_list_get(stw_list *list, size_t index) {
    stw_list_node *current = NULL;
    size_t i = 0;
    
    assert(list);
    assert(index < list->length);

    current = list->start;
    while (current) {
        if (i == index) {
            return current->value;
        }
        
        current = current->next;
        ++i;
    }

    return NULL;
}

void stw_list_push(stw_list *list, void *ptr) {
    assert(list);
    list->length++;

    if (!list->curr) {
        list->curr = malloc(sizeof(stw_list_node));
        list->curr->next = NULL;
        list->curr->value = ptr;
        list->start = list->curr;
        return;
    }

    list->curr->next = malloc(sizeof(stw_list_node));
    list->curr = list->curr->next;
    list->curr->next = NULL;
    list->curr->value = ptr;
}

void *stw_list_pop(stw_list *list) {
    void *value;
    size_t bytes;
    assert(list);
    
    bytes = sizeof(list->curr->value);
    value = malloc(bytes);
    memcpy(value, list->curr->value, bytes);
    
    return value;
}

/*                    FILE STUFF                    */

long stw_file_length(FILE *file) {
    long length;
    
    long pos = ftell(file);
    fseek(file, 0, SEEK_END);
    length = ftell(file);
    fseek(file, pos, SEEK_SET);

    return length;
}

char *stw_readfile(char *filepath, size_t *o_length) {
    FILE *file = fopen(filepath, "r");

    size_t length = stw_file_length(file)+1;
    char *buffer = calloc(length, 1);
    length = fread(buffer, 1, length, file);
    if (o_length != NULL) {
        *o_length = length;
    }

    fclose(file);
    
    return buffer;
}

/* 
 * Returns a vector of the list of files in the specified directory.
 * `free_content` flag must be set in stw_vector_free() method.
 */
stw_vector *stw_listfiles(char *directory) {
    DIR *dir;
    struct dirent *ent;
    stw_vector *vec = stw_vector_make();
 
    if ((dir = opendir(directory)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            char *str = malloc(sizeof(ent->d_name));
            strcpy(str, ent->d_name);
            stw_vector_push(vec, str);
        }

        closedir(dir);
    } else {
        fprintf(stderr, "Error reading directory \"%s\".", directory);
        return NULL;
    }

    return vec;
}

/*                    UTILS                     */

/* Sorts an array of integers, in ascending order. */
void stw_sort_ascend(int *array, size_t count) {
    size_t i, j;
    int temp;

    for (i = 0; i < count; ++i) {
        for (j = i+1; j < count; ++j) {
            if (array[j] < array[i]) {
                temp = array[i];
                array[i] = array[j];
                array[j] = temp;
            }
        }
    }
}

/* Sorts an array in descending order. */
void stw_sort_descend(int *array, size_t count) {
    size_t i, j;
    int temp;

    for (i = 0; i < count; ++i) {
        for (j = i+1; j < count; ++j) {
            if (array[j] > array[i]) {
                temp = array[i];
                array[i] = array[j];
                array[j] = temp;
            }
        }
    }
}

/*                    TIMERS                    */

static clock_t stw_global_clock = -1;

/* Starts the global timer. */
void stw_timer_start(void) {
    stw_global_clock = clock();
}

/* Stops the global timer, returns the amount of time since clock started. */
double stw_timer_stop(void) {
    clock_t end;

    /* Assert if timer hasn't started yet. */
    assert(stw_global_clock != -1);
    
    end = clock();
    stw_global_clock = -1;

    return (double)(end - stw_global_clock) / (double)CLOCKS_PER_SEC;
}

/*                    STRING                    */

static size_t string_free_keeper = 0;

stw_string *string_make(char *start) {
    stw_string *str = malloc(sizeof(stw_string));
    if (start != NULL) {
        str->length = strlen(start);
        str->capacity = str->length * 2;
        str->str = malloc(str->capacity + 1);
        strcpy(str->str, start);
    } else {
        str->length = 0;
        str->capacity = 16;
        str->str = malloc(str->capacity + 1);
    }

    string_free_keeper++;

    return str;
}

stw_string *string_copy(stw_string *dest, stw_string *src) {
    dest = string_make(src->str);
    return dest;
}

void string_append(stw_string *str, char *app) {
    str->length += strlen(app);
    if (str->length >= str->capacity) {
        str->capacity *= 2;
        str->str = realloc(str->str, str->capacity + 1);
    }
    strcat(str->str, app);
}

char *string_substr(stw_string *str, size_t start, size_t end) {
    char *sub = malloc(end-start + 1);
    strcpy(sub, str->str + start);

    return sub;
}

void string_remove_index(stw_string *str, size_t index) {
    size_t i;
    for (i = index+1; i < str->length-1; ++i) {
        str->str[i] = str->str[i+1];
    }
}

void string_print(stw_string *str) {
    puts(str->str);
}

void string_free(stw_string *str) {
    free(str->str);
    free(str);
    string_free_keeper--;
}

void string_check_leaks() {
    printf("Number of outstanding frees to do with strings: %d\n", string_free_keeper);
}

size_t stw_strlen(const char *str) {
    size_t len = 0;
    while (str[len++]);
    return len-1;
}

/* Find the amount of occurances a character has in a string. */
size_t stw_string_occurance(const char *str, const char find) {
    size_t occurances = 0;
    while (*str) {
        if (*str == find) {
            occurances++;
        }
        str++;
    }

    return occurances;
}

stw_vector *stw_splitstring_vector(char *str, char delim) {
    stw_vector *vector = stw_vector_make();
    size_t length = strlen(str);

    char *current_token = malloc(10);
    char *current_token_dup = NULL;
    size_t index = 0;

    size_t i;
    for (i = 0; i < length; ++i) {
        if (i == length-1) {
            current_token[index++] = str[i];
        }
        if (str[i] == delim || i == length-1) {
            current_token[index] = 0;
            index = 0;

            current_token_dup = malloc(length);
            strcpy(current_token_dup, current_token);
            stw_vector_push(vector, current_token_dup);

            memset(current_token, 0, length);
        } else {
            current_token[index++] = str[i];
        }
    }

    free(current_token);
    return vector;
}

/*                    RANDOM                    */

int stw_rand(int end) {
    return end;
}
int stw_rand_total(void) {
    return 0;
}
float stw_randf(float end) {
    return end;
}
bool stw_randb(void) {
    return false;
}

/*                    MATHS                    */

bool stw_is_prime(u32 a) {
    u32 j = 0;

    if (a <= 1) return false;
    if (a == 2 || a == 3 || a == 5) return true;
    if (a % 2 == 0 || a % 3 == 0) return false;
    
    for (j = a/2; j > 1; --j) {
        if (a % j == 0) return false;
    }
    return true;
}
