#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES 1
#endif
#include <math.h> 


GML_NAMESPACE_BEGIN


#if GML_SUPPORT_STL
#else

template < typename T >
const T & gml_max ( const T &a, const T &b )
{
	return ( a < b ) ? b : a;
}


template < typename T >
const T & gml_min ( const T &a, const T &b )
{
	return ( a < b ) ? a : b;
}

#endif // GML_SUPPORT_STL


//---------------------------------------------------------------
// epsilon specializations

template < >
GML_FORCEINLINE float epsilon< float > ()
{
	return 1.e-6f;
}


template < >
GML_FORCEINLINE double epsilon< double > ()
{
	return 1.e-12;
}


template < >
GML_FORCEINLINE int epsilon< int > ()
{
	return 0;
}


//---------------------------------------------------------------
// abs specializations

template < >
GML_FORCEINLINE float abs< float > ( const float &x )
{
	return ::fabsf( x );
}

template < >
GML_FORCEINLINE double abs< double > ( const double &x )
{
	return ::fabs( x );
}

template < >
GML_FORCEINLINE int abs< int > ( const int &x )
{
	return ::abs( x );
}


//---------------------------------------------------------------
// sqrt specializations

template < >
GML_FORCEINLINE float sqrt< float > ( const float & x )
{
	return ::sqrtf( x );
}


template < >
GML_FORCEINLINE double sqrt< double > ( const double & x )
{
	return ::sqrt( x );
}


//---------------------------------------------------------------
// acos specializations

template < >
GML_FORCEINLINE float acos< float > ( const float & x )
{
	return acosf( x );
}


template < >
GML_FORCEINLINE double acos< double > ( const double & x )
{
	return ::acos( x );
}


//---------------------------------------------------------------
//

template < >
GML_FORCEINLINE float cos< float > ( const float & x )
{
	return cosf( x );
}


template < >
GML_FORCEINLINE double cos< double > ( const double & x )
{
	return ::cos( x );
}


//---------------------------------------------------------------
//

template < >
GML_FORCEINLINE float sin< float > ( const float & x )
{
	return sinf( x );
}


template < >
GML_FORCEINLINE double sin< double > ( const double & x )
{
	return ::sin( x );
}


//---------------------------------------------------------------
// other functions

template < >
GML_FORCEINLINE float deg2rad< float > ( const float & x )
{
	return ( x * ( float )M_PI / 180.0f );
}


template < >
GML_FORCEINLINE double deg2rad< double > ( const double & x )
{
	return ( x * M_PI / 180.0 );
}


template < >
GML_FORCEINLINE float rad2deg< float > ( const float & x )
{
	return ( x * 180.0f / ( float )M_PI );
}


template < >
GML_FORCEINLINE double rad2deg< double > ( const double & x )
{
	return ( x * 180.0 / M_PI );
}


//---------------------------------------------------------------
// other functions

template < typename T, typename S >
GML_FORCEINLINE T clamp ( T x, S a, S b )
{
	return ( T )( ( x < a ) ? a : ( ( x > b ) ? b : x ) );
}


template < typename T, typename S >
GML_FORCEINLINE T lerp ( const T &a, const T &b, const S &alpha )
{
	return ( a * ( S( 1.0 ) - alpha ) + b * alpha );
}


template < typename T >
GML_FORCEINLINE bool realeq ( const T &lhs, const T &rhs )
{
	return realeq< T >( lhs, rhs, epsilon< T >() );
}


template < typename T >
GML_FORCEINLINE bool realeq ( const T &lhs, const T &rhs, T eps )
{
	return ( abs< T >( rhs - lhs ) <= eps );
}


template < typename T >
GML_FORCEINLINE T uniform_random ()
{
	return uniform_random< T >( T( 0 ), T( 1 ) );
}


template < typename T >
GML_FORCEINLINE T uniform_random ( T min, T max )
{
	return ( T )( rand() * ( max - min ) / RAND_MAX + min );
}


template < typename T >
GML_FORCEINLINE T allbitson ()
{
	static const long long on = ~0;
	return *( T * )&on;
}


//---------------------------------------------------------------
// signbit specializations

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif

template < typename T >
GML_FORCEINLINE int signbittoint ( const T &x )
{
	return ( x >> ( sizeof( T ) * CHAR_BIT - 1 ) ) & 0x1;
}


