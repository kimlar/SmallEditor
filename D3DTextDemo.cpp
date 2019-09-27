
#include "D3DTextDemo.h"
#include <xnamath.h>

struct VertexPos
{
	XMFLOAT3 pos;
	XMFLOAT2 tex0;
};


D3DTextDemo::D3DTextDemo() : solidColorVS_(0), solidColorPS_(0),
inputLayout_(0), vertexBuffer_(0),
colorMap_(0), colorMapSampler_(0)
{

}


D3DTextDemo::~D3DTextDemo()
{

}


bool D3DTextDemo::LoadContent()
{
	ID3DBlob* vsBuffer = 0;

	bool compileResult = CompileD3DShader("TextureMap.fx", "VS_Main", "vs_4_0", &vsBuffer);

	if (compileResult == false)
	{
		DXTRACE_MSG("Error compiling the vertex shader!");
		return false;
	}

	HRESULT d3dResult;

	d3dResult = d3dDevice_->CreateVertexShader(vsBuffer->GetBufferPointer(),
		vsBuffer->GetBufferSize(), 0, &solidColorVS_);

	if (FAILED(d3dResult))
	{
		DXTRACE_MSG("Error creating the vertex shader!");

		if (vsBuffer)
			vsBuffer->Release();

		return false;
	}

	D3D11_INPUT_ELEMENT_DESC solidColorLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	unsigned int totalLayoutElements = ARRAYSIZE(solidColorLayout);

	d3dResult = d3dDevice_->CreateInputLayout(solidColorLayout, totalLayoutElements,
		vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &inputLayout_);

	vsBuffer->Release();

	if (FAILED(d3dResult))
	{
		DXTRACE_MSG("Error creating the input layout!");
		return false;
	}

	ID3DBlob* psBuffer = 0;

	compileResult = CompileD3DShader("TextureMap.fx", "PS_Main", "ps_4_0", &psBuffer);

	if (compileResult == false)
	{
		DXTRACE_MSG("Error compiling pixel shader!");
		return false;
	}

	d3dResult = d3dDevice_->CreatePixelShader(psBuffer->GetBufferPointer(),
		psBuffer->GetBufferSize(), 0, &solidColorPS_);

	psBuffer->Release();

	if (FAILED(d3dResult))
	{
		DXTRACE_MSG("Error creating pixel shader!");
		return false;
	}

	//d3dResult = D3DX11CreateShaderResourceViewFromFile(d3dDevice_, "testfont.dds", 0, 0, &colorMap_, 0);
	d3dResult = D3DX11CreateShaderResourceViewFromFile(d3dDevice_, "Gohufont-14.dds", 0, 0, &colorMap_, 0);
	//d3dResult = D3DX11CreateShaderResourceViewFromFile(d3dDevice_, "font.dds", 0, 0, &colorMap_, 0);

	if (FAILED(d3dResult))
	{
		DXTRACE_MSG("Failed to load the texture image!");
		return false;
	}

	D3D11_SAMPLER_DESC colorMapDesc;
	ZeroMemory(&colorMapDesc, sizeof(colorMapDesc));
	colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP; //D3D11_TEXTURE_ADDRESS_WRAP
	colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP; //D3D11_TEXTURE_ADDRESS_WRAP
	colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP; //D3D11_TEXTURE_ADDRESS_WRAP
	colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	//colorMapDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	//colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	colorMapDesc.MaxLOD = 0.0f;
	//colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;

	d3dResult = d3dDevice_->CreateSamplerState(&colorMapDesc, &colorMapSampler_);

	if (FAILED(d3dResult))
	{
		DXTRACE_MSG("Failed to create color map sampler state!");
		return false;
	}

	D3D11_BUFFER_DESC vertexDesc;
	ZeroMemory(&vertexDesc, sizeof(vertexDesc));
	vertexDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	const int sizeOfSprite = sizeof(VertexPos) * 6;
	//const int maxLetters = 24;

	vertexDesc.ByteWidth = sizeOfSprite * maxLetters;

	d3dResult = d3dDevice_->CreateBuffer(&vertexDesc, 0, &vertexBuffer_);

	if (FAILED(d3dResult))
	{
		DXTRACE_MSG("Failed to create vertex buffer!");
		return false;
	}



	// --------------------------------------
	textMessage = "Hello world!";
	// --------------------------------------

	return true;
}

