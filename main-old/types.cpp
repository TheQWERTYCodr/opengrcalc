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
#include "include/types.hpp"
using namespace std;

namespace types
{
	// namespace
	namespace {
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
	};

	typedef    int8_t   i8;
	typedef    int16_t  i16;
	typedef    int32_t  i32;
	typedef    int64_t  i64;
	
	typedef   uint8_t   u8;
	typedef   uint16_t  u16;
	typedef   uint32_t  u32;
	typedef   uint64_t  u64;

	typedef  float16_t  f16;
	typedef  float32_t  f32;
	typedef  float64_t  f64;
	typedef  float128_t f128;

	typedef std::complex<f16> c16;
	typedef std::complex<f32> c32;
	typedef std::complex<f64> c64;
	typedef std::complex<f128> c128;


	
	using std::array;
	using std::vector;
	using std::list;
	template<class T> using flist = std::forward_list<T>;
	using std::map;
	template<class A, class B> using umap = std::unordered_map<A,B>;
	using std::multimap;
	using std::set;

	
};


/*
concepts: Tensor, FixedPoint, Integer, Signed, Unsigned, Width<int>, Builtin, Fast, Real, Range<int>, Precision<int>, FloatingPoint, Platform, Tuple, Length

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
		plChar								Tensor, Platform, Builtin
		plShort								Tensor, Platform, Builtin
		plInt								Tensor, Platform, Builtin
		plLong								Tensor, Platform, Builtin
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
*/