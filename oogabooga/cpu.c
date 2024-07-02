// #Portability rip ARM
typedef struct Cpu_Info_X86 {
    u32 eax;
    u32 ebx;
    u32 ecx;
    u32 edx;
} Cpu_Info_X86;

typedef struct Cpu_Capabilities {
	bool sse1;
	bool sse2;
	bool sse3;
	bool ssse3;
	bool sse41;
	bool sse42;
	bool avx;
	bool avx2;
	bool avx512;
	
} Cpu_Capabilities;

///
// Compiler specific stuff
#if COMPILER_MVSC
    #include <intrin.h>
    #pragma intrinsic(__rdtsc)
    inline u64 rdtsc() {
        return __rdtsc();
    }
    inline Cpu_Info_X86 cpuid(u32 function_id) {
    	Cpu_Info_X86 i;
    	__cpuid((int*)&i, function_id);
    	return i;
    }
#elif COMPILER_GCC
    inline u64 rdtsc() {
        unsigned int lo, hi;
        __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
        return ((u64)hi << 32) | lo;
    }
    Cpu_Info_X86 info;
	    __asm__ __volatile__(
	        "cpuid"
	        : "=a"(info.eax), "=b"(info.ebx), "=c"(info.ecx), "=d"(info.edx)
	        : "a"(function_id), "c"(0));
	    return info;
	}
#elif COMPILER_CLANG
	#include <intrin.h>
    inline u64 rdtsc() {
        unsigned int lo, hi;
        __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
        return ((u64)hi << 32) | lo;
    }
    inline Cpu_Info_X86 cpuid(u32 function_id) {
    Cpu_Info_X86 info;
	    __asm__ __volatile__(
	        "cpuid"
	        : "=a"(info.eax), "=b"(info.ebx), "=c"(info.ecx), "=d"(info.edx)
	        : "a"(function_id), "c"(0));
	    return info;
	}
#else
    inline u64 rdtsc() { return 0; }
    inline Cpu_Info_X86 cpuid(u32 function_id) {return (Cpu_Info_X86){0};}
#endif


Cpu_Capabilities query_cpu_capabilities() {
    Cpu_Capabilities result = {0};

    Cpu_Info_X86 info = cpuid(1);

    result.sse1  = (info.edx & (1 << 25)) != 0;
    result.sse2  = (info.edx & (1 << 26)) != 0;
    result.sse3  = (info.ecx & (1 << 0)) != 0;
    result.ssse3 = (info.ecx & (1 << 9)) != 0;
    result.sse41 = (info.ecx & (1 << 19)) != 0;
    result.sse42 = (info.ecx & (1 << 20)) != 0;
    
    result.avx = (info.ecx & (1 << 28)) != 0;

    Cpu_Info_X86 ext_info = cpuid(7);
    result.avx2 = (ext_info.ebx & (1 << 5)) != 0;
    
    result.avx512 = (ext_info.ebx & (1 << 16)) != 0;

    return result;
}

void (*simd_add_float32_64 )(float32 *a, float32 *b, float32* result) = 0;
void (*simd_add_float32_128)(float32 *a, float32 *b, float32* result) = 0;
void (*simd_add_float32_256)(float32 *a, float32 *b, float32* result) = 0;
void (*simd_add_float32_512)(float32 *a, float32 *b, float32* result) = 0;
void (*simd_sub_float32_64 )(float32 *a, float32 *b, float32* result) = 0;
void (*simd_sub_float32_128)(float32 *a, float32 *b, float32* result) = 0;
void (*simd_sub_float32_256)(float32 *a, float32 *b, float32* result) = 0;
void (*simd_sub_float32_512)(float32 *a, float32 *b, float32* result) = 0;
void (*simd_mul_float32_64 )(float32 *a, float32 *b, float32* result) = 0;
void (*simd_mul_float32_128)(float32 *a, float32 *b, float32* result) = 0;
void (*simd_mul_float32_256)(float32 *a, float32 *b, float32* result) = 0;
void (*simd_mul_float32_512)(float32 *a, float32 *b, float32* result) = 0;
void (*simd_div_float32_64 )(float32 *a, float32 *b, float32* result) = 0;
void (*simd_div_float32_128)(float32 *a, float32 *b, float32* result) = 0;
void (*simd_div_float32_256)(float32 *a, float32 *b, float32* result) = 0;
void (*simd_div_float32_512)(float32 *a, float32 *b, float32* result) = 0;
void (*simd_add_int32_128)(s32 *a, s32 *b, s32* result) = 0;
void (*simd_add_int32_256)(s32 *a, s32 *b, s32* result) = 0;
void (*simd_add_int32_512)(s32 *a, s32 *b, s32* result) = 0;
void (*simd_sub_int32_128)(s32 *a, s32 *b, s32* result) = 0;
void (*simd_sub_int32_256)(s32 *a, s32 *b, s32* result) = 0;
void (*simd_sub_int32_512)(s32 *a, s32 *b, s32* result) = 0;
void (*simd_mul_int32_128)(s32 *a, s32 *b, s32* result) = 0;
void (*simd_mul_int32_256)(s32 *a, s32 *b, s32* result) = 0;
void (*simd_mul_int32_512)(s32 *a, s32 *b, s32* result) = 0;

inline void basic_add_float32_64(float32 *a, float32 *b, float32* result) {
    result[0] = a[0] + b[0];
    result[1] = a[1] + b[1];
}

inline void basic_add_float32_128(float32 *a, float32 *b, float32* result) {
    result[0] = a[0] + b[0];
    result[1] = a[1] + b[1];
    result[2] = a[2] + b[2];
    result[3] = a[3] + b[3];
}

inline void basic_add_float32_256(float32 *a, float32 *b, float32* result) {
    simd_add_float32_128(a, b, result);
    simd_add_float32_128(a+4, b+4, result+4);
}

inline void basic_add_float32_512(float32 *a, float32 *b, float32* result) {
    simd_add_float32_256(a, b, result);
    simd_add_float32_256(a+8, b+8, result+8);
}

inline void basic_sub_float32_64(float32 *a, float32 *b, float32* result) {
    result[0] = a[0] - b[0];
    result[1] = a[1] - b[1];
}

inline void basic_sub_float32_128(float32 *a, float32 *b, float32* result) {
    result[0] = a[0] - b[0];
    result[1] = a[1] - b[1];
    result[2] = a[2] - b[2];
    result[3] = a[3] - b[3];
}

inline void basic_sub_float32_256(float32 *a, float32 *b, float32* result) {
    simd_sub_float32_128(a, b, result);
    simd_sub_float32_128(a+4, b+4, result+4);
}

inline void basic_sub_float32_512(float32 *a, float32 *b, float32* result) {
    simd_sub_float32_256(a, b, result);
    simd_sub_float32_256(a+8, b+8, result+8);
}

inline void basic_mul_float32_64(float32 *a, float32 *b, float32* result) {
    result[0] = a[0] * b[0];
    result[1] = a[1] * b[1];
}

