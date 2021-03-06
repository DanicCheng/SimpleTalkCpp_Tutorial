#include "my_common.h"

#include <xmmintrin.h>

class alignas(16) MyVec4 {
public:
	union {
		struct{ float x, y, z, w; };
		__m128 _m;
	};

	void set(float x_, float y_, float z_, float w_) {
		x = x_;
		y = y_;
		z = z_;
		w = w_;
	}

	void print() {
		printf("Vec4<%f, %f, %f, %f>\n", x, y ,z, w);
	}
};

const int N = 10000000;
const int M = 40;

void add_cpp(MyVec4* dst, const MyVec4* a, const MyVec4* b, int size) {
	for (int i = 0; i < size; i++) {
		dst->x = a->x + b->x;
		dst->y = a->y + b->y;
		dst->z = a->z + b->z;
		dst->w = a->w + b->w;

		dst++;
		a++;
		b++;
	}
}

extern "C" void asm_add_cpp(MyVec4* dst, const MyVec4* a, const MyVec4* b, int size);

__forceinline
void add_cpp_inline_asm(MyVec4* arg_dst, const MyVec4* arg_a, const MyVec4* arg_b, int arg_size) {
	__asm {
		push ecx
		push esi
		push ebx
		push edi

		mov ecx, arg_size
		mov esi, arg_a
		mov ebx, arg_b
		mov edi, arg_dst

	my_loop:
		movaps xmm0, DWORD PTR[esi]
		addps  xmm0, DWORD PTR[ebx]
		movaps DWORD PTR[edi], xmm0

		add esi, 16;
		add ebx, 16;
		add edi, 16;

		dec ecx
		jnz my_loop

		pop edi
		pop ebx
		pop esi
		pop ecx
	}
}

void add_cpp_mm(MyVec4* dst, const MyVec4* a, const MyVec4* b, int size) {
	for (int i = 0; i < size; i++) {
		dst->_m = _mm_add_ps(a->_m, b->_m);

		dst++;
		a++;
		b++;
	}
}

template<class T> inline
T* my_malloc(size_t n) {
	void* p = _aligned_malloc(sizeof(T) * n, alignof(T));
	return reinterpret_cast<T*>(p);
}

inline
void my_free(void* p) {
	_aligned_free(p);
}

void test_add() {
	MyVec4* a  = my_malloc<MyVec4>(N);
	MyVec4* b  = my_malloc<MyVec4>(N);
	MyVec4* dst = my_malloc<MyVec4>(N);

	for (int i = 0; i < N; i++) {
		float f = static_cast<float>(i);
		a[i].set(f + 1.0f,
				 f + 2.0f,
				 f + 3.0f,
				 f + 4.0f);

		b[i].set(f + 5.0f,
				 f + 6.0f,
				 f + 7.0f,
				 f + 8.0f);
	}

	printf("a[2] = "); a[2].print();
	printf("b[2] = "); b[2].print();

	printf("from: dst=%p a=%p b=%p\n", dst, a, b);
	printf("to:   dst=%p a=%p b=%p\n", dst + N, a + N, b + N);

	{
		printf("\n-- warn up cache - add_cpp --\n");
		MyTimer timer;
		for (int j = 0; j < M; j++) {
			add_cpp(dst, a, b, N);
		}
		timer.print();
		dst[2].print();
	}

	{
		printf("\n-- add_cpp --\n");
		MyTimer timer;
		for (int j = 0; j < M; j++) {
			add_cpp(dst, a, b, N);
		}
		timer.print();
		dst[2].print();
	}

	{
		printf("\n-- asm_add_cpp --\n");
		MyTimer timer;
		for (int j = 0; j < M; j++) {
			asm_add_cpp(dst, a, b, N);
		}
		timer.print();
		dst[2].print();
	}

	{
		printf("\n-- add_cpp_inline_asm --\n");
		MyTimer timer;
		for (int j = 0; j < M; j++) {
			add_cpp_inline_asm(dst, a, b, N);
		}
		timer.print();
		dst[2].print();
	}

	{
		printf("\n-- add_cpp_mm --\n");
		MyTimer timer;
		for (int j = 0; j < M; j++) {
			add_cpp_mm(dst, a, b, N);
		}
		timer.print();
		dst[2].print();
	}

	my_free(a);
	my_free(b);
	my_free(dst);
}

int main() {
	test_add();

	printf("\n===== Program Ended =====\n  Press any key to exit !!\n");
	_getch();
	return 0;
}

