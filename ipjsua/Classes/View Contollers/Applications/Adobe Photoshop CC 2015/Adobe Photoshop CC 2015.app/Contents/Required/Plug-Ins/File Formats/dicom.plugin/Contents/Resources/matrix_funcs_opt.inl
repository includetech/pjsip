
#if GML_SUPPORT_SSE

GML_NAMESPACE_BEGIN


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//                   MATRIX 4 FLOAT OPT
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// generators

template < >
GML_FORCEINLINE void mset< SSE, 4, float > ( float * const _dst, float x )
{
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &dst0 = ( ( __m128 * )_dst )[ 0 ];
	__m128 &dst1 = ( ( __m128 * )_dst )[ 1 ];
	__m128 &dst2 = ( ( __m128 * )_dst )[ 2 ];
	__m128 &dst3 = ( ( __m128 * )_dst )[ 3 ];

	dst0 = _mm_set1_ps( x );
	dst1 = dst0;
	dst2 = dst0;
	dst3 = dst0;
}


template < >
GML_FORCEINLINE void mdiag< SSE, 4, float > ( float * const _dst, float x )
{
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &dst0 = ( ( __m128 * )_dst )[ 0 ];
	__m128 &dst1 = ( ( __m128 * )_dst )[ 1 ];
	__m128 &dst2 = ( ( __m128 * )_dst )[ 2 ];
	__m128 &dst3 = ( ( __m128 * )_dst )[ 3 ];

	dst0 = _mm_set_ss( x );
	dst1 = _mm_shuffle_ps( dst0, dst0, _MM_SHUFFLE( 1, 1, 0, 1 ) );
	dst2 = _mm_shuffle_ps( dst0, dst0, _MM_SHUFFLE( 1, 0, 1, 1 ) );
	dst3 = _mm_shuffle_ps( dst0, dst0, _MM_SHUFFLE( 0, 1, 1, 1 ) );
}


template < >
GML_FORCEINLINE void mdiag< SSE, 4, float > ( float * const _dst, float const * const _v )
{
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &dst0 = ( ( __m128 * )_dst )[ 0 ];
	__m128 &dst1 = ( ( __m128 * )_dst )[ 1 ];
	__m128 &dst2 = ( ( __m128 * )_dst )[ 2 ];
	__m128 &dst3 = ( ( __m128 * )_dst )[ 3 ];

	__m128 &v = *( __m128 * )_v;
	__m128 v0 = _mm_movelh_ps( _mm_setzero_ps(), v );
	__m128 v1 = _mm_movehl_ps( v, _mm_setzero_ps() );

	dst0 = _mm_shuffle_ps( v0, v0, _MM_SHUFFLE( 0, 0, 0, 2 ) );
	dst1 = _mm_shuffle_ps( v0, v0, _MM_SHUFFLE( 0, 0, 3, 0 ) );
	dst2 = _mm_shuffle_ps( v1, v1, _MM_SHUFFLE( 0, 2, 0, 0 ) );
	dst3 = _mm_shuffle_ps( v1, v1, _MM_SHUFFLE( 3, 0, 0, 0 ) );
}


//--------------------------------------------------------------------------
// matrix determinant

template < >
GML_INLINE float mdet< SSE, 4, float > ( float const * const _lhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );

	__m128 &lhs0 = ( ( __m128 * )_lhs )[ 0 ];
	__m128 &lhs1 = ( ( __m128 * )_lhs )[ 1 ];
	__m128 &lhs2 = ( ( __m128 * )_lhs )[ 2 ];
	__m128 &lhs3 = ( ( __m128 * )_lhs )[ 3 ];	

	__m128 FFAA, BCDE, tmp1, tmp2, tmp3, tmp4;

	// make 2x2 determinants from rows 3 and 4
	tmp1 = _mm_shuffle_ps( lhs3, lhs3, _MM_SHUFFLE( 2, 3, 0, 1 ) );
	tmp2 = _mm_mul_ps( lhs2, tmp1 );
	tmp3 = _mm_shuffle_ps( tmp2, tmp2, _MM_SHUFFLE( 0, 0, 2, 2 ) );
	tmp4 = _mm_shuffle_ps( tmp2, tmp2, _MM_SHUFFLE( 1, 1, 3, 3 ) );
	FFAA = _mm_sub_ps( tmp3, tmp4 );

	tmp1 = _mm_shuffle_ps( lhs2, lhs2, _MM_SHUFFLE( 1, 1, 0, 0 ) );
	tmp2 = _mm_shuffle_ps( lhs3, lhs3, _MM_SHUFFLE( 3, 2, 3, 2 ) );
	tmp1 = _mm_mul_ps( tmp1, tmp2 );
	tmp3 = _mm_shuffle_ps( lhs2, lhs2, _MM_SHUFFLE( 3, 2, 3, 2 ) );
	tmp4 = _mm_shuffle_ps( lhs3, lhs3, _MM_SHUFFLE( 1, 1, 0, 0 ) );
	tmp3 = _mm_mul_ps( tmp3, tmp4 );
	BCDE = _mm_sub_ps( tmp1, tmp3 );

	// multiply 2x2 dets by row 1 and add together
	tmp1 = _mm_shuffle_ps( lhs1, lhs1, _MM_SHUFFLE( 2, 3, 0, 1 ) );
	tmp4 = _mm_mul_ps( FFAA, tmp1 );

	tmp1 = _mm_shuffle_ps( lhs1, lhs1, _MM_SHUFFLE( 1, 1, 2, 2 ) );
	tmp2 = _mm_shuffle_ps( BCDE, BCDE, _MM_SHUFFLE( 0, 1, 1, 3 ) );
	tmp3 = _mm_mul_ps( tmp1, tmp2 );
	tmp4 = _mm_sub_ps( tmp4, tmp3 );

	tmp1 = _mm_shuffle_ps( lhs1, lhs1, _MM_SHUFFLE( 0, 0, 3, 3 ) );
	tmp2 = _mm_shuffle_ps( BCDE, BCDE, _MM_SHUFFLE( 2, 3, 0, 2 ) );
	tmp3 = _mm_mul_ps( tmp1, tmp2 );
	tmp4 = _mm_add_ps( tmp4, tmp3 );

	// final multiply 3x3s by row 0 and add together
	tmp1 = _mm_mul_ps( lhs0, tmp4 );
	tmp2 = _mm_sub_ps( tmp1, _mm_rol_ps( tmp1, 3 ) );
	tmp3 = _mm_add_ps( tmp2, _mm_rol_ps( tmp2, 2 ) );

	return *( float * )&tmp3;
}


