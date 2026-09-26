/* wmsr.c - neighbour table + trust-weighted W-MSR consensus.
 *
 * Build this up step by step:
 *   Step 8  : neighbour table + staleness eviction
 *   Step 9  : plain averaging
 *   Step 10 : sensor anchor (lambda term)
 *   Step 11 : W-MSR trim (sort value/weight pairs, drop F hi + F lo)
 *   Step 27 : read per-neighbour alpha when defence is on
 *
 * Reference implementation is in the build guide's companion listing.
 * Start from the guide, hit the bugs yourself, keep this as your copy.
 */
#include "wmsr.h"

/* TODO Step 8: static neighbor_t g_nb[MAX_TRACKED]; mutex; slot_of() ... */
