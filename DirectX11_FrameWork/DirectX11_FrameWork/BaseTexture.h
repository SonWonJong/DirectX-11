#pragma once
class BaseTexture
{
private:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

public:
	BaseTexture();
	~BaseTexture();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, const char*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	bool LoadTarga(const char*, int&, int&);

private:
	unsigned char* m_TargaData = nullptr;
	ID3D11Texture2D* m_Texture = nullptr;
	ID3D11ShaderResourceView* m_TextureView = nullptr;
};

