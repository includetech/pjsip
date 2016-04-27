
GML_NAMESPACE_BEGIN

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//                   VECTOR 4 FLOAT OPT
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// generators


template < >
GML_FORCEINLINE void vzero< SSE, 4, float > ( float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_xor_ps( dst, dst );
}


template < >
GML_FORCEINLINE void vset< SSE, 4, float > ( float * const _dst, float x )
{
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_set1_ps( x );
}


//--------------------------------------------------------------------------
// unary operations

template < >
GML_FORCEINLINE void vcopy< SSE, 4, float > ( float const * const _lhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = lhs;
}


template < >
GML_FORCEINLINE void vneg< SSE, 4, float > ( float const * const _lhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_sub_ps( _mm_setzero_ps(), lhs );
}


template < >
GML_FORCEINLINE void vrecip< SSE, 4, float > ( float const * const _lhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = RcpNR_ps( lhs );
}


template < >
GML_FORCEINLINE void vabs< SSE, 4, float > ( float const * const _lhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &dst = *( __m128 * )_dst;

	const float _NEGMASK = 0.0f * -1.0f;
	const __m128 _MASKSIGN_ = _mm_set_ps( _NEGMASK, _NEGMASK, _NEGMASK, _NEGMASK );
	dst = _mm_andnot_ps( _MASKSIGN_, lhs );
}


template < >
GML_FORCEINLINE void vclamp< SSE, 4, float > ( float const * const _lhs, float * const _dst, float min, float max )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_max_ps( lhs, _mm_set1_ps( min ) );
	dst = _mm_min_ps( dst, _mm_set1_ps( max ) );
}


template < >
GML_FORCEINLINE bool vnorm< SSE, 4, float > ( float const * const _lhs, float * const _dst, float const * const _fallback, float eps  )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &dst = *( __m128 * )_dst;

	__m128 r0 = _fallback ? *( __m128 * )_fallback : lhs;

	// put v.v into r0
	__m128 r = _mm_mul_ps( lhs, lhs );
	r = AddHorizontal_ps( r );

	// put mask into tmp
	__m128 tmp = _mm_cmple_ss( _mm_set_ps1( eps ), r );
	tmp = _mm_shuffle_ps( tmp, tmp, 0x00 );

	r = _mm_shuffle_ps( r, r, 0x00 );
	r = RSqrtNR_ps( r );

	dst = MaskSelect_ps( tmp, _mm_mul_ps( lhs, r ), r0 );

	return ( ( *( float * )&tmp ) != 0.f );
}


template < >
GML_FORCEINLINE float vlen< SSE, 4, float > ( float const * const _lhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );

	__m128 &lhs = *( __m128 * )_lhs;

	// put v.v into r0
	__m128 r = _mm_mul_ps( lhs, lhs );
	r = AddHorizontal_ps( r );

	// sqrt for length
	r = _mm_sqrt_ss( r );

	return *( float * )&r;
}


template < >
GML_FORCEINLINE float vmax< SSE, 4, float > ( float const * const _lhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );

	__m128 &lhs = *( __m128 * )_lhs;

	__m128 r1 = _mm_rol_ps( lhs, 1 );
	r1 = _mm_max_ps( lhs, r1 );
	__m128 r2 = _mm_rol_ps( r1, 2 );
	r2 = _mm_max_ps( r1, r2 );

	return *( float * )&r2;
}


template < >
GML_FORCEINLINE float vmin< SSE, 4, float > ( float const * const _lhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );

	__m128 &lhs = *( __m128 * )_lhs;

	__m128 r1 = _mm_rol_ps( lhs, 1 );
	r1 = _mm_min_ps( lhs, r1 );
	__m128 r2 = _mm_rol_ps( r1, 2 );
	r2 = _mm_min_ps( r1, r2 );
	return *( float * )&r2;
}


//--------------------------------------------------------------------------
// scalar algebra

template < >
GML_FORCEINLINE void vadd< SSE, 4, float > ( float const * const _lhs, float rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_add_ps( lhs, _mm_set1_ps( rhs ) );
}


template < >
GML_FORCEINLINE void vsub< SSE, 4, float > ( float const * const _lhs, float rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_sub_ps( lhs, _mm_set1_ps( rhs ) );
}


template < >
GML_FORCEINLINE void vmul< SSE, 4, float > ( float const * const _lhs, float rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_mul_ps( lhs, _mm_set1_ps( rhs ) );
}


template < >
GML_FORCEINLINE void vdiv< SSE, 4, float > ( float const * const _lhs, float rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_div_ps( lhs, _mm_set1_ps( rhs ) );
}


//--------------------------------------------------------------------------
// component-wise algebra

template < >
GML_FORCEINLINE void vadd< SSE, 4, float > ( float const * const _lhs, float const * const _rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &rhs = *( __m128 * )_rhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_add_ps( lhs, rhs );
}


template < >
GML_FORCEINLINE void vsub< SSE, 4, float > ( float const * const _lhs, float const * const _rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &rhs = *( __m128 * )_rhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_sub_ps( lhs, rhs );
}


