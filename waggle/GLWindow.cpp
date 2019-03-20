#include "stdafx.h"
#include "GLWindow.h"


uint64_t GLWindow::CurrentTick()
{
	LARGE_INTEGER pc;
	QueryPerformanceCounter(&pc);
	return pc.QuadPart;
}

void GLWindow::InitWGL()
{
	_wgl = LoadLibrary(TEXT("opengl32.dll"));

	wglCreateContext = (WGLCREATECONTEXT)::GetProcAddress(_wgl, "wglCreateContext");
	wglDeleteContext = (WGLDELETECONTEXT)::GetProcAddress(_wgl, "wglDeleteContext");
	wglGetProcAddress = (WGLGETPROCADDRESS)::GetProcAddress(_wgl, "wglGetProcAddress");
	wglGetCurrentDC = (WGLGETCURRENTDC)::GetProcAddress(_wgl, "wglGetCurrentDC");
	wglGetCurrentContext = (WGLGETCURRENTCONTEXT)::GetProcAddress(_wgl, "wglGetCurrentContext");
	wglMakeCurrent = (WGLMAKECURRENT)::GetProcAddress(_wgl, "wglMakeCurrent");
	wglShareLists = (WGLSHARELISTS)::GetProcAddress(_wgl, "wglShareLists");

	HDC dc = GetDC(*this);
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;

	if (!SetPixelFormat(dc, ChoosePixelFormat(dc, &pfd), &pfd)) {
		throw std::runtime_error("Failed to set pixel format");
	}

	HGLRC glrc = wglCreateContext(dc);
	if (!glrc) {
		throw std::runtime_error("Failed to create GL context");
	}

	HDC currentDc = wglGetCurrentDC();
	HGLRC currentRc = wglGetCurrentContext();

	if (!wglMakeCurrent(dc, glrc)) {
		wglMakeCurrent(currentDc, currentRc);
		wglDeleteContext(glrc);
		throw std::runtime_error("Failed to set context");
	}

	GetExtensionsStringEXT = (WGLGETEXTENSIONSSTRINGEXT)wglGetProcAddress("wglGetExtensionsStringEXT");
	GetExtensionsStringARB = (WGLGETEXTENSIONSSTRINGARB)wglGetProcAddress("wglGetExtensionsStringARB");
	CreateContextAttribsARB = (WGLCREATECONTEXTATTRIBSARB)wglGetProcAddress("wglCreateContextAttribsARB");
	SwapIntervalEXT = (WGLSWAPINTERVALEXT)wglGetProcAddress("wglSwapIntervalEXT");
	GetPixelFormatAttribivARB = (WGLGETPIXELFORMATATTRIBIVARB)wglGetProcAddress("wglGetPixelFormatAttribivARB");

	_has_WGL_ARB_multisample = HasExtension("WGL_ARB_multisample");
	_has_WGL_ARB_framebuffer_sRGB = HasExtension("WGL_ARB_framebuffer_sRGB");
	_has_WGL_EXT_framebuffer_sRGB = HasExtension("WGL_EXT_framebuffer_sRGB");
	_has_WGL_ARB_create_context = HasExtension("WGL_ARB_create_context");
	_has_WGL_ARB_create_context_profile = HasExtension("WGL_ARB_create_context_profile");
	_has_WGL_EXT_create_context_es2_profile = HasExtension("WGL_EXT_create_context_es2_profile");
	_has_WGL_ARB_create_context_robustness = HasExtension("WGL_ARB_create_context_robustness");
	_has_WGL_ARB_create_context_no_error = HasExtension("WGL_ARB_create_context_no_error");
	_has_WGL_EXT_swap_control = HasExtension("WGL_EXT_swap_control");
	_has_WGL_EXT_colorspace = HasExtension("WGL_EXT_colorspace");
	_has_WGL_ARB_pixel_format = HasExtension("WGL_ARB_pixel_format");
	_has_WGL_ARB_context_flush_control = HasExtension("WGL_ARB_context_flush_control");


	wglMakeCurrent(currentDc, currentRc);
	wglDeleteContext(glrc);
}

static int GetAttributeValue(const std::vector<int>& attrs, const std::vector<int>& values, int attr) {
	for (int i = 0; i < attrs.size(); i++) {
		if (attrs[i] == attr) {
			return values[i];
		}
	}
	return 0;
}