inline void basic_mul_float32_128(float32 *a, float32 *b, float32* result) {
    result[0] = a[0] * b[0];
    result[1] = a[1] * b[1];
    result[2] = a[2] * b[2];
    result[3] = a[3] * b[3];
}

inline void basic_mul_float32_256(float32 *a, float32 *b, float32* result) {
    simd_mul_float32_128(a, b, result);
    simd_mul_float32_128(a+4, b+4, result+4);
}

inline void basic_mul_float32_512(float32 *a, float32 *b, float32* result) {
    simd_mul_float32_256(a, b, result);
    simd_mul_float32_256(a+8, b+8, result+8);
}

inline void basic_div_float32_64(float32 *a, float32 *b, float32* result) {
    result[0] = a[0] / b[0];
    result[1] = a[1] / b[1];
}

inline void basic_div_float32_128(float32 *a, float32 *b, float32* result) {
    result[0] = a[0] / b[0];
    result[1] = a[1] / b[1];
    result[2] = a[2] / b[2];
    result[3] = a[3] / b[3];
}

inline void basic_div_float32_256(float32 *a, float32 *b, float32* result) {
    simd_div_float32_128(a, b, result);
    simd_div_float32_128(a+4, b+4, result+4);
}

inline void basic_div_float32_512(float32 *a, float32 *b, float32* result) {
    simd_div_float32_256(a, b, result);
    simd_div_float32_256(a+8, b+8, result+8);
}

inline void basic_add_int32_128(s32 *a, s32 *b, s32* result) {
    result[0] = a[0] + b[0];
    result[1] = a[1] + b[1];
    result[2] = a[2] + b[2];
    result[3] = a[3] + b[3];
}

inline void basic_add_int32_256(s32 *a, s32 *b, s32* result) {
    simd_add_int32_128(a, b, result);
    simd_add_int32_128(a+4, b+4, result+4);
}

inline void basic_add_int32_512(s32 *a, s32 *b, s32* result) {
    simd_add_int32_256(a, b, result);
    simd_add_int32_256(a+8, b+8, result+8);
}

inline void basic_sub_int32_128(s32 *a, s32 *b, s32* result) {
    result[0] = a[0] - b[0];
    result[1] = a[1] - b[1];
    result[2] = a[2] - b[2];
    result[3] = a[3] - b[3];
}

inline void basic_sub_int32_256(s32 *a, s32 *b, s32* result) {
    simd_sub_int32_128(a, b, result);
    simd_sub_int32_128(a+4, b+4, result+4);
}

inline void basic_sub_int32_512(s32 *a, s32 *b, s32* result) {
    simd_sub_int32_256(a, b, result);
    simd_sub_int32_256(a+8, b+8, result+8);
}

inline void basic_mul_int32_128(s32 *a, s32 *b, s32* result) {
    result[0] = a[0] * b[0];
    result[1] = a[1] * b[1];
    result[2] = a[2] * b[2];
    result[3] = a[3] * b[3];
}

inline void basic_mul_int32_256(s32 *a, s32 *b, s32* result) {
    simd_mul_int32_128(a, b, result);
    simd_mul_int32_128(a+4, b+4, result+4);
}

inline void basic_mul_int32_512(s32 *a, s32 *b, s32* result) {
    simd_mul_int32_256(a, b, result);
    simd_mul_int32_256(a+8, b+8, result+8);
}

#if ENABLE_SIMD

#if COMPILER_MSVC
	// SSE 1 float32
	inline void sse1_add_float32_64 (float32 *a, float32 *b, float32* result);
	inline void sse1_add_float32_128(float32 *a, float32 *b, float32* result);
	inline void sse1_sub_float32_64 (float32 *a, float32 *b, float32* result);
	inline void sse1_sub_float32_128(float32 *a, float32 *b, float32* result);
	inline void sse1_mul_float32_64 (float32 *a, float32 *b, float32* result);
	inline void sse1_mul_float32_128(float32 *a, float32 *b, float32* result);
	inline void sse1_div_float32_64 (float32 *a, float32 *b, float32* result);
	inline void sse1_div_float32_128(float32 *a, float32 *b, float32* result);
	
	// SSE 2 float32
	inline void sse2_add_float32_64 (float32 *a, float32 *b, float32* result);
	inline void sse2_add_float32_128(float32 *a, float32 *b, float32* result);
	inline void sse2_sub_float32_64 (float32 *a, float32 *b, float32* result);
	inline void sse2_sub_float32_128(float32 *a, float32 *b, float32* result);
	inline void sse2_mul_float32_64 (float32 *a, float32 *b, float32* result);
	inline void sse2_mul_float32_128(float32 *a, float32 *b, float32* result);
	inline void sse2_div_float32_64 (float32 *a, float32 *b, float32* result);
	inline void sse2_div_float32_128(float32 *a, float32 *b, float32* result);
	
	// SSE 2 int32
	inline void sse2_add_int32_128(s32 *a, s32 *b, s32* result);
	inline void sse2_sub_int32_128(s32 *a, s32 *b, s32* result);
	inline void sse2_mul_int32_128(s32 *a, s32 *b, s32* result);
	
	// SSE 3 float32
	inline void sse3_add_float32_64 (float32 *a, float32 *b, float32* result);
	inline void sse3_add_float32_128(float32 *a, float32 *b, float32* result);
	inline void sse3_sub_float32_64 (float32 *a, float32 *b, float32* result);
	inline void sse3_sub_float32_128(float32 *a, float32 *b, float32* result);
	inline void sse3_mul_float32_64 (float32 *a, float32 *b, float32* result);
	inline void sse3_mul_float32_128(float32 *a, float32 *b, float32* result);
	inline void sse3_div_float32_64 (float32 *a, float32 *b, float32* result);
	inline void sse3_div_float32_128(float32 *a, float32 *b, float32* result);
	
	// SSSE 3 float32
	inline void ssse3_add_float32_64 (float32 *a, float32 *b, float32* result);
	inline void ssse3_add_float32_128(float32 *a, float32 *b, float32* result);
	inline void ssse3_sub_float32_64 (float32 *a, float32 *b, float32* result);
	inline void ssse3_sub_float32_128(float32 *a, float32 *b, float32* result);
	inline void ssse3_mul_float32_64 (float32 *a, float32 *b, float32* result);
	inline void ssse3_mul_float32_128(float32 *a, float32 *b, float32* result);
	inline void ssse3_div_float32_64 (float32 *a, float32 *b, float32* result);
	inline void ssse3_div_float32_128(float32 *a, float32 *b, float32* result);
	
	// SSSE 4.1 float32
	inline void sse41_add_float32_64 (float32 *a, float32 *b, float32* result);
	inline void sse41_add_float32_128(float32 *a, float32 *b, float32* result);
	inline void sse41_sub_float32_64 (float32 *a, float32 *b, float32* result);
	inline void sse41_sub_float32_128(float32 *a, float32 *b, float32* result);
	inline void sse41_mul_float32_64 (float32 *a, float32 *b, float32* result);
	inline void sse41_mul_float32_128(float32 *a, float32 *b, float32* result);
	inline void sse41_div_float32_64 (float32 *a, float32 *b, float32* result);
	inline void sse41_div_float32_128(float32 *a, float32 *b, float32* result);
	
	// SSSE 4.2 float32
	inline void sse42_add_float32_64 (float32 *a, float32 *b, float32* result);
	inline void sse42_add_float32_128(float32 *a, float32 *b, float32* result);
	inline void sse42_sub_float32_64 (float32 *a, float32 *b, float32* result);
	inline void sse42_sub_float32_128(float32 *a, float32 *b, float32* result);
	inline void sse42_mul_float32_64 (float32 *a, float32 *b, float32* result);
	inline void sse42_mul_float32_128(float32 *a, float32 *b, float32* result);
	inline void sse42_div_float32_64 (float32 *a, float32 *b, float32* result);
	inline void sse42_div_float32_128(float32 *a, float32 *b, float32* result);
	
	// AVX 1/2 float32
	inline void avx_add_float32_256(float32 *a, float32 *b, float32* result);
	inline void avx_sub_float32_256(float32 *a, float32 *b, float32* result);
	inline void avx_mul_float32_256(float32 *a, float32 *b, float32* result);
	inline void avx_div_float32_256(float32 *a, float32 *b, float32* result);
	
	// AVX2 2 int32
	inline void avx2_add_int32_256(s32 *a, s32 *b, s32* result);
	inline void avx2_sub_int32_256(s32 *a, s32 *b, s32* result);
	inline void avx2_mul_int32_256(s32 *a, s32 *b, s32* result);
	
	// AVX2 512 float32
	inline void avx512_add_float32_512(float32 *a, float32 *b, float32* result);
	inline void avx512_sub_float32_512(float32 *a, float32 *b, float32* result);
	inline void avx512_mul_float32_512(float32 *a, float32 *b, float32* result);
	inline void avx512_div_float32_512(float32 *a, float32 *b, float32* result);
	
	// AVX2 512 float32
	inline void avx512_add_int32_512(s32 *a, s32 *b, s32* result);
	inline void avx512_sub_int32_512(s32 *a, s32 *b, s32* result);
	inline void avx512_mul_int32_512(s32 *a, s32 *b, s32* result);
	
