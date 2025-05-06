#ifndef DEFINE__H
#define DEFINE__H

#define assert( condition ) if ( condition == false ) throw

template <typename T, unsigned char size> struct Vector;
template <typename T> using Vector2 = Vector<T, 2>;
template <typename T> using Vector3 = Vector<T, 3>;

using byte		= unsigned char;


using int8		= __int8;
using int16		= __int16;
using int32		= __int32;
using int64		= __int64;
using uint8		= unsigned __int8;
using uint16	= unsigned __int16;
using uint32	= unsigned __int32;
using uint64	= unsigned __int64;


using float32	= float;
using float64	= double;


using Vector2i	= Vector2<__int32>;
using Vector2f	= Vector2<float>;

using Vector3i	= Vector3<__int32>;
using Vector3f	= Vector3<float>;

#endif //!DEFINE__H