static int PickBestConfig(const std::vector<pixel_oonfig_t>& configs) {
	pixel_oonfig_t bestConfig = configs[0];
	int bestScore = 0;

	for (pixel_oonfig_t cfg : configs) {
		int score = 0;
		if (cfg.accum_alpha_bits >= bestConfig.accum_alpha_bits) { score++; }
		if (cfg.accum_blue_bits >= bestConfig.accum_blue_bits) { score++; }
		if (cfg.accum_green_bits >= bestConfig.accum_green_bits) { score++; }
		if (cfg.accum_red_bits >= bestConfig.accum_red_bits) { score++; }
		if (cfg.alpha_bits >= bestConfig.alpha_bits) { score++; }
		if (cfg.aux_buffers >= bestConfig.aux_buffers) { score++; }
		if (cfg.blue_bits >= bestConfig.blue_bits) { score++; }
		if (cfg.depth_bits >= bestConfig.depth_bits) { score++; }
		if (cfg.double_buffer >= bestConfig.double_buffer) { score++; }
		if (cfg.green_bits >= bestConfig.green_bits) { score++; }
		if (cfg.red_bits >= bestConfig.red_bits) { score++; }
		if (cfg.samples >= bestConfig.samples) { score++; }
		if (cfg.sRGB > bestConfig.sRGB) { score++; }
		if (cfg.stencil_bits > bestConfig.stencil_bits) { score++; }
		if (cfg.stereo > bestConfig.stereo) { score++; }

		if (score >= bestScore) {
			bestScore = score;
			bestConfig = cfg;
		}
	}
	return bestConfig.id;
}

