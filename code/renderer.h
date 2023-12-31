#pragma once
#include <inttypes.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include "std.h"

typedef enum
{
	UNIFORM_TYPE_V2,
	UNIFORM_TYPE_FLOAT,
} uniform_type_t;

typedef struct
{
	uniform_type_t Type;
	const char* Name;
	union
	{
		v2_t V2;
		float Float;
	};
} uniform_t;

extern uniform_t Uniform(uniform_type_t Type, const char* Name);
extern uniform_t Uniform_V2(v2_t X,const char *Name);
extern uniform_t Uniform_Float(float X,const char *Name);

typedef struct
{
	GLuint Program;
	int32_t Count;
	int32_t Offset;
	uniform_t* Uniforms;
	int32_t UniformCount;
} render_command_t;

typedef int32_t bitmap_t;
typedef struct
{
	int32_t CommandCount;
	render_command_t Commands[256];
	int32_t UniformCount;
	uniform_t UniformBuffer[256];
	GLfloat Vertices[(65536 * 2)];
	int32_t Rendering;
	int32_t Offset;
	int32_t Viewport[4];
	GLuint VBO;
	GLuint VAO;
	GLuint Program;
	GLuint CustomProgram;
	float TextureScale[2];
	GLuint TextureHandle;
	bitmap_t Font;
	int32_t MemoryInitialized;
} renderer_t;
extern GLuint CompileProgram(const char* vertexSource, const char* fragmentSource);
extern void BeginViewport(renderer_t* state, int32_t viewport[4],int32_t flags);
extern void EndViewport(renderer_t* state);
extern void OutputRenderCommands(renderer_t* state);
extern void BeginProgram(renderer_t* State, GLuint Program, uniform_t *Uniforms, int32_t UniformCount);
extern void EndProgram(renderer_t* State);
extern void RenderString(renderer_t* state, const char* string,float x, float y);
extern void RenderStringCentered(renderer_t* state, const char* string, rectangle_t Bounds);
extern void RenderRectanglef(renderer_t* state, float x, float y, float width, float height, float r, float g, float b, float a);
extern void RenderRectangleUVf(renderer_t* state, float x, float y, float width, float height, float r, float g, float b, float a,float min_uv_x,float min_uv_y,float max_uv_x,float max_uv_y);
extern void RenderRectangle(renderer_t* state, rectangle_t Bounds, float r, float g, float b, float a);
extern void RenderRectangleUV(renderer_t* state, rectangle_t Bounds, float r, float g, float b, float a, float min_uv_x, float min_uv_y, float max_uv_x, float max_uv_y);
extern void RenderRectangleUV01(renderer_t* state, rectangle_t Bounds, float r, float g, float b, float a);