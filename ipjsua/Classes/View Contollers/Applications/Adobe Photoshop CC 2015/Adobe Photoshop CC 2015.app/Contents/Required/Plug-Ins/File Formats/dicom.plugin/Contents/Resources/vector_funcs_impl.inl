
#include <math.h>


GML_NAMESPACE_BEGIN


//--------------------------------------------------------------------------
// generators

template < STRATEGY O, int N, typename T >
GML_FORCEINLINE void vzero ( T * const dst )
{
	GML_ASSERT( dst );

	vset< O, N >( dst, T( 0 ) );
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE void vrand ( T * const dst )
{
	vrand< O, N, T >( dst, T( 0 ), T( 1 ) );
}


template < STRATEGY O, int N, typename T >
GML_INLINE void vrand ( T * const dst, T min, T max )
{
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = T( rand() );
	vdiv< O, N >( dst, T( RAND_MAX ) / ( max - min ), dst );
//	vmul< O, N >( dst, max - min, dst );
//	vdiv< O, N >( dst, T( RAND_MAX ), dst );
	vadd< O, N >( dst, min, dst );
}


template < STRATEGY O, int N, typename T >
GML_INLINE void vset ( T * const dst, T x )
{
	GML_ASSERT( dst );
	
	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = x;
}


template < STRATEGY O, int N, typename T >
GML_INLINE void vset ( T * const dst, T x, T y, T z, T w )
{
	GML_ASSERT( dst );

	if( N >= 1 )
		dst[ 0 ] = x;
	if( N >= 2 )
		dst[ 1 ] = y;
	if( N >= 3 )
		dst[ 2 ] = z;
	if( N >= 4 )
		dst[ 3 ] = w;
	for( int i = 4 ; i < N ; ++i )
		dst[ i ] = T( 0 );
}


//--------------------------------------------------------------------------
// unary operations

template < STRATEGY O, int N, typename T >
GML_INLINE void vcopy ( T const * const lhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = lhs[ i ];
}


template < STRATEGY O, int N, typename T >
GML_INLINE void vneg ( T const * const lhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = - lhs[ i ];
}


template < STRATEGY O, int N, typename T >
GML_INLINE void vrecip ( T const * const lhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = T( 1 ) / lhs[ i ];
}


template < STRATEGY O, int N, typename T >
GML_INLINE void vabs ( T const * const lhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = abs< T >( lhs[ i ] );
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE void vclamp ( T const * const lhs, T * const dst )
{
	vclamp< O, N, T >( lhs, dst, T( 0 ), T( 1 ) );
}


template < STRATEGY O, int N, typename T >
GML_INLINE void vclamp ( T const * const lhs, T * const dst, T min, T max )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = clamp( lhs[ i ], min, max );
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE bool vnorm ( T const * const lhs, T * const dst )
{
	return vnorm< O, N, T >( lhs, dst, NULL );
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE bool vnorm ( T const * const lhs, T * const dst, T const * const fallback )
{
	return vnorm< O, N, T >( lhs, dst, fallback, epsilon< T >() );
}


template < STRATEGY O, int N, typename T >
GML_INLINE bool vnorm ( T const * const lhs, T * const dst, T const * const fallback, T eps  )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	T magsqr = vdot< O, N >( lhs, lhs );
	bool mask = ( magsqr > eps );
	T const * const ptr = mask ? lhs : ( ( fallback ) ? fallback : lhs );
	T denom = mask ? ( 1 / sqrt< T >( magsqr ) ) : T( 1 );
	vmul< O, N >( ptr, denom, dst );

	return mask;
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE T vlensqr ( T const * const lhs )
{
	GML_ASSERT( lhs );

	return vdot< O, N >( lhs, lhs );
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE T vlen ( T const * const lhs )
{
	GML_ASSERT( lhs );

	return sqrt< T >( vlensqr< O, N >( lhs ) );
}


template < STRATEGY O, int N, typename T >
GML_INLINE T vmax ( T const * const lhs )
{
	GML_ASSERT( lhs );

	T maxval = lhs[ 0 ];
	for( int i = 1 ; i < N ; ++i )
		maxval = GML_MAX( maxval, lhs[ i ] );
	return maxval;
}


template < STRATEGY O, int N, typename T >
GML_INLINE T vmin ( T const * const lhs )
{
	GML_ASSERT( lhs );

	T minval = lhs[ 0 ];
	for( int i = 1 ; i < N ; ++i )
		minval = GML_MIN( minval, lhs[ i ] );
	return minval;
}


//--------------------------------------------------------------------------
// scalar algebra

template < STRATEGY O, int N, typename T >
GML_INLINE void vadd ( T const * const lhs, T rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = lhs[ i ] + rhs;
}


template < STRATEGY O, int N, typename T >
GML_INLINE void vsub ( T const * const lhs, T rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = lhs[ i ] - rhs;
}


template < STRATEGY O, int N, typename T >
GML_INLINE void vmul ( T const * const lhs, T rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = lhs[ i ] * rhs;
}


template < STRATEGY O, int N, typename T >
GML_INLINE void vdiv ( T const * const lhs, T rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = lhs[ i ] / rhs;
}


//--------------------------------------------------------------------------
// component-wise algebra

template < STRATEGY O, int N, typename T >
GML_INLINE void vadd ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = lhs[ i ] + rhs[ i ];
}


template < STRATEGY O, int N, typename T >
GML_INLINE void vsub ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = lhs[ i ] - rhs[ i ];
}


template < STRATEGY O, int N, typename T >
GML_INLINE void vmul ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = lhs[ i ] * rhs[ i ];
}


template < STRATEGY O, int N, typename T >
GML_INLINE void vdiv ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = lhs[ i ] / rhs[ i ];
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE void vproj ( T const * const v, T const * const onto, T * const dst )
{
	vproj< O, N, T >( v, onto, dst, epsilon< T >() );
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE void vproj ( T const * const v, T const * const onto, T * const dst, T eps )
{
	GML_ASSERT( v );
	GML_ASSERT( onto );
	GML_ASSERT( dst );

	T l2 = vlensqr< O, N >( onto );
	T scale = ( l2 < eps ) ? T( 0 ) : ( vdot< O, N >( v, onto ) / l2 );
	vmul< O, N >( onto, scale, dst );
}


template < STRATEGY O, int N, typename T >
GML_INLINE void vmax ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = GML_MAX( lhs[ i ], rhs[ i ] );
}


template < STRATEGY O, int N, typename T >
GML_INLINE void vmin ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = GML_MIN( lhs[ i ], rhs[ i ] );
}


