GML_NAMESPACE_BEGIN


// column major storage
#define IDX(N,R,C) ( ( C ) * ( N ) + ( R ) )


//--------------------------------------------------------------------------
// generators

template < STRATEGY O, int N, typename T >
GML_FORCEINLINE void mzero ( T * const dst )
{
	GML_ASSERT( dst );

	mset< O, N >( dst, T( 0 ) );
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE void miden ( T * const dst )
{
	GML_ASSERT( dst );

	mdiag< O, N >( dst, T( 1 ) );
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE void mrand ( T * const dst )
{
	mrand< O, N, T >( dst, T( 0 ), T( 1 ) );
}


template < STRATEGY O, int N, typename T >
GML_INLINE void mrand ( T * const dst, T min, T max )
{
	GML_ASSERT( dst );

	for( int i = 0 ; i < N*N ; ++i )
		dst[ i ] = T( rand() );
	mdiv< O, N >( dst, T( RAND_MAX ), dst );
	mmul< O, N >( dst, max - min, dst );
	madd< O, N >( dst, min, dst );
}


template < STRATEGY O, int N, typename T >
GML_INLINE void mset ( T * const dst, T x )
{
	GML_ASSERT( dst );

	for( int i = 0 ; i < N*N ; ++i )
		dst[ i ] = x;
}


template < STRATEGY O, int N, typename T >
GML_INLINE void mdiag ( T * const dst, T x )
{
	GML_ASSERT( dst );

	mset< O, N, T >( dst, T( 0 ) );
	for( int i = 0 ; i < N ; ++i )
		dst[ IDX( N, i, i ) ] = x;
}


template < STRATEGY O, int N, typename T >
GML_INLINE void mdiag ( T * const dst, T const * const v )
{
	GML_ASSERT( dst );
	GML_ASSERT( v );

	mset< O, N, T >( dst, T( 0 ) );
	for( int i = 0 ; i < N ; ++i )
		dst[ IDX( N, i, i ) ] = v[ i ];
}


//--------------------------------------------------------------------------
// matrix handling

template < STRATEGY O, int N, typename T >
GML_FORCEINLINE T & melem ( T * const lhs, int row, int col )
{
	GML_ASSERT( lhs );
	GML_ASSERT( row >= 0 && row < N );
	GML_ASSERT( col >= 0 && col < N );

	return lhs[ IDX( N, row, col ) ];
}


template < STRATEGY O, int N, typename T >
GML_INLINE void mrow ( T const * const lhs, T * const dst, int row )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );
	GML_ASSERT( row >= 0 && row < N );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = lhs[ IDX( N, row, i ) ];
}


template < STRATEGY O, int N, typename T >
GML_INLINE void mcol ( T const * const lhs, T * const dst, int col )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );
	GML_ASSERT( col >= 0 && col < N );

	for( int i = 0 ; i < N ; ++i )
		dst[ i ] = lhs[ IDX( N, i, col ) ];
}


template < STRATEGY O, int N, typename T >
GML_INLINE T mmax ( T const * const lhs )
{
	GML_ASSERT( lhs );

	T maxval = vmax< O, N >( lhs );
	for( int i = 1 ; i < N ; ++i )
		maxval = GML_MAX( maxval, ( vmax< O, N >( lhs + i * N ) ) );
	return maxval;
}


template < STRATEGY O, int N, typename T >
GML_INLINE T mmin ( T const * const lhs )
{
	GML_ASSERT( lhs );

	T minval = vmin< O, N >( lhs );
	for( int i = 1 ; i < N ; ++i )
		minval = GML_MIN( minval, ( vmin< O, N >( lhs + i * N ) ) );
	return minval;
}


template < STRATEGY O, int N, typename T >
GML_INLINE void mcopy ( T const * const lhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );
	GML_ASSERT( lhs != dst );

	for( int i = 0 ; i < N ; ++i )
		vcopy< O, N >( lhs + i*N, dst + i*N );
}


template < STRATEGY O, int N, typename T >
GML_INLINE void mneg ( T const * const lhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		vneg< O, N >( lhs + i*N, dst + i*N );
}


template < STRATEGY O, int N, typename T >
GML_INLINE void mrecip ( T const * const lhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		vrecip< O, N >( lhs + i*N, dst + i*N );
}


