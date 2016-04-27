
#if GML_SUPPORT_SSE

#if GML_SSE_VER >= GML_SSE2

GML_NAMESPACE_BEGIN

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//                   VECTOR 4 INT OPT
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// generators


template < >
GML_FORCEINLINE void vzero< SSE, 4, int > ( int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &dst = *( __m128i * )_dst;

	dst = _mm_setzero_si128();
}


template < >
GML_FORCEINLINE void vset< SSE, 4, int > ( int * const _dst, int x )
{
	GML_SSE_CHECK_ALIGN( _dst );
	
	__m128i &dst = *( __m128i * )_dst;
	
	dst = _mm_set1_epi32( x );
}


template < >
GML_FORCEINLINE void vset< SSE, 4, int > ( int * const _dst, int x, int y, int z, int w )
{
	GML_SSE_CHECK_ALIGN( _dst );
	
	__m128i &dst = *( __m128i * )_dst;
	
	dst = _mm_set_epi32( w, z, y, x );
}


//--------------------------------------------------------------------------
// unary operations

template < >
GML_FORCEINLINE void vcopy< SSE, 4, int > ( int const * const _lhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &dst = *( __m128i * )_dst;

	dst = lhs;
}


template < >
GML_FORCEINLINE void vneg< SSE, 4, int > ( int const * const _lhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &dst = *( __m128i * )_dst;

	dst = _mm_sub_epi32( _mm_setzero_si128(), lhs );
}


#if GML_SSE_VER >= GML_SSSE3

template < >
GML_FORCEINLINE void vabs< SSE, 4, int > ( int const * const _lhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &dst = *( __m128i * )_dst;

	dst = _mm_abs_epi32( lhs );
}

#else // GML_SSE_VER >= GML_SSSE3

// http://www-graphics.stanford.edu/~seander/bithacks.html#IntegerAbs
template < >
GML_FORCEINLINE void vabs< SSE, 4, int > ( int const * const _lhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &dst = *( __m128i * )_dst;

	const __m128i sign = _mm_srai_epi32( lhs, 31 );
	dst = _mm_xor_si128( _mm_add_epi32( lhs, sign ), sign );
}

#endif // GML_SSE_VER >= GML_SSSE3


#if GML_SSE_VER >= GML_SSE4_1

template < >
GML_FORCEINLINE int vmax< SSE, 4, int > ( int const * const _lhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );

	__m128i &lhs = *( __m128i * )_lhs;

	__m128i r1 = _mm_rol_ps( lhs, 1 );
	r1 = _mm_max_epi32( lhs, r1 );
	__m128i r2 = _mm_rol_ps( r1, 2 );
	r2 = _mm_max_epi32( r1, r2 );

	return *( int * )&r2;
}

template < >
GML_FORCEINLINE int vmin< SSE, 4, int > ( int const * const _lhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );

	__m128i &lhs = *( __m128i * )_lhs;

	__m128i r1 = _mm_rol_ps( lhs, 1 );
	r1 = _mm_min_epi32( lhs, r1 );
	__m128i r2 = _mm_rol_ps( r1, 2 );
	r2 = _mm_min_epi32( r1, r2 );
	return *( int * )&r2;
}


template < >
GML_FORCEINLINE void vclamp< SSE, 4, int > ( int const * const _lhs, int * const _dst, int min, int max )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &dst = *( __m128i * )_dst;

	dst = _mm_max_epi32( lhs, _mm_set1_epi32( min ) );
	dst = _mm_min_epi32( dst, _mm_set1_epi32( max ) );
}

#else // GML_SSE_VER >= GML_SSE4_1

template < >
GML_FORCEINLINE int vmax< SSE, 4, int > ( int const * const _lhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );

	__m128i &lhs = *( __m128i * )_lhs;

	__m128i r1 = _mm_rol_epi32( lhs, 1 );
	__m128i mask = _mm_cmpgt_epi32( lhs, r1 );
	r1 = _mm_or_si128( _mm_and_si128( mask, lhs ), _mm_andnot_si128( mask, r1 ) );

	__m128i r2 = _mm_rol_epi32( r1, 2 );
	mask = _mm_cmpgt_epi32( r1, r2 );
	r2 = _mm_or_si128( _mm_and_si128( mask, r1 ), _mm_andnot_si128( mask, r2 ) );

	return *( int * )&r2;
}