//--------------------------------------------------------------------------
// matrix inverse

// this is to supress the warning that tmp1, row1 and row3 are used
// uninitialized.  it's okay because they're completely set before
// they're used, it's just faster to do it this way.
#pragma warning( disable : 4700 )
#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wuninitialized"
#endif
template < >
GML_INLINE bool minv< SSE, 4, float > ( float const * const _lhs, float * const _dst, float eps )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	/*
	 * code yanked from Intel's "Streaming SIMD Extensions - 
	 * Inverse of 4x4 Matrix" at
	 * http://download.intel.com/design/PentiumIII/sml/24504301.pdf
	 */

	__m128 minor0, minor1, minor2, minor3;
	__m128 row0, row1, row2, row3;
	__m128 det, tmp1;

#if _DEBUG
	// debug mode does annoying runtime checks that bring up dialog boxes.
	// but in debug mode performance isn't as important so go ahead and initialize
	// them first
	tmp1 = _mm_setzero_ps();
	row1 = _mm_setzero_ps();
	row3 = _mm_setzero_ps();
#endif

	tmp1 = _mm_loadh_pi( _mm_loadl_pi( tmp1, ( __m64 * )( _lhs      ) ), ( __m64 * )( _lhs +  4 ) );
	row1 = _mm_loadh_pi( _mm_loadl_pi( row1, ( __m64 * )( _lhs +  8 ) ), ( __m64 * )( _lhs + 12 ) );

	row0 = _mm_shuffle_ps( tmp1, row1, 0x88 );
	row1 = _mm_shuffle_ps( row1, tmp1, 0xdd );

	tmp1 = _mm_loadh_pi( _mm_loadl_pi( tmp1, ( __m64 * )( _lhs +  2 ) ), ( __m64 * )( _lhs +  6 ) );
	row3 = _mm_loadh_pi( _mm_loadl_pi( row3, ( __m64 * )( _lhs + 10 ) ), ( __m64 * )( _lhs + 14 ) );

	row2 = _mm_shuffle_ps( tmp1, row3, 0x88 );
	row3 = _mm_shuffle_ps( row3, tmp1, 0xdd );

	//---------

	tmp1 = _mm_mul_ps( row2, row3 );
	tmp1 = _mm_shuffle_ps( tmp1, tmp1, 0xb1 );

	minor0 = _mm_mul_ps( row1, tmp1 );
	minor1 = _mm_mul_ps( row0, tmp1 );

	tmp1 = _mm_shuffle_ps( tmp1, tmp1, 0x4e );

	minor0 = _mm_sub_ps( _mm_mul_ps( row1, tmp1 ), minor0 );
	minor1 = _mm_sub_ps( _mm_mul_ps( row0, tmp1 ), minor1 );
	minor1 = _mm_shuffle_ps( minor1, minor1, 0x4e );

	//---------

	tmp1 = _mm_mul_ps( row1, row2 );
	tmp1 = _mm_shuffle_ps( tmp1, tmp1, 0xb1 );

	minor0 = _mm_add_ps( _mm_mul_ps( row3, tmp1 ), minor0 );
	minor3 = _mm_mul_ps( row0, tmp1 );

	tmp1 = _mm_shuffle_ps( tmp1, tmp1, 0x4e );

	minor0 = _mm_sub_ps( minor0, _mm_mul_ps( row3, tmp1 ) );
	minor3 = _mm_sub_ps( _mm_mul_ps( row0, tmp1 ), minor3 );
	minor3 = _mm_shuffle_ps( minor3, minor3, 0x4e );

	//---------

	tmp1 = _mm_mul_ps( _mm_shuffle_ps( row1, row1, 0x4e ), row3 );
	tmp1 = _mm_shuffle_ps( tmp1, tmp1, 0xb1 );
	row2 = _mm_shuffle_ps( row2, row2, 0x4e );

	minor0 = _mm_add_ps( _mm_mul_ps( row2, tmp1 ), minor0 );
	minor2 = _mm_mul_ps( row0, tmp1 );

	tmp1 = _mm_shuffle_ps( tmp1, tmp1, 0x4e );

	minor0 = _mm_sub_ps( minor0, _mm_mul_ps( row2, tmp1 ) );
	minor2 = _mm_sub_ps( _mm_mul_ps( row0, tmp1 ), minor2 );
	minor2 = _mm_shuffle_ps( minor2, minor2, 0x4e );

	//---------

	tmp1 = _mm_mul_ps( row0, row1 );
	tmp1 = _mm_shuffle_ps( tmp1, tmp1, 0xb1 );

	minor2 = _mm_add_ps( _mm_mul_ps( row3, tmp1 ), minor2 );
	minor3 = _mm_sub_ps( _mm_mul_ps( row2, tmp1 ), minor3 );

	tmp1 = _mm_shuffle_ps( tmp1, tmp1, 0x4e );

	minor2 = _mm_sub_ps( _mm_mul_ps( row3, tmp1 ), minor2 );
	minor3 = _mm_sub_ps( minor3, _mm_mul_ps( row2, tmp1 ) );

	//---------

	tmp1 = _mm_mul_ps( row0, row3 );
	tmp1 = _mm_shuffle_ps( tmp1, tmp1, 0xb1 );

	minor1 = _mm_sub_ps( minor1, _mm_mul_ps( row2, tmp1 ) );
	minor2 = _mm_add_ps( _mm_mul_ps( row1, tmp1 ), minor2 );

	tmp1 = _mm_shuffle_ps( tmp1, tmp1, 0x4e );

	minor1 = _mm_add_ps( _mm_mul_ps( row2, tmp1 ), minor1 );
	minor2 = _mm_sub_ps( minor2, _mm_mul_ps( row1, tmp1 ) );

	//---------

	tmp1 = _mm_mul_ps( row0, row2 );
	tmp1 = _mm_shuffle_ps( tmp1, tmp1, 0xb1 );

	minor1 = _mm_add_ps( _mm_mul_ps( row3, tmp1 ), minor1 );
	minor3 = _mm_sub_ps( minor3, _mm_mul_ps( row1, tmp1 ) );

	tmp1 = _mm_shuffle_ps( tmp1, tmp1, 0x4e );

	minor1 = _mm_sub_ps( minor1, _mm_mul_ps( row3, tmp1 ) );
	minor3 = _mm_add_ps( _mm_mul_ps( row1, tmp1 ), minor3 );

	//---------

	det = _mm_mul_ps( row0, minor0 );
	det = _mm_add_ps( _mm_shuffle_ps( det, det, 0x4e ), det );
	det = _mm_add_ss( _mm_shuffle_ps( det, det, 0xb1 ), det );
	tmp1 = _mm_rcp_ss( det );

	det = _mm_sub_ss( _mm_add_ss( tmp1, tmp1 ), _mm_mul_ss( det, _mm_mul_ss( tmp1, tmp1 ) ) );
	det = _mm_shuffle_ps( det, det, 0x00 );

	minor0 = _mm_mul_ps( det, minor0 );
	_mm_store_ps( _dst, minor0 );

	minor1 = _mm_mul_ps( det, minor1 );
	_mm_store_ps( _dst + 4, minor1 );

	minor2 = _mm_mul_ps( det, minor2 );
	_mm_store_ps( _dst + 8, minor2 );

	minor3 = _mm_mul_ps( det, minor3 );
	_mm_store_ps( _dst + 12, minor3 );

	return ( abs< float >( *( float * )&det ) > eps );
}
#if __clang__
#pragma clang diagnostic pop
#endif
#pragma warning( default : 4700 )


