/* csi_fp.h - CSI amplitude + phase-slope fingerprinting and clustering.
 * Built across Steps 20-26.
 */
#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "protocol.h"

#define N_SUB        52     /* usable LLTF subcarriers in HT20 */
#define RING_DEPTH   16     /* CSI frames averaged per fingerprint */
#define MIN_FRAMES    5     /* below this, alpha stays at the prior */

extern float g_tau_amp;     /* calibrate in Step 24 */
extern float g_tau_cfo;     /* calibrate in Step 24 */

void  csi_fp_init(void);
void  csi_fp_push(int slot, const int8_t *buf, int len);
bool  csi_fp_ready(int slot);
void  csi_fp_amp(int slot, float *out_n_sub);
float csi_fp_cfo(int slot);
float csi_fp_cosine(const float *a, const float *b);
void  csi_fp_update_trust(void);
int   csi_fp_cluster_size(int slot);
