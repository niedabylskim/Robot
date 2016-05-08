#ifndef __GK2_CAMERA_H_
#define __GK2_CAMERA_H_

#include <DirectXMath.h>
using namespace DirectX;
namespace gk2
{
	class Camera
	{
	public:
		Camera(float minDistance, float maxDistance, float distance =  0.0f);
		DirectX::XMMATRIX GetViewMatrix() const;
		void GetViewMatrix(DirectX::XMMATRIX& viewMatrix) const;
		void Rotate(float dx, float dy);
		void Move(XMFLOAT3 v);
		XMFLOAT4 GetPosition() const;
	private:
		float m_angleX;
		float m_angleY;
		XMFLOAT4 m_position;
	};
}

#endif __GK2_CAMERA_H_