template < >
GML_FORCEINLINE int vmin< SSE, 4, int > ( int const * const _lhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );

	__m128i &lhs = *( __m128i * )_lhs;

	__m128i r1 = _mm_rol_epi32( lhs, 1 );
	__m128i mask = _mm_cmplt_epi32( lhs, r1 );
	r1 = _mm_or_si128( _mm_and_si128( mask, lhs ), _mm_andnot_si128( mask, r1 ) );

	__m128i r2 = _mm_rol_epi32( r1, 2 );
	mask = _mm_cmplt_epi32( r1, r2 );
	r2 = _mm_or_si128( _mm_and_si128( mask, r1 ), _mm_andnot_si128( mask, r2 ) );

	return *( int * )&r2;
}


template < >
GML_FORCEINLINE void vclamp< SSE, 4, int > ( int const * const _lhs, int * const _dst, int min, int max )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &dst = *( __m128i * )_dst;

	__m128i tmp = _mm_set1_epi32( min );
	__m128i mask = _mm_cmplt_epi32( lhs, tmp );
	dst = _mm_or_si128( _mm_and_si128( mask, tmp ), _mm_andnot_si128( mask, lhs ) );
	tmp = _mm_set1_epi32( max );
	mask = _mm_cmpgt_epi32( dst, tmp );
	dst = _mm_or_si128( _mm_and_si128( mask, tmp ), _mm_andnot_si128( mask, dst ) );
}

#endif // GML_SSE_VER >= GML_SSE4_1


//--------------------------------------------------------------------------
// scalar algebra

template < >
GML_FORCEINLINE void vadd< SSE, 4, int > ( int const * const _lhs, int rhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &dst = *( __m128i * )_dst;

	dst = _mm_add_epi32( lhs, _mm_set1_epi32( rhs ) );
}


template < >
GML_FORCEINLINE void vsub< SSE, 4, int > ( int const * const _lhs, int rhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &dst = *( __m128i * )_dst;

	dst = _mm_sub_epi32( lhs, _mm_set1_epi32( rhs ) );
}


template < >
GML_FORCEINLINE void vmul< SSE, 4, int > ( int const * const _lhs, int _rhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &dst = *( __m128i * )_dst;

	__m128i rhs = _mm_set1_epi32( _rhs );
	const __m128i mask = _mm_set_epi32( 0, -1, 0, -1 );

	// _mm_mul_epu32 multiplies the first and third ints.  then shift 4 bytes right to
	// multiply the second and fourth ints, and or the two together.  mask truncates
	// the results to 32-bits for each multiply (output is 64-bits otherwise).

	__m128i low = _mm_and_si128( _mm_mul_epu32( lhs, rhs ), mask );
	__m128i high = _mm_and_si128( _mm_mul_epu32( _mm_srli_si128( lhs, 4 ), rhs ), mask );
	dst = _mm_or_si128( low, _mm_slli_si128( high, 4 ) );
}

/*
template < >
GML_FORCEINLINE void vdiv< SSE, 4, int > ( int const * const _lhs, int rhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &dst = *( __m128i * )_dst;

	dst = _mm_div_ps( lhs, _mm_set1_ps( rhs ) );
}
*/


//--------------------------------------------------------------------------
// component-wise algebra

template < >
GML_FORCEINLINE void vadd< SSE, 4, int > ( int const * const _lhs, int const * const _rhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &rhs = *( __m128i * )_rhs;
	__m128i &dst = *( __m128i * )_dst;

	dst = _mm_add_epi32( lhs, rhs );
}


template < >
GML_FORCEINLINE void vsub< SSE, 4, int > ( int const * const _lhs, int const * const _rhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &rhs = *( __m128i * )_rhs;
	__m128i &dst = *( __m128i * )_dst;

	dst = _mm_sub_epi32( lhs, rhs );
}


template < >
GML_FORCEINLINE void vmul< SSE, 4, int > ( int const * const _lhs, int const * const _rhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &rhs = *( __m128i * )_rhs;
	__m128i &dst = *( __m128i * )_dst;

	const __m128i mask = _mm_set_epi32( 0, -1, 0, -1 );

	// _mm_mul_epu32 multiplies the first and third ints.  then shift 4 bytes right to
	// multiply the second and fourth ints, and or the two together.  mask truncates
	// the results to 32-bits for each multiply (output is 64-bits otherwise).

	__m128i low = _mm_and_si128( _mm_mul_epu32( lhs, rhs ), mask );
	__m128i high = _mm_and_si128( _mm_mul_epu32( _mm_srli_si128( lhs, 4 ), _mm_srli_si128( rhs, 4 ) ), mask );
	dst = _mm_or_si128( low, _mm_slli_si128( high, 4 ) );
}


/*
template < >
GML_FORCEINLINE void vdiv< SSE, 4, int > ( int const * const _lhs, int const * const _rhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &rhs = *( __m128i * )_rhs;
	__m128i &dst = *( __m128i * )_dst;

	dst = _mm_div_ps( lhs, rhs );
}
*/