template < >
GML_FORCEINLINE void vmul< SSE, 4, float > ( float const * const _lhs, float const * const _rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &rhs = *( __m128 * )_rhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_mul_ps( lhs, rhs );
}


template < >
GML_FORCEINLINE void vdiv< SSE, 4, float > ( float const * const _lhs, float const * const _rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &rhs = *( __m128 * )_rhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_div_ps( lhs, rhs );
}


template < >
GML_FORCEINLINE void vmax< SSE, 4, float > ( float const * const _lhs, float const * const _rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &rhs = *( __m128 * )_rhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_max_ps( lhs, rhs );
}


template < >
GML_FORCEINLINE void vmin< SSE, 4, float > ( float const * const _lhs, float const * const _rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &rhs = *( __m128 * )_rhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_min_ps( lhs, rhs );
}


//--------------------------------------------------------------------------
// vector operations

template < >
GML_FORCEINLINE float vdot< SSE, 4, float > ( float const * const _lhs, float const * const _rhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &rhs = *( __m128 * )_rhs;

	__m128 r = _mm_mul_ps( lhs, rhs );
	r = AddHorizontal_ps( r );

	return *( float * )&r;
}


//--------------------------------------------------------------------------
// comparison operations

template < >
GML_FORCEINLINE bool veq< SSE, 4, float > ( float const * const _lhs, float const * const _rhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &rhs = *( __m128 * )_rhs;

	__m128 r1 = _mm_cmpneq_ps( lhs, rhs );
	int mask = _mm_movemask_ps( r1 );
	return ( mask == 0 );
}


template < >
GML_FORCEINLINE bool vlt< SSE, 4, float > ( float const * const _lhs, float const * const _rhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &rhs = *( __m128 * )_rhs;

	__m128 lt = _mm_cmplt_ps( lhs, rhs );
	__m128 gt = _mm_cmpgt_ps( lhs, rhs );
	int mlt = _mm_movemask_ps( lt );
	int mgt = _mm_movemask_ps( gt );
	return ( mlt > mgt );
}


template < >
GML_FORCEINLINE bool vgt< SSE, 4, float > ( float const * const _lhs, float const * const _rhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &rhs = *( __m128 * )_rhs;

	__m128 lt = _mm_cmplt_ps( lhs, rhs );
	__m128 gt = _mm_cmpgt_ps( lhs, rhs );
	int mlt = _mm_movemask_ps( lt );
	int mgt = _mm_movemask_ps( gt );
	return ( mlt < mgt );
}


template < >
GML_FORCEINLINE void veq< SSE, 4, float > ( float const * const _lhs, float const * const _rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &rhs = *( __m128 * )_rhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_cmpeq_ps( lhs, rhs );
}


template < >
GML_FORCEINLINE void vneq< SSE, 4, float > ( float const * const _lhs, float const * const _rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &rhs = *( __m128 * )_rhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_cmpneq_ps( lhs, rhs );
}


template < >
GML_FORCEINLINE void vlt< SSE, 4, float > ( float const * const _lhs, float const * const _rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &rhs = *( __m128 * )_rhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_cmplt_ps( lhs, rhs );
}


template < >
GML_FORCEINLINE void vgt< SSE, 4, float > ( float const * const _lhs, float const * const _rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &rhs = *( __m128 * )_rhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_cmpgt_ps( lhs, rhs );
}


template < >
GML_FORCEINLINE void vle< SSE, 4, float > ( float const * const _lhs, float const * const _rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &rhs = *( __m128 * )_rhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_cmple_ps( lhs, rhs );
}


template < >
GML_FORCEINLINE void vge< SSE, 4, float > ( float const * const _lhs, float const * const _rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &rhs = *( __m128 * )_rhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_cmpge_ps( lhs, rhs );
}


template < >
GML_FORCEINLINE bool vrealeq< SSE, 4, float > ( float const * const _lhs, float const * const _rhs, float eps )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );

	GML_SSE_ALIGN float tmp[ 4 ];
	vsub< SSE, 4 >( _lhs, _rhs, tmp );
	vabs< SSE, 4 >( tmp, tmp );
	float r = vmax< SSE, 4 >( tmp );
	return ( r < eps );
}


//--------------------------------------------------------------------------

template < >
GML_FORCEINLINE int vmask< SSE, 4, float > ( float const * const _lhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );

	__m128 &lhs = *( __m128 * )_lhs;

	int mask = _mm_movemask_ps( lhs );
	return mask;
}


//--------------------------------------------------------------------------
// bit operations

template < >
GML_FORCEINLINE void vand< SSE, 4, float > ( float const * const _lhs, float const * const _rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &rhs = *( __m128 * )_rhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_and_ps( lhs, rhs );
}


template < >
GML_FORCEINLINE void vandnot< SSE, 4, float > ( float const * const _lhs, float const * const _rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &rhs = *( __m128 * )_rhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_andnot_ps( lhs, rhs );
}


template < >
GML_FORCEINLINE void vor< SSE, 4, float > ( float const * const _lhs, float const * const _rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &rhs = *( __m128 * )_rhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_or_ps( lhs, rhs );
}


template < >
GML_FORCEINLINE void vxor< SSE, 4, float > ( float const * const _lhs, float const * const _rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &rhs = *( __m128 * )_rhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_xor_ps( lhs, rhs );
}