template < STRATEGY O, int N, typename T >
GML_INLINE void mabs ( T const * const lhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		vabs< O, N >( lhs + i*N, dst + i*N );
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE void mclamp ( T const * const lhs, T * const dst )
{
	mclamp< O, N, T >( lhs, dst, T( 0 ), T( 1 ) );
}


template < STRATEGY O, int N, typename T >
GML_INLINE void mclamp ( T const * const lhs, T * const dst, T min, T max )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		vclamp< O, N >( lhs + i*N, dst + i*N, min, max );
}


//--------------------------------------------------------------------------
// determinant is messy because of template function partial specialization workaround

template < int N >
struct _mdetimpl
{
	template < STRATEGY O, typename T >
	static T mdet ( T const * const lhs );
};

template <>
struct _mdetimpl< 1 >
{
	template < STRATEGY O, typename T >
	static GML_FORCEINLINE T mdet ( T const * const lhs )
	{
		return lhs[ 0 ];
	}
};


template <>
struct _mdetimpl< 2 >
{
	template < STRATEGY O, typename T >
	static GML_FORCEINLINE T mdet ( T const * const lhs )
	{
		return ( lhs[ 0 ] * lhs[ 3 ] - lhs[ 1 ] * lhs[ 2 ] );
	}
};


template <>
struct _mdetimpl< 3 >
{
	template < STRATEGY O, typename T >
	static GML_FORCEINLINE T mdet ( T const * const lhs )
	{
		return ( lhs[ 0 ] * lhs[ 4 ] * lhs[ 8 ]
			+ lhs[ 3 ] * lhs[ 7 ] * lhs[ 2 ]
			+ lhs[ 6 ] * lhs[ 1 ] * lhs[ 5 ]
			- lhs[ 0 ] * lhs[ 7 ] * lhs[ 5 ]
			- lhs[ 3 ] * lhs[ 1 ] * lhs[ 8 ]
			- lhs[ 6 ] * lhs[ 4 ] * lhs[ 2 ] );
	}
};


template < int N >
template < STRATEGY O, typename T >
GML_INLINE T _mdetimpl< N >::mdet ( T const * const lhs )
{
	const int M = N - 1;
	GML_SSE_ALIGN T m[ M*M ];
	T det = 0;
	int sign;

	for( int c = 0 ; c < N ; ++c )
	{
		mminor< O, N, T >( lhs, m, 0, c );
		sign = ( c % 2 ) ? -1 : 1;
		det += sign * lhs[ IDX( N, 0, c ) ] * _mdetimpl< M >::template mdet< O, T >( m );
	}

	return det;
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE T mdet ( T const * const lhs )
{
	GML_ASSERT( lhs );

	return _mdetimpl< N >::template mdet< O, T >( lhs );
}


//--------------------------------------------------------------------------
// linear algebra

template < STRATEGY O, int N, typename T >
GML_INLINE T mtrace ( T const * const lhs )
{
	GML_ASSERT( lhs );

	T sum = 0;
	for( int i = 0 ; i < N ; ++i )
		sum += lhs[ IDX( N, i, i ) ];
	return sum;
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE void mtrans ( T const * const lhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );
	GML_ASSERT( lhs != dst );

	for( int i = 0 ; i < N ; ++i )
		for( int j = 0 ; j < N ; ++j )
			dst[ IDX( N, j, i ) ] = lhs[ IDX( N, i, j ) ];
}


template < STRATEGY O, int N, typename T >
GML_INLINE void mminor ( T const * const lhs, T * const dst, int row, int col )
{
	GML_COMPILE_ASSERT( N > 1 );
	GML_ASSERT( lhs );
	GML_ASSERT( dst );
	GML_ASSERT( lhs != dst );
	GML_ASSERT( row >= 0 && row < N );
	GML_ASSERT( col >= 0 && col < N );

	const int M = N - 1;
	for( int c = 0 ; c < M ; ++c )
	{
		int coff = ( c >= col ) ? 1 : 0;
		for( int r = 0 ; r < M ; ++r )
		{
			int roff = ( r >= row ) ? 1 : 0;
			dst[ IDX( M, r, c ) ] = lhs[ IDX( N, r + roff, c + coff ) ];
		}
	}
}


template < STRATEGY O, int N, typename T >
GML_INLINE T mcofactor ( T const * const lhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );
	GML_ASSERT( lhs != dst );

	const int M = N - 1;
	GML_SSE_ALIGN T m[ M*M ];
	T det = 0;
	for( int c = 0 ; c < N ; ++c )
	{
		for( int r = 0 ; r < N ; ++r )
		{
			mminor< O, N, T >( lhs, m, r, c );
			int sign = ( ( r + c ) % 2 ) ? -1 : 1;
			T val = sign * mdet< O, M, T >( m );
			dst[ IDX( N, r, c ) ] = val;

			det += ( r == 0 ) ? val * lhs[ IDX( N, r, c ) ] : 0;
		}
	}

	return det;
}