//--------------------------------------------------------------------------
// matrix handling

template < >
GML_FORCEINLINE void mcol< SSE, 4, float > ( float const * const _lhs, float * const _dst, int col )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );
	GML_ASSERT( col >= 0 && col < 4 );

	__m128 &dst = *( __m128 * )_dst;
	__m128 &lhs = *( ( ( __m128 * )_lhs ) + col );

	dst = lhs;
}


template < >
GML_FORCEINLINE float mmax< SSE, 4, float > ( float const * const lhs )
{
	GML_SSE_CHECK_ALIGN( lhs );

	GML_SSE_ALIGN float v[ 4 ];
	v[ 0 ] = vmax< SSE, 4 >( lhs );
	v[ 1 ] = vmax< SSE, 4 >( lhs + 4 );
	v[ 2 ] = vmax< SSE, 4 >( lhs + 8 );
	v[ 3 ] = vmax< SSE, 4 >( lhs + 12 );

	return vmax< SSE, 4 >( v );
}


template < >
GML_FORCEINLINE float mmin< SSE, 4, float > ( float const * const lhs )
{
	GML_SSE_CHECK_ALIGN( lhs );

	GML_SSE_ALIGN float v[ 4 ];
	v[ 0 ] = vmin< SSE, 4 >( lhs );
	v[ 1 ] = vmin< SSE, 4 >( lhs + 4 );
	v[ 2 ] = vmin< SSE, 4 >( lhs + 8 );
	v[ 3 ] = vmin< SSE, 4 >( lhs + 12 );

	return vmin< SSE, 4 >( v );
}


template < >
GML_FORCEINLINE void mcopy< SSE, 4, float > ( float const * const _lhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &dst0 = ( ( __m128 * )_dst )[ 0 ];
	__m128 &dst1 = ( ( __m128 * )_dst )[ 1 ];
	__m128 &dst2 = ( ( __m128 * )_dst )[ 2 ];
	__m128 &dst3 = ( ( __m128 * )_dst )[ 3 ];	

	__m128 &lhs0 = ( ( __m128 * )_lhs )[ 0 ];
	__m128 &lhs1 = ( ( __m128 * )_lhs )[ 1 ];
	__m128 &lhs2 = ( ( __m128 * )_lhs )[ 2 ];
	__m128 &lhs3 = ( ( __m128 * )_lhs )[ 3 ];	

	dst0 = lhs0;
	dst1 = lhs1;
	dst2 = lhs2;
	dst3 = lhs3;
}


template < >
GML_FORCEINLINE void mtrans< SSE, 4, float > ( float const * const _lhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &dst0 = ( ( __m128 * )_dst )[ 0 ];
	__m128 &dst1 = ( ( __m128 * )_dst )[ 1 ];
	__m128 &dst2 = ( ( __m128 * )_dst )[ 2 ];
	__m128 &dst3 = ( ( __m128 * )_dst )[ 3 ];	

	__m128 &lhs0 = ( ( __m128 * )_lhs )[ 0 ];
	__m128 &lhs1 = ( ( __m128 * )_lhs )[ 1 ];
	__m128 &lhs2 = ( ( __m128 * )_lhs )[ 2 ];
	__m128 &lhs3 = ( ( __m128 * )_lhs )[ 3 ];	

	__m128 tmp1 = _mm_shuffle_ps( lhs0, lhs1, _MM_SHUFFLE( 1, 0, 1, 0 ) );
	__m128 tmp2 = _mm_shuffle_ps( lhs2, lhs3, _MM_SHUFFLE( 1, 0, 1, 0 ) );

	dst0 = _mm_shuffle_ps( tmp1, tmp2, _MM_SHUFFLE( 2, 0, 2, 0 ) );
	dst1 = _mm_shuffle_ps( tmp1, tmp2, _MM_SHUFFLE( 3, 1, 3, 1 ) );

	tmp1 = _mm_shuffle_ps( lhs0, lhs1, _MM_SHUFFLE( 3, 2, 3, 2 ) );
	tmp2 = _mm_shuffle_ps( lhs2, lhs3, _MM_SHUFFLE( 3, 2, 3, 2 ) );

	dst2 = _mm_shuffle_ps( tmp1, tmp2, _MM_SHUFFLE( 2, 0, 2, 0 ) );
	dst3 = _mm_shuffle_ps( tmp1, tmp2, _MM_SHUFFLE( 3, 1, 3, 1 ) );
}


//--------------------------------------------------------------------------
// matrix algebra

#define MULT( i ) \
/* get the vector */ \
v3 = _mm_load_ps( _rhs + 4*i ); \
 \
/* put each element of the vector in its own m128 */ \
/* so v0 = <x,x,x,x>, etc. */ \
v0 = _mm_shuffle_ps( v3, v3, _MM_SHUFFLE( 0, 0, 0, 0 ) ); \
v1 = _mm_shuffle_ps( v3, v3, _MM_SHUFFLE( 1, 1, 1, 1 ) ); \
v2 = _mm_shuffle_ps( v3, v3, _MM_SHUFFLE( 2, 2, 2, 2 ) ); \
v3 = _mm_shuffle_ps( v3, v3, _MM_SHUFFLE( 3, 3, 3, 3 ) ); \
 \
/* component-wise mult the columns by the elements of the vector */ \
/* so v0 = lhs0*x, etc.  */ \
v0 = _mm_mul_ps( lhs0, v0 ); \
v1 = _mm_mul_ps( lhs1, v1 ); \
v2 = _mm_mul_ps( lhs2, v2 ); \
v3 = _mm_mul_ps( lhs3, v3 ); \
 \
