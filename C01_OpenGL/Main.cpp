#include "Main.h"

/* This function, WndProc (short for Window Procedure) is used by the Windows
*  API to handle events and messages sent to the window.
*
*  LRESULT: A platform specific return type (usually a long int)
*  CALLBACK: A calling convention that denotes this function handles async events
*  WndProc: The function name, short for Window Procedure
*  HWND hwnd: A reference or "handle" to the window.
*  UINT msg: The actual event message
*  WPARAM wParam: Extra information about the event
*  LPARAM lParam: Extra information about the event
*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//callbacks for winapi
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if (wParam == 27)
		{
			PostQuitMessage(0);
		}
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
		break;
	}
	return 0;
}

/* wWinMain is the equivalent of "int main()" but for a windows application.
*
*  HINSTANCE hInstance: A handle to the application (which is different from the window)
*  HINSTANCE hPrevInstance: A handle to the previous instance of the application (not used in modern windows)
*  LPWSTR lpCmdLine: A long pointer to a unicode string. Essentially a string containing command line params
*  int nCmdShow: An integer that defines how the window should open.
*/
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	// Initialize Window ------------------------------------------------------------------
#pragma region Window Initilization

	/* The following code essentially attaches the console back to our windows application if
	*  possible. It gives us access to the cout, cin, and error streams.
	*/
	if (AllocConsole())
	{
		FILE* stream;
		_wfreopen_s(&stream, TEXT("CONIN$"), TEXT("rb"), stdin);
		_wfreopen_s(&stream, TEXT("CONOUT$"), TEXT("wb"), stdout);
		_wfreopen_s(&stream, TEXT("CONOUT$"), TEXT("wb"), stderr);
	}

	//Set the value to the provided arguments


	WNDCLASSEX wc; //Window Information Container
	wc.cbSize = sizeof(WNDCLASSEX); //How big is our container?
	wc.style = 0; //What is the style? Style 0 is the default
	wc.lpfnWndProc = WndProc; //What is our procedure function (defined above)
	wc.cbClsExtra = 0; //How much extra memory do we need for this class?
	wc.cbWndExtra = 0; //How much extra memory do we need for the window?
	wc.hInstance = hInstance; //Pass down the application instance.
	wc.hIcon = LoadIcon(wc.hInstance, (LPCTSTR)IDI_ICON); //Give it the application icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); //Set the cursor for the window. 0 is standard.
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); //Set the background color
	wc.lpszMenuName = NULL; //Do we want a menu?
	wc.lpszClassName = L"MEWINDOW"; //What is the name of the window class
	wc.hIconSm = LoadIcon(wc.hInstance, (LPCTSTR)IDI_ICON); //Small icon


	//Register the container
	if (!RegisterClassEx(&wc))
	{
		//If this fails we have no window and we should exit the program
		MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		exit(0);
		return E_FAIL;
	}

	//Setting window stile accordingly
	DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;	// Window Extended Style
	DWORD dwWindowStyle = WS_OVERLAPPEDWINDOW; // Window Borderless Style

	RECT rc = { (long)0, (long)0, (long)600, (long)600 };// Grabs Rectangle Upper Left / Lower Right Values
	AdjustWindowRectEx(&rc, dwWindowStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	LPCWSTR m_sWindowName = L"Window"; //Name of the window
	//Create the window and store the returning handler in our window handler
	HWND hWindowHandler = CreateWindowEx(
		dwExStyle, //Style from above
		L"MEWINDOW", //Classname from above
		m_sWindowName, //Window name from above
		dwWindowStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, //Style from above, with extra flags.
		800, 100, //Starting position of the window
		600, 600, //Starting size of the window
		NULL, NULL, //Window parent and menu
		hInstance, //hInstance (from above)
		NULL //Any additional application data
	);

	//If it failed...
	if (hWindowHandler == NULL)
	{
		//Inform the user and return fail
		MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		exit(0);
		return E_FAIL;
	}

	ShowWindow(hWindowHandler, nCmdShow); //Show the window
	UpdateWindow(hWindowHandler); //Update the window
#pragma endregion
	// OpenGL Init ------------------------------------------------------------------------
#pragma region OpenGL Initilization
	HDC   hDC = NULL;			// Private GDI Device Context
	if (!(hDC = GetDC(hWindowHandler)))				// Did We Get A Device Context?
	{
		MessageBox(NULL, L"Can't Create A GL Device Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		exit(0);
		return E_FAIL;
	}

	static	PIXELFORMATDESCRIPTOR pfd =	// pfd tells the window how we want render to be
	{
		sizeof(PIXELFORMATDESCRIPTOR),	// Size Of This Pixel Format Descriptor
		1,								// Version Number
		PFD_DRAW_TO_WINDOW |			// Format Must Support Window
		PFD_SUPPORT_OPENGL |			// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,				// Must Support Double Buffering
		PFD_TYPE_RGBA,					// Request An RGBA Format
		16,								// Select Our Color Depth
		0, 0, 0, 0, 0, 0,				// Color Bits Ignored
		0,								// No Alpha Buffer
		0,								// Shift Bit Ignored
		0,								// No Accumulation Buffer
		0, 0, 0, 0,						// Accumulation Bits Ignored
		32,								// 32Bit Z-Buffer (Depth Buffer)  
		0,								// No Stencil Buffer
		0,								// No Auxiliary Buffer
		PFD_MAIN_PLANE,					// Main Drawing Layer
		0,								// Reserved
		0, 0, 0							// Layer Masks Ignored
	};

	GLuint PixelFormat;								// Holds The Results After Searching For A Match
	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		MessageBox(NULL, L"Can't Find A Suitable PixelFormat.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		exit(0);
		return E_FAIL;
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd))		// Are We Able To Set The Pixel Format?
	{
		MessageBox(NULL, L"Can't Set The PixelFormat.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		exit(0);
		return E_FAIL;
	}

	HGLRC hRC = NULL;					// Permanent Rendering Context
	if (!(hRC = wglCreateContext(hDC)))	// Are We Able To Get A Rendering Context?
	{
		MessageBox(NULL, L"Can't Create A GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		exit(0);
		return E_FAIL;
	}

	//Create a temporal context to initialize OpenGL, then try to update it to "modern" OpenGL
	HGLRC tempHRC = wglCreateContext(hDC);	// Create an OpenGL 2.1 context for our device context
	if (!wglMakeCurrent(hDC, tempHRC))			// Try To Activate The Rendering Context
	{
		MessageBox(NULL, L"Can't Activate The GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		exit(0);
		return E_FAIL;
	}

	int nMajor = 3, nMinor = 1;
	int attributes[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, nMajor, // Set the major version of OpenGL
		WGL_CONTEXT_MINOR_VERSION_ARB, nMinor, // Set the minor version of OpenGL
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, // Set our OpenGL context to be forward compatible
		0
	};
	//version and major where available
	if (wglewIsSupported("WGL_ARB_create_context") == 1) // If the OpenGL 3.x context creation extension is available
	{
		hRC = wglCreateContextAttribsARB(hDC, NULL, attributes); //Try to create a context with the major an minor version specified
		wglMakeCurrent(NULL, NULL); // Remove the temporary context from being active
		wglDeleteContext(tempHRC); // Delete the temporary context (OpenGL 2.1)
		wglMakeCurrent(hDC, hRC); // Make our context current (Whatever context was created)
	}

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		exit(0);
		return E_FAIL;
	}

	//Set the clear color
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

#pragma endregion

	// Shader Compilation -----------------------------------------------------------------
#pragma region Shader Initilization
	const GLchar* vertexShader = R"glsl(
		#version 330
		in vec3 positionBuffer;
		void main()
		{
			gl_Position = vec4( positionBuffer, 1.0 );
		}
	)glsl";
	const GLchar* fragmentShader = R"glsl(
		#version 330
		out vec4 fragment;
		void main()
		{
			fragment = vec4( 1.0, 0.0, 0.0, 1.0);
			return;
		}
	)glsl";

	// Create and compile the vertex shader
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);

	//Params: ID, # of strings to make shader, the shader string, how much of the string to use (null means all)
	glShaderSource(vertexShaderID, 1, &vertexShader, NULL);
	glCompileShader(vertexShaderID);

	// Create and compile the fragment shader
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, &fragmentShader, NULL);
	glCompileShader(fragmentShaderID);

	// Link the vertex and fragment shader into a shader program
	GLuint shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShaderID);
	glAttachShader(shaderProgramID, fragmentShaderID);

	glLinkProgram(shaderProgramID);


	//Check the program
	GLint result = GL_FALSE;
	GLint log = GL_FALSE;
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &result); //Are the shader rograms good?
	glGetProgramiv(shaderProgramID, GL_INFO_LOG_LENGTH, &log);
	if (log > 0) {
		std::vector<char> ProgramErrorMessage(log + 1);
		glGetProgramInfoLog(shaderProgramID, log, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}
	else
	{
		printf("Shader Compiled!\n");
	}
	glUseProgram(shaderProgramID);

	//Flag the shaders for deletion. They won't actually be deleted until the
	//shader program gets deleted.
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

#pragma endregion
	// Shape Configuration ----------------------------------------------------------------
#pragma region Shape Initilization

	//Create a vertex array object (which defines how our vertex data is organized)
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLfloat positions[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f
	};

	//Create a vertex buffer object, which contains the actual data
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo); //Make the VBO the active array buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), &positions, GL_STATIC_DRAW); //Pass the data in.

	//Take that buffer data and give it to the shader program
	GLint positionBufferID = glGetAttribLocation(shaderProgramID, "positionBuffer");
	glEnableVertexAttribArray(positionBufferID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
#pragma endregion

	// Game Loop --------------------------------------------------------------------------
#pragma region Game Loop
	std::cout << "Game Loop Started" << std::endl;
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			glClear(GL_COLOR_BUFFER_BIT);

			glDrawArrays(GL_TRIANGLES, 0, 3);

			SwapBuffers(hDC);
		}
	}
#pragma endregion
	glDeleteProgram(shaderProgramID);

	glDeleteBuffers(1, &vbo);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vao);

	return 0;
}
