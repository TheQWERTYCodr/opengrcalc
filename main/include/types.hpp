#pragma once
#include <complex>
#include <concepts>
#include <cstdint>
#include <cmath>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>
using namespace std;

namespace types
{
	#ifdef __STDCPP_FLOAT16_T__
	using std::float16_t;
	#else
	using float16_t = _Float16;
	#endif
	#ifdef __STDCPP_FLOAT32_T__
	using std::float32_t;
	#else
	using float32_t = _Float32;
	#endif
	#ifdef __STDCPP_FLOAT64_T__
	using std::float64_t;
	#else
	using float64_t = _Float64;
	#endif
	#ifdef __STDCPP_FLOAT128_T__
	using std::float128_t;
	#else
	using float128_t =  __float128;
	#endif

	template<class T> static constexpr bool _Tensor = false;
	template<class T> concept Tensor = _Tensor<T>;
	template<class T> static constexpr bool _Integer = false;
	template<class T> concept Integer = _Integer<T>;
	template<class T> static constexpr bool _Signed = false;
	template<class T> concept Signed = _Signed<T>;
	template<class T> static constexpr bool _Unsigned = false;
	template<class T> concept Unsigned = _Unsigned<T>;
	template<class T, int i> static constexpr bool _Width = false;
	template<class T, int i> concept Width = _Width<T,i>;
	template<class T> static constexpr bool _Builtin = false;
	template<class T> concept Builtin = _Builtin<T>;
	template<class T> static constexpr bool _Fast = false;
	template<class T> concept Fast = _Fast<T>;
	template<class T> static constexpr bool _Real = false;
	template<class T> concept Real = _Real<T>;
	template<class T, int i> static constexpr bool _Range = false;
	template<class T, int i> concept Range = _Range<T,i>;
	template<class T, int i> static constexpr bool _Precision = false;
	template<class T, int i> concept Precision = _Precision<T,i>;
	

	static constexpr bool test = false;
	typedef    int8_t   i8;
	template<> inline constexpr bool _Width<i8,8> = true;
	template<> inline constexpr bool _Tensor <i8> = true;
	template<> inline constexpr bool _Integer<i8> = true;
	template<> inline constexpr bool _Signed <i8> = true;
	typedef    int16_t  i16;
	typedef    int32_t  i32;
	typedef    int64_t  i64;
	template<> inline constexpr bool _Width<i16,16> = true;
	template<> inline constexpr bool _Width<i32,32> = true;
	template<> inline constexpr bool _Width<i64,64> = true;
	typedef   uint8_t   u8;
	typedef   uint16_t  u16;
	typedef   uint32_t  u32;
	typedef   uint64_t  u64;
	template<> inline constexpr bool _Width<u8,8> = true;
	template<> inline constexpr bool _Width<u16,8> = true;
	template<> inline constexpr bool _Width<u32,8> = true;
	template<> inline constexpr bool _Width<u64,8> = true;

	typedef  float16_t  f16;
	typedef  float32_t  f32;
	typedef  float64_t  f64;
	typedef  float128_t f128;

	typedef std::complex<f16> cf16;
	typedef std::complex<f32> cf32;
	typedef std::complex<f64> cf64;
	typedef std::complex<f128> cf128;

	template<class A, class B>
	class fixedreal {
	public:
		using T = fixedreal;
		A hi;
		B lo;
		constexpr fixedreal(A,B);
		constexpr ~fixedreal();
		T &operator+=(T&);
		T &operator-=(T&);
		T &operator*=(T&);
		T &operator/=(T&);
		friend T &operator+(T&,T&);
	};

	typedef fixedreal<u8, u8> r8u8;
	typedef fixedreal<u8, u16> r8u16;
	typedef fixedreal<u8, u32> r8u32;
	typedef fixedreal<u8, u64> r8u64;
	typedef fixedreal<u16, u8> r16u8;
	typedef fixedreal<u16, u16> r16u16;
	typedef fixedreal<u16, u32> r16u32;
	typedef fixedreal<u16, u64> r16u64;

	using std::array;
	using std::vector;
	using std::list;
	template<class T> using flist = std::forward_list<T>;
	using std::map;
	template<class A, class B> using umap = std::unordered_map<A,B>;
	using std::multimap;
	using std::set;

	template<class T>
	class Point2D {
		T x, y;
		Point2D(T x, T y) : x(x),y(y) {}
		Point2D &operator+=(Point2D<T> &m) {
			this->x+=m.x;
			this->y+=m.y;
			return this;
		}
		Point2D &operator*=(T m) {
			this->x*=m;
			this->y*=m;
			return this;
		}

	};
};


