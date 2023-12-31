#pragma once
static void Host(input_t Input, client_state_t* State, renderer_t* Renderer, gui_t* GUI, arena_t* Memory)
{
	// NOTE(): Initialization
	if (State->Initialized <= 0)
	{
		State->Programs[0] = LoadProgramFromFile("Shaders/default.vert", "Shaders/Button.frag", *Memory);
		State->Programs[1] = LoadProgramFromFile("Shaders/default.vert", "Shaders/Graph.frag", *Memory);
		State->Buffer = PushStack(string_buffer_t, 512, Memory);
		State->Tokens = PushStack(token_stream_t, 256, Memory);
#if _DEBUG
		SetString(&State->Buffer, "2+4/5*(5-3)");
#endif
		State->Initialized = TRUE;
	}
	// NOTE(): Rendering
	v2_t Viewport = { 600.0f,400.0f };
	BeginViewport(Renderer, (int32_t[4]) { 0, 0, (int32_t)Viewport.x, (int32_t)Viewport.y }, 0);
	// NOTE(): GUI
	if (BeginGUI(GUI, Input))
	{
		rectangle_t Panel = { 0.0f,0.0f,140.0f,Viewport.y };
		// NOTE(): Graph
		{
			rectangle_t Bounds = { Panel.W ,0.0f,Viewport.x - Panel.W,Panel.H };
			if (DragArena(GUI,Bounds,ID_GRAPHVIEW))
			{
				State-> CameraOffset = Add(State->CameraOffset, MulS(GUI->CursorDelta, 0.001f));
			}
			{
				uniform_t uniforms[] =
				{
					Uniform_V2(State->CameraOffset, "CameraOffset"),
					//Uniform_V2(bounds.Size, "Resolution"),
				};
				BeginProgram(Renderer, State->Programs[1], uniforms, ArraySize(uniforms));
				RenderRectangleUV01(Renderer, Bounds, 0.95f, 0.95f, 0.95f, 1.0f);
				EndProgram(Renderer);
			}
		}
		// NOTE(): Console
		if (InputField(GUI, Renderer, Panel.W, Panel.H - 14.0f, Viewport.x - Panel.W, 14.0f, &State->Buffer, ID_INPUT_FIELD))
		{
			ParseLn(State, State->Buffer, *Memory);
		}
		// NOTE(): Panel
		RenderRectangle(Renderer, Panel, 0.12f, 0.12f, 0.12f, 1.0f); // NOTE(): Background
		RenderString(Renderer, "Graphing", 4.0f, 4.0f);
		BeginProgram(Renderer, State->Programs[0], 0, 0);
		SetItemSize(GUI, Panel.W / (float)4, 40.0f);
		BeginRows(GUI, 4, Panel.X, (Panel.Y + (Panel.H - GUI->ItemSize.y * 5.0f)));
		if (Button(GUI, Renderer, "DEL", ID_DEL))
		{
			PopChar(&State->Buffer);
		}
		Button(GUI, Renderer, "?", ID_NULL0);
		Button(GUI, Renderer, "?", ID_NULL1);
		if (Button(GUI, Renderer, "CLS", ID_CLS))
		{
			ClearString(&State->Buffer);
		}
		NumPad(GUI, Renderer, &State->Buffer, "(", ID_PAREN_OPEN);
		NumPad(GUI, Renderer, &State->Buffer, ")", ID_PAREN_CLOSE);
		if (Button(GUI, Renderer, "=", ID_EQUALS))
		{
			ParseLn(State, State->Buffer, *Memory);
			ClearString(&State->Buffer);
			sprintf(State->Buffer.Items, "%f", Evaluate(&State->Tokens,*Memory));
		}
		NumPad(GUI, Renderer, &State->Buffer, "/", ID_DIV);
		NumPad(GUI, Renderer, &State->Buffer, "7", ID_NUM7);
		NumPad(GUI, Renderer, &State->Buffer, "8", ID_NUM8);
		NumPad(GUI, Renderer, &State->Buffer, "9", ID_NUM9);
		NumPadLabeled(GUI, Renderer, &State->Buffer, '*', "X", ID_MUL);
		NumPad(GUI, Renderer, &State->Buffer, "3", ID_NUM3);
		NumPad(GUI, Renderer, &State->Buffer, "4", ID_NUM4);
		NumPad(GUI, Renderer, &State->Buffer, "5", ID_NUM5);
		NumPad(GUI, Renderer, &State->Buffer, "-", ID_SUB);
		NumPad(GUI, Renderer, &State->Buffer, "0", ID_NUM0);
		NumPad(GUI, Renderer, &State->Buffer, "1", ID_NUM1);
		NumPad(GUI, Renderer, &State->Buffer, "2", ID_NUM2);
		NumPad(GUI, Renderer, &State->Buffer, "+", ID_ADD);
		EndProgram(Renderer);
		EndRows(GUI);

		EndGUI(GUI);
	}
	EndViewport(Renderer);
}
