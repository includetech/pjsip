
#if GML_SUPPORT_SSE

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


template < >
GML_FORCEINLINE void vset< SSE, 4, float > ( float * const _dst, float x, float y, float z, float w )
{
	GML_SSE_CHECK_ALIGN( _dst );
	
	__m128 &dst = *( __m128 * )_dst;
	
	dst = _mm_set_ps( w, z, y, x );
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

	vmul< SSE, 4 >( _dst, ( max - min ) / ( float )RAND_MAX, _dst );
	vadd< SSE, 4 >( _dst, min, _dst );
}



//--------------------------------------------------------------------------

GML_NAMESPACE_END

#endif // GML_SUPPORT_SSE