/*
bool D3DTextDemo::DrawString(const char* message, float startX, float startY)
{
	// Size in bytes for a single sprite.
	const int sizeOfSprite = sizeof(VertexPos) * 6;

	// Demo's dynamic buffer setup for max of 24 letters.
	//const int maxLetters = 24;

	int length = strlen(message);

	// Clamp for strings too long.
	if (length > maxLetters)
		length = maxLetters;

	// Char's width on screen.
	float charWidth = 4*8.0f / 800.0f; // 32.0f / 800.0f;

	// Char's height on screen.
	float charHeight = 4*14.0f / 640.0f; // 32.0f / 640.0f;

	// Char's texel width.
	float texelWidth = 14.0f / 134.0f; // 32.0f / 864.0f;

	// verts per-triangle (3) * total triangles (2) = 6.
	const int verticesPerLetter = 6;

	D3D11_MAPPED_SUBRESOURCE mapResource;
	HRESULT d3dResult = d3dContext_->Map(vertexBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);

	if (FAILED(d3dResult))
	{
		DXTRACE_MSG("Failed to map resource!");
		return false;
	}

	// Point to our vertex buffer's internal data.
	VertexPos* spritePtr = (VertexPos*)mapResource.pData;

	const int indexA = static_cast<char>('A');
	const int indexZ = static_cast<char>('Z');

	for (int i = 0; i < length; ++i)
	{
		float thisStartX = startX + (charWidth * static_cast<float>(i));
		float thisEndX = thisStartX + charWidth;
		float thisEndY = startY + charHeight;

		spritePtr[0].pos = XMFLOAT3(thisEndX, thisEndY, 1.0f);
		spritePtr[1].pos = XMFLOAT3(thisEndX, startY, 1.0f);
		spritePtr[2].pos = XMFLOAT3(thisStartX, startY, 1.0f);
		spritePtr[3].pos = XMFLOAT3(thisStartX, startY, 1.0f);
		spritePtr[4].pos = XMFLOAT3(thisStartX, thisEndY, 1.0f);
		spritePtr[5].pos = XMFLOAT3(thisEndX, thisEndY, 1.0f);

		int texLookup = 0;
		int letter = static_cast<char>(message[i]);

		if (letter < indexA || letter > indexZ)
		{
			// Grab one index past Z, which is a blank space in the texture.
			texLookup = (indexZ - indexA) + 1;
		}
		else
		{
			// A = 0, B = 1, Z = 25, etc.
			texLookup = (letter - indexA);
		}

		float tuStart = 0.0f + (texelWidth * static_cast<float>(texLookup));
		float tuEnd = tuStart + texelWidth;

		spritePtr[0].tex0 = XMFLOAT2(tuEnd, 0.0f);
		spritePtr[1].tex0 = XMFLOAT2(tuEnd, 1.0f);
		spritePtr[2].tex0 = XMFLOAT2(tuStart, 1.0f);
		spritePtr[3].tex0 = XMFLOAT2(tuStart, 1.0f);
		spritePtr[4].tex0 = XMFLOAT2(tuStart, 0.0f);
		spritePtr[5].tex0 = XMFLOAT2(tuEnd, 0.0f);

		spritePtr += 6;
	}
	

	d3dContext_->Unmap(vertexBuffer_, 0);
	d3dContext_->Draw(6 * length, 0);

	return true;
}
*/