//--------------------------------------------------------------------------
// vector operations

template < STRATEGY O, int N, typename T >
GML_INLINE T vdot ( T const * const lhs, T const * const rhs )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );

	T sum = T( 0 );
	for( int i = 0 ; i < N ; ++i )
		sum += lhs[ i ] * rhs[ i ];
	return sum;
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE T vangle ( T const * const lhs, T const * const rhs )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );

	T denom = sqrt< T >( ( vlensqr< O, N >( lhs ) * vlensqr< O, N >( rhs ) ) );
	return acos< T >( vdot< O, N >( lhs, rhs ) / denom );
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE T vcomp ( T const * const v, T const * const onto )
{
	return vcomp< O, N, T >( v, onto, epsilon< T >() );
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE T vcomp ( T const * const v, T const * const onto, T eps )
{
	GML_ASSERT( v );
	GML_ASSERT( onto );

	T l = vlen< O, N >( onto );
	return ( l < eps ) ? T( 0 ) : ( vdot< O, N >( v, onto ) / l );
}


//--------------------------------------------------------------------------
// comparison operations

template < STRATEGY O, int N, typename T >
GML_INLINE bool veq ( T const * const lhs, T const * const rhs )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );

	GML_SSE_ALIGN T eval[ N ];
	vneq< O, N >( lhs, rhs, eval );
	int mask = vmask< O, N >( eval );
	return ( mask == 0 );
}


