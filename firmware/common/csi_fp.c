/* csi_fp.c - fingerprints and Sybil clustering.
 *
 * Build order:
 *   Step 19 : verify the raw CSI buffer layout on YOUR hardware first
 *   Step 20 : amplitude vector per subcarrier
 *   Step 21 : median across a ring + L2 normalise
 *   Step 22 : cosine similarity between two fingerprints
 *   Step 23 : phase-slope (CFO-ish) feature
 *   Step 25 : cluster identities by connected components
 *   Step 26 : hard exclusion (alpha = 0 for clusters of size > 1)
 *
 * The subcarrier map (SUB_IDX) is a PREDICTION until Step 19 confirms it
 * on your IDF version. Your captured CSI already shows the guard-band
 * zeros - use what you observed, not what any guide assumes.
 */
#include "csi_fp.h"

float g_tau_amp = 0.85f;   /* placeholder; real value from Step 24 */
float g_tau_cfo = 0.02f;   /* placeholder; real value from Step 24 */

/* TODO Step 20+: SUB_IDX table, ring buffer, median, normalise ... */