template < >
GML_FORCEINLINE void vmax< SSE, 4, int > ( int const * const _lhs, int const * const _rhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &rhs = *( __m128i * )_rhs;
	__m128i &dst = *( __m128i * )_dst;

	__m128i mask = _mm_cmpgt_epi32( lhs, rhs );
	dst = _mm_or_si128( _mm_and_si128( mask, lhs ), _mm_andnot_si128( mask, rhs ) );
}


template < >
GML_FORCEINLINE void vmin< SSE, 4, int > ( int const * const _lhs, int const * const _rhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &rhs = *( __m128i * )_rhs;
	__m128i &dst = *( __m128i * )_dst;

	__m128i mask = _mm_cmplt_epi32( lhs, rhs );
	dst = _mm_or_si128( _mm_and_si128( mask, lhs ), _mm_andnot_si128( mask, rhs ) );
}


//--------------------------------------------------------------------------
// vector operations

template < >
GML_FORCEINLINE int vdot< SSE, 4, int > ( int const * const _lhs, int const * const _rhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &rhs = *( __m128i * )_rhs;

#if GML_SSE_VER >= GML_SSE4_1

	__m128i r = _mm_mul_epi32( lhs, rhs );
	r = AddHorizontal_epi32( r );

#else // GML_SSE_VER >= GML_SSE4_1

	// _mm_mul_epu32 multiplies the first and third ints.  then shift 4 bytes right to
	// multiply the second and fourth ints, and or the two together.  mask truncates
	// the results to 32-bits for each multiply (output is 64-bits otherwise).

	const __m128i mask = _mm_set_epi32( 0, -1, 0, -1 );
	__m128i low = _mm_and_si128( _mm_mul_epu32( lhs, rhs ), mask );
	__m128i high = _mm_and_si128( _mm_mul_epu32( _mm_srli_si128( lhs, 4 ), _mm_srli_si128( rhs, 4 ) ), mask );

	__m128i r = _mm_add_epi32( low, high );
	r = _mm_add_epi32( r, _mm_srli_si128( r, 8 ) );

#endif // GML_SSE_VER >= GML_SSE4_1

	return *( int * )&r;
}


//--------------------------------------------------------------------------
// comparison operations

template < >
GML_FORCEINLINE bool veq< SSE, 4, int > ( int const * const _lhs, int const * const _rhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &rhs = *( __m128i * )_rhs;

	__m128i r1 = _mm_cmpeq_epi32( lhs, rhs );
	int mask = _mm_movemask_epi8( r1 );
	return ( mask == 0xFFFF );
}


template < >
GML_FORCEINLINE bool vlt< SSE, 4, int > ( int const * const _lhs, int const * const _rhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &rhs = *( __m128i * )_rhs;

	__m128i lt = _mm_cmplt_epi32( lhs, rhs );
	__m128i gt = _mm_cmpgt_epi32( lhs, rhs );
	int mlt = _mm_movemask_epi8( lt );
	int mgt = _mm_movemask_epi8( gt );
	return ( mlt > mgt );
}


template < >
GML_FORCEINLINE bool vgt< SSE, 4, int > ( int const * const _lhs, int const * const _rhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &rhs = *( __m128i * )_rhs;

	__m128i lt = _mm_cmplt_epi32( lhs, rhs );
	__m128i gt = _mm_cmpgt_epi32( lhs, rhs );
	int mlt = _mm_movemask_epi8( lt );
	int mgt = _mm_movemask_epi8( gt );
	return ( mlt < mgt );
}


template < >
GML_FORCEINLINE void veq< SSE, 4, int > ( int const * const _lhs, int const * const _rhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &rhs = *( __m128i * )_rhs;
	__m128i &dst = *( __m128i * )_dst;

	dst = _mm_cmpeq_epi32( lhs, rhs );
}


template < >
GML_FORCEINLINE void vneq< SSE, 4, int > ( int const * const _lhs, int const * const _rhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &rhs = *( __m128i * )_rhs;
	__m128i &dst = *( __m128i * )_dst;

	dst = _mm_not_si128( _mm_cmpeq_epi32( lhs, rhs ) );
}


template < >
GML_FORCEINLINE void vlt< SSE, 4, int > ( int const * const _lhs, int const * const _rhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &rhs = *( __m128i * )_rhs;
	__m128i &dst = *( __m128i * )_dst;

	dst = _mm_cmplt_epi32( lhs, rhs );
}


