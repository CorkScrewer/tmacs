#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdint.h>

/*                   TYPEDEFS & MACROS                   */

typedef int bool;
#define false 0
#define true  1

typedef int8_t   i8 ;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef uint8_t  u8 ;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define I8_MIN INT8_MIN
#define I16_MIN INT16_MIN
#define I32_MIN INT32_MIN
#define I64_MIN INT64_MIN

#define I8_MAX INT8_MAX
#define I16_MAX INT16_MAX
#define I32_MAX INT32_MAX
#define I64_MAX INT64_MAX

#define U8_MAX UINT8_MAX
#define U16_MAX UINT16_MAX
#define U32_MAX UINT32_MAX
#define U64_MAX UINT64_MAX


#ifndef M_PI
#define M_PI 3.141592653F
#endif

#define RAD_TO_DEG 57.295779513

/*                    STW_VECTOR                    */

typedef struct {
    size_t length, capacity;
    void **array;
} stw_vector;

stw_vector *stw_vector_make(void);
void stw_vector_push(stw_vector *vec, void *v);
void stw_vector_add(stw_vector *vec, size_t index, void *v);
void stw_vector_remove(stw_vector *vec, size_t index);
int  stw_vector_find(stw_vector *vec, void *v);
void *stw_vector_pop(stw_vector *vec);
void stw_vector_free_content(stw_vector *vec);
void stw_vector_free_content_by(stw_vector *vec, void (*free_func)(void**));
void stw_vector_free(stw_vector *vec, bool free_content);
void stw_vector_check_leaks();

/*                    LINKED LIST                    */

#include <memory.h>

typedef struct stw_list_node {
    struct stw_list_node *next;
    void *value;
} stw_list_node;

typedef struct {
    size_t length;
    stw_list_node *start, *curr;
} stw_list;

stw_list *stw_list_make(void);
void stw_list_free(stw_list *list, bool free_elements);
void *stw_list_get(stw_list *list, size_t index);
void stw_list_push(stw_list *list, void *ptr);
void *stw_list_pop(stw_list *list);

/*                    FILE STUFF                    */

#include <dirent.h> /* For stw_listfiles */


long stw_file_length(FILE *file);
char *stw_readfile(char *filepath, size_t *o_length);
stw_vector *stw_listfiles(char *directory);

/*                    UTILS                     */

void stw_sort_ascend(int *array, size_t count);
void stw_sort_descend(int *array, size_t count);

/*                    TIMERS                    */

#include <time.h>

void stw_timer_start(void);
double stw_timer_stop(void);

/*                    STRING                    */

#include <string.h>

typedef struct {
    size_t length, capacity;
    char *str;
} stw_string;

stw_string *string_make(char *start);
stw_string *string_copy(stw_string *dest, stw_string *src);
void string_append(stw_string *str, char *app);
char *string_substr(stw_string *str, size_t start, size_t end);
void string_remove_index(stw_string *str, size_t index);
void string_print(stw_string *str);
void string_free(stw_string *str);
void string_check_leaks();

size_t stw_strlen(const char *str);
size_t stw_string_occurance(const char *str, const char find);
stw_vector *stw_splitstring_vector(char *str, char delim);

/*                    RANDOM                    */

int stw_rand(int end);
int stw_rand_total(void);
float stw_randf(float end);
bool stw_randb(void);

/*                    MATHS                    */

bool stw_is_prime(size_t a);
