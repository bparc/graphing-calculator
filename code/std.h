#pragma once
#include <inttypes.h>
#include <assert.h>

#ifndef TRUE
#define TRUE 1
#endif

#define KB(Bytes) (Bytes*1024)
#define MB(Bytes) (Bytes*1024*1024)
#define GB(Bytes) (Bytes*1024*1024*1024)

extern void ZeroMemory(void* Source, size_t Size);
#define ArraySize(Array) (sizeof(Array) / sizeof(*Array))
#define ZeroStruct(Struct) ZeroMemory(Struct, sizeof(*Struct))

// arena_t
typedef struct
{
	void* Base;
	uint8_t* At;
	uint8_t* End;
} arena_t;

// NOTE() Allocates a memory arena on the heap. Needs to be freed with FreeArena() call.
extern arena_t AllocateArena(size_t RequestedMemory);
extern void FreeArena(arena_t* Arena);

extern void* PushBytes(arena_t* Arena, size_t Size);
extern void* PushZeroBytes(arena_t* Arena, size_t Size);
#define PushStruct(Arena, Type) ((Type*)PushZeroBytes(Arena,sizeof(Type)))
#define PushArray(Arena,Type,Count) ((Type*)PushBytes(Arena,sizeof(Type)*Count))

// stack_t
#define stack_t(Type) struct { int32_t Count; int32_t Capacity; Type *Items;}
#define sInit(Stack,MaxCount,Memory) ((Stack)->Count = 0,(Stack)->Capacity=MaxCount,(Stack)->Items = Memory,Stack)
#define sPush(Stack) (assert((Stack)->Count<(Stack)->Capacity),&(Stack)->Items[(Stack)->Count++])
#define sPushVal(Stack,Value) (assert((Stack)->Count<(Stack)->Capacity),((Stack)->Items[(Stack)->Count++] = Value))
#define sPushCount(Stack,HowMany) (assert((Stack)->Count+HowMany<=(Stack)->Capacity),(Stack)->Count+=HowMany,&((Stack)->Items[(Stack)->Count-HowMany]))
#define sFlush(Stack) ((Stack)->Count = 0,Stack)
#define sTop(Stack) (assert((Stack)->Count>0),(Stack)->Items[(Stack)->Count-1])
#define sPop(Stack) (assert((Stack)->Count>0),(Stack)->Items[--(Stack)->Count])

#define sPushOrNull(Stack) ((Stack)->Count < (Stack)->Capacity ? &(Stack)->Items[(Stack)->Count++] : NULL)

typedef stack_t(float) float_stack_t;
typedef stack_t(int32_t) int32_stack_t;
typedef stack_t(int16_t) int16_stack_t;
typedef stack_t(int8_t) int8_stack_t;

// string_buffer_t
typedef stack_t(char) string_buffer_t;
extern void ClearString(string_buffer_t* Buffer);
extern void SetString(string_buffer_t* Buffer, const char* String);
extern void PopChar(string_buffer_t* Buffer);
extern void PushChar(string_buffer_t* Buffer,char Ch);
#define PushStack(Type,Capacity,Arena) (Type) { 0, Capacity, PushZeroBytes(Arena, Capacity * sizeof(*((Type*)0)->Items))}
extern int32_t CountString(const char* String);

// File
typedef struct
{
	uint8_t* Bytes;
	int32_t Size;
} file_t;

extern file_t LoadFile(const char *Path,arena_t *Arena);
// Math
extern float MaxF(float X, float Y);
extern float MinF(float X, float Y);
extern float ClampF(float Min, float Max, float X);

typedef struct
{
	float x;
	float y;
} v2_t;

typedef union
{
	struct
	{
		float x;
		float y;
		float z;
		float w;
	};
	struct
	{
		v2_t xy;
		v2_t zw;
	};
} v4_t;

extern v4_t V4(float x, float y, float z, float w);
extern v2_t Add(v2_t A, v2_t B);
extern v2_t Sub(v2_t A, v2_t B);
extern v2_t Mul(v2_t A, v2_t B);
extern v2_t MulS(v2_t A, float Scalar);

extern v4_t MulS4(v4_t A, float Scalar);
extern float Lerp(float A, float B, float T);
extern v2_t Lerp2(v2_t A, v2_t B, float T);
extern v4_t Lerp4(v4_t A, v4_t B, float T);

typedef union
{
	struct
	{
		float X;
		float Y;
		float W;
		float H;
	};
	struct
	{
		v2_t Position;
		v2_t Size;
	};
} rectangle_t;

extern int32_t IsPointInRectangle(rectangle_t X, v2_t Y);