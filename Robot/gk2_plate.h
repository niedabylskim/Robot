#include <d3d11.h>
#include <DirectXMath.h>
#include <list>
#include <memory>
#include "gk2_deviceHelper.h"
#include "gk2_constantBuffer.h"
#include <random>

namespace gk2
{
	struct PlateVertex
	{
		DirectX::XMFLOAT3 Pos;
		static const unsigned int LayoutElements = 1;
		static const D3D11_INPUT_ELEMENT_DESC Layout[LayoutElements];

		PlateVertex() : Pos(0.0f, 0.0f, 0.0f) { }
	};
	class Plate
	{
	public:
		Plate(DeviceHelper& device);

		void SetViewMtxBuffer(const std::shared_ptr<CBMatrix>& view);
		void SetProjMtxBuffer(const std::shared_ptr<CBMatrix>& proj);
		void SetWorldMtxBuffer(const std::shared_ptr<CBMatrix>& world);

		void Update(std::shared_ptr<ID3D11DeviceContext>& context, float dt, DirectX::XMFLOAT4 cameraPos, DirectX::XMFLOAT3 startPos);
		void Render(std::shared_ptr<ID3D11DeviceContext>& context) const;

	private:
		
		static const unsigned int OFFSET;
		static const unsigned int STRIDE;
		
		std::shared_ptr<CBMatrix> m_viewCB;
		std::shared_ptr<CBMatrix> m_projCB;
		std::shared_ptr<CBMatrix> m_worldCB;

		std::shared_ptr<ID3D11SamplerState> m_samplerState;
		std::shared_ptr<ID3D11ShaderResourceView> m_cloudTexture;
		std::shared_ptr<ID3D11ShaderResourceView> m_opacityTexture;

		std::shared_ptr<ID3D11VertexShader> m_vs;
		std::shared_ptr<ID3D11GeometryShader> m_gs;
		std::shared_ptr<ID3D11PixelShader> m_ps;
		std::shared_ptr<ID3D11InputLayout> m_layout;
		
		void UpdateVertexBuffer(std::shared_ptr<ID3D11DeviceContext>& context, DirectX::XMFLOAT4 cameraPos);
	};
}
