#ifndef __GK2_MESH_LOADER_H_
#define __GK2_MESH_LOADER_H_

#include "gk2_deviceHelper.h"
#include "gk2_mesh.h"
#include <string>

namespace gk2
{
	class MeshLoader
	{
	public:
		explicit MeshLoader(const DeviceHelper& device): m_device(device) { }
		Mesh LoadMesh(const std::wstring& fileName);
		Mesh GetQuad(float side);
		Mesh GetDisc(int slices, float radius /* = 0.5f */);
		Mesh GetSphere(int stacks, int slices, float radius /* = 0.5f */);

	private:
		DeviceHelper m_device;
	};
}

#endif __GK2_MESH_LOADER_H_
