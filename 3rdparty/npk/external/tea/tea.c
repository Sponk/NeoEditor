#include <stddef.h>

void tea_decode(int* v,int* k)
{
	unsigned int n=32, sum, y=v[0], z=v[1],
		delta=0x9e3779b9 ;
	sum=delta<<5 ;
	/* start cycle */
	while (n-->0) {
		z-= (y<<4)+k[2] ^ y+sum ^ (y>>5)+k[3] ;
		y-= (z<<4)+k[0] ^ z+sum ^ (z>>5)+k[1] ;
		sum-=delta ;  }
	/* end cycle */
	v[0]=y ; v[1]=z ;
}

void tea_decode_byte(char* v, int* k, int p)
{
	char y[] = "NpK!TeA";
    *v = *v^(char)(k[p%4]%0xFF)^y[p];
}

void tea_decode_buffer(char* in_buffer, unsigned int in_size, int* key, int cipherRemains)
{
	char *p;
	unsigned int remain = in_size % 8;
	unsigned int align_size = in_size - remain;
	for (p = in_buffer; p < in_buffer + align_size; p += 8)
		tea_decode( (int*)p, key);
    if( remain > 0 && cipherRemains )
        for (p = in_buffer + align_size; p < in_buffer + in_size; p += 1)
            tea_decode_byte( p, key, --remain );
}
