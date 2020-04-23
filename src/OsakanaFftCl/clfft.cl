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

/**
	rup = up + tf * dn;
	rdn = up - tf * dn;
 */
void butterfly(const float2* tf, float2* up, float2* dn)
{
    float4 tf4;
    tf4.xy = *tf;
    tf4.zw = *tf;

    float4 dn4;
    dn4.xy = *dn;
    dn4.zw = -(*dn);

    float4 tfdn = tf4 * dn4;
    float4 up4;
    up4.xy = *up;
    up4.zw = *up;

    float4 r = up4 + tfdn;
    *up     = r.xy;
    *dn     = r.zw;
}

/**
 * FFT Main functions
 */
__kernel void clfft(
    const    int     N,
    const    int     log2N,
    __global float2* sample,
    __global float2* output,
    __local  float2* locbuf)
{
    // 0, 1, 2, 3, ...
    unsigned const int global_idx   = get_global_id(0);
    // 0, 4, 2, 6, ... for logN=4
    const int ridx = reverse(log2N, global_idx);

    // bit reverse orderd data
    locbuf[global_idx] = sample[ridx];
    barrier(CLK_LOCAL_MEM_FENCE);

    // debug
    //output[global_idx].x = -1;
    //output[global_idx].y = -1;

    // 0, 2, 4, 6, ...
    const int idx_up = global_idx;
    for(int stage = 0; stage < log2N; stage++){
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
            const int    idx_dn  = idx_up + dist;
            const float2 tf      = twiddle(il_idx, ilsize);

            float2 up = locbuf[idx_up];
            float2 dn = locbuf[idx_dn];
            butterfly(&tf, &up, &dn);
            locbuf[idx_up] = up;
            locbuf[idx_dn] = dn;
        }

        barrier(CLK_LOCAL_MEM_FENCE);
    }

    // copy local buf to ouput buffer
    output[global_idx] = locbuf[global_idx];
}
