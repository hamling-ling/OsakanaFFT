#define M_PI           (3.14159265358979323846f)
#define M_2PI          (2.0f*M_PI)

/**
    Compute bit-reverse permutation
 */
unsigned int reverse(unsigned int width, unsigned int x)
{
    x = (x & 0x55555555) <<  1 | (x >>  1) & 0x55555555;
    x = (x & 0x33333333) <<  2 | (x >>  2) & 0x33333333;
    x = (x & 0x0F0F0F0F) <<  4 | (x >>  4) & 0x0F0F0F0F;
    x = (x << 24) | ((x & 0xFF00) << 8) | ((x >> 8) & 0xFF00) | (x >> 24);
    x = x >> (32-width);
    return x;
}

/**
    Compute Twiddle factor for n
    W^n_N = exp(-i2pin/N)
    = cos(2 pi n/N) - isin(2 pi n/N)
 */
float2 twiddle(const int n, const int Nin)
{
    const float theta = (M_2PI * n / Nin);
    float2      ret;
    ret.x = cos(theta);
    ret.y = -sin(theta);

    return ret;
}

float2 complex_mult(const float2* a, const float2* b)
{
    float2 ret;
    ret.x = a->x * b->x - a->y * b->y;
    ret.y = a->x * b->y + a->y * b->x;
    return ret;
}

/**
    rup = up + tf * dn;
    rdn = up - tf * dn;
 */
void butterfly(const float2* tf, float2* up, float2* dn)
{
    float4 tfdn;
    tfdn.xy = complex_mult(tf, dn);
    tfdn.zw = -tfdn.xy;

    float4 up4;
    up4.xy = *up;
    up4.zw = *up;

    float4 r = up4 + tfdn;
    *up     = r.xy;
    *dn     = r.zw;
}

/**
 * Bit Reverse Order function
 */
__kernel void bitrevese(
    const    int     log2N,
    __global float*  sample,
    __global float2* output)
{
    // 0, 1, 2, 3, ...
    const int global_idx   = get_global_id(0);
    // 0, 4, 2, 6, ... for logN=4
    const int ridx = reverse(log2N, global_idx);

    // copy local buf to ouput buffer
    float2 out;
    out.x = sample[ridx];
    out.y = 0.0f;
    output[global_idx] = out;
}

bool get_butterly_vals(
                       const int log2N,
                       const int stage,
                       const int idx_up,
                       int*      idx_dn,
                       float2*   tf)
{
    // num of islands, group of adjacent butterflies
    const int islands = log2N >> stage;
    // island size, 2, 4, 8, ... for stage 0, 1, 2, ...
    const int ilsize  = 2 << stage;
    // butterfly distance, 1, 2, 4, ... for stage 0, 1, 2, ...
    const int dist    = 1 << stage;
    // index in island
    const int il_idx = idx_up % ilsize;
    // if idx_up is upper half in belonging island, compute butterfly
    if(il_idx < ilsize / 2) {
        // 1, 2, 4, ... for idx_up=0
        *idx_dn  = idx_up + dist;
        *tf      = twiddle(il_idx, ilsize);
        //printf("idx_up=%d, idx_dn=%d for il_idx=%d\n", idx_up, *idx_dn, il_idx);
        return true;
    } else {
        // return something not affect
        *idx_dn  = idx_up;
        tf->x    = 0.0f;
        tf->y    = 0.0f;
        //printf("idx_up=%d, idx_dn=%d for il_idx=%d\n", idx_up, *idx_dn, il_idx);
        return false;
    }
}

/**
 * FFT Main function
 */
__kernel void clfft_single_stage(
    const    int     N,
    const    int     log2N,
    const    int     stage,
    __global float2* x)
{
    const int idx_up   = get_global_id(0);
    int       idx_dn   = 0;
    float2    tf;
    get_butterly_vals(log2N, stage, idx_up, &idx_dn, &tf);

    float2 up = x[idx_up];
    float2 dn = x[idx_dn];
    butterfly(&tf, &up, &dn);
    x[idx_up] = up;
    x[idx_dn] = dn;
}

/**
 * FFT Main function
 */
__kernel void clfft_multi_stages(
    const    int     N,
    const    int     log2N,
    const    int     stage_start,
    const    int     stage_num,
    __global float2* x,
    __local  float2* locbuf)
{
    const int group_id    = get_group_id(0);
    const int local_size  = get_local_size(0);
    const int local_id    = get_local_id(0);

    event_t evt_copy_in = async_work_group_copy(locbuf,
                                                x + group_id * local_size,
                                                local_size,
                                                0);

    const int global_id   = get_global_id(0);
    const int global_size = get_global_size(0);

    const int idx_up   = global_id;
    const int stage_end = stage_start + stage_num;

    wait_group_events(1, &evt_copy_in);

    for(int stage = stage_start; stage < stage_end; stage++) {

        int    idx_dn  = 0;
        float2 tf;
        bool   proceed = get_butterly_vals(log2N, stage, idx_up, &idx_dn, &tf);

        barrier(CLK_LOCAL_MEM_FENCE);

        const int idx_up_loc = idx_up % local_size;
        const int idx_dn_loc = idx_dn % local_size;

        float2 up = locbuf[idx_up_loc];
        float2 dn = locbuf[idx_dn_loc];

        butterfly(&tf, &up, &dn);

        barrier(CLK_LOCAL_MEM_FENCE);
        if(proceed) {
            locbuf[idx_up_loc] = up;
            locbuf[idx_dn_loc] = dn;
        }
    }

    barrier(CLK_LOCAL_MEM_FENCE);
    event_t evt_copy_out = async_work_group_copy(x + local_size * group_id,
                                                 locbuf,
                                                 local_size,
                                                 0);
    wait_group_events(1, &evt_copy_out);
}
