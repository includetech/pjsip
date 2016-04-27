
#if GML_SUPPORT_SSE

#if GML_SSE_VER >= GML_SSE2

GML_NAMESPACE_BEGIN

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


template < >
GML_FORCEINLINE void vset< SSE, 2, double > ( double * const _dst, double x, double y, double GML_UNUSED( z ), double GML_UNUSED( w ) )
{
	GML_SSE_CHECK_ALIGN( _dst );
	
	__m128d &dst = *( __m128d * )_dst;
	
	dst = _mm_set_pd( y, x );
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

#endif // GML_SSE_VER >= GML_SSE2

#endif // GML_SUPPORT_SSE
