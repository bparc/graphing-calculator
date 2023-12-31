#include "std.h"
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "renderer.h"
#ifdef _DEBUG
#include <stdio.h>
#endif

static GLuint LoadProgramFromFile(const char* VertPath, const char* FragPath,arena_t Arena)
{
	file_t sources[2];
	sources[0] = LoadFile(VertPath, &Arena);
	sources[1] = LoadFile(FragPath, &Arena);
	GLuint result = 0;
	if (sources[0].Size > 0 && sources[1].Size > 0)
	{
		result = CompileProgram(sources[0].Bytes, sources[1].Bytes);
	}
	assert(result);
	return result;
}

#include "gui.h"
#include "lexer.h"
typedef stack_t(token_t)token_stream_t;
static int32_t GetOperatorPrecedense(token_t A)
{
	int32_t result = 0;
	assert(A.Type == TOK_OPERATOR);
	if (A.Operator == '/' || A.Operator == '*')
	{
		result++;
	}
	return result;
}

static int32_t CompareOperatorPrecedenses(token_t A, token_t B)
{
	int32_t result = (GetOperatorPrecedense(A)<=GetOperatorPrecedense(B));
	return result;
}

static void ParseInfix(const char* String, token_stream_t*Stack,arena_t Arena)
{
	token_stream_t OperatorStack = PushStack(token_stream_t, 32, &Arena);
	lexer_t Parser = CreateLexer(String);
	Stack->Count = 0;
	while (1)
	{
		token_t Token = GetToken(&Parser);
		if (Token.Type != TOK_END_OF_FILE)
		{
			switch (Token.Type)
			{
			case TOK_NUMBER:
			{
				sPushVal(Stack, Token);
			} break;
			case TOK_OPERATOR:
			{
				while (OperatorStack.Count > 0)
				{
					if ((sTop(&OperatorStack).Type == TOK_PAREN_OPEN))
					{
						break;
					}
					else if(!CompareOperatorPrecedenses(Token, sTop(&OperatorStack)))
					{
						break;
					}
					else
					{
						sPushVal(Stack, sPop(&OperatorStack));
					}
				}
				sPushVal(&OperatorStack, Token);
			} break;
			case TOK_PAREN_OPEN:
			{
				sPushVal(&OperatorStack, Token);
			} break;
			case TOK_PAREN_CLOSE:
			{
				while (OperatorStack.Count>0)
				{
					if (sTop(&OperatorStack).Type != TOK_PAREN_OPEN)
					{
						sPushVal(Stack, sPop(&OperatorStack));
					}
					else
					{
						break;
					}
				}
				assert((sTop(&OperatorStack).Type == TOK_PAREN_OPEN));
				if (OperatorStack.Count > 0)
				{
					sPop(&OperatorStack);
				}
			} break;
			default:
			{
				assert(0);
			} break;
			}
		}
		else
		{
			break;
		}
	}
	while (OperatorStack.Count > 0)
	{
		sPushVal(Stack, sPop(&OperatorStack));
	}
	return;
}

static float DoMathOp(char Operator, float A, float B)
{
	switch (Operator)
	{
	case '+':
	{
		A += B;
	} break;
	case '-':
	{
		A -= B;
	} break;
	case '*':
	{
		A *= B;
	} break;
	case '/':
	{
		A /= B;
	} break;
	default:
	{
		assert(0);
	} break;
	};
	float result = A;
	return result;
}

static float Evaluate(token_stream_t *Tokens, arena_t Arena)
{
	float_stack_t Numbers = PushStack(float_stack_t, 256, &Arena);
	for (int32_t index = 0; index < Tokens->Count; index++)
	{
		token_t* Token = (Tokens->Items + index);
		switch (Token->Type)
		{
		case TOK_OPERATOR:
		{
			float A = sPop(&Numbers);
			float B = sPop(&Numbers);
			sPushVal(&Numbers, DoMathOp(Token->Operator, B, A));
		} break;
		case TOK_NUMBER:
		{
			sPushVal(&Numbers, Token->Number);
		} break;
		default:
		{
			assert(0);
		} break;
		}
	}
	assert(Numbers.Count == 1);
	float result = sTop(&Numbers);
	return result;
}

typedef struct
{
	int32_t Initialized;
	v2_t CameraOffset;
	GLuint Programs[2];
	string_buffer_t Buffer;
	token_stream_t Tokens;
} client_state_t;

static void ParseLn(client_state_t *State, string_buffer_t InputField, arena_t Arena)
{
	ParseInfix(InputField.Items, &State->Tokens,Arena);
}

#include "host.h"
static input_t SDL_Input(double* Time);

int32_t main(void)
{
	double Time = 0.0;
	int32_t Running = 0;
	SDL_GLContext* Context = NULL;
	SDL_Window* Window = SDL_CreateWindow("Project1.exe", SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 600, 400, SDL_WINDOW_OPENGL);
	arena_t Memory = AllocateArena(MB(4));
	if (Window && Memory.Base)
	{
		SDL_SetWindowResizable(Window, SDL_FALSE);
		Context = SDL_GL_CreateContext(Window);
		if (Context && (glewInit()==GLEW_OK))
		{
			client_state_t* ClientState = PushStruct(&Memory, client_state_t);
			renderer_t* Renderer = PushStruct(&Memory, renderer_t);
			gui_t* GUI = PushStruct(&Memory, gui_t);
			Running = TRUE;
			while (Running)
			{
				// NOTE(): Events
				{
					SDL_Event Event;
					while (SDL_PollEvent(&Event))
					{
						if (Event.type == SDL_QUIT)
						{
							Running = 0;
						}
					}
				}
				input_t Input = SDL_Input(&Time);
				Host(Input, ClientState, Renderer, GUI, &Memory);
				// NOTE(): OpenGL
				glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				OutputRenderCommands(Renderer);
				SDL_GL_SwapWindow(Window);
			}
		}
	}
	return 0;
}

static input_t SDL_Input(double* Time)
{
	input_t result = { 0 };
	double prev_time = *Time;
	*Time = (double)SDL_GetPerformanceCounter() / (double)SDL_GetPerformanceFrequency();
	result.dT = (float)(*Time - prev_time);
	int x;
	int y;
	Uint32 buttons = SDL_GetMouseState(&x, &y);
	result.Cursor[0] = (float)x;
	result.Cursor[1] = (float)y;
	result.Buttons[0] = buttons & SDL_BUTTON_LEFT;
	return result;
}