template < STRATEGY O, int N, typename T >
GML_INLINE T madjoint ( T const * const lhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );
	GML_ASSERT( lhs != dst );

	const int M = N - 1;
	GML_SSE_ALIGN T m[ M*M ];
	T det = 0;
	for( int c = 0 ; c < N ; ++c )
	{
		for( int r = 0 ; r < N ; ++r )
		{
			mminor< O, N, T >( lhs, m, r, c );
			int sign = ( ( r + c ) % 2 ) ? -1 : 1;
			T val = sign * mdet< O, M, T >( m );
			dst[ IDX( N, c, r ) ] = val;

			det += ( r == 0 ) ? val * lhs[ IDX( N, r, c ) ] : 0;
		}
	}

	return det;
}


//--------------------------------------------------------------------------
// inverse is messy because of template function partial specialization workaround

template < int N >
struct _minvimpl
{
	template < STRATEGY O, typename T >
	static bool minv ( T const * const lhs, T * const dst, T eps );
};


template < >
struct _minvimpl< 2 >
{
	template < STRATEGY O, typename T >
	static GML_FORCEINLINE bool minv ( T const * const lhs, T * const dst, T eps )
	{
		T det = mdet< O, 2, T >( lhs );

		//TODO make faster - use a mask?  no branch?
		// if the determinant is (near) 0, no inverse exists, bailout
		if( abs< T >( det ) < eps )
			return false;

		dst[ 0 ] =   lhs[ 3 ] / det;
		dst[ 1 ] = - lhs[ 1 ] / det;
		dst[ 2 ] = - lhs[ 2 ] / det;
		dst[ 3 ] =   lhs[ 0 ] / det;

		return true;
	}
};


template < >
struct _minvimpl< 3 >
{
	template < STRATEGY O, typename T >
	static GML_FORCEINLINE bool minv ( T const * const lhs, T * const dst, T eps )
	{
		T det = mdet< O, 3, T >( lhs );

		//TODO make faster - use a mask?  no branch?
		// if the determinant is (near) 0, no inverse exists, bailout
		if( abs< T >( det ) < eps )
			return false;

		dst[ 0 ] =   ( lhs[ 4 ]*lhs[ 8 ] - lhs[ 7 ]*lhs[ 5 ] ) / det;
		dst[ 1 ] = - ( lhs[ 1 ]*lhs[ 8 ] - lhs[ 7 ]*lhs[ 2 ] ) / det;
		dst[ 2 ] =   ( lhs[ 1 ]*lhs[ 5 ] - lhs[ 2 ]*lhs[ 4 ] ) / det;
		dst[ 3 ] = - ( lhs[ 3 ]*lhs[ 8 ] - lhs[ 5 ]*lhs[ 6 ] ) / det;
		dst[ 4 ] =   ( lhs[ 0 ]*lhs[ 8 ] - lhs[ 2 ]*lhs[ 6 ] ) / det;
		dst[ 5 ] = - ( lhs[ 0 ]*lhs[ 5 ] - lhs[ 2 ]*lhs[ 3 ] ) / det;
		dst[ 6 ] =   ( lhs[ 3 ]*lhs[ 7 ] - lhs[ 4 ]*lhs[ 6 ] ) / det;
		dst[ 7 ] = - ( lhs[ 0 ]*lhs[ 7 ] - lhs[ 1 ]*lhs[ 6 ] ) / det;
		dst[ 8 ] =   ( lhs[ 0 ]*lhs[ 4 ] - lhs[ 3 ]*lhs[ 1 ] ) / det;

		return true;
	}
};