template < >
GML_FORCEINLINE int signbittoint< float > ( const float &x )
{
	return ( ( ( int & )x ) >> 31 ) & 0x1;
}


template < >
GML_FORCEINLINE int signbittoint< double > ( const double &x )
{
	return ( int )( ( ( ( long long & )x ) >> 63 ) & 0x1 );
}

/*
template < >
GML_FORCEINLINE float signval ( const float &x )
{
	const float _one = 1.0f;
	const float &one = _one;
	return ( float )( ( ( int & )one ) | ( ( ( int & )x ) & 0x70000000 ) );
}


template < >
GML_FORCEINLINE double signval ( const double &x )
{
	const double _one = 1.0;
	const double &one = _one;
	return ( double )( ( ( long long & )one ) | ( ( ( long long & )x ) & 0x7000000000000000 ) );
}


template < >
GML_FORCEINLINE int signval ( const int &x )
{
	const int one = 1;
	return ( one | ( x & 0x70000000 ) );
}
*/

//---------------------------------------------------------------
// opengl wrapper specializations

#if GML_SUPPORT_OPENGL

template < >
GML_FORCEINLINE void glGetvTmpl< GLfloat > ( GLenum pname, GLfloat *params )
{
	glGetFloatv( pname, params );
}


template < >
GML_FORCEINLINE void glGetvTmpl< GLdouble > ( GLenum pname, GLdouble *params )
{
	glGetDoublev( pname, params );
}


template < >
GML_FORCEINLINE void glGetvTmpl< GLboolean > ( GLenum pname, GLboolean *params )
{
	glGetBooleanv( pname, params );
}


template < >
GML_FORCEINLINE void glGetvTmpl< GLint > ( GLenum pname, GLint *params )
{
	glGetIntegerv( pname, params );
}


template < >
GML_FORCEINLINE void glLoadMatrixTmpl< GLfloat > ( const GLfloat *m )
{
	glLoadMatrixf( m );
}


template < >
GML_FORCEINLINE void glLoadMatrixTmpl< GLdouble > ( const GLdouble *m )
{
	glLoadMatrixd( m );
}


template < >
GML_FORCEINLINE void glMultMatrixTmpl< GLfloat > ( const GLfloat *m )
{
	glMultMatrixf( m );
}


template < >
GML_FORCEINLINE void glMultMatrixTmpl< GLdouble > ( const GLdouble *m )
{
	glMultMatrixd( m );
}


template < >
GML_FORCEINLINE void glRotateTmpl< GLfloat > ( GLfloat angle, GLfloat x, 
											  GLfloat y, GLfloat z )
{
	glRotatef( angle, x, y, z );
}


template < >
GML_FORCEINLINE void glRotateTmpl< GLdouble > ( GLdouble angle, GLdouble x, 
											  GLdouble y, GLdouble z )
{
	glRotated( angle, x, y, z );
}


template < >
GML_FORCEINLINE void glTranslateTmpl< GLfloat > ( GLfloat x, GLfloat y, GLfloat z )
{
	glTranslatef( x, y, z );
}


template < >
GML_FORCEINLINE void glTranslateTmpl< GLdouble > ( GLdouble x, GLdouble y, GLdouble z )
{
	glTranslated( x, y, z );
}


template < >
GML_FORCEINLINE void glTranslateTmpl< GLfloat > ( const GLfloat *v )
{
	glTranslatef( v[ 0 ], v[ 1 ], v[ 2 ] );
}


template < >
GML_FORCEINLINE void glTranslateTmpl< GLdouble > ( const GLdouble *v )
{
	glTranslated( v[ 0 ], v[ 1 ], v[ 2 ] );
}


template < >
GML_FORCEINLINE void glScaleTmpl< GLfloat > ( GLfloat x, GLfloat y, GLfloat z )
{
	glScalef( x, y, z );
}


template < >
GML_FORCEINLINE void glScaleTmpl< GLdouble > ( GLdouble x, GLdouble y, GLdouble z )
{
	glScaled( x, y, z );
}


template < >
GML_FORCEINLINE void glScaleTmpl< GLfloat > ( const GLfloat *v )
{
	glScalef( v[ 0 ], v[ 1 ], v[ 2 ] );
}


template < >
GML_FORCEINLINE void glScaleTmpl< GLdouble > ( const GLdouble *v )
{
	glScaled( v[ 0 ], v[ 1 ], v[ 2 ] );
}