template < STRATEGY O, int N, typename T >
GML_INLINE bool vlt ( T const * const lhs, T const * const rhs )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );

	GML_SSE_ALIGN T tmp[ N ];
	vlt< O, N >( lhs, rhs, tmp );
	int mlt = vmask< O, N >( tmp );
	vgt< O, N >( lhs, rhs, tmp );
	int mgt = vmask< O, N >( tmp );
	return ( mlt > mgt );
}


template < STRATEGY O, int N, typename T >
GML_INLINE bool vgt ( T const * const lhs, T const * const rhs )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );

	GML_SSE_ALIGN T tmp[ N ];
	vlt< O, N >( lhs, rhs, tmp );
	int mlt = vmask< O, N >( tmp );
	vgt< O, N >( lhs, rhs, tmp );
	int mgt = vmask< O, N >( tmp );
	return ( mlt < mgt );
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE bool vneq ( T const * const lhs, T const * const rhs )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );

	return ! veq< O, N >( lhs, rhs );
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE bool vle ( T const * const lhs, T const * const rhs )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );

	return ! vgt< O, N >( lhs, rhs );
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE bool vge ( T const * const lhs, T const * const rhs )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );

	return ! vlt< O, N >( lhs, rhs );
}


template < STRATEGY O, int N, typename T >
GML_INLINE void veq ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = ( lhs[ i ] == rhs[ i ] ) ? allbitson< T >() : 0;
}


template < STRATEGY O, int N, typename T >
GML_INLINE void vneq ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = ( lhs[ i ] != rhs[ i ] ) ? allbitson< T >() : 0;
}


template < STRATEGY O, int N, typename T >
GML_INLINE void vlt ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = ( lhs[ i ] < rhs[ i ] ) ? allbitson< T >() : 0;
}


template < STRATEGY O, int N, typename T >
GML_INLINE void vgt ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = ( lhs[ i ] > rhs[ i ] ) ? allbitson< T >() : 0;
}


template < STRATEGY O, int N, typename T >
GML_INLINE void vle ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = ( lhs[ i ] <= rhs[ i ] ) ? allbitson< T >() : 0;
}


template < STRATEGY O, int N, typename T >
GML_INLINE void vge ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = ( lhs[ i ] >= rhs[ i ] ) ? allbitson< T >() : 0;
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE bool vrealeq ( T const * const lhs, T const * const rhs )
{
	return vrealeq< O, N, T >( lhs, rhs, epsilon< T >() );
}


template < STRATEGY O, int N, typename T >
GML_INLINE bool vrealeq ( T const * const lhs, T const * const rhs, T eps )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );

	GML_SSE_ALIGN T tmp[ N ];
	vsub< O, N >( lhs, rhs, tmp );
	vabs< O, N >( tmp, tmp );
	T r = vmax< O, N >( tmp );
	return ( r < eps );
}


//--------------------------------------------------------------------------

template < STRATEGY O, int N, typename T >
GML_INLINE bool vnan ( T const * const lhs )
{
	GML_ASSERT( lhs );

	bool eval = false;
	for( int i = 0 ; i < N ; ++i )
		eval = eval || GML_ISNAN( lhs[ i ] );
	return eval;
}


template < STRATEGY O, int N, typename T >
GML_INLINE bool vfinite ( T const * const lhs )
{
	GML_ASSERT( lhs );

	bool eval = true;
	for( int i = 0 ; i < N ; ++i )
		eval = eval && GML_ISFINITE( lhs[ i ] );
	return eval;
}


template < STRATEGY O, int N, typename T >
GML_INLINE int vmask ( T const * const lhs )
{
	GML_ASSERT( lhs );

	int mask = 0;
	for( int i = 0 ; i < N ; ++i )
		mask |= ( signbittoint< T >( lhs[ i ] ) << i );
	return mask;
}


//--------------------------------------------------------------------------
// bit operations

template < STRATEGY O, int N, typename T >
GML_INLINE void vand ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	const int bytes = N * sizeof( T );
	const int sizes[ 4 ] = { 4, 1, 2, 1 };
	const int n = bytes / sizes[ bytes % 4 ];

	for( int i = 0 ; i < n ; ++i )
		toptr< bytes >( dst )[ i ] = toptr< bytes >( lhs )[ i ] & toptr< bytes >( rhs )[ i ];
}