template < >
GML_FORCEINLINE void vnot< SSE, 4, float > ( float const * const _lhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_xor_ps( lhs, _mm_set1_ps( allbitson< float >() ) );
}


//--------------------------------------------------------------------------
// SSE operations

template < >
GML_FORCEINLINE void vselect< SSE, 4, float > ( float const * const _mask, float const * const _lhs, float const * const _rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _mask );
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &mask = *( __m128 * )_mask;
	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &rhs = *( __m128 * )_rhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_or_ps( _mm_and_ps( mask, lhs ), _mm_andnot_ps( mask, rhs ) );
}


//--------------------------------------------------------------------------


template < >
GML_FORCEINLINE bool vhomog< SSE, 4, float > ( float const * const _lhs, float * const _dst, float const * const _fallback, float eps  )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &dst = *( __m128 * )_dst;

	__m128 r0 = _fallback ? *( __m128 * )_fallback : lhs;

	__m128 r = _mm_shuffle_ps( lhs, lhs, _MM_SHUFFLE( 3, 3, 3, 3 ) );
	__m128 tmp = _mm_cmple_ps( _mm_set_ps1( eps ), r );
	r = RcpNR_ps( r );

	dst = MaskSelect_ps( tmp, _mm_mul_ps( lhs, r ), r0 );

	return ( ( *( float * )&tmp ) != 0.f );
}


template < >
GML_FORCEINLINE void vcross< SSE, 4, float > ( float const * const _lhs, float const * const _rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &rhs = *( __m128 * )_rhs;
	__m128 &dst = *( __m128 * )_dst;

	const GML_SSE_ALIGN unsigned int __0FFF_[ 4 ] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 };
	const __m128 _0FFF_ = *( __m128 * )__0FFF_;

	__m128 l1, l2, m1, m2;
	l1 = _mm_shuffle_ps( lhs, lhs, _MM_SHUFFLE( 3, 1, 0, 2 ) );
	l2 = _mm_shuffle_ps( rhs, rhs, _MM_SHUFFLE( 3, 0, 2, 1 ) );
	m2 = _mm_and_ps( _mm_mul_ps( l1, l2 ), _0FFF_ );
	l1 = _mm_shuffle_ps( lhs, lhs, _MM_SHUFFLE( 3, 0, 2, 1 ) );
	l2 = _mm_shuffle_ps( rhs, rhs, _MM_SHUFFLE( 3, 1, 0, 2 ) );
	m1 = _mm_mul_ps( l1, l2 );
	dst = _mm_sub_ps( m1, m2 );
}


//--------------------------------------------------------------------------

// has to go at the bottom because of its use of other funcs
template < >
GML_FORCEINLINE void vrand< SSE, 4, float > ( float * const _dst, float min, float max )
{
	GML_SSE_CHECK_ALIGN( _dst );

	_dst[ 0 ] = ( float )rand();
	_dst[ 1 ] = ( float )rand();
	_dst[ 2 ] = ( float )rand();
	_dst[ 3 ] = ( float )rand();

	vdiv< SSE, 4 >( _dst, ( float )RAND_MAX, _dst );
	vmul< SSE, 4 >( _dst, max - min, _dst );
	vadd< SSE, 4 >( _dst, min, _dst );
}



//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//                   VECTOR 4 DOUBLE OPT
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// generators

template < >
GML_FORCEINLINE void vset< SSE, 4, double > ( double * const _dst, double x )
{
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	dst0 = _mm_set1_pd( x );
	dst1 = _mm_set1_pd( x );
}


//--------------------------------------------------------------------------
// unary operations

template < >
GML_FORCEINLINE void vcopy< SSE, 4, double > ( double const * const _lhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	dst0 = lhs0;
	dst1 = lhs1;
}


template < >
GML_FORCEINLINE void vneg< SSE, 4, double > ( double const * const _lhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	dst0 = _mm_sub_pd( _mm_setzero_pd(), lhs0 );
	dst1 = _mm_sub_pd( _mm_setzero_pd(), lhs1 );
}


template < >
GML_FORCEINLINE void vrecip< SSE, 4, double > ( double const * const _lhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	dst0 = _mm_div_pd( _mm_set1_pd( 1.0 ), lhs0 );  //TODO FAST APPROX
	dst1 = _mm_div_pd( _mm_set1_pd( 1.0 ), lhs1 );
}


template < >
GML_FORCEINLINE void vabs< SSE, 4, double > ( double const * const _lhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	const float _NEGMASK = 0.0f * -1.0f;
	const __m128d _MASKSIGN_ = _mm_set_pd( _NEGMASK, _NEGMASK );
	dst0 = _mm_andnot_pd( _MASKSIGN_, lhs0 );
	dst1 = _mm_andnot_pd( _MASKSIGN_, lhs1 );
}


template < >
GML_FORCEINLINE void vclamp< SSE, 4, double > ( double const * const _lhs, double * const _dst, double min, double max )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	dst0 = _mm_max_pd( lhs0, _mm_set1_pd( min ) );
	dst1 = _mm_max_pd( lhs1, _mm_set1_pd( min ) );
	dst0 = _mm_min_pd( dst0, _mm_set1_pd( max ) );
	dst1 = _mm_min_pd( dst1, _mm_set1_pd( max ) );
}


