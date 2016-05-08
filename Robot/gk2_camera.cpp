#include "gk2_camera.h"

using namespace gk2;
using namespace DirectX;

Camera::Camera(float minDistance, float maxDistance, float distance)
	: m_angleX(0.0f), m_angleY(0.0f)
{
	m_position = XMFLOAT4(0.0f, 0.0f, -5.0f, 1.0f);
}

void Camera::Rotate(float dx, float dy)
{
	m_angleX += dx;
	m_angleY += dy;
}

XMMATRIX Camera::GetViewMatrix() const
{
	XMMATRIX viewMtx;
	GetViewMatrix(viewMtx);
	return viewMtx;
}

void Camera::GetViewMatrix(XMMATRIX& viewMtx) const
{
	viewMtx = XMMatrixTranslation(-m_position.x, -m_position.y, -m_position.z)*
	XMMatrixRotationY(-m_angleY) * XMMatrixRotationX(-m_angleX);
}

void Camera::Move(XMFLOAT3 v)
{
	XMVECTOR pos1 = XMVector3TransformCoord(XMLoadFloat3(&v), XMMatrixRotationX(m_angleX) * XMMatrixRotationY(m_angleY));
	XMFLOAT3 p;
	XMStoreFloat3(&p, pos1);
	m_position.x += p.x;
	m_position.y += p.y;
	m_position.z += p.z;
}

XMFLOAT4 Camera::GetPosition() const
{
	XMMATRIX viewMtx;
	GetViewMatrix(viewMtx);
	XMVECTOR det;
	viewMtx = XMMatrixInverse(&det, viewMtx);
	//auto alt = XMMatrixTranslation(0.0f, 0.0f, -m_distance) * XMMatrixRotationY(m_angleY) * XMMatrixRotationX(-m_angleX);
	XMFLOAT3 res(0.0f, 0.0f, 0.0f);
	auto transl = XMVector3TransformCoord(XMLoadFloat3(&res), viewMtx);
	XMStoreFloat3(&res, transl);
	return XMFLOAT4(res.x, res.y, res.z, 1.0f);
}