template < STRATEGY O, int N, typename T >
GML_INLINE void vandnot ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	const int bytes = N * sizeof( T );
	const int sizes[ 4 ] = { 4, 1, 2, 1 };
	const int n = bytes / sizes[ bytes % 4 ];

	for( int i = 0 ; i < n ; ++i )
		toptr< bytes >( dst )[ i ] = ( ~ toptr< bytes >( lhs )[ i ] ) & toptr< bytes >( rhs )[ i ];
}


template < STRATEGY O, int N, typename T >
GML_INLINE void vor ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	const int bytes = N * sizeof( T );
	const int sizes[ 4 ] = { 4, 1, 2, 1 };
	const int n = bytes / sizes[ bytes % 4 ];

	for( int i = 0 ; i < n ; ++i )
		toptr< bytes >( dst )[ i ] = toptr< bytes >( lhs )[ i ] | toptr< bytes >( rhs )[ i ];
}


template < STRATEGY O, int N, typename T >
GML_INLINE void vxor ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	const int bytes = N * sizeof( T );
	const int sizes[ 4 ] = { 4, 1, 2, 1 };
	const int n = bytes / sizes[ bytes % 4 ];

	for( int i = 0 ; i < n ; ++i )
		toptr< bytes >( dst )[ i ] = toptr< bytes >( lhs )[ i ] ^ toptr< bytes >( rhs )[ i ];
}


template < STRATEGY O, int N, typename T >
GML_INLINE void vnot ( T const * const lhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	const int bytes = N * sizeof( T );
	const int sizes[ 4 ] = { 4, 1, 2, 1 };
	const int n = bytes / sizes[ bytes % 4 ];

	for( int i = 0 ; i < n ; ++i )
		toptr< bytes >( dst )[ i ] = ~ toptr< bytes >( lhs )[ i ];
}


//--------------------------------------------------------------------------
// SSE operations

template < STRATEGY O, int N, typename T >
GML_INLINE void vselect ( T const * const mask, T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( mask );
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = ( mask[ i ] != 0 ) ? lhs[ i ] : rhs[ i ];
}


//--------------------------------------------------------------------------
// not available for all types

template < STRATEGY O, int N, typename T >
GML_FORCEINLINE bool vhomog ( T const * const lhs, T * const dst  )
{
	return vhomog< O, N, T >( lhs, dst, NULL );
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE bool vhomog ( T const * const lhs, T * const dst, T const * const fallback  )
{
	return vhomog< O, N, T >( lhs, dst, fallback, epsilon< T >() );
}


template < STRATEGY O, int N, typename T >
GML_INLINE bool vhomog ( T const * const lhs, T * const dst, T const * const fallback, T eps  )
{
	GML_COMPILE_ASSERT( N == 4 );
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	bool mask = ( lhs[ 3 ] > eps );
	T const * const ptr = mask ? lhs : ( ( fallback ) ? fallback : lhs );
	T denom = mask ? ( 1 / lhs[ 3 ] ) : T( 1 );
	vmul< O, N >( ptr, denom, dst );

	return mask;
}


// warning: conditional expression is constant
#pragma warning( disable : 4127 )

template < STRATEGY O, int N, typename T >
GML_FORCEINLINE void vcross ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_COMPILE_ASSERT( N == 4 || N == 3 );
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	dst[ 0 ] = lhs[ 1 ] * rhs[ 2 ] - lhs[ 2 ] * rhs[ 1 ];
	dst[ 1 ] = lhs[ 2 ] * rhs[ 0 ] - lhs[ 0 ] * rhs[ 2 ];
	dst[ 2 ] = lhs[ 0 ] * rhs[ 1 ] - lhs[ 1 ] * rhs[ 0 ];

	if( N == 4 )
		dst[ 3 ] = 0;
}

#pragma warning( default : 4127 )


//--------------------------------------------------------------------------

GML_NAMESPACE_END
