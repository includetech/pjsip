
#include "vector_funcs.hpp"


GML_NAMESPACE_BEGIN


//--------------------------------------------------------------------------
//

template < STRATEGY O, typename T >
GML_FORCEINLINE void qzero ( T * const dst )
{
	GML_ASSERT( dst );

	vset< O, 4 >( dst, T( 0 ) );
}


template < STRATEGY O, typename T >
GML_FORCEINLINE void qiden ( T * const dst )
{
	GML_ASSERT( dst );

	qset< O >( dst, T( 0 ), T( 0 ), T( 0 ), T( 1 ) );
}


template < STRATEGY O, typename T >
GML_FORCEINLINE void qset ( T * const dst, T x, T y, T z, T w )
{
	GML_ASSERT( dst );

	dst[ 0 ] = x;
	dst[ 1 ] = y;
	dst[ 2 ] = z;
	dst[ 3 ] = w;
}


//--------------------------------------------------------------------------
//

template < STRATEGY O, typename T >
GML_FORCEINLINE void qcopy ( T const * const lhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	vcopy< O, 4 >( lhs, dst );
}


template < STRATEGY O, typename T >
GML_FORCEINLINE void qneg ( T const * const lhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	vneg< O, 4 >( lhs, dst );
}


template < STRATEGY O, typename T >
GML_FORCEINLINE void qconj ( T const * const lhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	dst[ 0 ] = - lhs[ 0 ];
	dst[ 1 ] = - lhs[ 1 ];
	dst[ 2 ] = - lhs[ 2 ];
	dst[ 3 ] = lhs[ 3 ];
}


template < STRATEGY O, typename T >
GML_FORCEINLINE bool qnorm ( T const * const lhs, T * const dst )
{
	return qnorm< O, T >( lhs, dst, NULL );
}


template < STRATEGY O, typename T >
GML_FORCEINLINE bool qnorm ( T const * const lhs, T * const dst, T const * const fallback )
{
	return qnorm< O, T >( lhs, dst, fallback, epsilon< T >() );
}


template < STRATEGY O, typename T >
GML_FORCEINLINE bool qnorm ( T const * const lhs, T * const dst, T const * const fallback, T eps )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	return vnorm< O, 4 >( lhs, dst, fallback, eps );
}


//--------------------------------------------------------------------------
//

template < STRATEGY O, typename T >
GML_FORCEINLINE T qlen ( T const * const lhs )
{
	GML_ASSERT( lhs );

	return vlen< O, 4 >( lhs );
}


template < STRATEGY O, typename T >
GML_FORCEINLINE T qlensqr ( T const * const lhs )
{
	GML_ASSERT( lhs );

	return vlensqr< O, 4 >( lhs );
}


//--------------------------------------------------------------------------
//

template < STRATEGY O, typename T >
GML_FORCEINLINE void qadd ( T const * const lhs, T rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	dst[ 0 ] = lhs[ 0 ];
	dst[ 1 ] = lhs[ 1 ];
	dst[ 2 ] = lhs[ 2 ];
	dst[ 3 ] = lhs[ 3 ] + rhs;
}


template < STRATEGY O, typename T >
GML_FORCEINLINE void qsub ( T const * const lhs, T rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	dst[ 0 ] = lhs[ 0 ];
	dst[ 1 ] = lhs[ 1 ];
	dst[ 2 ] = lhs[ 2 ];
	dst[ 3 ] = lhs[ 3 ] - rhs;
}


template < STRATEGY O, typename T >
GML_FORCEINLINE void qmul ( T const * const lhs, T rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	vmul< O, 4 >( lhs, rhs, dst );
}


template < STRATEGY O, typename T >
GML_FORCEINLINE void qdiv ( T const * const lhs, T rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	vdiv< O, 4 >( lhs, rhs, dst );
}


//--------------------------------------------------------------------------
//

template < STRATEGY O, typename T >
GML_FORCEINLINE void qadd ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	vadd< O, 4 >( lhs, rhs, dst );
}


template < STRATEGY O, typename T >
GML_FORCEINLINE void qsub ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	vsub< O, 4 >( lhs, rhs, dst );
}


template < STRATEGY O, typename T >
GML_FORCEINLINE void qmul ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	//
	dst[ 0 ] = lhs[ 3 ]*rhs[ 0 ] + lhs[ 0 ]*rhs[ 3 ] + lhs[ 1 ]*rhs[ 2 ] - lhs[ 2 ]*rhs[ 1 ];
	dst[ 1 ] = lhs[ 3 ]*rhs[ 1 ] + lhs[ 1 ]*rhs[ 3 ] + lhs[ 2 ]*rhs[ 0 ] - lhs[ 0 ]*rhs[ 2 ];
	dst[ 2 ] = lhs[ 3 ]*rhs[ 2 ] + lhs[ 2 ]*rhs[ 3 ] + lhs[ 0 ]*rhs[ 1 ] - lhs[ 1 ]*rhs[ 0 ];
	dst[ 3 ] = lhs[ 3 ]*rhs[ 3 ] - lhs[ 0 ]*rhs[ 0 ] - lhs[ 1 ]*rhs[ 1 ] - lhs[ 2 ]*rhs[ 2 ];
}


//--------------------------------------------------------------------------
//

template < STRATEGY O, typename T >
GML_FORCEINLINE T qdot ( T const * const lhs, T const * const rhs )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );

	return vdot< O, 4 >( lhs, rhs );
}


//--------------------------------------------------------------------------
//

template < STRATEGY O, typename T >
GML_FORCEINLINE bool qeq ( T const * const lhs, T const * const rhs )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );

	return veq< O, 4 >( lhs, rhs );
}


template < STRATEGY O, typename T >
GML_FORCEINLINE bool qneq ( T const * const lhs, T const * const rhs )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );

	return vneq< O, 4 >( lhs, rhs );
}


template < STRATEGY O, typename T >
GML_FORCEINLINE bool qlt ( T const * const lhs, T const * const rhs )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );

	return vlt< O, 4 >( lhs, rhs );
}


template < STRATEGY O, typename T >
GML_FORCEINLINE bool qgt ( T const * const lhs, T const * const rhs )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );

	return vgt< O, 4 >( lhs, rhs );
}


template < STRATEGY O, typename T >
GML_FORCEINLINE bool qle ( T const * const lhs, T const * const rhs )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );

	return vle< O, 4 >( lhs, rhs );
}


template < STRATEGY O, typename T >
GML_FORCEINLINE bool qge ( T const * const lhs, T const * const rhs )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );

	return vge< O, 4 >( lhs, rhs );
}


template < STRATEGY O, typename T >
GML_FORCEINLINE bool qrealeq ( T const * const lhs, T const * const rhs )
{
	return qrealeq< O, T >( lhs, rhs, epsilon< T >() );
}


template < STRATEGY O, typename T >
GML_FORCEINLINE bool qrealeq ( T const * const lhs, T const * const rhs, T eps )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );

	return vrealeq< O, 4 >( lhs, rhs, eps );
}


//--------------------------------------------------------------------------
//

template < STRATEGY O, typename T >
GML_FORCEINLINE bool qnan ( T const * const lhs )
{
	GML_ASSERT( lhs );

	return vnan< O, 4 >( lhs );
}


template < STRATEGY O, typename T >
GML_FORCEINLINE bool qfinite ( T const * const lhs )
{
	GML_ASSERT( lhs );

	return vfinite< O, 4 >( lhs );
}


//--------------------------------------------------------------------------

GML_NAMESPACE_END