/* add together the scaled columns */ \
v0 = _mm_add_ps( v0, v1 ); \
v2 = _mm_add_ps( v2, v3 ); \
v0 = _mm_add_ps( v0, v2 ); 


template < >
GML_INLINE void mvmul< SSE, 4, float > ( float const * const _lhs, float const * const _rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );
	GML_ASSERT( _lhs != _dst );
	GML_ASSERT( _rhs != _dst );

	// the columns of the matrix
	__m128 &lhs0 = ( ( __m128 * )_lhs )[ 0 ];
	__m128 &lhs1 = ( ( __m128 * )_lhs )[ 1 ];
	__m128 &lhs2 = ( ( __m128 * )_lhs )[ 2 ];
	__m128 &lhs3 = ( ( __m128 * )_lhs )[ 3 ];	

	__m128 v0, v1, v2, v3;

	MULT( 0 )
	_mm_store_ps( _dst, v0 );
}


template < >
GML_INLINE void mvmul< SSE, 4, float > ( float const * const _lhs, float const * const _rhs, float * const _dst, int n )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );
	GML_ASSERT( _lhs != _dst );
	GML_ASSERT( _rhs != _dst );
	GML_ASSERT( n > 0 );

	// the columns of the matrix
	__m128 &lhs0 = ( ( __m128 * )_lhs )[ 0 ];
	__m128 &lhs1 = ( ( __m128 * )_lhs )[ 1 ];
	__m128 &lhs2 = ( ( __m128 * )_lhs )[ 2 ];
	__m128 &lhs3 = ( ( __m128 * )_lhs )[ 3 ];	

	__m128 v0, v1, v2, v3;

	for( int i = 0 ; i < n ; ++i )
	{
		MULT( i )
		_mm_store_ps( _dst + 4*i, v0 );
	}
}


template < >
GML_INLINE void mvmuls< SSE, 4, float > ( float const * const _lhs, float const * const _rhs, float * const _dst, int n )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );
	GML_ASSERT( _lhs != _dst );
	GML_ASSERT( _rhs != _dst );
	GML_ASSERT( n > 0 );

	// the columns of the matrix
	__m128 &lhs0 = ( ( __m128 * )_lhs )[ 0 ];
	__m128 &lhs1 = ( ( __m128 * )_lhs )[ 1 ];
	__m128 &lhs2 = ( ( __m128 * )_lhs )[ 2 ];
	__m128 &lhs3 = ( ( __m128 * )_lhs )[ 3 ];	

	__m128 v0, v1, v2, v3;

	for( int i = 0 ; i < n ; ++i )
	{
		MULT( i )

		// save the final vector in dst - streaming means it won't
		// be stored in the cache afterwards
		_mm_stream_ps( _dst + 4*i, v0 );
	}
}

#undef MULT


template < >
GML_FORCEINLINE float mqform< SSE, 4, float > ( float const * const u, float const * const M, float const * const v )
{
	GML_SSE_CHECK_ALIGN( u );
	GML_SSE_CHECK_ALIGN( M );
	GML_SSE_CHECK_ALIGN( v );

	GML_SSE_ALIGN float tmp[ 4 ];

	mvmul< SSE, 4 >( M, v, tmp );
	return vdot< SSE, 4 >( u, tmp );
}



//--------------------------------------------------------------------------

template < >
GML_INLINE bool meq< SSE, 4, float > ( float const * const lhs, float const * const rhs )
{
	GML_SSE_CHECK_ALIGN( lhs );
	GML_SSE_CHECK_ALIGN( rhs );

	GML_SSE_ALIGN float _v[ 4 ];
	_v[ 0 ] = veq< SSE, 4 >( lhs, rhs );
	_v[ 1 ] = veq< SSE, 4 >( lhs +  4, rhs +  4 );
	_v[ 2 ] = veq< SSE, 4 >( lhs +  8, rhs +  8 );
	_v[ 3 ] = veq< SSE, 4 >( lhs + 12, rhs + 12 );

	__m128 &v = *( __m128 * )_v;
	v = _mm_cmpeq_ps( v, _mm_setzero_ps() );
	int mask = _mm_movemask_ps( v );
	return ( mask == 0 );
}


template < >
GML_INLINE bool mlt< SSE, 4, float > ( float const * const lhs, float const * const rhs )
{
	GML_SSE_CHECK_ALIGN( lhs );
	GML_SSE_CHECK_ALIGN( rhs );

	GML_SSE_ALIGN float tmp[ 16 ];
	mlt< SSE, 4 >( lhs, rhs, tmp );
	long long mlt = mmask< SSE, 4 >( tmp );
	mgt< SSE, 4 >( lhs, rhs, tmp );
	long long mgt = mmask< SSE, 4 >( tmp );
	return ( mlt > mgt );
}


template < >
GML_INLINE bool mgt< SSE, 4, float > ( float const * const lhs, float const * const rhs )
{
	GML_SSE_CHECK_ALIGN( lhs );
	GML_SSE_CHECK_ALIGN( rhs );

	GML_SSE_ALIGN float tmp[ 16 ];
	mlt< SSE, 4 >( lhs, rhs, tmp );
	long long mlt = mmask< SSE, 4 >( tmp );
	mgt< SSE, 4 >( lhs, rhs, tmp );
	long long mgt = mmask< SSE, 4 >( tmp );
	return ( mlt < mgt );
}


template < >
GML_INLINE bool mrealeq< SSE, 4, float > ( float const * const lhs, float const * const rhs, float eps )
{
	GML_SSE_CHECK_ALIGN( lhs );
	GML_SSE_CHECK_ALIGN( rhs );

	GML_SSE_ALIGN float tmp[ 16 ];
	msub< SSE, 4 >( lhs, rhs, tmp );
	mabs< SSE, 4 >( tmp, tmp );
	float r = mmax< SSE, 4 >( tmp );
	return ( r < eps );
}


//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//                   MATRIX 4 DOUBLE OPT
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

#if GML_SSE_VER >= GML_SSE2

//--------------------------------------------------------------------------
// generators

template < >
GML_FORCEINLINE void mset< SSE, 4, double > ( double * const _dst, double x )
{
	GML_SSE_CHECK_ALIGN( _dst );

	_mm_store_pd( _dst,      _mm_set1_pd( x ) );
	_mm_store_pd( _dst +  2, _mm_set1_pd( x ) );
	_mm_store_pd( _dst +  4, _mm_set1_pd( x ) );
	_mm_store_pd( _dst +  6, _mm_set1_pd( x ) );
	_mm_store_pd( _dst +  8, _mm_set1_pd( x ) );
	_mm_store_pd( _dst + 10, _mm_set1_pd( x ) );
	_mm_store_pd( _dst + 12, _mm_set1_pd( x ) );
	_mm_store_pd( _dst + 14, _mm_set1_pd( x ) );
}


