#pragma once
#include <cmath>
#include "./types.hpp"
using namespace std;
template<class T>
class math {
private:
	math() {}
	~math() {}
public:
	static T abs(T x) {return ::fabs(x);}
	static T mod(T x, T y) {return ::fmod(x,y);}
	static T remainder(T x, T y) {return ::remainder(x,y);}
	static T remquo(T x, T y, int *quo) {return ::remquo(x,y,quo);}
	static T fma(T x, T y) {return ::fma(x,y);}
	static T max(T x, T y) {return ::max(x,y);}
	static T min(T x, T y) {return ::min(x,y);}
	static T posdiff(T x, T y) {return ::fdim(x,y);}
	static T exp(T x) {return ::exp(x);}
	static T exp10(T x) {return ::pow(10.0,x);} // exp10 isn't provided by the standard library
	static T exp2(T x) {return ::exp2(x);}
	static T expm1(T x) {return ::expm1(x);}
	static T log(T x) {return ::log(x);}
	static T log10(T x) {return ::log10(x);}
	static T log2(T x) {return ::log2(x);}
	static T log1p(T x) {return ::log1p(x);}
	static T pow(T x, T y) {return ::pow(x);}
	static T sqrt(T x) {return ::sqrt(x);}
	static T cbrt(T x) {return ::cbrt(x);}
	static T hypot(T x, T y) {return ::hypot(x,y);}
	static T sin(T x) {return ::sin(x);}
	static T cos(T x) {return ::cos(x);}
	static T tan(T x) {return ::tan(x);}
	static T asin(T x) {return ::asin(x);}
	static T acos(T x) {return ::acos(x);}
	static T atan(T x) {return ::atan(x);}
	static T atan2(T y, T x) {
		static_assert(std::is_convertible<T,types::f128>::value, "atan2(y,x) only accepts real arguments");
		return ::atan2(y,x);
	}
	static T sinh(T x) {return ::sinh(x);}
	static T cosh(T x) {return ::cosh(x);}
	static T tanh(T x) {return ::tanh(x);}
	static T asinh(T x) {return ::asinh(x);}
	static T acosh(T x) {return ::acosh(x);}
	static T atanh(T x) {return ::atanh(x);}
	static T erf(T x) {return ::erf(x);}
	static T erfc(T x) {return ::erfc(x);}
	static T tgamma(T x) {return ::tgamma(x);}
	static T lgamma(T x) {return ::lgamma(x);}
	static T ceil(T x) {
		static_assert(std::is_convertible<T,types::f128>::value, "ceil(x) only accepts real arguments");
		return ::ceil(x);
	}
	static T floor(T x) {
		static_assert(std::is_convertible<T,types::f128>::value, "floor(x) only accepts real arguments");
		return ::floor(x);
	}
	static T trunc(T x) {return ::trunc(x);}
	static T round(T x) {return ::round(x);}
};

template <class T>
class math<complex<T>> {
	typedef complex<T> C;
	static T real(C x) {return x.real();}
	static T imag(C x) {return x.imag();}
	static T conj(C x) {return std::conj(x);}
};