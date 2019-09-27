#include "Dx11DemoBase.h"
#include <D3Dcompiler.h>
#pragma comment(lib, "legacy_stdio_definitions.lib") // Needed for __vsnwprintf()

Dx11DemoBase::Dx11DemoBase() : driverType_(D3D_DRIVER_TYPE_NULL),
featureLevel_(D3D_FEATURE_LEVEL_11_0), d3dDevice_(0), d3dContext_(0),
swapChain_(0), backBufferTarget_(0)
{

}

Dx11DemoBase::~Dx11DemoBase()
{
	Shutdown();
}

bool Dx11DemoBase::LoadContent()
{
	// Override with demo specifics, if any...

	return true;
}

void Dx11DemoBase::UnloadContent()
{
	// Override with demo specifics, if any...
}

bool Dx11DemoBase::Initialize(HINSTANCE hInstance, HWND hwnd)
{
	hInstance_ = hInstance;
	hwnd_ = hwnd;

	RECT dimensions;
	GetClientRect(hwnd, &dimensions);

	unsigned int width = dimensions.right - dimensions.left;
	unsigned int height = dimensions.bottom - dimensions.top;
	canvasWidth = width;
	canvasHeight = height;

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_SOFTWARE
	};

	unsigned int totalDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	unsigned int totalFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = true;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	unsigned int creationFlags = 0;

#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT result;
	unsigned int driver = 0;

	for (driver = 0; driver < totalDriverTypes; ++driver)
	{
		result = D3D11CreateDeviceAndSwapChain(0, driverTypes[driver], 0,
			creationFlags, featureLevels, totalFeatureLevels,
			D3D11_SDK_VERSION, &swapChainDesc, &swapChain_,
			&d3dDevice_, &featureLevel_, &d3dContext_);

		if (SUCCEEDED(result))
		{
			driverType_ = driverTypes[driver];
			break;
		}
	}

	if (FAILED(result))
	{
		DXTRACE_MSG("Failed to create the Direct3D device!");
		return false;
	}

	ID3D11Texture2D* backBufferTexture;
	result = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(LPVOID*)& backBufferTexture);

	if (FAILED(result))
	{
		DXTRACE_MSG("Failed to get the swap chain back buffer!");
		return false;
	}

	result = d3dDevice_->CreateRenderTargetView(backBufferTexture, 0,
		&backBufferTarget_);

	if (backBufferTexture)
		backBufferTexture->Release();

	if (FAILED(result))
	{
		DXTRACE_MSG("Failed to create the render target view!");
		return false;
	}

	d3dContext_->OMSetRenderTargets(1, &backBufferTarget_, 0);

	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	d3dContext_->RSSetViewports(1, &viewport);

	return LoadContent();
}

void Dx11DemoBase::Shutdown()
{
	UnloadContent();

	if (backBufferTarget_) backBufferTarget_->Release();
	if (swapChain_) swapChain_->Release();
	if (d3dContext_) d3dContext_->Release();
	if (d3dDevice_) d3dDevice_->Release();
	d3dDevice_ = 0;
	d3dContext_ = 0;
	swapChain_ = 0;
	backBufferTarget_ = 0;
}

bool Dx11DemoBase::CompileD3DShader(const char* filePath, const char* entry, const char* shaderModel, ID3DBlob** buffer)
{
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* errorBuffer = 0;
	HRESULT result;

	/* Conversion from char* to LPWSTR */
	/*
	int wchars_num = MultiByteToWideChar(CP_UTF8, 0, filePath, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[wchars_num];
	MultiByteToWideChar(CP_UTF8, 0, filePath , -1, wstr, wchars_num);
	
	result = D3DX11CompileFromFile(wstr, 0, 0, entry, shaderModel,
		shaderFlags, 0, 0, buffer, &errorBuffer, 0);

	delete[] wstr; // Don't forget to delete
	*/

	result = D3DX11CompileFromFile(filePath, 0, 0, entry, shaderModel,
		shaderFlags, 0, 0, buffer, &errorBuffer, 0);

	if (FAILED(result))
	{
		if (errorBuffer != 0)
		{
			OutputDebugStringA((char*)errorBuffer->GetBufferPointer());
			errorBuffer->Release();
		}

		return false;
	}

	if (errorBuffer != 0)
		errorBuffer->Release();

	return true;
}