template < >
GML_FORCEINLINE void mdiag< SSE, 4, double > ( double * const _dst, double _x )
{
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d x = _mm_set_sd( _x );

	_mm_store_pd( _dst,      x );
	_mm_store_pd( _dst +  2, _mm_setzero_pd() );
	_mm_store_pd( _dst +  4, _mm_swap_pd( x ) );
	_mm_store_pd( _dst +  6, _mm_setzero_pd() );
	_mm_store_pd( _dst +  8, _mm_setzero_pd() );
	_mm_store_pd( _dst + 10, x );
	_mm_store_pd( _dst + 12, _mm_setzero_pd() );
	_mm_store_pd( _dst + 14, _mm_swap_pd( x ) );
}


template < >
GML_FORCEINLINE void mdiag< SSE, 4, double > ( double * const _dst, double const * const _v )
{
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d zero = _mm_setzero_pd();
	__m128d &v0 = ( ( __m128d * )_v )[ 0 ];
	__m128d &v1 = ( ( __m128d * )_v )[ 1 ];

	_mm_store_pd( _dst,      _mm_shuffle_pd( v0, zero, _MM_SHUFFLE2( 0, 0 ) ) );
	_mm_store_pd( _dst +  2, zero );
	_mm_store_pd( _dst +  4, _mm_shuffle_pd( zero, v0, _MM_SHUFFLE2( 1, 0 ) ) );
	_mm_store_pd( _dst +  6, zero );
	_mm_store_pd( _dst +  8, zero );
	_mm_store_pd( _dst + 10, _mm_shuffle_pd( v1, zero, _MM_SHUFFLE2( 0, 0 ) ) );
	_mm_store_pd( _dst + 12, zero );
	_mm_store_pd( _dst + 14, _mm_shuffle_pd( zero, v1, _MM_SHUFFLE2( 1, 0 ) ) );
}


//--------------------------------------------------------------------------
// matrix determinant

// this is to supress the warning that jf, nb, ai, and em are used
// uninitialized.  it's okay because they're completely set before
// they're used, it's just faster to do it this way.
#pragma warning( disable : 4700 )
#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wuninitialized"
#endif
template < >
GML_INLINE double mdet< SSE, 4, double > ( double const * const _lhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );

	__m128d lhs0b = ( ( __m128d * )_lhs )[ 1 ];
	__m128d lhs1b = ( ( __m128d * )_lhs )[ 3 ];
	__m128d lhs2b = ( ( __m128d * )_lhs )[ 5 ];
	__m128d lhs3b = ( ( __m128d * )_lhs )[ 7 ];	

	__m128d tmp1, tmp2, tmp3, tmp4;
	__m128d FA, CB, DE;
	__m128d jf, nb, ai, em;

	// A,B,C,D,E,F are 2x2 dets from rows 2,3.  columns are
	// A: 0,1  B: 0,2  C: 0,3  D: 1,2  E: 1,3  F: 2,3
	tmp1 = _mm_mul_pd( lhs0b, _mm_swap_pd( lhs1b ) );
	tmp2 = _mm_mul_pd( lhs2b, _mm_swap_pd( lhs3b ) );
	tmp3 = _mm_shuffle_pd( tmp2, tmp1, _MM_SHUFFLE2( 0, 0 ) );
	tmp4 = _mm_shuffle_pd( tmp2, tmp1, _MM_SHUFFLE2( 1, 1 ) );
	FA = _mm_sub_pd( tmp3, tmp4 );

	tmp1 = _mm_mul_pd( lhs0b, _mm_swap_pd( lhs3b ) );
	tmp2 = _mm_mul_pd( lhs0b, _mm_swap_pd( lhs2b ) );
	tmp3 = _mm_shuffle_pd( tmp1, tmp2, _MM_SHUFFLE2( 0, 0 ) );
	tmp4 = _mm_shuffle_pd( tmp1, tmp2, _MM_SHUFFLE2( 1, 1 ) );
	CB = _mm_sub_pd( tmp3, tmp4 );

	tmp1 = _mm_mul_pd( lhs1b, _mm_swap_pd( lhs2b ) );
	tmp2 = _mm_mul_pd( lhs1b, _mm_swap_pd( lhs3b ) );
	tmp3 = _mm_shuffle_pd( tmp1, tmp2, _MM_SHUFFLE2( 0, 0 ) );
	tmp4 = _mm_shuffle_pd( tmp1, tmp2, _MM_SHUFFLE2( 1, 1 ) );
	DE = _mm_sub_pd( tmp3, tmp4 );

	// for a matrix in column-major order, elements named sequentially with letters,
	// j=m12, f=m11, n=m13, b=m10
	jf = _mm_loadl_pd( _mm_loadh_pd( jf, _lhs + 5 ), _lhs +  9 );
	nb = _mm_loadl_pd( _mm_loadh_pd( nb, _lhs + 1 ), _lhs + 13 );

	// compute 3x3 dets for minors M00 and M02
	tmp1 = _mm_mul_pd( _mm_shuffle_pd( jf, nb, _MM_SHUFFLE2( 0, 1 ) ), FA );
	tmp1 = _mm_sub_pd( tmp1, _mm_mul_pd( jf, _mm_shuffle_pd( DE, CB, _MM_SHUFFLE2( 0, 1 ) ) ) );
	tmp1 = _mm_add_pd( tmp1, _mm_mul_pd( nb, DE ) );

	// positive components of 4x4 det
	ai = _mm_loadl_pd( _mm_loadh_pd( ai, _lhs + 8 ), _lhs + 0 );
	tmp1 = _mm_mul_pd( ai, tmp1 );

	// compute 3x3 dets for minors M01 and M03
	tmp2 = _mm_mul_pd( _mm_shuffle_pd( nb, jf, _MM_SHUFFLE2( 0, 1 ) ), FA );
	tmp2 = _mm_sub_pd( tmp2, _mm_mul_pd( jf, CB ) );
	tmp2 = _mm_add_pd( tmp2, _mm_mul_pd( nb, _mm_shuffle_pd( CB, DE, _MM_SHUFFLE2( 0, 1 ) ) ) );

	// negative components of 4x4 det
	em = _mm_loadl_pd( _mm_loadh_pd( em, _lhs + 12 ), _lhs + 4 );
	tmp2 = _mm_mul_pd( em, tmp2 );

	// subtract the negative components from the positive ones and finally add them together for 4x4 det
	tmp1 = _mm_sub_pd( tmp1, tmp2 );
	#if GML_SSE_VER >= GML_SSE3
		tmp1 = _mm_hadd_pd( tmp1, tmp1 );
	#else
		tmp1 = _mm_add_sd( _mm_shuffle_pd( tmp1, tmp1, 1 ), tmp1 );
	#endif

	return *( double * )&tmp1;
}