template < >
GML_FORCEINLINE void vgt< SSE, 4, int > ( int const * const _lhs, int const * const _rhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &rhs = *( __m128i * )_rhs;
	__m128i &dst = *( __m128i * )_dst;

	dst = _mm_cmpgt_epi32( lhs, rhs );
}


template < >
GML_FORCEINLINE void vle< SSE, 4, int > ( int const * const _lhs, int const * const _rhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &rhs = *( __m128i * )_rhs;
	__m128i &dst = *( __m128i * )_dst;

	dst = _mm_or_si128( _mm_cmpeq_epi32( lhs, rhs ), _mm_cmplt_epi32( lhs, rhs ) );
}


template < >
GML_FORCEINLINE void vge< SSE, 4, int > ( int const * const _lhs, int const * const _rhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &rhs = *( __m128i * )_rhs;
	__m128i &dst = *( __m128i * )_dst;

	dst = _mm_or_si128( _mm_cmpeq_epi32( lhs, rhs ), _mm_cmpgt_epi32( lhs, rhs ) );
}


//--------------------------------------------------------------------------

template < >
GML_FORCEINLINE int vmask< SSE, 4, int > ( int const * const _lhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );

	__m128i &lhs = *( __m128i * )_lhs;

	int mask = _mm_movemask_epi8( lhs );
	mask = ( ( mask & 0x00000008 ) >> 3  ) |
		   ( ( mask & 0x00000080 ) >> 6  ) |
		   ( ( mask & 0x00000800 ) >> 9  ) |
		   ( ( mask & 0x00008000 ) >> 12 );
	return mask;
}


//--------------------------------------------------------------------------
// bit operations

template < >
GML_FORCEINLINE void vand< SSE, 4, int > ( int const * const _lhs, int const * const _rhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &rhs = *( __m128i * )_rhs;
	__m128i &dst = *( __m128i * )_dst;

	dst = _mm_and_si128( lhs, rhs );
}


template < >
GML_FORCEINLINE void vandnot< SSE, 4, int > ( int const * const _lhs, int const * const _rhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &rhs = *( __m128i * )_rhs;
	__m128i &dst = *( __m128i * )_dst;

	dst = _mm_andnot_si128( lhs, rhs );
}


template < >
GML_FORCEINLINE void vor< SSE, 4, int > ( int const * const _lhs, int const * const _rhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &rhs = *( __m128i * )_rhs;
	__m128i &dst = *( __m128i * )_dst;

	dst = _mm_or_si128( lhs, rhs );
}


template < >
GML_FORCEINLINE void vxor< SSE, 4, int > ( int const * const _lhs, int const * const _rhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &rhs = *( __m128i * )_rhs;
	__m128i &dst = *( __m128i * )_dst;

	dst = _mm_xor_si128( lhs, rhs );
}


template < >
GML_FORCEINLINE void vnot< SSE, 4, int > ( int const * const _lhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &dst = *( __m128i * )_dst;

	dst = _mm_xor_si128( lhs, _mm_set1_epi32( allbitson< int >() ) );
}


//--------------------------------------------------------------------------
// SSE operations

template < >
GML_FORCEINLINE void vselect< SSE, 4, int > ( int const * const _mask, int const * const _lhs, int const * const _rhs, int * const _dst )
{
	GML_SSE_CHECK_ALIGN( _mask );
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128i &mask = *( __m128i * )_mask;
	__m128i &lhs = *( __m128i * )_lhs;
	__m128i &rhs = *( __m128i * )_rhs;
	__m128i &dst = *( __m128i * )_dst;

	dst = _mm_or_si128( _mm_and_si128( mask, lhs ), _mm_andnot_si128( mask, rhs ) );
}


//--------------------------------------------------------------------------


// has to go at the bottom because of its use of other funcs
template < >
GML_FORCEINLINE void vrand< SSE, 4, int > ( int * const _dst, int min, int max )
{
	GML_SSE_CHECK_ALIGN( _dst );

	_dst[ 0 ] = ( int )rand();
	_dst[ 1 ] = ( int )rand();
	_dst[ 2 ] = ( int )rand();
	_dst[ 3 ] = ( int )rand();

	vdiv< SSE, 4 >( _dst, ( int )RAND_MAX / ( max - min ), _dst );
//	vmul< SSE, 4 >( _dst, max - min, _dst );
//	vdiv< SSE, 4 >( _dst, ( int )RAND_MAX, _dst );
	vadd< SSE, 4 >( _dst, min, _dst );
}


//--------------------------------------------------------------------------

GML_NAMESPACE_END

#endif // GML_SSE_VER >= GML_SSE2

#endif // GML_SUPPORT_SSE