void GLWindow::CreateWGLContext()
{
	int pixelFormat = PickPixelFormat();
	if (!pixelFormat) {
		throw std::runtime_error("Failed to pick a pixel format");
	}

	HDC dc = GetDC(*this);
	PIXELFORMATDESCRIPTOR pfd;
	DescribePixelFormat(dc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	SetPixelFormat(dc, pixelFormat, &pfd);

	if (ForwardCompatible() && !_has_WGL_ARB_create_context) {
		throw std::runtime_error("Want forward compat context, missing WGL_ARB_create_context");
	}
	if (Profile() && !_has_WGL_ARB_create_context_profile) {
		throw std::runtime_error("Want profile, missing WGL_ARB_create_context_profile");
	}

	if (_has_WGL_ARB_create_context) {
		std::vector<int> attribs;
		int mask = 0, flags = 0;
		if (ForwardCompatible()) {
			flags |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
		}
		if (Profile() == gl_core_profile) {
			mask |= WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
		}
		else if (Profile() == gl_compat_profile) {
			mask |= WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
		}

		flags |= WGL_CONTEXT_DEBUG_BIT_ARB;

		if (MajorVersion() != 1 && MinorVersion() != 0) {
			attribs.push_back(WGL_CONTEXT_MAJOR_VERSION_ARB);
			attribs.push_back(MajorVersion());
			attribs.push_back(WGL_CONTEXT_MINOR_VERSION_ARB);
			attribs.push_back(MinorVersion());
		}

		if (flags) {
			attribs.push_back(WGL_CONTEXT_FLAGS_ARB);
			attribs.push_back(flags);
		}
		if (mask) {
			attribs.push_back(WGL_CONTEXT_PROFILE_MASK_ARB);
			attribs.push_back(mask);
		}
		attribs.push_back(0);
		attribs.push_back(0);

		_rc = CreateContextAttribsARB(dc, nullptr, attribs.data());
		if (!_rc) {
			throw std::runtime_error("Failed to create context");
		}
	}
	else {

	}
}

bool GLWindow::HasExtension(const char * extension) const
{
	const char* extensions = NULL;

	if (GetExtensionsStringARB)
		extensions = GetExtensionsStringARB(wglGetCurrentDC());
	else if (GetExtensionsStringEXT)
		extensions = GetExtensionsStringEXT();

	if (!extensions) {
		return false;
	}
	return ExtensionInExtensions(extension, extensions);
}

bool GLWindow::ExtensionInExtensions(const char * extension, const char * extensions) const
{
	size_t extLen = strlen(extension);
	const char* start = extensions;
	for (const char* where = strstr(start, extension); where != nullptr; where = strstr(start, extension)) {
		const char* end = where + extLen;
		if (((where == start) || *(where - 1) == ' ') &&
			(*end == ' ' || *end == '\0')) {
			return true;
		}
	}
	return false;
}

int GLWindow::PickPixelFormat() const
{
	int nativeCount = 0;
	std::vector<int> attrs;
	HDC dc = GetDC(*this);
	if (_has_WGL_ARB_pixel_format) {
		const int attrib = WGL_NUMBER_PIXEL_FORMATS_ARB;

		if (!GetPixelFormatAttribivARB(dc, 1, 0, 1, &attrib, &nativeCount)) {
			throw std::runtime_error("Failed to get Pixel Formet Attributes");
		}
		attrs.push_back(WGL_SUPPORT_OPENGL_ARB);
		attrs.push_back(WGL_DRAW_TO_WINDOW_ARB);
		attrs.push_back(WGL_PIXEL_TYPE_ARB);
		attrs.push_back(WGL_ACCELERATION_ARB);
		attrs.push_back(WGL_RED_BITS_ARB);
		attrs.push_back(WGL_RED_SHIFT_ARB);
		attrs.push_back(WGL_GREEN_BITS_ARB);
		attrs.push_back(WGL_GREEN_SHIFT_ARB);
		attrs.push_back(WGL_BLUE_BITS_ARB);
		attrs.push_back(WGL_BLUE_SHIFT_ARB);
		attrs.push_back(WGL_ALPHA_BITS_ARB);
		attrs.push_back(WGL_ALPHA_SHIFT_ARB);
		attrs.push_back(WGL_DEPTH_BITS_ARB);
		attrs.push_back(WGL_STENCIL_BITS_ARB);
		attrs.push_back(WGL_ACCUM_BITS_ARB);
		attrs.push_back(WGL_ACCUM_RED_BITS_ARB);
		attrs.push_back(WGL_ACCUM_GREEN_BITS_ARB);
		attrs.push_back(WGL_ACCUM_BLUE_BITS_ARB);
		attrs.push_back(WGL_ACCUM_ALPHA_BITS_ARB);
		attrs.push_back(WGL_AUX_BUFFERS_ARB);
		attrs.push_back(WGL_STEREO_ARB);
		attrs.push_back(WGL_DOUBLE_BUFFER_ARB);

		if (_has_WGL_ARB_multisample) {

			attrs.push_back(WGL_SAMPLES_ARB);
		}
		if (_has_WGL_ARB_framebuffer_sRGB || _has_WGL_EXT_framebuffer_sRGB)
			attrs.push_back(WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB);
	}
	else {
		nativeCount = DescribePixelFormat(dc, 1, sizeof(PIXELFORMATDESCRIPTOR), nullptr);
	}

	std::vector<int> values;
	values.resize(attrs.size());
	std::vector<pixel_oonfig_t> configs;
	for (int i = 0; i < nativeCount; i++) {
		pixel_oonfig_t pixelConfig;

		if (_has_WGL_ARB_pixel_format) {
			if (!GetPixelFormatAttribivARB(dc, i + 1, 0, attrs.size(), attrs.data(), values.data())) {
				throw std::runtime_error("Failed to get Pixel Format Attributes");
			}
			if (!GetAttributeValue(attrs, values, WGL_SUPPORT_OPENGL_ARB) ||
				!GetAttributeValue(attrs, values, WGL_DRAW_TO_WINDOW_ARB)) {
				continue;
			}
			if (GetAttributeValue(attrs, values, WGL_PIXEL_TYPE_ARB) != WGL_TYPE_RGBA_ARB) {
				continue;
			}
			if (GetAttributeValue(attrs, values, WGL_ACCELERATION_ARB) == WGL_NO_ACCELERATION_ARB) {
				continue;
			}

			pixelConfig.red_bits = GetAttributeValue(attrs, values, WGL_RED_BITS_ARB);
			pixelConfig.green_bits = GetAttributeValue(attrs, values, WGL_GREEN_BITS_ARB);
			pixelConfig.blue_bits = GetAttributeValue(attrs, values, WGL_BLUE_BITS_ARB);
			pixelConfig.alpha_bits = GetAttributeValue(attrs, values, WGL_ALPHA_BITS_ARB);

			pixelConfig.depth_bits = GetAttributeValue(attrs, values, WGL_DEPTH_BITS_ARB);
			pixelConfig.stencil_bits = GetAttributeValue(attrs, values, WGL_STENCIL_BITS_ARB);

			pixelConfig.accum_red_bits = GetAttributeValue(attrs, values, WGL_ACCUM_RED_BITS_ARB);
			pixelConfig.accum_green_bits = GetAttributeValue(attrs, values, WGL_ACCUM_GREEN_BITS_ARB);
			pixelConfig.accum_blue_bits = GetAttributeValue(attrs, values, WGL_ACCUM_BLUE_BITS_ARB);
			pixelConfig.accum_alpha_bits = GetAttributeValue(attrs, values, WGL_ACCUM_ALPHA_BITS_ARB);

			pixelConfig.aux_buffers = GetAttributeValue(attrs, values, WGL_AUX_BUFFERS_ARB);

			if (GetAttributeValue(attrs, values, WGL_STEREO_ARB)) {
				pixelConfig.stereo = true;
			}
			if (GetAttributeValue(attrs, values, WGL_DOUBLE_BUFFER_ARB)) {
				pixelConfig.double_buffer = true;
			}

			if (_has_WGL_ARB_multisample) {
				pixelConfig.samples = GetAttributeValue(attrs, values, WGL_SAMPLES_ARB);
			}

			if (_has_WGL_ARB_framebuffer_sRGB ||
				_has_WGL_EXT_framebuffer_sRGB)
			{
				if (GetAttributeValue(attrs, values, WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB)) {
					pixelConfig.sRGB = true;
				}
			}
		}
		else {
			// Get pixel format attributes through legacy PFDs

			PIXELFORMATDESCRIPTOR pfd;

			if (!DescribePixelFormat(dc, i + 1, sizeof(PIXELFORMATDESCRIPTOR), &pfd))
			{
				throw std::runtime_error("Failed to get pixel format");
			}

			if (!(pfd.dwFlags & PFD_DRAW_TO_WINDOW) ||
				!(pfd.dwFlags & PFD_SUPPORT_OPENGL))
			{
				continue;
			}

			if (!(pfd.dwFlags & PFD_GENERIC_ACCELERATED) &&
				(pfd.dwFlags & PFD_GENERIC_FORMAT))
			{
				continue;
			}

			if (pfd.iPixelType != PFD_TYPE_RGBA)
				continue;

			pixelConfig.red_bits = pfd.cRedBits;
			pixelConfig.green_bits = pfd.cGreenBits;
			pixelConfig.blue_bits = pfd.cBlueBits;
			pixelConfig.alpha_bits = pfd.cAlphaBits;

			pixelConfig.depth_bits = pfd.cDepthBits;
			pixelConfig.stencil_bits = pfd.cStencilBits;

			pixelConfig.accum_red_bits = pfd.cAccumRedBits;
			pixelConfig.accum_green_bits = pfd.cAccumGreenBits;
			pixelConfig.accum_blue_bits = pfd.cAccumBlueBits;
			pixelConfig.accum_alpha_bits = pfd.cAccumAlphaBits;

			pixelConfig.aux_buffers = pfd.cAuxBuffers;

			if (pfd.dwFlags & PFD_STEREO)
				pixelConfig.stereo = true;
			if (pfd.dwFlags & PFD_DOUBLEBUFFER)
				pixelConfig.double_buffer = true;
		}
		pixelConfig.id = i + 1;
		configs.push_back(pixelConfig);
	}
	if (configs.empty()) {
		throw std::runtime_error("No usable pixel configs");
	}

	return PickBestConfig(configs);
}

void GLWindow::MakeCurrent()
{
	wglMakeCurrent(GetDC(*this), _rc);
}

void GLWindow::SwapBuffers()
{
	::SwapBuffers(GetDC(*this));
}

uint64_t GLWindow::ElapsedTime() const
{
	return CurrentTick() - _startTime;
}

GLWindow::GLWindow()
	:_startTime(CurrentTick())
{
	Observe(WM_CREATE, [this](WPARAM wParam, LPARAM lParam) {
		InitWGL();
		CreateWGLContext();
		MakeCurrent();
		glewInit();
		Setup();
	});
}


GLWindow::~GLWindow()
{
}

void GLWindow::Idle()
{
	MakeCurrent();
	Render();
	SwapBuffers();
}
