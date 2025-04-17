#ifndef DEFINES__H
#define DEFINES__H

#include "TVector2.h"

typedef			 char Int8;
typedef unsigned char Uint8;

typedef signed   short Int16;
typedef unsigned short Uint16;

typedef signed   int Int32;
typedef unsigned int Uint32;

typedef signed   __int64 Int64;
typedef unsigned __int64 Uint64;

typedef Engine::Vector2<Int32> Vector2i;
typedef Engine::Vector2<Uint32> Vector2ui;

typedef Engine::Vector2<Int64> Vector2f;
typedef Engine::Vector2<Uint64> Vector2uf;

#endif // !DEFINES__H
