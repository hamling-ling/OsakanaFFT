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
    __global float2* sample,
    __global float2* output)
{
    // 0, 1, 2, 3, ...
    const int global_idx   = get_global_id(0);
    // 0, 4, 2, 6, ... for logN=4
    const int ridx = reverse(log2N, global_idx);

    // copy local buf to ouput buffer
    output[global_idx] = sample[ridx];
}

/**
 * FFT Main function
 */
__kernel void clfft(
    const    int     N,
    const    int     log2N,
    const    int     stage,
    __global float2* x)
{
    const int gid   = get_global_id(0);

    // 0, 2, 4, 6, ...
    const int idx_up = gid;

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
        //printf("gid=%d, stage=%d, twiddle=(%f,%f)\n", global_idx, stage, tf.x, tf.y);
        float2 up = x[idx_up];
        float2 dn = x[idx_dn];

        float2 upbak = up;
        float2 dnbak = dn;
        butterfly(&tf, &up, &dn);
        //printf("gid=%d, st=%d, tf=%f+%fi\n", gid, stage, tf.x, tf.y);
        //printf("gid=%d, st=%d, [%d]x[%d] %f, %f => %f, %f\n",
        //       gid, stage, idx_up, idx_dn,
        //       upbak.x, dnbak.x, up.x, dn.x);
        x[idx_up] = up;
        x[idx_dn] = dn;
        //printf("gid=%d, st=%d, [%d]=%f, [%d]=%f\n",
        //       gid, stage, idx_up, idx_dn,
        //       up.x, dn.x);
    }
}
