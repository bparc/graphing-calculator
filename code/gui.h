#pragma once
enum
{
	ID_NULL,
	ID_NUM0,
	ID_NUM1,
	ID_NUM2,
	ID_NUM3,
	ID_NUM4,
	ID_NUM5,
	ID_NUM6,
	ID_NUM7,
	ID_NUM8,
	ID_NUM9,
	ID_MUL,
	ID_DIV,
	ID_ADD,
	ID_SUB,
	ID_EQUALS,
	ID_PAREN_OPEN,
	ID_PAREN_CLOSE,
	ID_INPUT_FIELD,
	ID_GRAPHVIEW,
	ID_NULL0,
	ID_NULL1,
	ID_NULL2,
	ID_CLS,
	ID_DEL,
	ID_MODE_SWITCH,
	ID_COUNT,
} gui_id_t;

typedef struct
{
	union
	{
		float Cursor[2];
		v2_t CursorPosition;
	};
	int32_t Buttons[3];
	float dT;
} input_t;

struct gui_temp_state_t
{
	int32_t Interact;
	int32_t Hovered;
};

typedef struct
{
	union
	{
		struct gui_temp_state_t Temp;
		struct gui_temp_state_t;
	};

	int32_t Active;
	float ButtonColors[ID_COUNT];
	input_t Input;
	v2_t CursorDelta;
	int32_t ItemsPerRow;
	v2_t ItemSize;
	int32_t ItemIndex;
	v2_t Cursor;
} gui_t;

static void BeginRows(gui_t* State, int32_t Count, float X, float Y)
{
	State->ItemsPerRow = Count;
	State->ItemIndex = 0;
	State->Cursor = (v2_t){ X,Y };
}

static void EndRows(gui_t* State)
{

}

static void SetItemSize(gui_t* State, float X, float Y)
{
	State->ItemSize = (v2_t){ X,Y };
}

static int32_t BeginGUI(gui_t* State, input_t Input)
{
	int32_t result = TRUE;
	{
		ZeroStruct(&State->Temp);
	}
	// NOTE(): Update State->Input
	{
		State->Interact = ((State->Input.Buttons[0] == 0) && (Input.Buttons[0] > 0));
		State->CursorDelta = Sub(State->Input.CursorPosition, Input.CursorPosition);
		State->Input = Input;
	}
	return result;
}
static void EndGUI(gui_t* State)
{

}

static int32_t IsActive(gui_t* State, int32_t ID)
{
	int32_t result = State->Active == ID;
	return result;
}

static int32_t Interact(gui_t* State, rectangle_t Bounds, int32_t ID, int32_t Flags)
{
	int32_t result = 0;
	if ((State->Active == 0))
	{
		if (IsPointInRectangle(Bounds, State->Input.CursorPosition))
		{
			State->Hovered = ID;
			if (State->Interact)
			{
				State->Active = ID;
			}
		}
	}
	if ((State->Active == ID))
	{
		if (!State->Input.Buttons[0])
		{
			result = IsPointInRectangle(Bounds, State->Input.CursorPosition);
			State->Active = 0;
		}
	}
	return result;
}

static int32_t DragArena(gui_t* State, rectangle_t Bounds, int32_t ID)
{
	Interact(State, Bounds, ID, 0);
	int32_t result = IsActive(State, ID);
	return result;
}

static int32_t ButtonEx(gui_t* State, renderer_t* Renderer, float X, float Y, float Width, float Height, const char* Label, int32_t ID)
{
	rectangle_t bounds = { X,Y,Width,Height };
	int32_t result = Interact(State, bounds, ID, 0);
	float* Light = &State->ButtonColors[ID];
	*Light = MaxF(*Light - State->Input.dT * 2.0f, 0.0f);
	v4_t Color = { 0.15f,0.15f,0.15f };
	Color = Lerp4(Color, MulS4(Color, 1.8f), *Light);
	if ((State->Hovered == ID))
	{
		*Light = 1.0f;
	}
	if (IsActive(State, ID))
	{
		Color = V4(0.0f, 89.0f / 255.0f, 1.0f, 0.0f);
	}
	RenderRectangleUV01(Renderer, bounds, Color.x, Color.y, Color.z, 0.0f);
	RenderStringCentered(Renderer, Label, bounds);
	return result;
}

static int32_t Button(gui_t* State, renderer_t* Renderer, const char* Label, int32_t ID)
{
	int32_t result = ButtonEx(State, Renderer, State->Cursor.x, State->Cursor.y, State->ItemSize.x, State->ItemSize.y, Label, ID);
	State->Cursor.x += State->ItemSize.x;
	if (++State->ItemIndex >= State->ItemsPerRow)
	{
		State->ItemIndex = 0;
		State->Cursor.x = 0.0f;
		State->Cursor.y += State->ItemSize.y;
	}
	return result;
}

static int32_t NumPadLabeled(gui_t* State, renderer_t* Renderer, string_buffer_t* Output, char Input, const char* Label, int32_t ID)
{
	int32_t result = Button(State, Renderer, Label, ID);
	if (result)
	{
		if (Output->Count < Output->Capacity)
		{
			PushChar(Output, Input);
		}
	}
	return result;
}

static int32_t NumPad(gui_t* State, renderer_t* Renderer, string_buffer_t* Output, const char* Label, int32_t ID)
{
	int32_t result = NumPadLabeled(State, Renderer, Output, Label[0], Label, ID);
	return result;
}

static int32_t InputField(gui_t* State, renderer_t* Renderer, float X, float Y, float Width, float Height, string_buffer_t* Buffer, int32_t ID)
{
	int32_t result = 0;
	rectangle_t bounds = { X,Y,Width,Height };
	RenderRectangle(Renderer, bounds, 0.0f, 0.0f, 0.0f, 1.0f);

	X += 4.0f;
	Y += 4.0f;
	RenderString(Renderer, ">", X, Y);
	X += 10.0f;
	RenderString(Renderer, Buffer->Items, X, Y);
	return result;
}
