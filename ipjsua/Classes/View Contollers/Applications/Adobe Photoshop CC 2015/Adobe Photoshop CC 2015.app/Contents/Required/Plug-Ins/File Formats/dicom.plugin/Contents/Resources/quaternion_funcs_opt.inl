
#if GML_SUPPORT_SSE

GML_NAMESPACE_BEGIN

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//                   QUATERNION FLOAT OPT
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------


template < >
GML_FORCEINLINE void qconj< SSE, float > ( float const * const _lhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_mul_ps( lhs, _mm_set_ps( 1, -1, -1, -1 ) );
}


template < >
GML_FORCEINLINE void qadd< SSE, float > ( float const * const _lhs, float rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_add_ps( lhs, _mm_set_ps( rhs, 0, 0, 0 ) );
}


template < >
GML_FORCEINLINE void qsub< SSE, float > ( float const * const _lhs, float rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &dst = *( __m128 * )_dst;

	dst = _mm_sub_ps( lhs, _mm_set_ps( rhs, 0, 0, 0 ) );
}


template < >
GML_FORCEINLINE void qmul< SSE, float > ( float const * const _lhs, float const * const _rhs, float * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _rhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128 &lhs = *( __m128 * )_lhs;
	__m128 &rhs = *( __m128 * )_rhs;
	__m128 &dst = *( __m128 * )_dst;

	__m128 tmp;
	tmp = _mm_mul_ps( _mm_shuffle_ps( lhs, lhs, _MM_SHUFFLE( 0, 2, 1, 0 ) ),
		_mm_shuffle_ps( rhs, rhs, _MM_SHUFFLE( 0, 3, 3, 3 ) ) );
	tmp = _mm_add_ps( tmp,
		_mm_mul_ps( _mm_shuffle_ps( lhs, lhs, _MM_SHUFFLE( 1, 0, 2, 1 ) ),
		_mm_shuffle_ps( rhs, rhs, _MM_SHUFFLE( 1, 1, 0, 2 ) ) )
		);

	dst = _mm_mul_ps( _mm_shuffle_ps( lhs, lhs, _MM_SHUFFLE( 3, 3, 3, 3 ) ), rhs );
	dst = _mm_add_ps( dst, _mm_mul_ps( tmp, _mm_set_ps( -1, 1, 1, 1 ) ) );
	dst = _mm_sub_ps( dst, 
		_mm_mul_ps( _mm_shuffle_ps( lhs, lhs, _MM_SHUFFLE( 2, 1, 0, 2 ) ),
		_mm_shuffle_ps( rhs, rhs, _MM_SHUFFLE( 2, 0, 2, 1 ) )  )
		);
}


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//                   QUATERNION DOUBLE OPT
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

#if GML_SSE_VER >= GML_SSE2


template < >
GML_FORCEINLINE void qconj< SSE, double > ( double const * const _lhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	dst0 = _mm_mul_pd( lhs0, _mm_set_pd( -1, -1 ) );
	dst1 = _mm_mul_pd( lhs1, _mm_set_pd(  1, -1 ) );
}


template < >
GML_FORCEINLINE void qadd< SSE, double > ( double const * const _lhs, double rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	dst0 = lhs0;
	dst1 = _mm_add_pd( lhs1, _mm_set_pd( rhs, 0 ) );
}


template < >
GML_FORCEINLINE void qsub< SSE, double > ( double const * const _lhs, double rhs, double * const _dst )
{
	GML_SSE_CHECK_ALIGN( _lhs );
	GML_SSE_CHECK_ALIGN( _dst );

	__m128d &lhs0 = *( __m128d * )_lhs;
	__m128d &lhs1 = *( __m128d * )( _lhs + 2 );
	__m128d &dst0 = *( __m128d * )_dst;
	__m128d &dst1 = *( __m128d * )( _dst + 2 );

	dst0 = lhs0;
	dst1 = _mm_sub_pd( lhs1, _mm_set_pd( rhs, 0 ) );
}


template < >
GML_INLINE void qmul< SSE, double > ( double const * const _lhs, double const * const _rhs, double * const _dst )
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

	__m128d tmp1, tmp2;

	tmp1 = _mm_shuffle_pd( lhs1, lhs1, _MM_SHUFFLE2( 1, 1 ) );
	dst0 = _mm_mul_pd( tmp1, rhs0 );
	dst1 = _mm_mul_pd( tmp1, rhs1 );

	dst0 = _mm_add_pd( dst0, _mm_mul_pd( lhs0, _mm_shuffle_pd( rhs1, rhs1, _MM_SHUFFLE2( 1, 1 ) ) ) );
	tmp2 = _mm_mul_pd( lhs0, _mm_shuffle_pd( rhs0, rhs0, _MM_SHUFFLE2( 1, 1 ) ) );

	tmp1 = _mm_shuffle_pd( lhs1, lhs0, _MM_SHUFFLE2( 0, 0 ) );
	dst0 = _mm_sub_pd( dst0, _mm_mul_pd( tmp1, _mm_shuffle_pd( rhs0, rhs1, _MM_SHUFFLE2( 0, 1 ) ) ) );
	tmp2 = _mm_add_pd( tmp2, _mm_mul_pd( tmp1, _mm_shuffle_pd( rhs1, rhs0, _MM_SHUFFLE2( 0, 1 ) ) ) );

	dst1 = _mm_add_pd( dst1, _mm_mul_pd( tmp2, _mm_set_pd( -1, 1 ) ) );

	tmp1 = _mm_shuffle_pd( lhs0, lhs1, _MM_SHUFFLE2( 0, 1 ) );
	dst0 = _mm_add_pd( dst0, _mm_mul_pd( tmp1, _mm_shuffle_pd( rhs1, rhs0, _MM_SHUFFLE2( 0, 0 ) ) ) );
	dst1 = _mm_sub_pd( dst1, _mm_mul_pd( tmp1, _mm_shuffle_pd( rhs0, rhs1, _MM_SHUFFLE2( 0, 0 ) ) ) );
}


//--------------------------------------------------------------------------

#endif // GML_SSE_VER >= GML_SSE2

GML_NAMESPACE_END

#endif // GML_SUPPORT_SSE
