#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <DxErr.h>

class Dx11DemoBase
{
public:
	Dx11DemoBase();
	virtual ~Dx11DemoBase();

	bool Initialize(HINSTANCE hInstance, HWND hwnd);
	void Shutdown();

	bool CompileD3DShader(const char* filePath, const char* entry, const char* shaderModel, ID3DBlob** buffer);

	virtual bool LoadContent();
	virtual void UnloadContent();

	virtual void Update(float dt) = 0;
	virtual void Render() = 0;

protected:
	HINSTANCE hInstance_;
	HWND hwnd_;

	D3D_DRIVER_TYPE driverType_;
	D3D_FEATURE_LEVEL featureLevel_;

	ID3D11Device* d3dDevice_;
	ID3D11DeviceContext* d3dContext_;
	IDXGISwapChain* swapChain_;
	ID3D11RenderTargetView* backBufferTarget_;

	unsigned int canvasWidth = 0;
	unsigned int canvasHeight = 0;
};
