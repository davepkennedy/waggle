#pragma once
#include "BaseWindow.h"

#define WGL_CONTEXT_DEBUG_BIT_ARB 0x00000001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_ES2_PROFILE_BIT_EXT 0x00000004
#define WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB 0x00000004

#define WGL_NUMBER_PIXEL_FORMATS_ARB	0x2000
#define WGL_DRAW_TO_WINDOW_ARB			0x2001
#define WGL_DRAW_TO_BITMAP_ARB			0x2002
#define WGL_ACCELERATION_ARB			0x2003
#define WGL_NEED_PALETTE_ARB			0x2004
#define WGL_NEED_SYSTEM_PALETTE_ARB		0x2005
#define WGL_SWAP_LAYER_BUFFERS_ARB		0x2006
#define WGL_SWAP_METHOD_ARB				0x2007
#define WGL_NUMBER_OVERLAYS_ARB			0x2008
#define WGL_NUMBER_UNDERLAYS_ARB		0x2009
#define WGL_TRANSPARENT_ARB				0x200A
#define WGL_TRANSPARENT_RED_VALUE_ARB   0x2037
#define WGL_TRANSPARENT_GREEN_VALUE_ARB	0x2038
#define WGL_TRANSPARENT_BLUE_VALUE_ARB	0x2039
#define WGL_TRANSPARENT_ALPHA_VALUE_ARB	0x203A
#define WGL_TRANSPARENT_INDEX_VALUE_ARB	0x203B
#define WGL_SHARE_DEPTH_ARB				0x200C
#define WGL_SHARE_STENCIL_ARB			0x200D
#define WGL_SHARE_ACCUM_ARB				0x200E
#define WGL_SUPPORT_GDI_ARB				0x200F
#define WGL_SUPPORT_OPENGL_ARB			0x2010
#define WGL_DOUBLE_BUFFER_ARB			0x2011
#define WGL_STEREO_ARB					0x2012
#define WGL_PIXEL_TYPE_ARB				0x2013
#define WGL_COLOR_BITS_ARB				0x2014
#define WGL_RED_BITS_ARB				0x2015
#define WGL_RED_SHIFT_ARB				0x2016
#define WGL_GREEN_BITS_ARB				0x2017
#define WGL_GREEN_SHIFT_ARB				0x2018
#define WGL_BLUE_BITS_ARB				0x2019
#define WGL_BLUE_SHIFT_ARB				0x201A
#define WGL_ALPHA_BITS_ARB				0x201B
#define WGL_ALPHA_SHIFT_ARB				0x201C
#define WGL_ACCUM_BITS_ARB				0x201D
#define WGL_ACCUM_RED_BITS_ARB			0x201E
#define WGL_ACCUM_GREEN_BITS_ARB		0x201F
#define WGL_ACCUM_BLUE_BITS_ARB			0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB		0x2021
#define WGL_DEPTH_BITS_ARB				0x2022
#define WGL_STENCIL_BITS_ARB			0x2023
#define WGL_AUX_BUFFERS_ARB				0x2024
#define WGL_NO_ACCELERATION_ARB			0x2025
#define WGL_GENERIC_ACCELERATION_ARB	0x2026
#define WGL_FULL_ACCELERATION_ARB		0x2027
#define WGL_SWAP_EXCHANGE_ARB			0x2028
#define WGL_SWAP_COPY_ARB				0x2029
#define WGL_SWAP_UNDEFINED_ARB			0x202A
#define WGL_TYPE_RGBA_ARB				0x202B
#define WGL_TYPE_COLORINDEX_ARB			0x202C
#define WGL_SAMPLES_ARB					0x2042
#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB 0x20a9