#define GL_VERTEX_TMPL( N, C, T ) \
template < > \
GML_FORCEINLINE void glVertexTmpl< N, T > ( const T *v ) \
{ \
	glVertex##N##C##v( v ); \
}

GL_VERTEX_TMPL( 2, d, GLdouble )
GL_VERTEX_TMPL( 2, f, GLfloat  )
GL_VERTEX_TMPL( 2, i, GLint    )
GL_VERTEX_TMPL( 2, s, GLshort  )
GL_VERTEX_TMPL( 3, d, GLdouble )
GL_VERTEX_TMPL( 3, f, GLfloat  )
GL_VERTEX_TMPL( 3, i, GLint    )
GL_VERTEX_TMPL( 3, s, GLshort  )
GL_VERTEX_TMPL( 4, d, GLdouble )
GL_VERTEX_TMPL( 4, f, GLfloat  )
GL_VERTEX_TMPL( 4, i, GLint    )
GL_VERTEX_TMPL( 4, s, GLshort  )

#undef GL_VERTEX_TMPL


#define GL_NORMAL_TMPL( C, T ) \
template < > \
GML_FORCEINLINE void glNormalTmpl< 3, T > ( const T *v ) \
{ \
	glNormal3##C##v( v ); \
}

GL_NORMAL_TMPL( b, GLbyte   )
GL_NORMAL_TMPL( d, GLdouble )
GL_NORMAL_TMPL( f, GLfloat  )
GL_NORMAL_TMPL( i, GLint    )
GL_NORMAL_TMPL( s, GLshort  )

#undef GL_NORMAL_TMPL


#define GL_COLOR_TMPL( N, C, T ) \
template < > \
GML_FORCEINLINE void glColorTmpl< N, T > ( const T *v ) \
{ \
	glColor##N##C##v( v ); \
}

GL_COLOR_TMPL( 3,  b, GLbyte   )
GL_COLOR_TMPL( 3,  d, GLdouble )
GL_COLOR_TMPL( 3,  f, GLfloat  )
GL_COLOR_TMPL( 3,  i, GLint    )
GL_COLOR_TMPL( 3,  s, GLshort  )
GL_COLOR_TMPL( 3, ub, GLubyte  )
GL_COLOR_TMPL( 3, ui, GLuint   )
GL_COLOR_TMPL( 3, us, GLushort )
GL_COLOR_TMPL( 4,  b, GLbyte   )
GL_COLOR_TMPL( 4,  d, GLdouble )
GL_COLOR_TMPL( 4,  f, GLfloat  )
GL_COLOR_TMPL( 4,  i, GLint    )
GL_COLOR_TMPL( 4,  s, GLshort  )
GL_COLOR_TMPL( 4, ub, GLubyte  )
GL_COLOR_TMPL( 4, ui, GLuint   )
GL_COLOR_TMPL( 4, us, GLushort )

#undef GL_COLOR_TMPL


#define GL_TEXCOORD_TMPL( N, C, T ) \
template < > \
GML_FORCEINLINE void glTexCoordTmpl< N, T > ( const T *v ) \
{ \
	glTexCoord##N##C##v( v ); \
}

GL_TEXCOORD_TMPL( 1, d, GLdouble )
GL_TEXCOORD_TMPL( 1, f, GLfloat  )
GL_TEXCOORD_TMPL( 1, i, GLint    )
GL_TEXCOORD_TMPL( 1, s, GLshort  )
GL_TEXCOORD_TMPL( 2, d, GLdouble )
GL_TEXCOORD_TMPL( 2, f, GLfloat  )
GL_TEXCOORD_TMPL( 2, i, GLint    )
GL_TEXCOORD_TMPL( 2, s, GLshort  )
GL_TEXCOORD_TMPL( 3, d, GLdouble )
GL_TEXCOORD_TMPL( 3, f, GLfloat  )
GL_TEXCOORD_TMPL( 3, i, GLint    )
GL_TEXCOORD_TMPL( 3, s, GLshort  )
GL_TEXCOORD_TMPL( 4, d, GLdouble )
GL_TEXCOORD_TMPL( 4, f, GLfloat  )
GL_TEXCOORD_TMPL( 4, i, GLint    )
GL_TEXCOORD_TMPL( 4, s, GLshort  )

#undef GL_TEXCOORD_TMPL

#endif // GML_SUPPORT_OPENGL


//---------------------------------------------------------------

GML_NAMESPACE_END
