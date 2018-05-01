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

	// targa �̹��� �����͸� �޸𸮿� �ε�
	if (!LoadTarga(InFileName, Height, Width))
	{
		return false;
	}

	// �ؽ�ó ����ü�� ����
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

	// �� ý��ó ����
	HRESULT Result = InDevice->CreateTexture2D(&TextureDesc, nullptr, &m_Texture);
	if (FAILED(Result))
	{
		return false;
	}

	// targa �̹��� �������� �ʺ� ������ ����
	UINT RowPitch = (Width * 4) * sizeof(unsigned char);

	// targar �̹��� �����͸� �ؽ�ó�� ����
	InDeviceContext->UpdateSubresource(m_Texture, 0, nullptr, m_TargaData, RowPitch, 0);

	// ���̴� ���ҽ� �� ����ü�� �����մϴ�.
	D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
	ShaderResourceViewDesc.Format = TextureDesc.Format;
	ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	ShaderResourceViewDesc.Texture2D.MipLevels = -1;

	// �ؽ�ó�� ���̴� ���ҽ� �並 ����ϴ�.
	Result = InDevice->CreateShaderResourceView(m_Texture, &ShaderResourceViewDesc, &m_TextureView);
	if (FAILED(Result))
	{
		return false;
	}

	// �ؽ�ó �Ӹ� ����
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
	// targa ������ ���̳ʸ� ���� ������ ���ϴ�.
	FILE* FilePtr;
	if (fopen_s(&FilePtr, InFileName, "rb") != 0)
	{
		return false;
	}

	// ���� ����� �о�ɴϴ�.
	TargaHeader TargaFileHeader;
	unsigned int count = (unsigned int)fread(&TargaFileHeader, sizeof(TargaHeader), 1, FilePtr);
	if (count != 1)
	{
		return false;
	}

	// ���� ������� �߿� ������ ���ɴϴ�.
	InHeight = (int)TargaFileHeader.height;
	InWidth = (int)TargaFileHeader.width;
	int bpp = (int)TargaFileHeader.bpp;

	// ������ 32bit����
	if (bpp != 32)
	{
		return false;
	}

	// 32��Ʈ �̹��� �������� ũ�⸦ ����մϴ�.
	int ImageSize = InWidth * InHeight * 4;

	// targa �̹��� ������ �� �޸𸮸� �Ҵ��մϴ�.
	unsigned char* TargaImage = new unsigned char[ImageSize];
	if (!TargaImage)
	{
		return false;
	}

	//targa �̹��� �����͸� �н��ϴ�.
	count = (unsigned int)fread(TargaImage, 1, ImageSize, FilePtr);

	if (count != ImageSize)
	{
		return false;
	}

	// ������ �ݽ��ϴ�.
	if (fclose(FilePtr) != 0)
	{
		return false;
	}

	// targa ��� �����Ϳ� ���� �޸𸮸� �Ҵ��մϴ�.
	m_TargaData = new unsigned char[ImageSize];
	if (!m_TargaData)
	{
		return false;
	}

	// Targa ��� ������ �迭�� �ε����� �ʱ�ȭ�մϴ�.
	int Index = 0;
	int k = (InWidth * InHeight * 4) - (InWidth * 4);

	// ���� Targa ������ �Ųٷ� ����Ǿ����Ƿ� �ùٸ� ������ Targa�̹��� �����͸� Targa ��� �迭�� �����մϴ�.

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

		// targa �̹��� ������ �ε����� �������� ���� �� ���� ���ۺκп��� ���� ������ �ٽ� ����
		k -= (InWidth * 8);
	}

	delete[] TargaImage;
	TargaImage = 0;

	return true;

}
