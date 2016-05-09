#include "gk2_mesh.h"
#include "gk2_utils.h"

using namespace gk2;
using namespace std;
using namespace DirectX;

Mesh::Mesh(shared_ptr<ID3D11Buffer> vb, unsigned int stride, shared_ptr<ID3D11Buffer> ib, unsigned int indicesCount)
	: m_vertexBuffer(vb), m_indexBuffer(ib), m_stride(stride), m_indicesCount(indicesCount)
{
	m_worldMtx = XMMatrixIdentity();
}

Mesh::Mesh()
	: m_stride(0), m_indicesCount(0)
{
	m_worldMtx = XMMatrixIdentity();
}

Mesh::Mesh(const Mesh& right)
	: m_vertexBuffer(right.m_vertexBuffer), m_indexBuffer(right.m_indexBuffer),
	  m_stride(right.m_stride), m_indicesCount(right.m_indicesCount)
{
	m_worldMtx = XMMatrixIdentity();
}

Mesh::Mesh(std::shared_ptr<ID3D11Buffer> vb, unsigned int stride, std::shared_ptr<ID3D11Buffer> ib, unsigned int indicesCount,
	std::vector<VertexPosNormal> vertices, std::vector<unsigned short> indices, std::vector<std::vector<int>> edges)
	: m_vertexBuffer(vb), m_indexBuffer(ib), m_stride(stride), m_indicesCount(indicesCount), m_vertices(vertices), m_indices(indices), m_edges(edges)
{
	m_worldMtx = XMMatrixIdentity();
	n = new VertexPosNormal[4000];
	ind = new unsigned short[4000];
}

void* Mesh::operator new(size_t size)
{
	return Utils::New16Aligned(size);
}

void Mesh::operator delete(void* ptr)
{
	Utils::Delete16Aligned(ptr);
}

Mesh& Mesh::operator =(const Mesh& right)
{
	m_vertexBuffer = right.m_vertexBuffer;
	m_stride = right.m_stride;
	m_indexBuffer = right.m_indexBuffer;
	m_indicesCount = right.m_indicesCount;
	m_worldMtx = right.m_worldMtx;
	m_edges = right.m_edges;
	m_vertices = right.m_vertices;
	m_indices = right.m_indices;
	n = right.n;
	ind = right.ind;
	return *this;
}

Mesh gk2::Mesh::BuildShadowVolume(DeviceHelper m_device, DirectX::XMFLOAT4 light)
{
	auto m = getWorldMatrix();
	auto mI = XMMatrixInverse(NULL, m);
	XMVECTOR lightVec = XMLoadFloat4(&light);
	size_t trianglesCount = m_indices.size() / 3;
	size_t size = m_edges.size();
	vector<bool> visTriangles(trianglesCount);
	lightVec = XMVector4Transform(lightVec, mI);
	int k = 0, l = 0;
	vector<unsigned short> shadowEdges;
	for (int i = 0; i < trianglesCount; i++)
	{
		XMFLOAT3 p1 = m_vertices[m_indices[3 * i]].Pos;
		XMFLOAT3 p2 = m_vertices[m_indices[3 * i + 1]].Pos;
		XMFLOAT3 p3 = m_vertices[m_indices[3 * i + 2]].Pos;
		XMFLOAT3 vcross1 = XMFLOAT3(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
		XMFLOAT3 vcross2 = XMFLOAT3(p3.x - p1.x, p3.y - p1.y, p3.z - p1.z);
		XMVECTOR normal = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&vcross1), XMLoadFloat3(&vcross2)));
		visTriangles[i] = XMVector3Dot(normal, lightVec).m128_f32[0] >= 0.0f;
		if (visTriangles[i])
		{
			n[k] = m_vertices[m_indices[3 * i]];
			n[k + 1] = m_vertices[m_indices[3 * i + 1]];
			n[k + 2] = m_vertices[m_indices[3 * i + 2]];
			ind[l++] = k + 2;
			ind[l++] = k + 1;
			ind[l++] = k;
			k += 3;
			VertexPosNormal v1, v2, v3;
			auto vec1 = XMLoadFloat3(&p1);
			auto vec2 = XMLoadFloat3(&p2);
			auto vec3 = XMLoadFloat3(&p3);
			XMStoreFloat3(&v1.Pos, vec1 + vec1 - lightVec);
			XMStoreFloat3(&v2.Pos, vec2 + vec2 - lightVec);
			XMStoreFloat3(&v3.Pos, vec3 + vec3 - lightVec);
			n[k] = v1;
			n[k + 1] = v2;
			n[k + 2] = v3;
			ind[l++] = k;
			ind[l++] = k + 1;
			ind[l++] = k + 2;
			k += 3;
		}
	}
	for (int i = 0; i < size; i++)
		if (visTriangles[m_edges[i][2]] != visTriangles[m_edges[i][3]])
		{
			shadowEdges.push_back(i);
		}

	for (int i = 0; i < shadowEdges.size(); i++)
	{
		VertexPosNormal v1, v2, v3, v4;
		v1.Pos = m_vertices[m_edges[shadowEdges[i]][0]].Pos;
		v2.Pos = m_vertices[m_edges[shadowEdges[i]][1]].Pos;
		auto vec1 = XMLoadFloat3(&v1.Pos);
		auto vec2 = XMLoadFloat3(&v2.Pos);
		XMVECTOR vec3 = vec1 + vec1 - lightVec;
		XMVECTOR vec4 = vec2 + vec2 - lightVec;
		XMStoreFloat3(&v3.Pos, vec3);
		XMStoreFloat3(&v4.Pos, vec4);
		n[k] = v1;
		n[k + 1] = v2;
		n[k + 2] = v3;
		n[k + 3] = v4;
		ind[l++] = k + 1;
		ind[l++] = k + 2;
		ind[l++] = k;
		ind[l++] = k + 1;
		ind[l++] = k + 3;
		ind[l++] = k + 2;
		k += 4;
	}
	vector<VertexPosNormal> newVertices(n, n + k);
	vector<unsigned short> newIndices(ind, ind + l);
	return Mesh(m_device.CreateVertexBuffer(newVertices), sizeof(VertexPosNormal),
		m_device.CreateIndexBuffer(newIndices), newIndices.size());
}

void Mesh::Render(const shared_ptr<ID3D11DeviceContext>& context) const
{
	if (!m_vertexBuffer || !m_indexBuffer || !m_indicesCount)
		return;
	context->IASetIndexBuffer(m_indexBuffer.get(), DXGI_FORMAT_R16_UINT, 0);
	auto b = m_vertexBuffer.get();
	unsigned int offset = 0;
	context->IASetVertexBuffers(0, 1, &b, &m_stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->DrawIndexed(m_indicesCount, 0, 0);
}