#if __clang__
#pragma clang diagnostic pop
#endif


#pragma warning( default : 4700 )


//--------------------------------------------------------------------------
// matrix inverse

#if 0
//TODO

// this is to supress the warning that tmp1, row1 and row3 are used
// uninitialized.  it's okay because they're completely set before
// they're used, it's just faster to do it this way.
#pragma warning( disable : 4700 )
#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wuninitialized"
#endif
template < >
GML_INLINE bool minv< SSE, 4, float > ( float const * const _lhs, float * const _dst, float eps )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	/*
	 * code yanked from Intel's "Streaming SIMD Extensions - 
	 * Inverse of 4x4 Matrix" at
	 * http://download.intel.com/design/PentiumIII/sml/24504301.pdf
	 */

	__m128 minor0, minor1, minor2, minor3;
	__m128 row0, row1, row2, row3;
	__m128 det, tmp1;


	tmp1 = _mm_loadh_pi( _mm_loadl_pi( tmp1, ( __m64 * )( _lhs      ) ), ( __m64 * )( _lhs +  4 ) );
	row1 = _mm_loadh_pi( _mm_loadl_pi( row1, ( __m64 * )( _lhs +  8 ) ), ( __m64 * )( _lhs + 12 ) );

	row0 = _mm_shuffle_ps( tmp1, row1, 0x88 );
	row1 = _mm_shuffle_ps( row1, tmp1, 0xdd );

	tmp1 = _mm_loadh_pi( _mm_loadl_pi( tmp1, ( __m64 * )( _lhs +  2 ) ), ( __m64 * )( _lhs +  6 ) );
	row3 = _mm_loadh_pi( _mm_loadl_pi( row3, ( __m64 * )( _lhs + 10 ) ), ( __m64 * )( _lhs + 14 ) );

	row2 = _mm_shuffle_ps( tmp1, row3, 0x88 );
	row3 = _mm_shuffle_ps( row3, tmp1, 0xdd );

	//---------

	tmp1 = _mm_mul_ps( row2, row3 );
	tmp1 = _mm_shuffle_ps( tmp1, tmp1, 0xb1 );

	minor0 = _mm_mul_ps( row1, tmp1 );
	minor1 = _mm_mul_ps( row0, tmp1 );

	tmp1 = _mm_shuffle_ps( tmp1, tmp1, 0x4e );

	minor0 = _mm_sub_ps( _mm_mul_ps( row1, tmp1 ), minor0 );
	minor1 = _mm_sub_ps( _mm_mul_ps( row0, tmp1 ), minor1 );
	minor1 = _mm_shuffle_ps( minor1, minor1, 0x4e );

	//---------

	tmp1 = _mm_mul_ps( row1, row2 );
	tmp1 = _mm_shuffle_ps( tmp1, tmp1, 0xb1 );

	minor0 = _mm_add_ps( _mm_mul_ps( row3, tmp1 ), minor0 );
	minor3 = _mm_mul_ps( row0, tmp1 );

	tmp1 = _mm_shuffle_ps( tmp1, tmp1, 0x4e );

	minor0 = _mm_sub_ps( minor0, _mm_mul_ps( row3, tmp1 ) );
	minor3 = _mm_sub_ps( _mm_mul_ps( row0, tmp1 ), minor3 );
	minor3 = _mm_shuffle_ps( minor3, minor3, 0x4e );

	//---------

	tmp1 = _mm_mul_ps( _mm_shuffle_ps( row1, row1, 0x4e ), row3 );
	tmp1 = _mm_shuffle_ps( tmp1, tmp1, 0xb1 );
	row2 = _mm_shuffle_ps( row2, row2, 0x4e );

	minor0 = _mm_add_ps( _mm_mul_ps( row2, tmp1 ), minor0 );
	minor2 = _mm_mul_ps( row0, tmp1 );

	tmp1 = _mm_shuffle_ps( tmp1, tmp1, 0x4e );

	minor0 = _mm_sub_ps( minor0, _mm_mul_ps( row2, tmp1 ) );
	minor2 = _mm_sub_ps( _mm_mul_ps( row0, tmp1 ), minor2 );
	minor2 = _mm_shuffle_ps( minor2, minor2, 0x4e );

	//---------

	tmp1 = _mm_mul_ps( row0, row1 );
	tmp1 = _mm_shuffle_ps( tmp1, tmp1, 0xb1 );

	minor2 = _mm_add_ps( _mm_mul_ps( row3, tmp1 ), minor2 );
	minor3 = _mm_sub_ps( _mm_mul_ps( row2, tmp1 ), minor3 );

	tmp1 = _mm_shuffle_ps( tmp1, tmp1, 0x4e );

	minor2 = _mm_sub_ps( _mm_mul_ps( row3, tmp1 ), minor2 );
	minor3 = _mm_sub_ps( minor3, _mm_mul_ps( row2, tmp1 ) );

	//---------

	tmp1 = _mm_mul_ps( row0, row3 );
	tmp1 = _mm_shuffle_ps( tmp1, tmp1, 0xb1 );

	minor1 = _mm_sub_ps( minor1, _mm_mul_ps( row2, tmp1 ) );
	minor2 = _mm_add_ps( _mm_mul_ps( row1, tmp1 ), minor2 );

	tmp1 = _mm_shuffle_ps( tmp1, tmp1, 0x4e );

	minor1 = _mm_add_ps( _mm_mul_ps( row2, tmp1 ), minor1 );
	minor2 = _mm_sub_ps( minor2, _mm_mul_ps( row1, tmp1 ) );

	//---------

	tmp1 = _mm_mul_ps( row0, row2 );
	tmp1 = _mm_shuffle_ps( tmp1, tmp1, 0xb1 );

	minor1 = _mm_add_ps( _mm_mul_ps( row3, tmp1 ), minor1 );
	minor3 = _mm_sub_ps( minor3, _mm_mul_ps( row1, tmp1 ) );

	tmp1 = _mm_shuffle_ps( tmp1, tmp1, 0x4e );

	minor1 = _mm_sub_ps( minor1, _mm_mul_ps( row3, tmp1 ) );
	minor3 = _mm_add_ps( _mm_mul_ps( row1, tmp1 ), minor3 );

	//---------

	det = _mm_mul_ps( row0, minor0 );
	det = _mm_add_ps( _mm_shuffle_ps( det, det, 0x4e ), det );
	det = _mm_add_ss( _mm_shuffle_ps( det, det, 0xb1 ), det );
	tmp1 = _mm_rcp_ss( det );

	det = _mm_sub_ss( _mm_add_ss( tmp1, tmp1 ), _mm_mul_ss( det, _mm_mul_ss( tmp1, tmp1 ) ) );
	det = _mm_shuffle_ps( det, det, 0x00 );

	minor0 = _mm_mul_ps( det, minor0 );
	_mm_store_ps( _dst, minor0 );

	minor1 = _mm_mul_ps( det, minor1 );
	_mm_store_ps( _dst + 4, minor1 );

	minor2 = _mm_mul_ps( det, minor2 );
	_mm_store_ps( _dst + 8, minor2 );

	minor3 = _mm_mul_ps( det, minor3 );
	_mm_store_ps( _dst + 12, minor3 );

	return ( abs< float >( *( float * )&det ) > eps );
}
#if __clang__
#pragma clang diagnostic pop
#endif

