/* wmsr.h - neighbour table + trust-weighted W-MSR consensus.
 * Built across Steps 8-11 and 27. See docs/ build guide.
 */
#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "protocol.h"

typedef struct {
    uint8_t  id[6];
    float    state;
    float    alpha;        /* trust weight, 1.0 until Step 27 */
    int64_t  last_seen_us;
    uint32_t pkt_count;
    bool     valid;
} neighbor_t;

void  wmsr_init(float x0, float sensor);
void  wmsr_on_packet(const swarm_pkt_t *p);
void  wmsr_step(void);
float wmsr_state(void);
float wmsr_sensor(void);
void  wmsr_set_defense(bool on);
bool  wmsr_defense(void);
int   wmsr_slot_of(const uint8_t id[6]);
void  wmsr_set_alpha(int slot, float a);
bool  wmsr_slot_active(int slot);
void  wmsr_snapshot(neighbor_t *out, int max, int *n_out);
int   wmsr_active_count(void);
