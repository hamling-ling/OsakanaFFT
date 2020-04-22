#define M_PI           (3.14159265358979323846f)
#define M_2PI          (2.0f*3.14159265358979323846f)

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
float2 twiddle(int n, int Nin)
{
	const float theta = (M_2PI * n / Nin);
	float2 ret;
	ret.x = cos(theta);
	ret.y = -sin(theta);

	return ret;
}

__kernel void clfft(
    const    int    halfN,
    const    int    log2N,
    __global float2* sample,
    __global float2* output,
    __local  float2* locbuf)
{
    const int N = halfN << 1;
    // 0, 1, 2, 3, ...
    unsigned const int global_idx   = get_global_id(0);

    // 0, 2, 4, 6, ...
    const int idx2x0 = global_idx * 2;
    // 1, 3, 5, 7, ...
    const int idx2x1 = global_idx * 2 + 1;
    // 0, 4, 2, 6, ... for logN=4
    const int rdx2x0 = reverse(log2N, idx2x0);
    // 8, 12, 10, 14, ... for logN=4
    const int rdx2x1 = reverse(log2N, idx2x1);

    if (global_idx < halfN) {
        // for debug, return bit reverse order
        //output[idx2x0].x = sample[rdx2x0].x;
        //output[idx2x1].x = sample[rdx2x1].x;

        // for debug, return twittle factors
        output[idx2x0] = twiddle(idx2x0, N);
        output[idx2x1] = twiddle(idx2x1, N);
    }
}