#elif COMPILER_GCC || COMPILER_CLANG
	
	// SSE 1 float32
inline void sse1_add_float32_64(float32 *a, float32 *b, float32* result) {
	float a128[2];
	a128[0] = *a;
	a128[1] = 0;
	float b128[2];
	b128[0] = *b;
	b128[1] = 0;
	float r128[2];
	r128[0] = *result;
	r128[1] = 0;
    __asm__ (
        "movss (%0), %%xmm0\n\t"
        "movss (%1), %%xmm1\n\t"
        "addss %%xmm1, %%xmm0\n\t"
        "movss %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (r128)
        : "xmm0", "xmm1"
    );
    *result = r128[0];
}

inline void sse1_add_float32_128(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "movaps (%0), %%xmm0\n\t"
        "movaps (%1), %%xmm1\n\t"
        "addps %%xmm1, %%xmm0\n\t"
        "movaps %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

inline void sse1_sub_float32_64(float32 *a, float32 *b, float32* result) {
	float a128[2];
	a128[0] = *a;
	a128[1] = 0;
	float b128[2];
	b128[0] = *b;
	b128[1] = 0;
	float r128[2];
	r128[0] = *result;
	r128[1] = 0;
    __asm__ (
        "movss (%0), %%xmm0\n\t"
        "movss (%1), %%xmm1\n\t"
        "subss %%xmm1, %%xmm0\n\t"
        "movss %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (r128)
        : "xmm0", "xmm1"
    );
    *result = r128[0];
}

inline void sse1_sub_float32_128(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "movaps (%0), %%xmm0\n\t"
        "movaps (%1), %%xmm1\n\t"
        "subps %%xmm1, %%xmm0\n\t"
        "movaps %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

inline void sse1_mul_float32_64(float32 *a, float32 *b, float32* result) {
	float a128[2];
	a128[0] = *a;
	a128[1] = 0;
	float b128[2];
	b128[0] = *b;
	b128[1] = 0;
	float r128[2];
	r128[0] = *result;
	r128[1] = 0;
    __asm__ (
        "movss (%0), %%xmm0\n\t"
        "movss (%1), %%xmm1\n\t"
        "mulss %%xmm1, %%xmm0\n\t"
        "movss %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (r128)
        : "xmm0", "xmm1"
    );
    *result = r128[0];
}

inline void sse1_mul_float32_128(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "movaps (%0), %%xmm0\n\t"
        "movaps (%1), %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movaps %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

inline void sse1_div_float32_64(float32 *a, float32 *b, float32* result) {
	float a128[2];
	a128[0] = *a;
	a128[1] = 0;
	float b128[2];
	b128[0] = *b;
	b128[1] = 0;
	float r128[2];
	r128[0] = *result;
	r128[1] = 0;
    __asm__ (
        "movss (%0), %%xmm0\n\t"
        "movss (%1), %%xmm1\n\t"
        "divss %%xmm1, %%xmm0\n\t"
        "movss %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (r128)
        : "xmm0", "xmm1"
    );
    *result = r128[0];
}

inline void sse1_div_float32_128(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "movaps (%0), %%xmm0\n\t"
        "movaps (%1), %%xmm1\n\t"
        "divps %%xmm1, %%xmm0\n\t"
        "movaps %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

// SSE 2 float32
inline void sse2_add_float32_64(float32 *a, float32 *b, float32* result) {
	float a128[2];
	a128[0] = *a;
	a128[1] = 0;
	float b128[2];
	b128[0] = *b;
	b128[1] = 0;
	float r128[2];
	r128[0] = *result;
	r128[1] = 0;
    __asm__ (
        "movss (%0), %%xmm0\n\t"
        "movss (%1), %%xmm1\n\t"
        "addss %%xmm1, %%xmm0\n\t"
        "movss %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (r128)
        : "xmm0", "xmm1"
    );
    *result = r128[0];
}

inline void sse2_add_float32_128(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "movaps (%0), %%xmm0\n\t"
        "movaps (%1), %%xmm1\n\t"
        "addps %%xmm1, %%xmm0\n\t"
        "movaps %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

inline void sse2_sub_float32_64(float32 *a, float32 *b, float32* result) {
	float a128[2];
	a128[0] = *a;
	a128[1] = 0;
	float b128[2];
	b128[0] = *b;
	b128[1] = 0;
	float r128[2];
	r128[0] = *result;
	r128[1] = 0;
    __asm__ (
        "movss (%0), %%xmm0\n\t"
        "movss (%1), %%xmm1\n\t"
        "subss %%xmm1, %%xmm0\n\t"
        "movss %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (r128)
        : "xmm0", "xmm1"
    );
    *result = r128[0];
}

inline void sse2_sub_float32_128(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "movaps (%0), %%xmm0\n\t"
        "movaps (%1), %%xmm1\n\t"
        "subps %%xmm1, %%xmm0\n\t"
        "movaps %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

inline void sse2_mul_float32_64(float32 *a, float32 *b, float32* result) {
	float a128[2];
	a128[0] = *a;
	a128[1] = 0;
	float b128[2];
	b128[0] = *b;
	b128[1] = 0;
	float r128[2];
	r128[0] = *result;
	r128[1] = 0;
    __asm__ (
        "movss (%0), %%xmm0\n\t"
        "movss (%1), %%xmm1\n\t"
        "mulss %%xmm1, %%xmm0\n\t"
        "movss %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (r128)
        : "xmm0", "xmm1"
    );
    *result = r128[0];
}

inline void sse2_mul_float32_128(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "movaps (%0), %%xmm0\n\t"
        "movaps (%1), %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movaps %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

inline void sse2_div_float32_64(float32 *a, float32 *b, float32* result) {
	float a128[2];
	a128[0] = *a;
	a128[1] = 0;
	float b128[2];
	b128[0] = *b;
	b128[1] = 0;
	float r128[2];
	r128[0] = *result;
	r128[1] = 0;
    __asm__ (
        "movss (%0), %%xmm0\n\t"
        "movss (%1), %%xmm1\n\t"
        "divss %%xmm1, %%xmm0\n\t"
        "movss %%xmm0, (%2)\n\t"
        :
        : "r" (a128), "r" (b128), "r" (r128)
        : "xmm0", "xmm1"
    );
    *result = r128[0];
}

inline void sse2_div_float32_128(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "movaps (%0), %%xmm0\n\t"
        "movaps (%1), %%xmm1\n\t"
        "divps %%xmm1, %%xmm0\n\t"
        "movaps %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

// SSE 2 int32
inline void sse2_add_int32_128(s32 *a, s32 *b, s32* result) {
    __asm__ (
        "movdqa (%0), %%xmm0\n\t"
        "movdqa (%1), %%xmm1\n\t"
        "paddd %%xmm1, %%xmm0\n\t"
        "movdqa %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

inline void sse2_sub_int32_128(s32 *a, s32 *b, s32* result) {
    __asm__ (
        "movdqa (%0), %%xmm0\n\t"
        "movdqa (%1), %%xmm1\n\t"
        "psubd %%xmm1, %%xmm0\n\t"
        "movdqa %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

inline void sse2_mul_int32_128(s32 *a, s32 *b, s32* result) {
    __asm__ (
        "movdqa (%0), %%xmm0\n\t"
        "movdqa (%1), %%xmm1\n\t"
        "pmulld %%xmm1, %%xmm0\n\t"
        "movdqa %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

// SSE 3 float32
inline void sse3_add_float32_64(float32 *a, float32 *b, float32* result) {
	float a128[2];
	a128[0] = *a;
	a128[1] = 0;
	float b128[2];
	b128[0] = *b;
	b128[1] = 0;
	float r128[2];
	r128[0] = *result;
	r128[1] = 0;
    __asm__ (
        "movss (%0), %%xmm0\n\t"
        "movss (%1), %%xmm1\n\t"
        "addss %%xmm1, %%xmm0\n\t"
        "movss %%xmm0, (%2)\n\t"
        :
        : "r" (a128), "r" (b128), "r" (r128)
        : "xmm0", "xmm1"
    );
    *result = r128[0];
}

inline void sse3_add_float32_128(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "movaps (%0), %%xmm0\n\t"
        "movaps (%1), %%xmm1\n\t"
        "addps %%xmm1, %%xmm0\n\t"
        "movaps %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

inline void sse3_sub_float32_64(float32 *a, float32 *b, float32* result) {
	float a128[2];
	a128[0] = *a;
	a128[1] = 0;
	float b128[2];
	b128[0] = *b;
	b128[1] = 0;
	float r128[2];
	r128[0] = *result;
	r128[1] = 0;
    __asm__ (
        "movss (%0), %%xmm0\n\t"
        "movss (%1), %%xmm1\n\t"
        "subss %%xmm1, %%xmm0\n\t"
        "movss %%xmm0, (%2)\n\t"
        :
        : "r" (a128), "r" (b128), "r" (r128)
        : "xmm0", "xmm1"
    );
    *result = r128[0];
}

inline void sse3_sub_float32_128(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "movaps (%0), %%xmm0\n\t"
        "movaps (%1), %%xmm1\n\t"
        "subps %%xmm1, %%xmm0\n\t"
        "movaps %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

inline void sse3_mul_float32_64(float32 *a, float32 *b, float32* result) {
	float a128[2];
	a128[0] = *a;
	a128[1] = 0;
	float b128[2];
	b128[0] = *b;
	b128[1] = 0;
	float r128[2];
	r128[0] = *result;
	r128[1] = 0;
    __asm__ (
        "movss (%0), %%xmm0\n\t"
        "movss (%1), %%xmm1\n\t"
        "mulss %%xmm1, %%xmm0\n\t"
        "movss %%xmm0, (%2)\n\t"
        :
        : "r" (a128), "r" (b128), "r" (r128)
        : "xmm0", "xmm1"
    );
    *result = r128[0];
}

inline void sse3_mul_float32_128(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "movaps (%0), %%xmm0\n\t"
        "movaps (%1), %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movaps %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

inline void sse3_div_float32_64(float32 *a, float32 *b, float32* result) {
	float a128[2];
	a128[0] = *a;
	a128[1] = 0;
	float b128[2];
	b128[0] = *b;
	b128[1] = 0;
	float r128[2];
	r128[0] = *result;
	r128[1] = 0;
    __asm__ (
        "movss (%0), %%xmm0\n\t"
        "movss (%1), %%xmm1\n\t"
        "divss %%xmm1, %%xmm0\n\t"
        "movss %%xmm0, (%2)\n\t"
        :
        : "r" (a128), "r" (b128), "r" (r128)
        : "xmm0", "xmm1"
    );
    *result = r128[0];
}

inline void sse3_div_float32_128(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "movaps (%0), %%xmm0\n\t"
        "movaps (%1), %%xmm1\n\t"
        "divps %%xmm1, %%xmm0\n\t"
        "movaps %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

// SSSE 3 float32
inline void ssse3_add_float32_64(float32 *a, float32 *b, float32* result) {
	float a128[2];
	a128[0] = *a;
	a128[1] = 0;
	float b128[2];
	b128[0] = *b;
	b128[1] = 0;
	float r128[2];
	r128[0] = *result;
	r128[1] = 0;
    __asm__ (
        "movss (%0), %%xmm0\n\t"
        "movss (%1), %%xmm1\n\t"
        "addss %%xmm1, %%xmm0\n\t"
        "movss %%xmm0, (%2)\n\t"
        :
        : "r" (a128), "r" (b128), "r" (r128)
        : "xmm0", "xmm1"
    );
    *result = r128[0];
}

inline void ssse3_add_float32_128(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "movaps (%0), %%xmm0\n\t"
        "movaps (%1), %%xmm1\n\t"
        "addps %%xmm1, %%xmm0\n\t"
        "movaps %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

inline void ssse3_sub_float32_64(float32 *a, float32 *b, float32* result) {
	float a128[2];
	a128[0] = *a;
	a128[1] = 0;
	float b128[2];
	b128[0] = *b;
	b128[1] = 0;
	float r128[2];
	r128[0] = *result;
	r128[1] = 0;
    __asm__ (
        "movss (%0), %%xmm0\n\t"
        "movss (%1), %%xmm1\n\t"
        "subss %%xmm1, %%xmm0\n\t"
        "movss %%xmm0, (%2)\n\t"
        :
        : "r" (a128), "r" (b128), "r" (r128)
        : "xmm0", "xmm1"
    );
    *result = r128[0];
}

inline void ssse3_sub_float32_128(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "movaps (%0), %%xmm0\n\t"
        "movaps (%1), %%xmm1\n\t"
        "subps %%xmm1, %%xmm0\n\t"
        "movaps %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

inline void ssse3_mul_float32_64(float32 *a, float32 *b, float32* result) {
	float a128[2];
	a128[0] = *a;
	a128[1] = 0;
	float b128[2];
	b128[0] = *b;
	b128[1] = 0;
	float r128[2];
	r128[0] = *result;
	r128[1] = 0;
    __asm__ (
        "movss (%0), %%xmm0\n\t"
        "movss (%1), %%xmm1\n\t"
        "mulss %%xmm1, %%xmm0\n\t"
        "movss %%xmm0, (%2)\n\t"
        :
        : "r" (a128), "r" (b128), "r" (r128)
        : "xmm0", "xmm1"
    );
    *result = r128[0];
}

inline void ssse3_mul_float32_128(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "movaps (%0), %%xmm0\n\t"
        "movaps (%1), %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movaps %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

inline void ssse3_div_float32_64(float32 *a, float32 *b, float32* result) {
	float a128[2];
	a128[0] = *a;
	a128[1] = 0;
	float b128[2];
	b128[0] = *b;
	b128[1] = 0;
	float r128[2];
	r128[0] = *result;
	r128[1] = 0;
    __asm__ (
        "movss (%0), %%xmm0\n\t"
        "movss (%1), %%xmm1\n\t"
        "divss %%xmm1, %%xmm0\n\t"
        "movss %%xmm0, (%2)\n\t"
        :
        : "r" (a128), "r" (b128), "r" (r128)
        : "xmm0", "xmm1"
    );
    *result = r128[0];
}

inline void ssse3_div_float32_128(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "movaps (%0), %%xmm0\n\t"
        "movaps (%1), %%xmm1\n\t"
        "divps %%xmm1, %%xmm0\n\t"
        "movaps %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

// SSE4.1 float32
inline void sse41_add_float32_64(float32 *a, float32 *b, float32* result) {
	float a128[2];
	a128[0] = *a;
	a128[1] = 0;
	float b128[2];
	b128[0] = *b;
	b128[1] = 0;
	float r128[2];
	r128[0] = *result;
	r128[1] = 0;
    __asm__ (
        "movss (%0), %%xmm0\n\t"
        "movss (%1), %%xmm1\n\t"
        "addss %%xmm1, %%xmm0\n\t"
        "movss %%xmm0, (%2)\n\t"
        :
        : "r" (a128), "r" (b128), "r" (r128)
        : "xmm0", "xmm1"
    );
    *result = r128[0];
}

inline void sse41_add_float32_128(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "movaps (%0), %%xmm0\n\t"
        "movaps (%1), %%xmm1\n\t"
        "addps %%xmm1, %%xmm0\n\t"
        "movaps %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

inline void sse41_sub_float32_64(float32 *a, float32 *b, float32* result) {
	float a128[2];
	a128[0] = *a;
	a128[1] = 0;
	float b128[2];
	b128[0] = *b;
	b128[1] = 0;
	float r128[2];
	r128[0] = *result;
	r128[1] = 0;
    __asm__ (
        "movss (%0), %%xmm0\n\t"
        "movss (%1), %%xmm1\n\t"
        "subss %%xmm1, %%xmm0\n\t"
        "movss %%xmm0, (%2)\n\t"
        :
        : "r" (a128), "r" (b128), "r" (r128)
        : "xmm0", "xmm1"
    );
    *result = r128[0];
}

inline void sse41_sub_float32_128(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "movaps (%0), %%xmm0\n\t"
        "movaps (%1), %%xmm1\n\t"
        "subps %%xmm1, %%xmm0\n\t"
        "movaps %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

inline void sse41_mul_float32_64(float32 *a, float32 *b, float32* result) {
	float a128[2];
	a128[0] = *a;
	a128[1] = 0;
	float b128[2];
	b128[0] = *b;
	b128[1] = 0;
	float r128[2];
	r128[0] = *result;
	r128[1] = 0;
    __asm__ (
        "movss (%0), %%xmm0\n\t"
        "movss (%1), %%xmm1\n\t"
        "mulss %%xmm1, %%xmm0\n\t"
        "movss %%xmm0, (%2)\n\t"
        :
        : "r" (a128), "r" (b128), "r" (r128)
        : "xmm0", "xmm1"
    );
    *result = r128[0];
}

inline void sse41_mul_float32_128(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "movaps (%0), %%xmm0\n\t"
        "movaps (%1), %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movaps %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

inline void sse41_div_float32_64(float32 *a, float32 *b, float32* result) {
	float a128[2];
	a128[0] = *a;
	a128[1] = 0;
	float b128[2];
	b128[0] = *b;
	b128[1] = 0;
	float r128[2];
	r128[0] = *result;
	r128[1] = 0;
    __asm__ (
        "movss (%0), %%xmm0\n\t"
        "movss (%1), %%xmm1\n\t"
        "divss %%xmm1, %%xmm0\n\t"
        "movss %%xmm0, (%2)\n\t"
        :
        : "r" (a128), "r" (b128), "r" (r128)
        : "xmm0", "xmm1"
    );
    *result = r128[0];
}

inline void sse41_div_float32_128(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "movaps (%0), %%xmm0\n\t"
        "movaps (%1), %%xmm1\n\t"
        "divps %%xmm1, %%xmm0\n\t"
        "movaps %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

// SSE4.2 float32
inline void sse42_add_float32_64(float32 *a, float32 *b, float32* result) {
	float a128[2];
	a128[0] = *a;
	a128[1] = 0;
	float b128[2];
	b128[0] = *b;
	b128[1] = 0;
	float r128[2];
	r128[0] = *result;
	r128[1] = 0;
    __asm__ (
        "movss (%0), %%xmm0\n\t"
        "movss (%1), %%xmm1\n\t"
        "addss %%xmm1, %%xmm0\n\t"
        "movss %%xmm0, (%2)\n\t"
        :
        : "r" (a128), "r" (b128), "r" (r128)
        : "xmm0", "xmm1"
    );
    *result = r128[0];
}

inline void sse42_add_float32_128(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "movaps (%0), %%xmm0\n\t"
        "movaps (%1), %%xmm1\n\t"
        "addps %%xmm1, %%xmm0\n\t"
        "movaps %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

inline void sse42_sub_float32_64(float32 *a, float32 *b, float32* result) {
	float a128[2];
	a128[0] = *a;
	a128[1] = 0;
	float b128[2];
	b128[0] = *b;
	b128[1] = 0;
	float r128[2];
	r128[0] = *result;
	r128[1] = 0;
    __asm__ (
        "movss (%0), %%xmm0\n\t"
        "movss (%1), %%xmm1\n\t"
        "subss %%xmm1, %%xmm0\n\t"
        "movss %%xmm0, (%2)\n\t"
        :
        : "r" (a128), "r" (b128), "r" (r128)
        : "xmm0", "xmm1"
    );
    *result = r128[0];
}

inline void sse42_sub_float32_128(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "movaps (%0), %%xmm0\n\t"
        "movaps (%1), %%xmm1\n\t"
        "subps %%xmm1, %%xmm0\n\t"
        "movaps %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
    
}

inline void sse42_mul_float32_64(float32 *a, float32 *b, float32* result) {
	float a128[2];
	a128[0] = *a;
	a128[1] = 0;
	float b128[2];
	b128[0] = *b;
	b128[1] = 0;
	float r128[2];
	r128[0] = *result;
	r128[1] = 0;
    __asm__ (
        "movss (%0), %%xmm0\n\t"
        "movss (%1), %%xmm1\n\t"
        "mulss %%xmm1, %%xmm0\n\t"
        "movss %%xmm0, (%2)\n\t"
        :
        : "r" (a128), "r" (b128), "r" (r128)
        : "xmm0", "xmm1"
    );
    *result = r128[0];
}

inline void sse42_mul_float32_128(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "movaps (%0), %%xmm0\n\t"
        "movaps (%1), %%xmm1\n\t"
        "mulps %%xmm1, %%xmm0\n\t"
        "movaps %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

inline void sse42_div_float32_64(float32 *a, float32 *b, float32* result) {
	float a128[2];
	a128[0] = *a;
	a128[1] = 0;
	float b128[2];
	b128[0] = *b;
	b128[1] = 0;
	float r128[2];
	r128[0] = *result;
	r128[1] = 0;
    __asm__ (
        "movss (%0), %%xmm0\n\t"
        "movss (%1), %%xmm1\n\t"
        "divss %%xmm1, %%xmm0\n\t"
        "movss %%xmm0, (%2)\n\t"
        :
        : "r" (a128), "r" (b128), "r" (r128)
        : "xmm0", "xmm1"
    );
    *result = r128[0];
}

inline void sse42_div_float32_128(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "movaps (%0), %%xmm0\n\t"
        "movaps (%1), %%xmm1\n\t"
        "divps %%xmm1, %%xmm0\n\t"
        "movaps %%xmm0, (%2)\n\t"
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1"
    );
}

// AVX float32
inline void avx_add_float32_256(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "vmovups %1, %%ymm0\n\t"
        "vmovups %2, %%ymm1\n\t"
        "vaddps %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovups %%ymm0, %0\n\t"
        : "=m" (*result)
        : "m" (*a), "m" (*b)
        : "ymm0", "ymm1"
    );
}

inline void avx_sub_float32_256(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "vmovups %1, %%ymm0\n\t"
        "vmovups %2, %%ymm1\n\t"
        "vsubps %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovups %%ymm0, %0\n\t"
        : "=m" (*result)
        : "m" (*a), "m" (*b)
        : "ymm0", "ymm1"
    );
}

inline void avx_mul_float32_256(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "vmovups %1, %%ymm0\n\t"
        "vmovups %2, %%ymm1\n\t"
        "vmulps %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovups %%ymm0, %0\n\t"
        : "=m" (*result)
        : "m" (*a), "m" (*b)
        : "ymm0", "ymm1"
    );
}

inline void avx_div_float32_256(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "vmovups %1, %%ymm0\n\t"
        "vmovups %2, %%ymm1\n\t"
        "vdivps %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovups %%ymm0, %0\n\t"
        : "=m" (*result)
        : "m" (*a), "m" (*b)
        : "ymm0", "ymm1"
    );
}

// AVX2 int32
inline void avx2_add_int32_256(s32 *a, s32 *b, s32* result) {
    __asm__ (
        "vmovdqu %1, %%ymm0\n\t"
        "vmovdqu %2, %%ymm1\n\t"
        "vpaddd %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovdqu %%ymm0, %0\n\t"
        : "=m" (*result)
        : "m" (*a), "m" (*b)
        : "ymm0", "ymm1"
    );
}

inline void avx2_sub_int32_256(s32 *a, s32 *b, s32* result) {
    __asm__ (
        "vmovdqu %1, %%ymm0\n\t"
        "vmovdqu %2, %%ymm1\n\t"
        "vpsubd %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovdqu %%ymm0, %0\n\t"
        : "=m" (*result)
        : "m" (*a), "m" (*b)
        : "ymm0", "ymm1"
    );
}

inline void avx2_mul_int32_256(s32 *a, s32 *b, s32* result) {
    __asm__ (
        "vmovdqu %1, %%ymm0\n\t"
        "vmovdqu %2, %%ymm1\n\t"
        "vpmulld %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovdqu %%ymm0, %0\n\t"
        : "=m" (*result)
        : "m" (*a), "m" (*b)
        : "ymm0", "ymm1"
    );
}

// AVX-512 float32
inline void avx512_add_float32_512(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "vmovups %1, %%zmm0\n\t"
        "vmovups %2, %%zmm1\n\t"
        "vaddps %%zmm1, %%zmm0, %%zmm0\n\t"
        "vmovups %%zmm0, %0\n\t"
        : "=m" (*result)
        : "m" (*a), "m" (*b)
        : "zmm0", "zmm1"
    );
}

inline void avx512_sub_float32_512(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "vmovups %1, %%zmm0\n\t"
        "vmovups %2, %%zmm1\n\t"
        "vsubps %%zmm1, %%zmm0, %%zmm0\n\t"
        "vmovups %%zmm0, %0\n\t"
        : "=m" (*result)
        : "m" (*a), "m" (*b)
        : "zmm0", "zmm1"
    );
}

inline void avx512_mul_float32_512(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "vmovups %1, %%zmm0\n\t"
        "vmovups %2, %%zmm1\n\t"
        "vmulps %%zmm1, %%zmm0, %%zmm0\n\t"
        "vmovups %%zmm0, %0\n\t"
        : "=m" (*result)
        : "m" (*a), "m" (*b)
        : "zmm0", "zmm1"
    );
}

inline void avx512_div_float32_512(float32 *a, float32 *b, float32* result) {
    __asm__ (
        "vmovups %1, %%zmm0\n\t"
        "vmovups %2, %%zmm1\n\t"
        "vdivps %%zmm1, %%zmm0, %%zmm0\n\t"
        "vmovups %%zmm0, %0\n\t"
        : "=m" (*result)
        : "m" (*a), "m" (*b)
        : "zmm0", "zmm1"
    );
}

// AVX-512 int32
inline void avx512_add_int32_512(s32 *a, s32 *b, s32* result) {
    __asm__ (
        "vmovdqu32 %1, %%zmm0\n\t"
        "vmovdqu32 %2, %%zmm1\n\t"
        "vpaddd %%zmm1, %%zmm0, %%zmm0\n\t"
        "vmovdqu32 %%zmm0, %0\n\t"
        : "=m" (*result)
        : "m" (*a), "m" (*b)
        : "zmm0", "zmm1"
    );
}

inline void avx512_sub_int32_512(s32 *a, s32 *b, s32* result) {
    __asm__ (
        "vmovdqu32 %1, %%zmm0\n\t"
        "vmovdqu32 %2, %%zmm1\n\t"
        "vpsubd %%zmm1, %%zmm0, %%zmm0\n\t"
        "vmovdqu32 %%zmm0, %0\n\t"
        : "=m" (*result)
        : "m" (*a), "m" (*b)
        : "zmm0", "zmm1"
    );
}

inline void avx512_mul_int32_512(s32 *a, s32 *b, s32* result) {
    __asm__ (
        "vmovdqu32 %1, %%zmm0\n\t"
        "vmovdqu32 %2, %%zmm1\n\t"
        "vpmulld %%zmm1, %%zmm0, %%zmm0\n\t"
        "vmovdqu32 %%zmm0, %0\n\t"
        : "=m" (*result)
        : "m" (*a), "m" (*b)
        : "zmm0", "zmm1"
    );
}
	
#else
	// SSE 1 float32
	#define sse1_add_float32_64    basic_add_float32_64
	#define sse1_add_float32_128   basic_add_float32_128
	#define sse1_sub_float32_64    basic_sub_float32_64
	#define sse1_sub_float32_128   basic_sub_float32_128
	#define sse1_mul_float32_64    basic_mul_float32_64
	#define sse1_mul_float32_128   basic_mul_float32_128
	#define sse1_div_float32_64    basic_div_float32_64
	#define sse1_div_float32_128   basic_div_float32_128
	#define sse2_add_float32_64    basic_add_float32_64
	#define sse2_add_float32_128   basic_add_float32_128
	#define sse2_sub_float32_64    basic_sub_float32_64
	#define sse2_sub_float32_128   basic_sub_float32_128
	#define sse2_mul_float32_64    basic_mul_float32_64
	#define sse2_mul_float32_128   basic_mul_float32_128
	#define sse2_div_float32_64    basic_div_float32_64
	#define sse2_div_float32_128   basic_div_float32_128
	#define sse2_add_int32_128     basic_add_int32_128
	#define sse2_sub_int32_128     basic_sub_int32_128
	#define sse2_mul_int32_128     basic_mul_int32_128
	#define sse3_add_float32_64    basic_add_float32_64
	#define sse3_add_float32_128   basic_add_float32_128
	#define sse3_sub_float32_64    basic_sub_float32_64
	#define sse3_sub_float32_128   basic_sub_float32_128
	#define sse3_mul_float32_64    basic_mul_float32_64
	#define sse3_mul_float32_128   basic_mul_float32_128
	#define sse3_div_float32_64    basic_div_float32_64
	#define sse3_div_float32_128   basic_div_float32_128
	#define ssse3_add_float32_64   ssse3_add_float32_64
	#define ssse3_add_float32_128  basic_add_float32_128
	#define ssse3_sub_float32_64   basic_sub_float32_64
	#define ssse3_sub_float32_128  basic_sub_float32_128
	#define ssse3_mul_float32_64   basic_mul_float32_64
	#define ssse3_mul_float32_128  basic_mul_float32_128
	#define ssse3_div_float32_64   basic_div_float32_64
	#define ssse3_div_float32_128  basic_div_float32_128
	#define sse41_add_float32_64   basic_add_float32_64
	#define sse41_add_float32_128  basic_add_float32_128
	#define sse41_sub_float32_64   basic_sub_float32_64
	#define sse41_sub_float32_128  basic_sub_float32_128
	#define sse41_mul_float32_64   basic_mul_float32_64
	#define sse41_mul_float32_128  basic_mul_float32_128
	#define sse41_div_float32_64   basic_div_float32_64
	#define sse41_div_float32_128  basic_div_float32_128
	#define sse42_add_float32_64   basic_add_float32_64
	#define sse42_add_float32_128  basic_add_float32_128
	#define sse42_sub_float32_64   basic_sub_float32_64
	#define sse42_sub_float32_128  basic_sub_float32_128
	#define sse42_mul_float32_64   basic_mul_float32_64
	#define sse42_mul_float32_128  basic_mul_float32_128
	#define sse42_div_float32_64   basic_div_float32_64
	#define sse42_div_float32_128  basic_div_float32_128
	#define avx_add_float32_256    basic_add_float32_256
	#define avx_sub_float32_256    basic_sub_float32_256
	#define avx_mul_float32_256    basic_mul_float32_256
	#define avx_div_float32_256    basic_div_float32_256
	#define avx2_add_int32_256     basic_add_int32_256
	#define avx2_sub_int32_256     basic_sub_int32_256
	#define avx2_mul_int32_256     basic_mul_int32_256
	#define avx512_add_float32_512 basic_add_float32_512
	#define avx512_sub_float32_512 basic_sub_float32_512
	#define avx512_mul_float32_512 basic_mul_float32_512
	#define avx512_div_float32_512 basic_div_float32_512
	#define avx512_add_int32_512   basic_add_int32_512
	#define avx512_sub_int32_512   basic_sub_int32_512
	#define avx512_mul_int32_512   basic_mul_int32_512
#endif // compiler check

#endif // ENABLE_SIMD


void init_cpu_specific() {
#if ENABLE_SIMD
	Cpu_Capabilities cap = query_cpu_capabilities();
	
	if (cap.avx) {
		simd_add_float32_256 = avx_add_float32_256;
		simd_sub_float32_256 = avx_sub_float32_256;
		simd_mul_float32_256 = avx_mul_float32_256;
		simd_div_float32_256 = avx_div_float32_256;
	} else {
		simd_add_float32_256 = basic_add_float32_256;
		simd_sub_float32_256 = basic_sub_float32_256;
		simd_mul_float32_256 = basic_mul_float32_256;
		simd_div_float32_256 = basic_div_float32_256;
	}
	
	if (cap.avx2) {
		simd_add_int32_256 = avx2_add_int32_256;
		simd_sub_int32_256 = avx2_sub_int32_256;
		simd_mul_int32_256 = avx2_mul_int32_256;
	} else {
		simd_add_int32_256 = basic_add_int32_256;
		simd_sub_int32_256 = basic_sub_int32_256;
		simd_mul_int32_256 = basic_mul_int32_256;
	}
	
	if (cap.avx512) {
		simd_add_float32_512 = avx512_add_float32_512;
		simd_sub_float32_512 = avx512_sub_float32_512;
		simd_mul_float32_512 = avx512_mul_float32_512;
		simd_div_float32_512 = avx512_div_float32_512;
		simd_add_int32_512   = avx512_add_int32_512;
		simd_sub_int32_512   = avx512_sub_int32_512;
		simd_mul_int32_512   = avx512_mul_int32_512;
	} else {
		simd_add_float32_512 = basic_add_float32_512;
		simd_sub_float32_512 = basic_sub_float32_512;
		simd_mul_float32_512 = basic_mul_float32_512;
		simd_div_float32_512 = basic_div_float32_512;
		simd_add_int32_512   = basic_add_int32_512;
		simd_sub_int32_512   = basic_sub_int32_512;
		simd_mul_int32_512   = basic_mul_int32_512;
	}
	
	if (cap.sse2) {
		simd_add_int32_128 = sse2_add_int32_128;
		simd_sub_int32_128 = sse2_sub_int32_128;
		simd_mul_int32_128 = sse2_mul_int32_128;
	} else {
		simd_add_int32_128 = basic_add_int32_128;
		simd_sub_int32_128 = basic_sub_int32_128;
		simd_mul_int32_128 = basic_mul_int32_128;
	}
	
	if (cap.sse42) {
		simd_add_float32_64  = sse42_add_float32_64;
		simd_add_float32_128 = sse42_add_float32_128;
		simd_sub_float32_64  = sse42_sub_float32_64;
		simd_sub_float32_128 = sse42_sub_float32_128;
		simd_mul_float32_64  = sse42_mul_float32_64;
		simd_mul_float32_128 = sse42_mul_float32_128;
		simd_div_float32_64  = sse42_div_float32_64;
		simd_div_float32_128 = sse42_div_float32_128;
	} else if (cap.sse41) {
		simd_add_float32_64  = sse41_add_float32_64;
		simd_add_float32_128 = sse41_add_float32_128;
		simd_sub_float32_64  = sse41_sub_float32_64;
		simd_sub_float32_128 = sse41_sub_float32_128;
		simd_mul_float32_64  = sse41_mul_float32_64;
		simd_mul_float32_128 = sse41_mul_float32_128;
		simd_div_float32_64  = sse41_div_float32_64;
		simd_div_float32_128 = sse41_div_float32_128;
	} else if (cap.ssse3) {
		simd_add_float32_64  = ssse3_add_float32_64;
		simd_add_float32_128 = ssse3_add_float32_128;
		simd_sub_float32_64  = ssse3_sub_float32_64;
		simd_sub_float32_128 = ssse3_sub_float32_128;
		simd_mul_float32_64  = ssse3_mul_float32_64;
		simd_mul_float32_128 = ssse3_mul_float32_128;
		simd_div_float32_64  = ssse3_div_float32_64;
		simd_div_float32_128 = ssse3_div_float32_128;
	} else if (cap.sse3) {
		simd_add_float32_64  = sse3_add_float32_64;
		simd_add_float32_128 = sse3_add_float32_128;
		simd_sub_float32_64  = sse3_sub_float32_64;
		simd_sub_float32_128 = sse3_sub_float32_128;
		simd_mul_float32_64  = sse3_mul_float32_64;
		simd_mul_float32_128 = sse3_mul_float32_128;
		simd_div_float32_64  = sse3_div_float32_64;
		simd_div_float32_128 = sse3_div_float32_128;
	} else if (cap.sse2) {
		simd_add_float32_64  = sse2_add_float32_64;
		simd_add_float32_128 = sse2_add_float32_128;
		simd_sub_float32_64  = sse2_sub_float32_64;
		simd_sub_float32_128 = sse2_sub_float32_128;
		simd_mul_float32_64  = sse2_mul_float32_64;
		simd_mul_float32_128 = sse2_mul_float32_128;
		simd_div_float32_64  = sse2_div_float32_64;
		simd_div_float32_128 = sse2_div_float32_128;
	} else if (cap.sse1) {
		simd_add_float32_64  = sse1_add_float32_64;
		simd_add_float32_128 = sse1_add_float32_128;
		simd_sub_float32_64  = sse1_sub_float32_64;
		simd_sub_float32_128 = sse1_sub_float32_128;
		simd_mul_float32_64  = sse1_mul_float32_64;
		simd_mul_float32_128 = sse1_mul_float32_128;
		simd_div_float32_64  = sse1_div_float32_64;
		simd_div_float32_128 = sse1_div_float32_128;
	} else {
		simd_add_float32_64  = basic_add_float32_64;
		simd_add_float32_128 = basic_add_float32_128;
		simd_sub_float32_64  = basic_sub_float32_64;
		simd_sub_float32_128 = basic_sub_float32_128;
		simd_mul_float32_64  = basic_mul_float32_64;
		simd_mul_float32_128 = basic_mul_float32_128;
		simd_div_float32_64  = basic_div_float32_64;
		simd_div_float32_128 = basic_div_float32_128;
	}
#else // ENABLE_SIMD

	simd_add_float32_64 = 	basic_add_float32_64;
	simd_add_float32_128 = 	basic_add_float32_128;
	simd_add_float32_256 = 	basic_add_float32_256;
	simd_add_float32_512 = 	basic_add_float32_512;
	simd_sub_float32_64 = 	basic_sub_float32_64;
	simd_sub_float32_128 = 	basic_sub_float32_128;
	simd_sub_float32_256 = 	basic_sub_float32_256;
	simd_sub_float32_512 = 	basic_sub_float32_512;
	simd_mul_float32_64 = 	basic_mul_float32_64;
	simd_mul_float32_128 = 	basic_mul_float32_128;
	simd_mul_float32_256 = 	basic_mul_float32_256;
	simd_mul_float32_512 = 	basic_mul_float32_512;
	simd_div_float32_64 = 	basic_div_float32_64;
	simd_div_float32_128 = 	basic_div_float32_128;
	simd_div_float32_256 = 	basic_div_float32_256;
	simd_div_float32_512 = 	basic_div_float32_512;
	simd_add_int32_128 = 	basic_add_int32_128;
	simd_add_int32_256 = 	basic_add_int32_256;
	simd_add_int32_512 = 	basic_add_int32_512;
	simd_sub_int32_128 = 	basic_sub_int32_128;
	simd_sub_int32_256 = 	basic_sub_int32_256;
	simd_sub_int32_512 = 	basic_sub_int32_512;
	simd_mul_int32_128 = 	basic_mul_int32_128;
	simd_mul_int32_256 = 	basic_mul_int32_256;
	simd_mul_int32_512 = 	basic_mul_int32_512;

#endif // ENABLE_SIMD
}