/*
bool D3DTextDemo::DrawString(const char* message, float startX, float startY)
{
	int length = 1;


	// ----------------------------------------------------------------
	D3D11_MAPPED_SUBRESOURCE mapResource;
	HRESULT d3dResult = d3dContext_->Map(vertexBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
	if (FAILED(d3dResult))
	{
		DXTRACE_MSG("Failed to map resource!");
		return false;
	}
	// ----------------------------------------------------------------


	// ----------------------------------------------------------------
	VertexPos* spritePtr = (VertexPos*)mapResource.pData; // Point to our vertex buffer's internal data.
	// ----------------------------------------------------------------

	//
	float fontTextureWidth = 146.0f;
	float fontTextureHeight = 92.0f;

	float fontCharWidth = 8.0f;
	float fontCharHeight = 14.0f;
	//


	// ----------------------------------------------------------------
	float charScreenScale = 1.0f; // scale: 1=100%, 2=200%, 3=300% etc.
	float charScreenScaleWidth = 1.0f;
	float charScreenScaleHeight = 1.0f;
	float charScreenWidth = charScreenScale * charScreenScaleWidth * 2.0f;
	float charScreenHeight = charScreenScale * charScreenScaleHeight * 2.0f;

	float thisStartX = startX + (charScreenWidth * fontCharWidth / canvasWidth);
	float thisEndX = thisStartX + (charScreenWidth * fontCharWidth / canvasWidth);
	float thisEndY = startY + (charScreenHeight * fontCharHeight / canvasHeight);

	spritePtr[0].pos = XMFLOAT3(thisEndX, thisEndY, 1.0f);
	spritePtr[1].pos = XMFLOAT3(thisEndX, startY, 1.0f);
	spritePtr[2].pos = XMFLOAT3(thisStartX, startY, 1.0f);
	spritePtr[3].pos = XMFLOAT3(thisStartX, startY, 1.0f);
	spritePtr[4].pos = XMFLOAT3(thisStartX, thisEndY, 1.0f);
	spritePtr[5].pos = XMFLOAT3(thisEndX, thisEndY, 1.0f);
	// ----------------------------------------------------------------


	// ----------------------------------------------------------------
	float tuChar = 13.0f; // Select char from font grid
	float tvChar = 5.0f;  // ...
	
	float texelWidth = 1.0f / fontTextureWidth;
	float texelHeight = 1.0f / fontTextureHeight;

	float tuStart = texelWidth + (texelWidth * (fontCharWidth + 1.0f)) * tuChar;
	float tuEnd = tuStart + (texelWidth * fontCharWidth);

	float tvStart = texelHeight + (texelHeight * (fontCharHeight + 1.0f)) * tvChar;
	float tvEnd = tvStart + (texelHeight * fontCharHeight);

	spritePtr[0].tex0 = XMFLOAT2(tuEnd, tvStart);
	spritePtr[1].tex0 = XMFLOAT2(tuEnd, tvEnd);
	spritePtr[2].tex0 = XMFLOAT2(tuStart, tvEnd);
	spritePtr[3].tex0 = XMFLOAT2(tuStart, tvEnd);
	spritePtr[4].tex0 = XMFLOAT2(tuStart, tvStart);
	spritePtr[5].tex0 = XMFLOAT2(tuEnd, tvStart);
	// ----------------------------------------------------------------


	// ----------------------------------------------------------------
	spritePtr += 6;
	// ----------------------------------------------------------------




	// ----------------------------------------------------------------
	d3dContext_->Unmap(vertexBuffer_, 0);
	d3dContext_->Draw(6 * length, 0);
	// ----------------------------------------------------------------

	return true;
}
*/