template < >
GML_INLINE bool vnorm< SSE, 4, double > ( double const * const _lhs, double * const _dst, double const * const _fallback, double eps  )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	__m128d fb0 = _fallback ? *( __m128d * )_fallback : lhs0;
	__m128d fb1 = _fallback ? *( __m128d * )( _fallback + 2 ) : lhs1;

	// put v.v into r
	__m128d r0 = _mm_mul_pd( lhs0, lhs0 );
	__m128d r1 = _mm_mul_pd( lhs1, lhs1 );
	r0 = _mm_add_pd( r0, r1 );
	r0 = AddHorizontal_pd( r0 );

	// put mask into tmp
	__m128d tmp = _mm_cmple_sd( _mm_set1_pd( eps ), r0 );
	tmp = _mm_shuffle_pd( tmp, tmp, 0x00 );

	r0 = _mm_shuffle_pd( r0, r0, 0x00 );
	r0 = _mm_div_pd( _mm_set1_pd( 1.0 ), _mm_sqrt_pd( r0 ) );	//TODO FAST APPROXIMATION

	dst0 = MaskSelect_pd( tmp, _mm_mul_pd( lhs0, r0 ), fb0 );
	dst1 = MaskSelect_pd( tmp, _mm_mul_pd( lhs1, r0 ), fb1 );

	return ( ( *( double * )&tmp ) != 0.f );
}


template < >
GML_FORCEINLINE double vlen< SSE, 4, double > ( double const * const _lhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );

	// put v.v into r
	__m128d r0 = _mm_mul_pd( lhs0, lhs0 );
	__m128d r1 = _mm_mul_pd( lhs1, lhs1 );
	r0 = _mm_add_pd( r0, r1 );
	r0 = AddHorizontal_pd( r0 );

	// sqrt for length
	r0 = _mm_sqrt_sd( r0, r0 );

	return *( double * )&r0;
}


template < >
GML_FORCEINLINE double vmax< SSE, 4, double > ( double const * const _lhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );

	__m128d r = _mm_max_pd( lhs0, lhs1 );
	r = _mm_max_pd( r, _mm_swap_pd( r ) );

	return *( double * )&r;
}


template < >
GML_FORCEINLINE double vmin< SSE, 4, double > ( double const * const _lhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );

	__m128d r = _mm_min_pd( lhs0, lhs1 );
	r = _mm_min_pd( r, _mm_swap_pd( r ) );

	return *( double * )&r;
}


//--------------------------------------------------------------------------
// scalar algebra

template < >
GML_FORCEINLINE void vadd< SSE, 4, double > ( double const * const _lhs, double rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	dst0 = _mm_add_pd( lhs0, _mm_set1_pd( rhs ) );
	dst1 = _mm_add_pd( lhs1, _mm_set1_pd( rhs ) );
}


template < >
GML_FORCEINLINE void vsub< SSE, 4, double > ( double const * const _lhs, double rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	dst0 = _mm_sub_pd( lhs0, _mm_set1_pd( rhs ) );
	dst1 = _mm_sub_pd( lhs1, _mm_set1_pd( rhs ) );
}


template < >
GML_FORCEINLINE void vmul< SSE, 4, double > ( double const * const _lhs, double rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	dst0 = _mm_mul_pd( lhs0, _mm_set1_pd( rhs ) );
	dst1 = _mm_mul_pd( lhs1, _mm_set1_pd( rhs ) );
}


template < >
GML_FORCEINLINE void vdiv< SSE, 4, double > ( double const * const _lhs, double rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	dst0 = _mm_div_pd( lhs0, _mm_set1_pd( rhs ) );
	dst1 = _mm_div_pd( lhs1, _mm_set1_pd( rhs ) );
}


//--------------------------------------------------------------------------
// component-wise algebra

template < >
GML_FORCEINLINE void vadd< SSE, 4, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &rhs1 = *( __m128d * )( _rhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	dst0 = _mm_add_pd( lhs0, rhs0 );
	dst1 = _mm_add_pd( lhs1, rhs1 );
}


template < >
GML_FORCEINLINE void vsub< SSE, 4, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &rhs1 = *( __m128d * )( _rhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	dst0 = _mm_sub_pd( lhs0, rhs0 );
	dst1 = _mm_sub_pd( lhs1, rhs1 );
}


template < >
GML_FORCEINLINE void vmul< SSE, 4, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &rhs1 = *( __m128d * )( _rhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	dst0 = _mm_mul_pd( lhs0, rhs0 );
	dst1 = _mm_mul_pd( lhs1, rhs1 );
}


template < >
GML_FORCEINLINE void vdiv< SSE, 4, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &rhs1 = *( __m128d * )( _rhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	dst0 = _mm_div_pd( lhs0, rhs0 );
	dst1 = _mm_div_pd( lhs1, rhs1 );
}


