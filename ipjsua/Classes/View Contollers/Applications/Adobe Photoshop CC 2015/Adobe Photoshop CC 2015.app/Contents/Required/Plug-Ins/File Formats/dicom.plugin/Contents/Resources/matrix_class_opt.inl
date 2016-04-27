
#if GML_SUPPORT_SSE

/*
GML_NAMESPACE_BEGIN

template < >
GML_FORCEINLINE void Matrix< 4, float, SSE >::setIdentity ()
{
	v[ 0 ] = _mm_set_ss( 1 );
	v[ 1 ] = _mm_shuffle_ps( v[ 0 ], v[ 0 ], _MM_SHUFFLE( 1, 1, 0, 1 ) );
	v[ 2 ] = _mm_shuffle_ps( v[ 0 ], v[ 0 ], _MM_SHUFFLE( 1, 0, 1, 1 ) );
	v[ 3 ] = _mm_shuffle_ps( v[ 0 ], v[ 0 ], _MM_SHUFFLE( 0, 1, 1, 1 ) );
}


GML_NAMESPACE_END
*/

#endif // GML_SUPPORT_SSE