bool D3DTextDemo::DrawString(const char* message, float startX, float startY)
{
	int length = strlen(message);


	// ----------------------------------------------------------------
	D3D11_MAPPED_SUBRESOURCE mapResource;
	HRESULT d3dResult = d3dContext_->Map(vertexBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
	if (FAILED(d3dResult))
	{
		DXTRACE_MSG("Failed to map resource!");
		return false;
	}
	// ----------------------------------------------------------------


	// ----------------------------------------------------------------
	VertexPos* spritePtr = (VertexPos*)mapResource.pData; // Point to our vertex buffer's internal data.
	// ----------------------------------------------------------------

	//
	float fontTextureWidth = 146.0f;
	float fontTextureHeight = 92.0f;

	float fontCharWidth = 8.0f;
	float fontCharHeight = 14.0f;
	//


	// ----------------------------------------------------------------
	float charScreenScale = 1.0f; // scale: 1=100%, 2=200%, 3=300% etc.
	float charScreenScaleWidth = 1.0f;
	float charScreenScaleHeight = 1.0f;
	float charScreenWidth = charScreenScale * charScreenScaleWidth * 2.0f;
	float charScreenHeight = charScreenScale * charScreenScaleHeight * 2.0f;
	// ----------------------------------------------------------------

	float specialCharsX = 0.0f;
	float specialCharsY = 0.0f;

	float thisStartX = 0.0f;
	float thisStartY = 0.0f;
	float thisEndX = 0.0f;
	float thisEndY = 0.0f;

	for (int i = 0; i < length; i++)
	{
		int letter = static_cast<char>(message[i]);

		if (letter == 0x08) // Backspace
		{
			if (textMessage.size() == 1)
			{
				textMessage.clear();
				break;
			}

			textMessage.erase(i - 1, 2);
			continue;
		}

		if (letter == 0x09) // Tab
			specialCharsX += (charScreenWidth * fontCharWidth / canvasWidth) * 3.0f;

		/*
		if (letter == 0x0D) // New line '\r'
		{
			//thisStartX = 0.0f;
			//thisEndX = 0.0f;
			//thisStartY = 0.0f;
			//thisEndY = 0.0f;
			//specialCharsX -= 0.0f;
			specialCharsY += (charScreenHeight * fontCharHeight / canvasHeight);
		}
		*/


		// ----------------------------------------------------------------
		thisStartX = startX + specialCharsX + (charScreenWidth * fontCharWidth / canvasWidth) * static_cast<float>(i);
		thisStartY = startY - specialCharsY;
		thisEndX = thisStartX + (charScreenWidth * fontCharWidth / canvasWidth);
		thisEndY = thisStartY + (charScreenHeight * fontCharHeight / canvasHeight);

		spritePtr[0].pos = XMFLOAT3(thisEndX, thisEndY, 1.0f);
		spritePtr[1].pos = XMFLOAT3(thisEndX, thisStartY, 1.0f);
		spritePtr[2].pos = XMFLOAT3(thisStartX, thisStartY, 1.0f);
		spritePtr[3].pos = XMFLOAT3(thisStartX, thisStartY, 1.0f);
		spritePtr[4].pos = XMFLOAT3(thisStartX, thisEndY, 1.0f);
		spritePtr[5].pos = XMFLOAT3(thisEndX, thisEndY, 1.0f);
		// ----------------------------------------------------------------


		// ----------------------------------------------------------------		
		if (letter < 32 || letter > 127)
			letter = 32;

		float tuChar = 0.0f; // Select char from font grid
		float tvChar = 0.0f; // ...

		/*
		if (letter >= 32 && letter < 48)
		{
			tvChar = 0.0f;
		}
		else if (letter >= 48 && letter < 64)
		{
			tvChar = 1.0f;
		}
		else if (letter >= 64 && letter < 80)
		{
			tvChar = 2.0f;
		}
		else if (letter >= 80 && letter < 96)
		{
			tvChar = 3.0f;
		}
		else if (letter >= 96 && letter < 112)
		{
			tvChar = 4.0f;
		}
		else if (letter >= 112 && letter < 128)
		{
			tvChar = 5.0f;
		}
		*/

		tvChar = (float)((letter - 32) / 16);
		tuChar = (float)(letter % 16);


		//float tuChar = 13.0f; // Select char from font grid
		//float tvChar = 5.0f;  // ...

		float texelWidth = 1.0f / fontTextureWidth;
		float texelHeight = 1.0f / fontTextureHeight;

		float tuStart = texelWidth + (texelWidth * (fontCharWidth + 1.0f)) * tuChar;
		float tuEnd = tuStart + (texelWidth * fontCharWidth);

		float tvStart = texelHeight + (texelHeight * (fontCharHeight + 1.0f)) * tvChar;
		float tvEnd = tvStart + (texelHeight * fontCharHeight);

		spritePtr[0].tex0 = XMFLOAT2(tuEnd, tvStart);
		spritePtr[1].tex0 = XMFLOAT2(tuEnd, tvEnd);
		spritePtr[2].tex0 = XMFLOAT2(tuStart, tvEnd);
		spritePtr[3].tex0 = XMFLOAT2(tuStart, tvEnd);
		spritePtr[4].tex0 = XMFLOAT2(tuStart, tvStart);
		spritePtr[5].tex0 = XMFLOAT2(tuEnd, tvStart);
		// ----------------------------------------------------------------


		// ----------------------------------------------------------------
		spritePtr += 6;
		// ----------------------------------------------------------------
	}



	// ----------------------------------------------------------------
	d3dContext_->Unmap(vertexBuffer_, 0);
	d3dContext_->Draw(6 * length, 0);
	// ----------------------------------------------------------------

	return true;
}


void D3DTextDemo::UnloadContent()
{
	if (colorMapSampler_) colorMapSampler_->Release();
	if (colorMap_) colorMap_->Release();
	if (solidColorVS_) solidColorVS_->Release();
	if (solidColorPS_) solidColorPS_->Release();
	if (inputLayout_) inputLayout_->Release();
	if (vertexBuffer_) vertexBuffer_->Release();

	colorMapSampler_ = 0;
	colorMap_ = 0;
	solidColorVS_ = 0;
	solidColorPS_ = 0;
	inputLayout_ = 0;
	vertexBuffer_ = 0;
}


void D3DTextDemo::Update(float dt)
{
	// Nothing to update
}


void D3DTextDemo::Render()
{
	if (d3dContext_ == 0)
		return;

	float clearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	d3dContext_->ClearRenderTargetView(backBufferTarget_, clearColor);

	unsigned int stride = sizeof(VertexPos);
	unsigned int offset = 0;

	d3dContext_->IASetInputLayout(inputLayout_);
	d3dContext_->IASetVertexBuffers(0, 1, &vertexBuffer_, &stride, &offset);
	d3dContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	d3dContext_->VSSetShader(solidColorVS_, 0, 0);
	d3dContext_->PSSetShader(solidColorPS_, 0, 0);
	d3dContext_->PSSetShaderResources(0, 1, &colorMap_);
	d3dContext_->PSSetSamplers(0, 1, &colorMapSampler_);

	
	
	// ----------------------------------------------------------------
	// Turn on blending (don't forget to reset it afterwards)
	gBlendState = NULL;

	D3D11_BLEND_DESC blendState;
	ZeroMemory(&blendState, sizeof(D3D11_BLEND_DESC));
	blendState.AlphaToCoverageEnable = false;
	blendState.IndependentBlendEnable = false;
	blendState.RenderTarget[0].BlendEnable = TRUE;
	blendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	d3dDevice_->CreateBlendState(&blendState, &gBlendState);
	d3dContext_->OMSetBlendState(gBlendState, 0, 0xffffffff);
	// ----------------------------------------------------------------
	

	//DrawString("HELLO WORLD", -0.2f, 0.0f);
	//DrawString("DrawString(\"HELLO WORLD\", -0.2f, 0.0f);", -0.2f, 0.0f);
	DrawString(textMessage.c_str(), -0.2f, 0.0f);
	
	// ----------------------------------------------------------------
	// Turn off blending (reset it to default)
	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
	BlendState.RenderTarget[0].BlendEnable = FALSE;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	 d3dDevice_->CreateBlendState(&BlendState, &gBlendState);
	d3dContext_->OMSetBlendState(gBlendState, 0, 0xffffffff);
	// ----------------------------------------------------------------




	swapChain_->Present(0, 0);
}

void D3DTextDemo::CreateKeyboardEvent(char key)
{
	textMessage += key;
}