template < >
GML_FORCEINLINE void vmax< SSE, 4, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &rhs1 = *( __m128d * )( _rhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	dst0 = _mm_max_pd( lhs0, rhs0 );
	dst1 = _mm_max_pd( lhs1, rhs1 );
}


template < >
GML_FORCEINLINE void vmin< SSE, 4, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &rhs1 = *( __m128d * )( _rhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	dst0 = _mm_min_pd( lhs0, rhs0 );
	dst1 = _mm_min_pd( lhs1, rhs1 );
}


//--------------------------------------------------------------------------
// vector operations

template < >
GML_FORCEINLINE double vdot< SSE, 4, double > ( double const * const _lhs, double const * const _rhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &rhs1 = *( __m128d * )( _rhs + 2 );

	__m128d r0 = _mm_mul_pd( lhs0, rhs0 );
	__m128d r1 = _mm_mul_pd( lhs1, rhs1 );
	r0 = _mm_add_pd( r0, r1 );
	r0 = AddHorizontal_pd( r0 );

	return *( double * )&r0;
}


//--------------------------------------------------------------------------
// comparison operations

template < >
GML_FORCEINLINE bool veq< SSE, 4, double > ( double const * const _lhs, double const * const _rhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &rhs1 = *( __m128d * )( _rhs + 2 );

	__m128d r0 = _mm_cmpneq_pd( lhs0, rhs0 );
	__m128d r1 = _mm_cmpneq_pd( lhs1, rhs1 );

	int mask0 = _mm_movemask_pd( r0 );
	int mask1 = _mm_movemask_pd( r1 );
	int mask = ( ( ( mask1 << 2 ) & 0xC ) | ( mask0 & 0x3 ) );

	return ( mask == 0 );
}


template < >
GML_FORCEINLINE bool vlt< SSE, 4, double > ( double const * const _lhs, double const * const _rhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &rhs1 = *( __m128d * )( _rhs + 2 );

	__m128d lt0 = _mm_cmplt_pd( lhs0, rhs0 );
	__m128d lt1 = _mm_cmplt_pd( lhs1, rhs1 );
	__m128d gt0 = _mm_cmpgt_pd( lhs0, rhs0 );
	__m128d gt1 = _mm_cmpgt_pd( lhs1, rhs1 );

	int mlt0 = _mm_movemask_pd( lt0 );
	int mlt1 = _mm_movemask_pd( lt1 );
	int mlt = ( ( ( mlt1 << 2 ) & 0xC ) | ( mlt0 & 0x3 ) );
	int mgt0 = _mm_movemask_pd( gt0 );
	int mgt1 = _mm_movemask_pd( gt1 );
	int mgt = ( ( ( mgt1 << 2 ) & 0xC ) | ( mgt0 & 0x3 ) );

	return ( mlt > mgt );
}


template < >
GML_FORCEINLINE bool vgt< SSE, 4, double > ( double const * const _lhs, double const * const _rhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &rhs1 = *( __m128d * )( _rhs + 2 );

	__m128d lt0 = _mm_cmplt_pd( lhs0, rhs0 );
	__m128d lt1 = _mm_cmplt_pd( lhs1, rhs1 );
	__m128d gt0 = _mm_cmpgt_pd( lhs0, rhs0 );
	__m128d gt1 = _mm_cmpgt_pd( lhs1, rhs1 );

	int mlt0 = _mm_movemask_pd( lt0 );
	int mlt1 = _mm_movemask_pd( lt1 );
	int mlt = ( ( ( mlt1 << 2 ) & 0xC ) | ( mlt0 & 0x3 ) );
	int mgt0 = _mm_movemask_pd( gt0 );
	int mgt1 = _mm_movemask_pd( gt1 );
	int mgt = ( ( ( mgt1 << 2 ) & 0xC ) | ( mgt0 & 0x3 ) );

	return ( mlt < mgt );
}


template < >
GML_FORCEINLINE void veq< SSE, 4, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &rhs1 = *( __m128d * )( _rhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	dst0 = _mm_cmpeq_pd( lhs0, rhs0 );
	dst1 = _mm_cmpeq_pd( lhs1, rhs1 );
}


template < >
GML_FORCEINLINE void vneq< SSE, 4, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &rhs1 = *( __m128d * )( _rhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	dst0 = _mm_cmpneq_pd( lhs0, rhs0 );
	dst1 = _mm_cmpneq_pd( lhs1, rhs1 );
}


template < >
GML_FORCEINLINE void vlt< SSE, 4, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &rhs1 = *( __m128d * )( _rhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	dst0 = _mm_cmplt_pd( lhs0, rhs0 );
	dst1 = _mm_cmplt_pd( lhs1, rhs1 );
}


template < >
GML_FORCEINLINE void vgt< SSE, 4, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &rhs1 = *( __m128d * )( _rhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	dst0 = _mm_cmpgt_pd( lhs0, rhs0 );
	dst1 = _mm_cmpgt_pd( lhs1, rhs1 );
}


template < >
GML_FORCEINLINE void vle< SSE, 4, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &rhs1 = *( __m128d * )( _rhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	dst0 = _mm_cmple_pd( lhs0, rhs0 );
	dst1 = _mm_cmple_pd( lhs1, rhs1 );
}


