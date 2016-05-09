#ifndef __GK2_MESH_H_
#define __GK2_MESH_H_

#include <d3d11.h>
#include <memory>
#include <DirectXMath.h>
#include <vector>
#include "gk2_vertices.h"
#include "gk2_deviceHelper.h"

namespace gk2
{
	class Mesh
	{
	public:
		Mesh(std::shared_ptr<ID3D11Buffer> vb, unsigned int stride,
			 std::shared_ptr<ID3D11Buffer> ib, unsigned int indicesCount);
		Mesh();
		Mesh(const Mesh& right);

		Mesh(std::shared_ptr<ID3D11Buffer> vb, unsigned int stride, std::shared_ptr<ID3D11Buffer> ib, unsigned int indicesCount, std::vector<VertexPosNormal> vertices, std::vector<unsigned short> indices, std::vector<std::vector<int>> edges);

		const DirectX::XMMATRIX& getWorldMatrix() const { return m_worldMtx; }
		void setWorldMatrix(const DirectX::XMMATRIX& mtx) { m_worldMtx = mtx; }
		void Render(const std::shared_ptr<ID3D11DeviceContext>& context) const;

		Mesh& operator =(const Mesh& right);

		static void* operator new(size_t size);
		static void operator delete(void* ptr);

		std::vector<VertexPosNormal> m_vertices;
		std::vector<unsigned short> m_indices;
		std::vector<std::vector<int>> m_edges;
		Mesh BuildShadowVolume(DeviceHelper m_device, DirectX::XMFLOAT4 light);

	private:
		std::shared_ptr<ID3D11Buffer> m_vertexBuffer;
		std::shared_ptr<ID3D11Buffer> m_indexBuffer;
		unsigned int m_stride;
		unsigned int m_indicesCount;
		DirectX::XMMATRIX m_worldMtx;
		VertexPosNormal *n;
		unsigned short *ind;
	};
}

#endif __GK2_MESH_H_