template < int N >
template < STRATEGY O, typename T >
GML_INLINE bool _minvimpl< N >::minv ( T const * const lhs, T * const dst, T eps )
{
	const int M = N - 1;
	GML_SSE_ALIGN T minor[ M*M ];
	T det = 0;

	// first compute the first row of the adjoint.  at the same time,
	// use these values to compute the determinant.
	for( int c = 0 ; c < N ; ++c )
	{
		int sign = ( c % 2 ) ? -1 : 1;
		mminor< O, N, T >( lhs, minor, 0, c );
		T val = sign * mdet< O, M, T >( minor );
		dst[ IDX( N, c, 0 ) ] = val;
		det += val* lhs[ IDX( N, 0, c ) ];
	}

	// if the determinant is (near) 0, no inverse exists, bailout
	if( abs< T >( det ) < eps )
		return false;

	// now compute the rest of the adjoint
	for( int c = 0 ; c < N ; ++c )
	{
		for( int r = 1 ; r < N ; ++r )
		{
			int sign = ( ( r + c ) % 2 ) ? -1 : 1;
			mminor< O, N, T >( lhs, minor, r, c );
			dst[ IDX( N, c, r ) ] = sign * mdet< O, M, T >( minor );
		}
	}

	// inverse is the adjoint divided by the determinant
	mdiv< O, N, T >( dst, det, dst );

	return true;
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE bool minv ( T const * const lhs, T * const dst )
{
	return minv< O, N, T >( lhs, dst, epsilon< T >() );
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE bool minv ( T const * const lhs, T * const dst, T eps )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );
	GML_ASSERT( lhs != dst );

	return _minvimpl< N >::template minv< O, T >( lhs, dst, eps );
}


//--------------------------------------------------------------------------
// matrix algebra

template < STRATEGY O, int N, typename T >
GML_INLINE void madd ( T const * const lhs, T rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		vadd< O, N >( lhs + i*N, rhs, dst + i*N );
}


template < STRATEGY O, int N, typename T >
GML_INLINE void msub ( T const * const lhs, T rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		vsub< O, N >( lhs + i*N, rhs, dst + i*N );
}


template < STRATEGY O, int N, typename T >
GML_INLINE void mmul ( T const * const lhs, T rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		vmul< O, N >( lhs + i*N, rhs, dst + i*N );
}


template < STRATEGY O, int N, typename T >
GML_INLINE void mdiv ( T const * const lhs, T rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		vdiv< O, N >( lhs + i*N, rhs, dst + i*N );
}


template < STRATEGY O, int N, typename T >
GML_INLINE void madd ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		vadd< O, N >( lhs + i*N, rhs + i*N, dst + i*N );
}


template < STRATEGY O, int N, typename T >
GML_INLINE void msub ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		vsub< O, N >( lhs + i*N, rhs + i*N, dst + i*N );
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE void mmmul ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );
	GML_ASSERT( lhs != dst );
	GML_ASSERT( rhs != dst );

	mvmul< O, N, T >( lhs, rhs, dst, N );
}


template < STRATEGY O, int N, typename T >
GML_INLINE void mvmul ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );
	GML_ASSERT( lhs != dst );
	GML_ASSERT( rhs != dst );

	for( int j = 0 ; j < N ; ++j )
	{
		dst[ j ] = 0;
		for( int i = 0 ; i < N ; ++i )
			dst[j ] += lhs[ IDX( N, j, i ) ] * rhs[ i ];
	}
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE void mvmul ( T const * const lhs, T const * const rhs, T * const dst, int n )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );
	GML_ASSERT( lhs != dst );
	GML_ASSERT( rhs != dst );
	GML_ASSERT( n > 0 );

	for( int i = 0 ; i < n ; ++i )
		mvmul< O, N >( lhs, rhs + i*N, dst + i*N );
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE void mvmuls ( T const * const lhs, T const * const rhs, T * const dst, int n )
{
	mvmul< O, N >( lhs, rhs, dst, n );
}


template < STRATEGY O, int N, typename T >
GML_INLINE T mqform ( T const * const u, T const * const M, T const * const v )
{
	GML_ASSERT( u );
	GML_ASSERT( M );
	GML_ASSERT( v );

	T sum = 0;
	for( int j = 0 ; j < N ; ++j )
	{
		T Mv_j = 0;
		for( int i = 0 ; i < N ; ++i )
			Mv_j += M[ IDX( N, j, i ) ] * v[ i ];
		sum += Mv_j * u[ j ];
	}
	return sum;
}


template < STRATEGY O, int N, typename T >
GML_INLINE void mouterprod ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );
	GML_ASSERT( lhs != dst );
	GML_ASSERT( rhs != dst );

	for( int c = 0 ; c < N ; ++c )
		for( int r = 0 ; r < N ; ++r )
			dst[ IDX( N, r, c ) ] = lhs[ r ] * rhs[ c ];
}


//--------------------------------------------------------------------------
// comparisons

