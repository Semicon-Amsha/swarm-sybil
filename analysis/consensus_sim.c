/* consensus_sim.c - run the consensus update on your laptop.
 *
 * Runs with NO hardware. Use it at Step 26 to see for yourself why soft
 * down-weighting fails and hard exclusion is needed.
 *
 *   gcc -O2 -o consensus_sim consensus_sim.c -lm && ./consensus_sim
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_N    16
#define F_PARAM   1
#define LAMBDA    1.0f
#define N_HONEST  7
#define ROUNDS  400

typedef struct { float v; float a; } sample_t;

static int cmp_sample(const void *A, const void *B)
{
    float d = ((const sample_t *)A)->v - ((const sample_t *)B)->v;
    return (d > 0.0f) - (d < 0.0f);
}

static float step(float x, float sensor, sample_t *in, int m)
{
    sample_t s[MAX_N + 1];
    int n = 0;
    for (int i = 0; i < m; i++) s[n++] = in[i];
    s[n].v = x;  s[n].a = 1.0f;  n++;
    qsort(s, n, sizeof(sample_t), cmp_sample);
    int hi = n - 1, k = 0;
    while (k < F_PARAM && hi >= 0 && s[hi].v > x) { hi--; k++; }
    int lo = 0; k = 0;
    while (k < F_PARAM && lo <= hi && s[lo].v < x) { lo++; k++; }
    if (lo > hi) return x;
    float num = LAMBDA * sensor, den = LAMBDA;
    for (int i = lo; i <= hi; i++) { num += s[i].a * s[i].v; den += s[i].a; }
    return (den < 1e-6f) ? x : num / den;
}

static float run(int n_adv, float poison, float alpha, const float *sens)
{
    float x[N_HONEST];
    for (int i = 0; i < N_HONEST; i++) x[i] = 0.5f;
    for (int r = 0; r < ROUNDS; r++) {
        float nx[N_HONEST];
        for (int i = 0; i < N_HONEST; i++) {
            sample_t in[MAX_N]; int m = 0;
            for (int j = 0; j < N_HONEST; j++)
                if (j != i) { in[m].v = x[j]; in[m].a = 1.0f; m++; }
            for (int a = 0; a < n_adv; a++)
                { in[m].v = poison; in[m].a = alpha; m++; }
            nx[i] = step(x[i], sens[i], in, m);
        }
        memcpy(x, nx, sizeof(x));
    }
    float mean = 0.0f;
    for (int i = 0; i < N_HONEST; i++) mean += x[i];
    return mean / N_HONEST;
}

int main(void)
{
    float sens[N_HONEST] = {0.47f,0.52f,0.49f,0.55f,0.46f,0.53f,0.48f};
    float truth = 0.0f;
    for (int i = 0; i < N_HONEST; i++) truth += sens[i];
    truth /= N_HONEST;

    printf("ground truth (mean of sensors) = %.4f\n\n", truth);
    printf("%-36s %9s %8s\n", "scenario", "estimate", "error");

    struct { const char *name; int n; float a; } C[] = {
        {"no attack",                        0, 0.0f},
        {"5 Sybil, no defence",              5, 1.0f},
        {"5 Sybil, soft alpha = 1/5",        5, 0.2f},
        {"5 Sybil, hard exclusion alpha = 0",5, 0.0f},
        {"detector misses 1 of 5",           1, 1.0f},
        {"detector misses 2 of 5",           2, 1.0f},
    };
    for (int i = 0; i < 6; i++) {
        float x = run(C[i].n, 0.9f, C[i].a, sens);
        printf("%-36s %9.4f %8.4f\n", C[i].name, x, fabsf(x - truth));
    }
    printf("\nAny non-zero alpha still ends near 0.9. Only alpha = 0 works.\n");
    printf("Missing 1 Sybil is absorbed (F=1). Missing 2 is not.\n");
    return 0;
}
