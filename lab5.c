#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct header {
  uint64_t size;
  struct header *next;
  int id;
};

void initialize_block(struct header *block, uint64_t size, struct header *next,
                      int id) {
  block->size = size;
  block->next = next;
  block->id = id;
}

int find_first_fit(struct header *free_list_ptr, uint64_t size) {
  // Traverse the free list and return the ID of the first block that fits
  struct header *current = free_list_ptr;

  while (current != NULL) {
    if (current->size >= size) {
      return current->id;
    }
    current = current->next;
  }

  return -1; // No suitable block found
}

int find_best_fit(struct header *free_list_ptr, uint64_t size) {
  int best_fit_id = -1;
  uint64_t smallest_sufficient_size = UINT64_MAX;

  struct header *current = free_list_ptr;

  // Traverse the entire list to find the smallest block that fits
  while (current != NULL) {
    if (current->size >= size && current->size < smallest_sufficient_size) {
      smallest_sufficient_size = current->size;
      best_fit_id = current->id;
    }
    current = current->next;
  }

  return best_fit_id;
}

int find_worst_fit(struct header *free_list_ptr, uint64_t size) {
  int worst_fit_id = -1;
  uint64_t largest_sufficient_size = 0;

  struct header *current = free_list_ptr;

  // Traverse the entire list to find the largest block that fits
  while (current != NULL) {
    if (current->size >= size && current->size > largest_sufficient_size) {
      largest_sufficient_size = current->size;
      worst_fit_id = current->id;
    }
    current = current->next;
  }

  return worst_fit_id;
}

int main(void) {

  struct header *free_block1 = (struct header *)malloc(sizeof(struct header));
  struct header *free_block2 = (struct header *)malloc(sizeof(struct header));
  struct header *free_block3 = (struct header *)malloc(sizeof(struct header));
  struct header *free_block4 = (struct header *)malloc(sizeof(struct header));
  struct header *free_block5 = (struct header *)malloc(sizeof(struct header));

  initialize_block(free_block1, 6, free_block2, 1);
  initialize_block(free_block2, 12, free_block3, 2);
  initialize_block(free_block3, 24, free_block4, 3);
  initialize_block(free_block4, 8, free_block5, 4);
  initialize_block(free_block5, 4, NULL, 5);

  struct header *free_list_ptr = free_block1;

  int first_fit_id = find_first_fit(free_list_ptr, 7);
  int best_fit_id = find_best_fit(free_list_ptr, 7);
  int worst_fit_id = find_worst_fit(free_list_ptr, 7);

  // Print out the IDs
  printf("The ID for First-Fit algorithm is: %d\n", first_fit_id);
  printf("The ID for Best-Fit algorithm is: %d\n", best_fit_id);
  printf("The ID for Worst-Fit algorithm is: %d\n", worst_fit_id);

  // Clean up allocated memory
  free(free_block1);
  free(free_block2);
  free(free_block3);
  free(free_block4);
  free(free_block5);

  return 0;
}

/*
 * Part 2: Pseudo-code Algorithm for Coalescing Contiguous Free Blocks
 *
 * When a block is freed, we need to check if adjacent blocks are also free
 * and merge them into a single larger block to reduce fragmentation.
 *
 * ALGORITHM: Coalesce_Free_Blocks(newly_freed_block)
 *
 * INPUT: A pointer to the newly freed block
 * OUTPUT: The free list with coalesced blocks
 *
 * STEPS:
 *
 * 1. Add the newly freed block to the free list in address order
 *    - Traverse the free list to find the correct position
 *    - Insert the block so that blocks are ordered by memory address
 *
 * 2. Check for coalescing with the NEXT block:
 *    - Calculate the end address of the newly freed block:
 *      end_address = newly_freed_block_address + newly_freed_block_size
 *    - If next block exists AND end_address == next_block_address:
 *      a. Merge blocks: newly_freed_block.size += next_block.size
 *      b. Update link: newly_freed_block.next = next_block.next
 *      c. Remove next_block from the free list
 *
 * 3. Check for coalescing with the PREVIOUS block:
 *    - Find the block that comes before the newly freed block in the list
 *    - Calculate the end address of the previous block:
 *      prev_end_address = prev_block_address + prev_block_size
 *    - If prev block exists AND prev_end_address == newly_freed_block_address:
 *      a. Merge blocks: prev_block.size += newly_freed_block.size
 *      b. Update link: prev_block.next = newly_freed_block.next
 *      c. Remove newly_freed_block from the free list
 *
 * 4. Return the updated free list
 *
 * EXAMPLE with the given memory layout (after block z is freed):
 *
 * Before freeing z:
 * Free list: [block_a] -> [block_b] -> NULL
 * Allocated: [block_z] (between a and b in memory)
 *
 * After freeing z:
 * 1. Insert z into free list in address order: [a] -> [z] -> [b]
 * 2. Check if a and z are contiguous:
 *    - If (a.address + a.size == z.address), merge a and z
 *    - Result: [a_merged] -> [b], where a_merged.size = a.size + z.size
 * 3. Check if a_merged and b are contiguous:
 *    - If (a_merged.address + a_merged.size == b.address), merge them
 *    - Result: [a_b_z_merged], where size = a.size + z.size + b.size
 *
 * Final result: One large contiguous free block instead of three separate ones
 *
 * NOTES:
 * - Coalescing reduces external fragmentation
 * - Maintaining address order in the free list makes coalescing easier
 * - Both forward and backward coalescing should be checked
 * - This creates larger blocks that can satisfy bigger allocation requests
 */
