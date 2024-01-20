#pragma once
#include <string>

namespace dae
{
	struct Vector2;

	class Texture
	{
	public:
		Texture(SDL_Surface* pSurface, ID3D11Device* pDevice);
		~Texture();

		ID3D11ShaderResourceView* GetSRV() const { return m_pSRV; }
		ID3D11Texture2D* GetResource() const { return m_pResource; }
		static Texture* LoadFromFile(const std::string& path, ID3D11Device* pDevice);

	private:

		ID3D11Texture2D* m_pResource{ nullptr };
		ID3D11ShaderResourceView* m_pSRV{ nullptr };
	};
}

