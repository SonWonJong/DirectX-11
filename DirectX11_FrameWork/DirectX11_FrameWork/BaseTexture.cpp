#include "stdafx.h"
#include "BaseTexture.h"
#include <stdio.h>


BaseTexture::BaseTexture()
{
}


BaseTexture::~BaseTexture()
{
}


bool BaseTexture::Initialize(ID3D11Device* InDevice, ID3D11DeviceContext* InDeviceContext, const char* InFileName)
{
	int Width = 0;
	int Height = 0;

	// targa 이미지 데이터를 메모리에 로드
	if (!LoadTarga(InFileName, Height, Width))
	{
		return false;
	}

	// 텍스처 구조체를 설정
	D3D11_TEXTURE2D_DESC TextureDesc;
	TextureDesc.Height = Height;
	TextureDesc.Width = Width;
	TextureDesc.MipLevels = 0;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// 빈 첵스처 생성
	HRESULT Result = InDevice->CreateTexture2D(&TextureDesc, nullptr, &m_Texture);
	if (FAILED(Result))
	{
		return false;
	}

	// targa 이미지 데이터의 너비 사이즈 설정
	UINT RowPitch = (Width * 4) * sizeof(unsigned char);

	// targar 이미지 데이터를 텍스처에 복사
	InDeviceContext->UpdateSubresource(m_Texture, 0, nullptr, m_TargaData, RowPitch, 0);

	// 셰이더 리소스 뷰 구조체를 설정합니다.
	D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
	ShaderResourceViewDesc.Format = TextureDesc.Format;
	ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	ShaderResourceViewDesc.Texture2D.MipLevels = -1;

	// 텍스처의 셰이더 리소스 뷰를 만듭니다.
	Result = InDevice->CreateShaderResourceView(m_Texture, &ShaderResourceViewDesc, &m_TextureView);
	if (FAILED(Result))
	{
		return false;
	}

	// 텍스처 밉맵 생성
	InDeviceContext->GenerateMips(m_TextureView);

	delete[] m_TargaData;
	m_TargaData = nullptr;
	
	return true;
}

void BaseTexture::Shutdown()
{
	if (m_TextureView)
	{
		m_TextureView->Release();
		m_TextureView = nullptr;
	}

	if (m_Texture)
	{
		m_Texture->Release();
		m_Texture = nullptr;
	}

	if (m_TargaData)
	{
		delete[] m_TargaData;
		m_TargaData = nullptr;
	}
}

ID3D11ShaderResourceView* BaseTexture::GetTexture()
{
	return m_TextureView;
}

bool BaseTexture::LoadTarga(const char* InFileName, int& InHeight, int& InWidth)
{
	// targa 파일을 바이너리 모드로 파일을 엽니다.
	FILE* FilePtr;
	if (fopen_s(&FilePtr, InFileName, "rb") != 0)
	{
		return false;
	}

	// 파일 헤더를 읽어옵니다.
	TargaHeader TargaFileHeader;
	unsigned int count = (unsigned int)fread(&TargaFileHeader, sizeof(TargaHeader), 1, FilePtr);
	if (count != 1)
	{
		return false;
	}

	// 파일 헤더에서 중요 정보를 얻어옵니다.
	InHeight = (int)TargaFileHeader.height;
	InWidth = (int)TargaFileHeader.width;
	int bpp = (int)TargaFileHeader.bpp;

	// 파일이 32bit인지
	if (bpp != 32)
	{
		return false;
	}

	// 32비트 이미지 데이터의 크기를 계산합니다.
	int ImageSize = InWidth * InHeight * 4;

	// targa 이미지 데이터 용 메모리를 할당합니다.
	unsigned char* TargaImage = new unsigned char[ImageSize];
	if (!TargaImage)
	{
		return false;
	}

	//targa 이미지 데이터를 읽습니다.
	count = (unsigned int)fread(TargaImage, 1, ImageSize, FilePtr);

	if (count != ImageSize)
	{
		return false;
	}

	// 파일을 닫습니다.
	if (fclose(FilePtr) != 0)
	{
		return false;
	}

	// targa 대상 데이터에 대한 메모리를 할당합니다.
	m_TargaData = new unsigned char[ImageSize];
	if (!m_TargaData)
	{
		return false;
	}

	// Targa 대상 데이터 배열에 인덱스를 초기화합니다.
	int Index = 0;
	int k = (InWidth * InHeight * 4) - (InWidth * 4);

	// 이제 Targa 형식이 거꾸로 저장되었으므로 올바른 순서로 Targa이미지 데이터를 Targa 대상 배열에 복사합니다.

	for (int j = 0; j < InHeight; ++j)
	{
		for (int i = 0; i < InWidth; ++i)
		{
			m_TargaData[Index + 0] = TargaImage[k + 2];
			m_TargaData[Index + 1] = TargaImage[k + 1];
			m_TargaData[Index + 2] = TargaImage[k + 0];
			m_TargaData[Index + 3] = TargaImage[k + 3];

			k += 4;
			Index += 4;
		}

		// targa 이미지 데이터 인덱스를 역순으로 읽은 후 열의 시작부분에서 이전 행으로 다시 설정
		k -= (InWidth * 8);
	}

	delete[] TargaImage;
	TargaImage = 0;

	return true;

}