template < >
GML_FORCEINLINE void vge< SSE, 4, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &rhs1 = *( __m128d * )( _rhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	dst0 = _mm_cmpge_pd( lhs0, rhs0 );
	dst1 = _mm_cmpge_pd( lhs1, rhs1 );
}


template < >
GML_FORCEINLINE bool vrealeq< SSE, 4, double > ( double const * const _lhs, double const * const _rhs, double eps )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );

	GML_SSE_ALIGN double tmp[ 4 ];
	vsub< SSE, 4 >( _lhs, _rhs, tmp );
	vabs< SSE, 4 >( tmp, tmp );
	double r = vmax< SSE, 4 >( tmp );
	return ( r < eps );
}


//--------------------------------------------------------------------------

template < >
GML_FORCEINLINE int vmask< SSE, 4, double > ( double const * const _lhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );

	int mask0 = _mm_movemask_pd( lhs0 );
	int mask1 = _mm_movemask_pd( lhs1 );
	return ( ( ( mask1 << 2 ) & 0xC ) | ( mask0 & 0x3 ) );
}


//--------------------------------------------------------------------------
// SSE operations

template < >
GML_FORCEINLINE void vselect< SSE, 4, double > ( double const * const _mask, double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _mask );
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &mask0 = *( __m128d * )_mask;
	__m128d &lhs0  = *( __m128d * )_lhs;
	__m128d &rhs0  = *( __m128d * )_rhs;
	__m128d &dst0  = *( __m128d * )_dst;

	dst0 = _mm_or_pd( _mm_and_pd( mask0, lhs0 ), _mm_andnot_pd( mask0, rhs0 ) );

	__m128d &mask1 = *( __m128d * )( _mask + 2 );
	__m128d &lhs1  = *( __m128d * )( _lhs + 2 );
	__m128d &rhs1  = *( __m128d * )( _rhs + 2 );
	__m128d &dst1  = *( __m128d * )( _dst + 2 );

	dst1 = _mm_or_pd( _mm_and_pd( mask1, lhs1 ), _mm_andnot_pd( mask1, rhs1 ) );
}


//--------------------------------------------------------------------------


template < >
GML_INLINE bool vhomog< SSE, 4, double > ( double const * const _lhs, double * const _dst, double const * const _fallback, double eps  )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	__m128d fb0 = _fallback ? *( __m128d * )_fallback : lhs0;
	__m128d fb1 = _fallback ? *( __m128d * )( _fallback + 2 ) : lhs1;

	__m128d r = _mm_shuffle_pd( lhs1, lhs1, _MM_SHUFFLE2( 1, 1 ) );
	__m128d tmp = _mm_cmple_pd( _mm_set1_pd( eps ), r );
	r = _mm_div_pd( _mm_set1_pd( 1.0 ), r );

	dst0 = MaskSelect_pd( tmp, _mm_mul_pd( lhs0, r ), fb0 );
	dst1 = MaskSelect_pd( tmp, _mm_mul_pd( lhs1, r ), fb1 );

	return ( ( *( double * )&tmp ) != 0.f );
}


template < >
GML_INLINE void vcross< SSE, 4, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;          // { b, a }
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );  // { -, c }
	__m128d &rhs0 = *( __m128d * )_rhs;          // { y, x }
	__m128d &rhs1 = *( __m128d * )( _rhs + 2 );  // { -, z }
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	__m128d l1, l2, m1, m2;
	l1 = _mm_shuffle_pd( lhs0, lhs1, _MM_SHUFFLE2( 0, 1 ) );  // { c, b }
	l2 = _mm_shuffle_pd( rhs1, rhs0, _MM_SHUFFLE2( 0, 0 ) );  // { x, z }
	m1 = _mm_mul_pd( l1, l2 );                                // { cx, bz }
	l1 = _mm_shuffle_pd( lhs1, lhs0, _MM_SHUFFLE2( 0, 0 ) );  // { a, c }
	l2 = _mm_shuffle_pd( rhs0, rhs1, _MM_SHUFFLE2( 0, 1 ) );  // { z, y }
	m2 = _mm_mul_pd( l1, l2 );                                // { az, cy }
	dst0 = _mm_sub_pd( m1, m2 );                              // { cx-az, bz-cy }

	const GML_SSE_ALIGN unsigned int __0F_[ 4 ] = { 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000 };
	const __m128d _0F_ = *( __m128d * )__0F_;

	l2 = _mm_swap_pd( rhs0 );                                 // { x, y }
	m1 = _mm_mul_pd( lhs0, l2 );                              // { bx, ay }
	m2 = _mm_sub_sd( m1, _mm_swap_pd( m1 ) );                 // { bx-ay, ay-bx }
	dst1 = _mm_and_pd( m2, _0F_ );                            // { 0, ay-bx }
}


//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//                   VECTOR 2 DOUBLE OPT
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// generators

template < >
GML_FORCEINLINE void vset< SSE, 2, double > ( double * const _dst, double x )
{
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &dst = *( __m128d * )_dst;

	dst = _mm_set1_pd( x );
}


//--------------------------------------------------------------------------
// unary operations