typedef HGLRC(WINAPI *WGLCREATECONTEXT)(HDC);
typedef BOOL(WINAPI *WGLDELETECONTEXT)(HGLRC);
typedef PROC(WINAPI *WGLGETPROCADDRESS)(LPCSTR);
typedef HDC(WINAPI *WGLGETCURRENTDC)(void);
typedef HGLRC(WINAPI *WGLGETCURRENTCONTEXT)(void);
typedef BOOL(WINAPI *WGLMAKECURRENT)(HDC, HGLRC);
typedef BOOL(WINAPI *WGLSHARELISTS)(HGLRC, HGLRC);

typedef const char*(WINAPI *WGLGETEXTENSIONSSTRINGEXT)(void);
typedef const char*(WINAPI *WGLGETEXTENSIONSSTRINGARB)(HDC);
typedef HGLRC(WINAPI *WGLCREATECONTEXTATTRIBSARB)(HDC, HGLRC, const int*);
typedef BOOL(WINAPI *WGLSWAPINTERVALEXT)(int);
typedef BOOL (WINAPI *WGLGETPIXELFORMATATTRIBIVARB)(HDC, int, int, UINT, const int*, int*);

struct pixel_oonfig_t {
	int red_bits;
	int green_bits;
	int blue_bits;
	int alpha_bits;

	int depth_bits;
	int stencil_bits;

	int accum_red_bits;
	int accum_green_bits;
	int accum_blue_bits;
	int accum_alpha_bits;

	int aux_buffers;

	bool stereo;
	bool double_buffer;
	int samples;
	bool sRGB;

	int id;
};

enum gl_profile_t {
	gl_any_profile = 0,
	gl_core_profile = 1,
	gl_compat_profile = 2
};

class GLWindow : public BaseWindow
{
private:
	uint64_t _startTime;
	HGLRC _rc;
	HMODULE _wgl;
	WGLCREATECONTEXT wglCreateContext;
	WGLDELETECONTEXT wglDeleteContext;
	WGLGETPROCADDRESS wglGetProcAddress;
	WGLGETCURRENTDC wglGetCurrentDC;
	WGLGETCURRENTCONTEXT wglGetCurrentContext;
	WGLMAKECURRENT wglMakeCurrent;
	WGLSHARELISTS wglShareLists;

	WGLGETEXTENSIONSSTRINGEXT GetExtensionsStringEXT;
	WGLGETEXTENSIONSSTRINGARB GetExtensionsStringARB;
	WGLCREATECONTEXTATTRIBSARB CreateContextAttribsARB;
	WGLSWAPINTERVALEXT SwapIntervalEXT;
	WGLGETPIXELFORMATATTRIBIVARB GetPixelFormatAttribivARB;

	bool _has_WGL_ARB_multisample;
	bool _has_WGL_ARB_framebuffer_sRGB;
	bool _has_WGL_EXT_framebuffer_sRGB;
	bool _has_WGL_ARB_create_context;
	bool _has_WGL_ARB_create_context_profile;
	bool _has_WGL_EXT_create_context_es2_profile;
	bool _has_WGL_ARB_create_context_robustness;
	bool _has_WGL_ARB_create_context_no_error;
	bool _has_WGL_EXT_swap_control;
	bool _has_WGL_EXT_colorspace;
	bool _has_WGL_ARB_pixel_format;
	bool _has_WGL_ARB_context_flush_control;

	static uint64_t CurrentTick();
protected:
	void InitWGL();
	void CreateWGLContext();
	bool HasExtension(const char* extension) const;
	bool ExtensionInExtensions(const char* extension, const char* extensions) const;
	int PickPixelFormat() const;

	void MakeCurrent();
	void SwapBuffers();

	virtual gl_profile_t Profile() const { return gl_core_profile; }
	virtual bool ForwardCompatible() const { return true; }
	virtual int MajorVersion() const { return 4; }
	virtual int MinorVersion() const { return 1; }
	virtual bool Debug() const { return true; }

	uint64_t ElapsedTime() const;
public:
	GLWindow();
	~GLWindow();

	UINT ClassStyle() { return BaseWindow::ClassStyle() | CS_OWNDC; }
	virtual void Setup() {};
	virtual void Render() = 0;
	void Idle();
};

