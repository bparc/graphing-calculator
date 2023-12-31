#include "std.h"
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

extern void ZeroMemory(void* Source, size_t Size)
{
	memset(Source, 0, Size);
}

extern arena_t AllocateArena(size_t RequestedMemory)
{
	arena_t result;
	result.At = (result.Base = malloc(RequestedMemory));
	result.End = result.At + RequestedMemory;
	return result;
}

extern void FreeArena(arena_t* Arena)
{
	free(Arena->Base);
}

extern void* PushBytes(arena_t* Arena, size_t Size)
{
	void* result = 0;
	assert(Arena->At + Size <= Arena->End);
	result = Arena->At;
	Arena->At += Size;
	return result;
}

extern void* PushZeroBytes(arena_t* Arena, size_t Size)
{
	void* result = PushBytes(Arena,Size);
	ZeroMemory(result, Size);
	return result;
}

extern void ClearString(string_buffer_t* Buffer)
{
	Buffer->Count = 0;
	Buffer->Items[0] = 0;
}

extern void SetString(string_buffer_t* Buffer, const char* String)
{
	while (*String && Buffer->Count < Buffer->Capacity)
	{
		sPushVal(Buffer, *String++);
	}
}

extern void PopChar(string_buffer_t* Buffer)
{
	if (Buffer->Count > 0)
	{
		sPop(Buffer);
		Buffer->Items[Buffer->Count] = 0;
	}
}

extern void PushChar(string_buffer_t* Buffer, char Ch)
{
	if ((Buffer->Count + 1) < Buffer->Capacity)
	{
		sPushVal(Buffer, Ch);
		Buffer->Items[Buffer->Count] = 0;
	}
}

extern int32_t CountString(const char* String)
{
	int32_t result = 0;
	while (*String++)
	{
		result++;
	}
	return result;
}

extern file_t LoadFile(const char* Path,arena_t *Arena)
{
	file_t result = { 0 };
	FILE* file = fopen(Path, "rb");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		result.Size = ftell(file);
		fseek(file, 0, SEEK_SET);
		if (result.Size>0)
		{
			result.Bytes = PushBytes(Arena, (result.Size + 1));
			result.Bytes[result.Size] = '\0';
			if (fread(result.Bytes,result.Size,1,file)==1)
			{

			}
			else
			{
				ZeroStruct(&result);
			}
		}
		fclose(file);
	}
	return result;
}

extern float MaxF(float X, float Y)
{
	float result = X;
	if (Y > X)
	{
		result = Y;
	}
	return result;
}

extern v4_t V4(float x, float y, float z, float w)
{
	v4_t result = { x, y, z, w };
	return result;
}

extern v2_t Add(v2_t A, v2_t B)
{
	v2_t result = { A.x + B.x,A.y + B.y };
	return result;
}

extern v2_t Sub(v2_t A, v2_t B)
{
	v2_t result = { A.x - B.x,A.y - B.y };
	return result;
}

extern v2_t Mul(v2_t A, v2_t B)
{
	v2_t result = { A.x * B.x,A.y * B.y };
	return result;
}

extern v2_t MulS(v2_t A, float Scalar)
{
	v2_t result = { A.x * Scalar,A.y * Scalar };
	return result;
}

extern v4_t MulS4(v4_t A, float Scalar)
{
	v4_t result = { A.x * Scalar, A.y * Scalar, A.z * Scalar, A.w * Scalar };
	return result;
}

extern float Lerp(float A, float B, float T)
{
	float result = A;
	if (T <= 0.0f)
	{
		result = A;
	}
	else
	if (T > 1.0f)
	{
		result = B;
	}
	else
	{
		result = ((A * (1.0f - T)) + (B * T));
	}
	return result;
}

extern v2_t Lerp2(v2_t A, v2_t B, float T)
{
	v2_t result = A;
	result.x = Lerp(A.x, B.x, T);
	result.y = Lerp(A.y, B.y, T);
	return result;
}

extern v4_t Lerp4(v4_t A, v4_t B, float T)
{
	v4_t result = A;
	result.xy = Lerp2(A.xy, B.xy, T);
	result.zw = Lerp2(A.zw, B.zw, T);
	return result;
}

extern int32_t IsPointInRectangle(rectangle_t Bounds, v2_t Point)
{
	int32_t result = (Point.x >= Bounds.X && Point.x <= Bounds.X + Bounds.W) &&
		(Point.y >= Bounds.Y && Point.y <= Bounds.Y + Bounds.H);
	return result;
} 