template < >
GML_FORCEINLINE void vcopy< SSE, 2, double > ( double const * const _lhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &dst0 = *( __m128d * )_dst;

	dst0 = lhs0;
}


template < >
GML_FORCEINLINE void vneg< SSE, 2, double > ( double const * const _lhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &dst0 = *( __m128d * )_dst;

	dst0 = _mm_sub_pd( _mm_setzero_pd(), lhs0 );
}


template < >
GML_FORCEINLINE void vrecip< SSE, 2, double > ( double const * const _lhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &dst0 = *( __m128d * )_dst;

	dst0 = _mm_div_pd( _mm_set1_pd( 1.0 ), lhs0 );  //TODO FAST APPROX
}


template < >
GML_FORCEINLINE void vabs< SSE, 2, double > ( double const * const _lhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &dst0 = *( __m128d * )_dst;

	const float _NEGMASK = 0.0f * -1.0f;
	const __m128d _MASKSIGN_ = _mm_set_pd( _NEGMASK, _NEGMASK );
	dst0 = _mm_andnot_pd( _MASKSIGN_, lhs0 );
}


template < >
GML_FORCEINLINE void vclamp< SSE, 2, double > ( double const * const _lhs, double * const _dst, double min, double max )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &dst0 = *( __m128d * )_dst;

	dst0 = _mm_max_pd( lhs0, _mm_set1_pd( min ) );
	dst0 = _mm_min_pd( dst0, _mm_set1_pd( max ) );
}


template < >
GML_INLINE bool vnorm< SSE, 2, double > ( double const * const _lhs, double * const _dst, double const * const _fallback, double eps  )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &dst0 = *( __m128d * )_dst;

	__m128d fb0 = _fallback ? *( __m128d * )_fallback : lhs0;

	// put v.v into r
	__m128d r0 = _mm_mul_pd( lhs0, lhs0 );
	r0 = AddHorizontal_pd( r0 );

	// put mask into tmp
	__m128d tmp = _mm_cmple_sd( _mm_set1_pd( eps ), r0 );
	tmp = _mm_shuffle_pd( tmp, tmp, 0x00 );

	r0 = _mm_shuffle_pd( r0, r0, 0x00 );
	r0 = _mm_div_pd( _mm_set1_pd( 1.0 ), _mm_sqrt_pd( r0 ) );	//TODO FAST APPROXIMATION

	dst0 = MaskSelect_pd( tmp, _mm_mul_pd( lhs0, r0 ), fb0 );

	return ( ( *( double * )&tmp ) != 0.f );
}


template < >
GML_FORCEINLINE double vlen< SSE, 2, double > ( double const * const _lhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );

	__m128d &lhs0 = *( __m128d * )_lhs;

	// put v.v into r
	__m128d r0 = _mm_mul_pd( lhs0, lhs0 );
	r0 = AddHorizontal_pd( r0 );

	// sqrt for length
	r0 = _mm_sqrt_sd( r0, r0 );

	return *( double * )&r0;
}


template < >
GML_FORCEINLINE double vmax< SSE, 2, double > ( double const * const _lhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );

	__m128d &lhs0 = *( __m128d * )_lhs;

	__m128d r = _mm_max_pd( lhs0, _mm_swap_pd( lhs0 ) );

	return *( double * )&r;
}


template < >
GML_FORCEINLINE double vmin< SSE, 2, double > ( double const * const _lhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );

	__m128d &lhs0 = *( __m128d * )_lhs;

	__m128d r = _mm_min_pd( lhs0, _mm_swap_pd( lhs0 ) );

	return *( double * )&r;
}


//--------------------------------------------------------------------------
// scalar algebra

template < >
GML_FORCEINLINE void vadd< SSE, 2, double > ( double const * const _lhs, double rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &dst0 = *( __m128d * )_dst;

	dst0 = _mm_add_pd( lhs0, _mm_set1_pd( rhs ) );
}


template < >
GML_FORCEINLINE void vsub< SSE, 2, double > ( double const * const _lhs, double rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &dst0 = *( __m128d * )_dst;

	dst0 = _mm_sub_pd( lhs0, _mm_set1_pd( rhs ) );
}


template < >
GML_FORCEINLINE void vmul< SSE, 2, double > ( double const * const _lhs, double rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &dst0 = *( __m128d * )_dst;

	dst0 = _mm_mul_pd( lhs0, _mm_set1_pd( rhs ) );
}


template < >
GML_FORCEINLINE void vdiv< SSE, 2, double > ( double const * const _lhs, double rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &dst0 = *( __m128d * )_dst;

	dst0 = _mm_div_pd( lhs0, _mm_set1_pd( rhs ) );
}


//--------------------------------------------------------------------------
// component-wise algebra

template < >
GML_FORCEINLINE void vadd< SSE, 2, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &dst0 = *( __m128d * )_dst;

	dst0 = _mm_add_pd( lhs0, rhs0 );
}


template < >
GML_FORCEINLINE void vsub< SSE, 2, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &dst0 = *( __m128d * )_dst;

	dst0 = _mm_sub_pd( lhs0, rhs0 );
}


template < >
GML_FORCEINLINE void vmul< SSE, 2, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &dst0 = *( __m128d * )_dst;

	dst0 = _mm_mul_pd( lhs0, rhs0 );
}