#pragma warning( default : 4700 )

#endif // 0

//--------------------------------------------------------------------------
// matrix handling

template < >
GML_FORCEINLINE void mcol< SSE, 4, double > ( double const * const _lhs, double * const _dst, int col )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );
	GML_ASSERT( col >= 0 && col < 4 );

	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );
	__m128d &lhs0 = *( __m128d * )( _lhs + col * 4 );
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 + col * 4 );

	dst0 = lhs0;
	dst1 = lhs1;
}


template < >
GML_INLINE double mmax< SSE, 4, double > ( double const * const lhs )
{
	GML_SSE_CHECK_ALIGN( lhs );

	GML_SSE_ALIGN double v[ 4 ];
	v[ 0 ] = vmax< SSE, 4 >( lhs );
	v[ 1 ] = vmax< SSE, 4 >( lhs + 4 );
	v[ 2 ] = vmax< SSE, 4 >( lhs + 8 );
	v[ 3 ] = vmax< SSE, 4 >( lhs + 12 );

	return vmax< SSE, 4 >( v );
}


template < >
GML_INLINE double mmin< SSE, 4, double > ( double const * const lhs )
{
	GML_SSE_CHECK_ALIGN( lhs );

	GML_SSE_ALIGN double v[ 4 ];
	v[ 0 ] = vmin< SSE, 4 >( lhs );
	v[ 1 ] = vmin< SSE, 4 >( lhs + 4 );
	v[ 2 ] = vmin< SSE, 4 >( lhs + 8 );
	v[ 3 ] = vmin< SSE, 4 >( lhs + 12 );

	return vmin< SSE, 4 >( v );
}


template < >
GML_FORCEINLINE void mcopy< SSE, 4, double > ( double const * const _lhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &dst0 = ( ( __m128d * )_dst )[ 0 ];
	__m128d &dst1 = ( ( __m128d * )_dst )[ 1 ];
	__m128d &dst2 = ( ( __m128d * )_dst )[ 2 ];
	__m128d &dst3 = ( ( __m128d * )_dst )[ 3 ];	
	__m128d &lhs0 = ( ( __m128d * )_lhs )[ 0 ];
	__m128d &lhs1 = ( ( __m128d * )_lhs )[ 1 ];
	__m128d &lhs2 = ( ( __m128d * )_lhs )[ 2 ];
	__m128d &lhs3 = ( ( __m128d * )_lhs )[ 3 ];	

	dst0 = lhs0;
	dst1 = lhs1;
	dst2 = lhs2;
	dst3 = lhs3;

	__m128d &dst4 = ( ( __m128d * )_dst )[ 4 ];
	__m128d &dst5 = ( ( __m128d * )_dst )[ 5 ];
	__m128d &dst6 = ( ( __m128d * )_dst )[ 6 ];
	__m128d &dst7 = ( ( __m128d * )_dst )[ 7 ];	
	__m128d &lhs4 = ( ( __m128d * )_lhs )[ 4 ];
	__m128d &lhs5 = ( ( __m128d * )_lhs )[ 5 ];
	__m128d &lhs6 = ( ( __m128d * )_lhs )[ 6 ];
	__m128d &lhs7 = ( ( __m128d * )_lhs )[ 7 ];	

	dst4 = lhs4;
	dst5 = lhs5;
	dst6 = lhs6;
	dst7 = lhs7;
}


template < >
GML_FORCEINLINE void mtrans< SSE, 4, double > ( double const * const _lhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d tmp1, tmp2;
		
	tmp1 = ( ( __m128d * )_lhs )[ 0 ];
	tmp2 = ( ( __m128d * )_lhs )[ 2 ];
	_mm_store_pd( _dst +  0, _mm_shuffle_pd( tmp1, tmp2, _MM_SHUFFLE2( 0, 0 ) ) );
	_mm_store_pd( _dst +  4, _mm_shuffle_pd( tmp1, tmp2, _MM_SHUFFLE2( 1, 1 ) ) );

	tmp1 = ( ( __m128d * )_lhs )[ 1 ];
	tmp2 = ( ( __m128d * )_lhs )[ 3 ];
	_mm_store_pd( _dst +  8, _mm_shuffle_pd( tmp1, tmp2, _MM_SHUFFLE2( 0, 0 ) ) );
	_mm_store_pd( _dst + 12, _mm_shuffle_pd( tmp1, tmp2, _MM_SHUFFLE2( 1, 1 ) ) );

	tmp1 = ( ( __m128d * )_lhs )[ 4 ];
	tmp2 = ( ( __m128d * )_lhs )[ 6 ];
	_mm_store_pd( _dst +  2, _mm_shuffle_pd( tmp1, tmp2, _MM_SHUFFLE2( 0, 0 ) ) );
	_mm_store_pd( _dst +  6, _mm_shuffle_pd( tmp1, tmp2, _MM_SHUFFLE2( 1, 1 ) ) );

	tmp1 = ( ( __m128d * )_lhs )[ 5 ];
	tmp2 = ( ( __m128d * )_lhs )[ 7 ];
	_mm_store_pd( _dst + 10, _mm_shuffle_pd( tmp1, tmp2, _MM_SHUFFLE2( 0, 0 ) ) );
	_mm_store_pd( _dst + 14, _mm_shuffle_pd( tmp1, tmp2, _MM_SHUFFLE2( 1, 1 ) ) );
}


