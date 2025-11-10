#include "uthash.h"
#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Hash table structure
typedef struct {
  char word[50];
  int count;
  UT_hash_handle hh;
} word_count_t;

// Global hash table and mutex
word_count_t *word_counts = NULL;
pthread_mutex_t count_mutex; // Thread arguments structure
typedef struct {
  char *text;
  int start;
  int end;
  pthread_mutex_t *mutex;
} thread_args_t;

// Function to normalize a word (convert to lowercase)
void normalize_word(char *word) {
  for (int i = 0; word[i]; i++) {
    word[i] = tolower(word[i]);
  }
}
void add_word(char *word) {
  word_count_t *entry;

  HASH_FIND_STR(word_counts, word, entry);
  if (entry == NULL) {
    entry = (word_count_t *)malloc(sizeof(word_count_t));
    strcpy(entry->word, word);
    entry->count = 0;
    HASH_ADD_STR(word_counts, word, entry);
  }
  entry->count++;
}
void add_word_counts_in_chunk(char *text, int start, int end,
                              pthread_mutex_t *mutex) {
  char word[50];
  int word_idx = 0;

  for (int i = start; i < end; i++) {
    if (isalpha(text[i])) {
      word[word_idx++] = text[i];
    } else if (word_idx > 0) {
      word[word_idx] = '\0';
      normalize_word(word);

      // TASK 4: Lock mutex before accessing shared hash table
      pthread_mutex_lock(mutex);
      add_word(word);
      pthread_mutex_unlock(mutex);

      word_idx = 0;
    }
  }
  if (word_idx > 0) {
    word[word_idx] = '\0';
    normalize_word(word);

    pthread_mutex_lock(mutex);
    add_word(word);
    pthread_mutex_unlock(mutex);
  }
}
void *counter_thread_func(void *args) {
  thread_args_t *thread_args = (thread_args_t *)args;
  add_word_counts_in_chunk(thread_args->text, thread_args->start,
                           thread_args->end, thread_args->mutex);
  return NULL;
}
thread_args_t *pack_args(char *text, int start, int end,
                         pthread_mutex_t *mutex) {
  thread_args_t *args = (thread_args_t *)malloc(sizeof(thread_args_t));
  args->text = text;
  args->start = start;
  args->end = end;
  args->mutex = mutex;
  return args;
}
void count_words_seq(char *text, int text_len) {
  add_word_counts_in_chunk(text, 0, text_len, NULL);
}
void count_words_parallel(char *text, int text_len, int num_threads) {
  pthread_t threads[num_threads];
  thread_args_t *threads_args[num_threads];

  // TASK 2: Initialize the mutex
  pthread_mutex_init(&count_mutex, NULL);

  int chunk_size = text_len / num_threads;

  // TASK 2: Create threads
  for (int i = 0; i < num_threads; i++) {
    int start = i * chunk_size;
    int end = (i == num_threads - 1) ? text_len : (i + 1) * chunk_size;

    // Initialize thread arguments
    threads_args[i] =
        pack_args(text, start, end, &count_mutex); // Launch thread
    pthread_create(&threads[i], NULL, counter_thread_func, threads_args[i]);
  }

  // TASK 2: Wait for all threads to finish
  for (int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
  }

  // TASK 2: Cleanup dynamically allocated memory
  for (int i = 0; i < num_threads; i++) {
    free(threads_args[i]);
  }

  // Destroy mutex
  pthread_mutex_destroy(&count_mutex);
}/ Comparison function for sorting
int word_count_sort(word_count_t *a, word_count_t *b) {
  return strcmp(a->word, b->word);
}

// Print the word counts
void print_word_counts() {
  word_count_t *entry, *tmp;

  printf("%-30s %s\n", "Word", "Count");
  HASH_ITER(hh, word_counts, entry, tmp) {
    printf("%-30s %d\n", entry->word, entry->count);
  }
}
void cleanup() {
  word_count_t *entry, *tmp;
  HASH_ITER(hh, word_counts, entry, tmp) {
    HASH_DEL(word_counts, entry);
    free(entry);
  }
}

int main() {
  char *text = "The quick brown fox jumps over the lazy dog. "
               "The quick brown fox jumps over the lazy dog.";
  int text_len = strlen(text);
  // Sequential version (comment out when using parallel)
  // count_words_seq(text, text_len);

  // TASK 2: Parallel version (uncomment to use)
  int num_threads = 3;
  count_words_parallel(text, text_len, num_threads);

  // TASK 1: Sort the hash table before printing
  HASH_SORT(word_counts, word_count_sort);

  // Print results
  print_word_counts();

  // Cleanup
  cleanup();

  return 0;
}