template < >
GML_FORCEINLINE void vdiv< SSE, 2, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &dst0 = *( __m128d * )_dst;

	dst0 = _mm_div_pd( lhs0, rhs0 );
}


template < >
GML_FORCEINLINE void vmax< SSE, 2, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &dst0 = *( __m128d * )_dst;

	dst0 = _mm_max_pd( lhs0, rhs0 );
}


template < >
GML_FORCEINLINE void vmin< SSE, 2, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &dst0 = *( __m128d * )_dst;

	dst0 = _mm_min_pd( lhs0, rhs0 );
}


//--------------------------------------------------------------------------
// vector operations

template < >
GML_FORCEINLINE double vdot< SSE, 2, double > ( double const * const _lhs, double const * const _rhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &rhs0 = *( __m128d * )_rhs;

	__m128d r0 = _mm_mul_pd( lhs0, rhs0 );
	r0 = AddHorizontal_pd( r0 );

	return *( double * )&r0;
}


//--------------------------------------------------------------------------
// comparison operations

template < >
GML_FORCEINLINE bool veq< SSE, 2, double > ( double const * const _lhs, double const * const _rhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &rhs0 = *( __m128d * )_rhs;

	__m128d r0 = _mm_cmpneq_pd( lhs0, rhs0 );
	int mask = _mm_movemask_pd( r0 );

	return ( mask == 0 );
}


template < >
GML_FORCEINLINE bool vlt< SSE, 2, double > ( double const * const _lhs, double const * const _rhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &rhs0 = *( __m128d * )_rhs;

	__m128d lt0 = _mm_cmplt_pd( lhs0, rhs0 );
	__m128d gt0 = _mm_cmpgt_pd( lhs0, rhs0 );

	int mlt = _mm_movemask_pd( lt0 );
	int mgt = _mm_movemask_pd( gt0 );

	return ( mlt > mgt );
}


template < >
GML_FORCEINLINE bool vgt< SSE, 2, double > ( double const * const _lhs, double const * const _rhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &rhs0 = *( __m128d * )_rhs;

	__m128d lt0 = _mm_cmplt_pd( lhs0, rhs0 );
	__m128d gt0 = _mm_cmpgt_pd( lhs0, rhs0 );

	int mlt = _mm_movemask_pd( lt0 );
	int mgt = _mm_movemask_pd( gt0 );

	return ( mlt < mgt );
}


template < >
GML_FORCEINLINE void veq< SSE, 2, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &dst0 = *( __m128d * )_dst;

	dst0 = _mm_cmpeq_pd( lhs0, rhs0 );
}


template < >
GML_FORCEINLINE void vneq< SSE, 2, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &dst0 = *( __m128d * )_dst;

	dst0 = _mm_cmpneq_pd( lhs0, rhs0 );
}


template < >
GML_FORCEINLINE void vlt< SSE, 2, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &dst0 = *( __m128d * )_dst;

	dst0 = _mm_cmplt_pd( lhs0, rhs0 );
}


template < >
GML_FORCEINLINE void vgt< SSE, 2, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &dst0 = *( __m128d * )_dst;

	dst0 = _mm_cmpgt_pd( lhs0, rhs0 );
}


template < >
GML_FORCEINLINE void vle< SSE, 2, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &dst0 = *( __m128d * )_dst;

	dst0 = _mm_cmple_pd( lhs0, rhs0 );
}


template < >
GML_FORCEINLINE void vge< SSE, 2, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &rhs0 = *( __m128d * )_rhs;
	__m128d &dst0 = *( __m128d * )_dst;

	dst0 = _mm_cmpge_pd( lhs0, rhs0 );
}


template < >
GML_FORCEINLINE bool vrealeq< SSE, 2, double > ( double const * const _lhs, double const * const _rhs, double eps )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );

	GML_SSE_ALIGN double tmp[ 2 ];
	vsub< SSE, 2 >( _lhs, _rhs, tmp );
	vabs< SSE, 2 >( tmp, tmp );
	double r = vmax< SSE, 2 >( tmp );
	return ( r < eps );
}


//--------------------------------------------------------------------------

template < >
GML_FORCEINLINE int vmask< SSE, 2, double > ( double const * const _lhs )
{
	GML_SSE_CHECK_ALIGN( _lhs );

	__m128d &lhs0 = *( __m128d * )_lhs;

	int mask = _mm_movemask_pd( lhs0 );
	return mask;
}


//--------------------------------------------------------------------------
// SSE operations

template < >
GML_FORCEINLINE void vselect< SSE, 2, double > ( double const * const _mask, double const * const _lhs, double const * const _rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _mask );
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &mask0 = *( __m128d * )_mask;
	__m128d &lhs0  = *( __m128d * )_lhs;
	__m128d &rhs0  = *( __m128d * )_rhs;
	__m128d &dst0  = *( __m128d * )_dst;

	dst0 = _mm_or_pd( _mm_and_pd( mask0, lhs0 ), _mm_andnot_pd( mask0, rhs0 ) );
}


//--------------------------------------------------------------------------

GML_NAMESPACE_END