//--------------------------------------------------------------------------
// matrix algebra

#define MULT( i, off ) \
lhs0 = _mm_load_pd( _lhs +  0 + off ); \
lhs1 = _mm_load_pd( _lhs +  4 + off ); \
lhs2 = _mm_load_pd( _lhs +  8 + off ); \
lhs3 = _mm_load_pd( _lhs + 12 + off ); \
 \
v1 = _mm_load_pd( _rhs + 4*i ); \
v3 = _mm_load_pd( _rhs + 4*i + 2 ); \
 \
v0 = _mm_shuffle_pd( v1, v1, _MM_SHUFFLE2( 0, 0 ) ); \
v1 = _mm_shuffle_pd( v1, v1, _MM_SHUFFLE2( 1, 1 ) ); \
v2 = _mm_shuffle_pd( v3, v3, _MM_SHUFFLE2( 0, 0 ) ); \
v3 = _mm_shuffle_pd( v3, v3, _MM_SHUFFLE2( 1, 1 ) ); \
 \
v0 = _mm_mul_pd( lhs0, v0 ); \
v1 = _mm_mul_pd( lhs1, v1 ); \
v2 = _mm_mul_pd( lhs2, v2 ); \
v3 = _mm_mul_pd( lhs3, v3 ); \
 \
v0 = _mm_add_pd( v0, v1 ); \
v2 = _mm_add_pd( v2, v3 ); \
v0 = _mm_add_pd( v0, v2 );

template < >
GML_INLINE void mvmul< SSE, 4, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );
	GML_ASSERT( _lhs != _dst );
	GML_ASSERT( _rhs != _dst );

	// the columns of the matrix
	__m128d lhs0, lhs1, lhs2, lhs3;
	__m128d v0, v1, v2, v3;

	MULT( 0, 0 );
	_mm_store_pd( _dst, v0 );
	MULT( 0, 2 );
	_mm_store_pd( _dst + 2, v0 );
}


template < >
GML_INLINE void mvmul< SSE, 4, double > ( double const * const _lhs, double const * const _rhs, double * const _dst, int n )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );
	GML_ASSERT( _lhs != _dst );
	GML_ASSERT( _rhs != _dst );
	GML_ASSERT( n > 0 );

	// the columns of the matrix
	__m128d lhs0, lhs1, lhs2, lhs3;
	__m128d v0, v1, v2, v3;


	for( int i = 0 ; i < n ; ++i )
	{
		MULT( i, 0 );
		_mm_store_pd( _dst + 4*i, v0 );
		MULT( i, 2 );
		_mm_store_pd( _dst + 4*i + 2, v0 );
	}
}


template < >
GML_INLINE void mvmuls< SSE, 4, double > ( double const * const _lhs, double const * const _rhs, double * const _dst, int n )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );
	GML_ASSERT( _lhs != _dst );
	GML_ASSERT( _rhs != _dst );
	GML_ASSERT( n > 0 );

	// the columns of the matrix
	__m128d lhs0, lhs1, lhs2, lhs3;
	__m128d v0, v1, v2, v3;

	for( int i = 0 ; i < n ; ++i )
	{
		MULT( i, 0 );
		_mm_stream_pd( _dst + 4*i, v0 );
		MULT( i, 2 );
		_mm_stream_pd( _dst + 4*i + 2, v0 );
	}
}

#undef MULT


template < >
GML_INLINE double mqform< SSE, 4, double > ( double const * const u, double const * const M, double const * const v )
{
	GML_SSE_CHECK_ALIGN( u );
	GML_SSE_CHECK_ALIGN( M );
	GML_SSE_CHECK_ALIGN( v );

	GML_SSE_ALIGN double tmp[ 4 ];

	mvmul< SSE, 4 >( M, v, tmp );
	return vdot< SSE, 4 >( u, tmp );
}



//--------------------------------------------------------------------------

template < >
GML_INLINE bool meq< SSE, 4, double > ( double const * const lhs, double const * const rhs )
{
	GML_SSE_CHECK_ALIGN( lhs );
	GML_SSE_CHECK_ALIGN( rhs );

	GML_SSE_ALIGN float _v[ 4 ];
	_v[ 0 ] = veq< SSE, 4 >( lhs, rhs );
	_v[ 1 ] = veq< SSE, 4 >( lhs +  4, rhs +  4 );
	_v[ 2 ] = veq< SSE, 4 >( lhs +  8, rhs +  8 );
	_v[ 3 ] = veq< SSE, 4 >( lhs + 12, rhs + 12 );

	__m128 &v = *( __m128 * )_v;
	v = _mm_cmpeq_ps( v, _mm_setzero_ps() );
	int mask = _mm_movemask_ps( v );
	return ( mask == 0 );
}


template < >
GML_INLINE bool mlt< SSE, 4, double > ( double const * const lhs, double const * const rhs )
{
	GML_SSE_CHECK_ALIGN( lhs );
	GML_SSE_CHECK_ALIGN( rhs );

	GML_SSE_ALIGN double tmp[ 16 ];
	mlt< SSE, 4 >( lhs, rhs, tmp );
	long long mlt = mmask< SSE, 4 >( tmp );
	mgt< SSE, 4 >( lhs, rhs, tmp );
	long long mgt = mmask< SSE, 4 >( tmp );
	return ( mlt > mgt );
}


template < >
GML_INLINE bool mgt< SSE, 4, double > ( double const * const lhs, double const * const rhs )
{
	GML_SSE_CHECK_ALIGN( lhs );
	GML_SSE_CHECK_ALIGN( rhs );

	GML_SSE_ALIGN double tmp[ 16 ];
	mlt< SSE, 4 >( lhs, rhs, tmp );
	long long mlt = mmask< SSE, 4 >( tmp );
	mgt< SSE, 4 >( lhs, rhs, tmp );
	long long mgt = mmask< SSE, 4 >( tmp );
	return ( mlt < mgt );
}


template < >
GML_INLINE bool mrealeq< SSE, 4, double > ( double const * const lhs, double const * const rhs, double eps )
{
	GML_SSE_CHECK_ALIGN( lhs );
	GML_SSE_CHECK_ALIGN( rhs );

	GML_SSE_ALIGN double tmp[ 16 ];
	msub< SSE, 4 >( lhs, rhs, tmp );
	mabs< SSE, 4 >( tmp, tmp );
	double r = mmax< SSE, 4 >( tmp );
	return ( r < eps );
}


//--------------------------------------------------------------------------

#endif // GML_SSE_VER >= GML_SSE2

GML_NAMESPACE_END

#endif // GML_SUPPORT_SSE