/*
concepts: Tensor, FixedPoint, Integer, Signed, Unsigned, Width<int>, Builtin, Fast, Real, Range<int>, Precision<int>, Custom, FloatingPoint, Platform, Tuple, Length<int>

types:
tensors:
	fixed-point:
		integers:
			i8									Tensor, FixedPoint, Integer, Signed, Width<8>, Builtin
			i16									Tensor, FixedPoint, Integer, Signed, Width<16>, Builtin
			i32									Tensor, FixedPoint, Integer, Signed, Width<32>, Builtin
			i64									Tensor, FixedPoint, Integer, Signed, Width<64>, Builtin
			u8									Tensor, FixedPoint, Integer, Unsigned, Width<8>, Builtin
			u16									Tensor, FixedPoint, Integer, Unsigned, Width<16>, Builtin
			u32									Tensor, FixedPoint, Integer, Unsigned, Width<32>, Builtin
			u64									Tensor, FixedPoint, Integer, Unsigned, Width<64>, Builtin

			i8f									Tensor, FixedPoint, Integer, Signed, Fast, Builtin
			i16f								Tensor, FixedPoint, Integer, Signed, Fast, Builtin
			i32f								Tensor, FixedPoint, Integer, Signed, Fast, Builtin
			i64f								Tensor, FixedPoint, Integer, Signed, Fast, Builtin
			u8f									Tensor, FixedPoint, Integer, Unsigned, Fast, Builtin
			u16f								Tensor, FixedPoint, Integer, Unsigned, Fast, Builtin
			u32f								Tensor, FixedPoint, Integer, Unsigned, Fast, Builtin
			u64f								Tensor, FixedPoint, Integer, Unsigned, Fast, Builtin
		real:
			r8p8		fixed point 8.8			Tensor, FixedPoint, Real, Signed, Width<16>, Range<8>, Precision<8>, Custom
			r8p16		fixed point 8.16		Tensor, FixedPoint, Real, Signed, Width<24>, Range<8>, Precision<16>, Custom
			r8p32		fixed point 8.32		Tensor, FixedPoint, Real, Signed, Width<40>, Range<8>, Precision<32>, Custom
			r8p64		fixed point 8.64		Tensor, FixedPoint, Real, Signed, Width<72>, Range<8>, Precision<64>, Custom
			r16p8		fixed point 16.8		Tensor, FixedPoint, Real, Signed, Width<24>, Range<16>, Precision<8>, Custom
			r16p16		fixed point 16.16		Tensor, FixedPoint, Real, Signed, Width<32>, Range<16>, Precision<16>, Custom
			r16p32		fixed point 16.32		Tensor, FixedPoint, Real, Signed, Width<48>, Range<16>, Precision<32>, Custom
			r16p64		fixed point 16.64		Tensor, FixedPoint, Real, Signed, Width<80>, Range<16>, Precision<64>, Custom
			r32p8		fixed point 32.8		Tensor, FixedPoint, Real, Signed, Width<40>, Range<32>, Precision<8>, Custom
			r32p16		fixed point 32.16		Tensor, FixedPoint, Real, Signed, Width<48>, Range<32>, Precision<16>, Custom
			r32p32		fixed point 32.32		Tensor, FixedPoint, Real, Signed, Width<64>, Range<32>, Precision<32>, Custom
			r32p64		fixed point 32.64		Tensor, FixedPoint, Real, Signed, Width<96>, Range<32>, Precision<64>, Custom
			r64p8		fixed point 64.8		Tensor, FixedPoint, Real, Signed, Width<72>, Range<64>, Precision<8>, Custom
			r64p16		fixed point 64.16		Tensor, FixedPoint, Real, Signed, Width<80>, Range<64>, Precision<16>, Custom
			r64p32		fixed point 64.32		Tensor, FixedPoint, Real, Signed, Width<96>, Range<64>, Precision<32>, Custom
			r64p64		fixed point 64.64		Tensor, FixedPoint, Real, Signed, Width<128>, Range<64>, Precision<64>, Custom

			r16			alias for r8p8
			r32			alias for r16p16
			r64			alias for r32p32
			r128		alias for r64p64
	floating-point:
		f16									Tensor, FloatingPoint, Real, Signed, Width<16>, Builtin
		f32									Tensor, FloatingPoint, Real, Signed, Width<32>, Builtin
		f64									Tensor, FloatingPoint, Real, Signed, Width<64>, Builtin
		f128								Tensor, FloatingPoint, Real, Signed, Width<128>, Builtin
	other:
		bool								Tensor, *Width<1>, Builtin
		plChar								Tensor, FixedPoint, Integer, Platform, Builtin
		plSignedChar	[signed char]		Tensor, FixedPoint, Integer, Signed, Platform, Builtin
		plUnsignedChar	[unsigned char]		Tensor, FixedPoint, Integer, Unsigned, Platform, Builtin
		plShort			[short]				Tensor, FixedPoint, Integer, Platform, Builtin
		plInt								Tensor, FixedPoint, Integer, Platform, Builtin
		plLong								Tensor, FixedPoint, Integer, Platform, Builtin
tuples:
	2D points								Tuple, Length<2>, Custom
	complex numbers							Tuple, Length<2>
	3D points								Tuple, Length<3>, Custom
	quaternions								Tuple, Length<4>, Custom
special:
	arrays									
	functions								
	mappings
	strings
	generators
	rational numbers (also means we can do lazy division! hooray!)


	types:
	i8			[int8_t]						Tensor, FixedPoint, Integer, Signed, Width<8>, Builtin
	i16			[int16_t]						Tensor, FixedPoint, Integer, Signed, Width<16>, Builtin
	i32			[int32_t]						Tensor, FixedPoint, Integer, Signed, Width<32>, Builtin
	i64			[int64_t]						Tensor, FixedPoint, Integer, Signed, Width<64>, Builtin
	u8			[uint8_t]						Tensor, FixedPoint, Integer, Unsigned, Width<8>, Builtin
	u16			[uint16_t]						Tensor, FixedPoint, Integer, Unsigned, Width<16>, Builtin
	u32			[uint32_t]						Tensor, FixedPoint, Integer, Unsigned, Width<32>, Builtin
	u64			[uint64_t]						Tensor, FixedPoint, Integer, Unsigned, Width<64>, Builtin
	i8f			[int_fast8_t]					Tensor, FixedPoint, Integer, Signed, Fast, Builtin
	i16f		[int_fast16_t]					Tensor, FixedPoint, Integer, Signed, Fast, Builtin
	i32f		[int_fast32_t]					Tensor, FixedPoint, Integer, Signed, Fast, Builtin
	i64f		[int_fast64_t]					Tensor, FixedPoint, Integer, Signed, Fast, Builtin
	u8f			[uint_fast8_t]					Tensor, FixedPoint, Integer, Unsigned, Fast, Builtin
	u16f		[uint_fast16_t]					Tensor, FixedPoint, Integer, Unsigned, Fast, Builtin
	u32f		[uint_fast32_t]					Tensor, FixedPoint, Integer, Unsigned, Fast, Builtin
	u64f		[uint_fast64_t]					Tensor, FixedPoint, Integer, Unsigned, Fast, Builtin
	r8s8		fixed point 8.8					Tensor, FixedPoint, Real, Signed, Width<16>, Range<8>, Precision<8>, Custom
	r8s16		fixed point 8.16				Tensor, FixedPoint, Real, Signed, Width<24>, Range<8>, Precision<16>, Custom
	r8s32		fixed point 8.32				Tensor, FixedPoint, Real, Signed, Width<40>, Range<8>, Precision<32>, Custom
	r8s64		fixed point 8.64				Tensor, FixedPoint, Real, Signed, Width<72>, Range<8>, Precision<64>, Custom
	r16s8		fixed point 16.8				Tensor, FixedPoint, Real, Signed, Width<24>, Range<16>, Precision<8>, Custom
	r16s16		fixed point 16.16				Tensor, FixedPoint, Real, Signed, Width<32>, Range<16>, Precision<16>, Custom
	r16s32		fixed point 16.32				Tensor, FixedPoint, Real, Signed, Width<48>, Range<16>, Precision<32>, Custom
	r16s64		fixed point 16.64				Tensor, FixedPoint, Real, Signed, Width<80>, Range<16>, Precision<64>, Custom
	r32s8		fixed point 32.8				Tensor, FixedPoint, Real, Signed, Width<40>, Range<32>, Precision<8>, Custom
	r32s16		fixed point 32.16				Tensor, FixedPoint, Real, Signed, Width<48>, Range<32>, Precision<16>, Custom
	r32s32		fixed point 32.32				Tensor, FixedPoint, Real, Signed, Width<64>, Range<32>, Precision<32>, Custom
	r32s64		fixed point 32.64				Tensor, FixedPoint, Real, Signed, Width<96>, Range<32>, Precision<64>, Custom
	r64s8		fixed point 64.8				Tensor, FixedPoint, Real, Signed, Width<72>, Range<64>, Precision<8>, Custom
	r64s16		fixed point 64.16				Tensor, FixedPoint, Real, Signed, Width<80>, Range<64>, Precision<16>, Custom
	r64s32		fixed point 64.32				Tensor, FixedPoint, Real, Signed, Width<96>, Range<64>, Precision<32>, Custom
	r64s64		fixed point 64.64				Tensor, FixedPoint, Real, Signed, Width<128>, Range<64>, Precision<64>, Custom
	r8u8		unsigned fixed point 8.8		Tensor, FixedPoint, Real, Unsigned, Width<16>, Range<8>, Precision<8>, Custom
	r8u16		unsigned fixed point 8.16		Tensor, FixedPoint, Real, Unsigned, Width<24>, Range<8>, Precision<16>, Custom
	r8u32		unsigned fixed point 8.32		Tensor, FixedPoint, Real, Unsigned, Width<40>, Range<8>, Precision<32>, Custom
	r8u64		unsigned fixed point 8.64		Tensor, FixedPoint, Real, Unsigned, Width<72>, Range<8>, Precision<64>, Custom
	r16u8		unsigned fixed point 16.8		Tensor, FixedPoint, Real, Unsigned, Width<24>, Range<16>, Precision<8>, Custom
	r16u16		unsigned fixed point 16.16		Tensor, FixedPoint, Real, Unsigned, Width<32>, Range<16>, Precision<16>, Custom
	r16u32		unsigned fixed point 16.32		Tensor, FixedPoint, Real, Unsigned, Width<48>, Range<16>, Precision<32>, Custom
	r16u64		unsigned fixed point 16.64		Tensor, FixedPoint, Real, Unsigned, Width<80>, Range<16>, Precision<64>, Custom
	r32u8		unsigned fixed point 32.8		Tensor, FixedPoint, Real, Unsigned, Width<40>, Range<32>, Precision<8>, Custom
	r32u16		unsigned fixed point 32.16		Tensor, FixedPoint, Real, Unsigned, Width<48>, Range<32>, Precision<16>, Custom
	r32u32		unsigned fixed point 32.32		Tensor, FixedPoint, Real, Unsigned, Width<64>, Range<32>, Precision<32>, Custom
	r32u64		unsigned fixed point 32.64		Tensor, FixedPoint, Real, Unsigned, Width<96>, Range<32>, Precision<64>, Custom
	r64u8		unsigned fixed point 64.8		Tensor, FixedPoint, Real, Unsigned, Width<72>, Range<64>, Precision<8>, Custom
	r64u16		unsigned fixed point 64.16		Tensor, FixedPoint, Real, Unsigned, Width<80>, Range<64>, Precision<16>, Custom
	r64u32		unsigned fixed point 64.32		Tensor, FixedPoint, Real, Unsigned, Width<96>, Range<64>, Precision<32>, Custom
	r64u64		unsigned fixed point 64.64		Tensor, FixedPoint, Real, Unsigned, Width<128>, Range<64>, Precision<64>, Custom
	r16s		[r8s8]							Tensor, FixedPoint, Real, Signed, Width<16>, Range<8>, Precision<8>, Custom
	r32s		[r16s16]						Tensor, FixedPoint, Real, Signed, Width<32>, Range<16>, Precision<16>, Custom
	r64s		[r32s32]						Tensor, FixedPoint, Real, Signed, Width<64>, Range<32>, Precision<32>, Custom
	r128s		[r64s64]						Tensor, FixedPoint, Real, Signed, Width<128>, Range<64>, Precision<64>, Custom
	r16u		[r8u8]							Tensor, FixedPoint, Real, Unsigned, Width<16>, Range<8>, Precision<8>, Custom
	r32u		[r16u16]						Tensor, FixedPoint, Real, Unsigned, Width<32>, Range<16>, Precision<16>, Custom
	r64u		[r32u32]						Tensor, FixedPoint, Real, Unsigned, Width<64>, Range<32>, Precision<32>, Custom
	r128u		[r64u64]						Tensor, FixedPoint, Real, Unsigned, Width<128>, Range<64>, Precision<64>, Custom
	floating-point:
		f16									Tensor, FloatingPoint, Real, Signed, Width<16>, Builtin
		f32									Tensor, FloatingPoint, Real, Signed, Width<32>, Builtin
		f64									Tensor, FloatingPoint, Real, Signed, Width<64>, Builtin
		f128								Tensor, FloatingPoint, Real, Signed, Width<128>, Builtin
		bool								Tensor, *Width<1>, Builtin
		plChar								Tensor, Platform, Builtin
		plShort								Tensor, Platform, Builtin
		plInt								Tensor, Platform, Builtin
		plLong								Tensor, Platform, Builtin
	2D points								Tuple, Length<2>, Custom
	complex numbers							Tuple, Length<2>
	3D points								Tuple, Length<3>, Custom
	quaternions								Tuple, Length<4>, Custom
	array									Mapping, Array, Ordered
	functions								Mapping, Function
	map										Mapping, Ordered, Unique
	multimap								Mapping, Ordered, NonUnique
	umap		(unordered_map)				Mapping, Unordered, Unique
	umultimap	(unordered_multimap)		Mapping, Unordered, NonUnique, Hashing
	set										Set, Ordered, Unique
	multiset								Set, Ordered, NonUnique
	uset	(unordered_set)					Set, Unordered, Unique, Hashing
	strings									Array
	generators								Functional
	rational numbers (also means we can do lazy division! hooray!)
*/