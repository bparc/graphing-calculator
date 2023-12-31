#pragma once
#include <inttypes.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct
{
	const char* At;
} lexer_t;

typedef enum
{
	TOK_NONE,
	TOK_IDENTIFIER,
	TOK_NUMBER,
	TOK_OPERATOR,
	TOK_PAREN_OPEN,
	TOK_PAREN_CLOSE,
	TOK_END_OF_FILE,
} token_type_t;

typedef struct
{
	token_type_t Type;
	union
	{
		struct
		{
			const char* String;
			int32_t Length;
		};
		float Number;
		char Operator;
	};
} token_t;

static lexer_t CreateLexer(const char* String);
static int32_t CompareTokenIDs(token_t token, const char* string);
static int32_t RequireToken(lexer_t* lexer, token_type_t type);
static token_t GetToken(lexer_t* lexer);
static void SkipWhiteSpaces(lexer_t* lexer);
//

static int32_t IsOperator(char C)
{
	int32_t result = C == '*' ||
		C == '/' || C == '+' ||
		C == '-' || C == '^';
	return result;
}

static void SkipWhiteSpaces(lexer_t* lexer)
{
	while (*lexer->At == ' ')
	{
		lexer->At++;
	}
}

static int32_t CompareTokenIDs(token_t token, const char* string)
{
	for (int32_t index = 0; index < token.Length; index++, string++)
	{
		if ((token.String[index] != *string || (*string == 0)))
		{
			return 0;
		}
	}
	return (*string == 0);
}


static int32_t RequireToken(lexer_t* lexer, token_type_t type)
{
	int32_t result = (GetToken(lexer).Type == type);
	return result;
}

static token_t GetToken(lexer_t* lexer)
{
	token_t result = { TOK_NONE };
	SkipWhiteSpaces(lexer);
	if (isalpha(*lexer->At))
	{
		result.Type = TOK_IDENTIFIER;
		result.String = lexer->At;
		while (isalpha(*lexer->At))
		{
			result.Length++;
			lexer->At++;
		}
	}
	else
	if (isdigit(*lexer->At))
	{
		result.Type = TOK_NUMBER;
		char buffer[256] = "";
		char* at = buffer;
		while (isdigit(*lexer->At))
		{
			*at++ = *lexer->At++;
		}
		if (*lexer->At == '.')
		{
			*at++ = *lexer->At++;
			while (isdigit(*lexer->At))
			{
				*at++ = *lexer->At++;
			}
		}
		result.Number = strtof(buffer, NULL);
	}
	else
	if (IsOperator(*lexer->At))
	{
		result.Type = TOK_OPERATOR;
		result.Operator = *lexer->At++;
	}
	else
	if (*lexer->At == '(')
	{
		result.Type = TOK_PAREN_OPEN;
		lexer->At++;
	}
	else
	if (*lexer->At == ')')
	{
		result.Type = TOK_PAREN_CLOSE;
		lexer->At++;
	}
	else
	if (*lexer->At++ == 0)
	{
		result.Type = TOK_END_OF_FILE;
	}
	return result;
}

static lexer_t CreateLexer(const char* String)
{
	lexer_t result = { String };
	return result;
}