template < STRATEGY O, int N, typename T >
GML_INLINE bool meq ( T const * const lhs, T const * const rhs )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );

	int mask = 0;
	for( int i = 0 ; i < N ; ++i )
		mask |= ( vneq< O, N >( lhs + i*N, rhs + i*N ) & 0x1 ) << i;

	return ( mask == 0 );
}


template < STRATEGY O, int N, typename T >
GML_INLINE bool mlt ( T const * const lhs, T const * const rhs )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );

	GML_SSE_ALIGN T tmp[ N*N ];
	mlt< O, N >( lhs, rhs, tmp );
	long long mlt = mmask< O, N >( tmp );
	mgt< O, N >( lhs, rhs, tmp );
	long long mgt = mmask< O, N >( tmp );
	return ( mlt > mgt );
}


template < STRATEGY O, int N, typename T >
GML_INLINE bool mgt ( T const * const lhs, T const * const rhs )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );

	GML_SSE_ALIGN T tmp[ N*N ];
	mlt< O, N >( lhs, rhs, tmp );
	long long mlt = mmask< O, N >( tmp );
	mgt< O, N >( lhs, rhs, tmp );
	long long mgt = mmask< O, N >( tmp );
	return ( mlt < mgt );
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE bool mneq ( T const * const lhs, T const * const rhs )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );

	return ! meq< O, N >( lhs, rhs );
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE bool mle ( T const * const lhs, T const * const rhs )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );

	return ! mgt< O, N >( lhs, rhs );
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE bool mge ( T const * const lhs, T const * const rhs )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );

	return ! mlt< O, N >( lhs, rhs );
}


template < STRATEGY O, int N, typename T >
GML_INLINE void meq ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		veq< O, N >( lhs + i*N, rhs + i*N, dst + i*N );
}


template < STRATEGY O, int N, typename T >
GML_INLINE void mneq ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		vneq< O, N >( lhs + i*N, rhs + i*N, dst + i*N );
}


template < STRATEGY O, int N, typename T >
GML_INLINE void mlt ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		vlt< O, N >( lhs + i*N, rhs + i*N, dst + i*N );
}


template < STRATEGY O, int N, typename T >
GML_INLINE void mgt ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		vgt< O, N >( lhs + i*N, rhs + i*N, dst + i*N );
}


template < STRATEGY O, int N, typename T >
GML_INLINE void mle ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		vle< O, N >( lhs + i*N, rhs + i*N, dst + i*N );
}


template < STRATEGY O, int N, typename T >
GML_INLINE void mge ( T const * const lhs, T const * const rhs, T * const dst )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );
	GML_ASSERT( dst );

	for( int i = 0 ; i < N ; ++i )
		vge< O, N >( lhs + i*N, rhs + i*N, dst + i*N );
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE bool mrealeq ( T const * const lhs, T const * const rhs )
{
	return mrealeq< O, N, T >( lhs, rhs, epsilon< T >() );
}


template < STRATEGY O, int N, typename T >
GML_FORCEINLINE bool mrealeq ( T const * const lhs, T const * const rhs, T eps )
{
	GML_ASSERT( lhs );
	GML_ASSERT( rhs );

	GML_SSE_ALIGN T tmp[ N*N ];
	msub< O, N >( lhs, rhs, tmp );
	mabs< O, N >( tmp, tmp );
	T r = mmax< O, N >( tmp );
	return ( r < eps );
}


//--------------------------------------------------------------------------
// tests

template < STRATEGY O, int N, typename T >
GML_INLINE bool mnan ( T const * const lhs )
{
	GML_ASSERT( lhs );

	int mask = 0;
	for( int i = 0 ; i < N ; ++i )
		mask |= vnan< O, N >( lhs + i*N ) << i;
	return ( mask > 0 );
}


template < STRATEGY O, int N, typename T >
GML_INLINE bool mfinite ( T const * const lhs )
{
	GML_ASSERT( lhs );

	int mask = 0;
	for( int i = 0 ; i < N ; ++i )
		mask |= ( ! vfinite< O, N >( lhs + i*N ) ) << i;
	return ( mask == 0 );
}


template < STRATEGY O, int N, typename T >
GML_INLINE long long mmask ( T const * const lhs )
{
	GML_ASSERT( lhs );

	long long mask = 0;
	for( int i = 0 ; i < N ; ++i )
		mask |= ( vmask< O, N >( lhs + i*N ) << ( i*N ) );
	return mask;
}


//--------------------------------------------------------------------------

#undef IDX

GML_NAMESPACE_END
