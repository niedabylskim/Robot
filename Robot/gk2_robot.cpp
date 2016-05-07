#include "gk2_robot.h"
#include "gk2_window.h"
#include "gk2_utils.h"
#include <array>
#include <iostream>
#include <sstream>
#include <atlbase.h>
using namespace std;
using namespace gk2;
using namespace DirectX;

const unsigned int Robot::BS_MASK = 0xffffffff;
const float Robot::DISK_R = 0.3f;
const float Robot::DISK_POSITION = -1.51f;
const float Robot::DISK_ROTATION = XM_PI / 4.0f;
const XMFLOAT4 Robot::lightPosition = XMFLOAT4(-2, 2, -5,1);
float Robot::disk_angle = 0.0f;
Robot::Robot(HINSTANCE hInstance)
	: ApplicationBase(hInstance), m_camera(0.001f, 100.0f)
{

}

Robot::~Robot()
{

}

void* Robot::operator new(size_t size)
{
	return Utils::New16Aligned(size);
}

void Robot::operator delete(void* ptr)
{
	Utils::Delete16Aligned(ptr);
}

void Robot::InitializeConstantBuffers()
{
	m_projCB = make_shared<CBMatrix>(m_device);
	m_viewCB = make_shared<CBMatrix>(m_device);
	m_worldCB = make_shared<CBMatrix>(m_device);
	m_lightPosCB = make_shared<ConstantBuffer<XMFLOAT4>>(m_device);
	m_surfaceColorCB = make_shared<ConstantBuffer<XMFLOAT4>>(m_device);
	m_cameraPosCB = make_shared<ConstantBuffer<XMFLOAT4>>(m_device);
	m_lightAmbient = make_shared<ConstantBuffer<XMFLOAT4>>(m_device);
}

void Robot::InitializeCamera()
{
	auto s = getMainWindow()->getClientSize();
	auto ar = static_cast<float>(s.cx) / s.cy;
	m_projMtx = XMMatrixPerspectiveFovLH(XM_PIDIV4, ar, 0.01f, 100.0f);
	m_projCB->Update(m_context, m_projMtx);
	UpdateCamera();
}

void Robot::CreateScene()
{
	MeshLoader loader(m_device);

	m_parts[0] = loader.LoadMesh(L"resources/meshes/mesh1.txt");
	m_parts[1] = loader.LoadMesh(L"resources/meshes/mesh2.txt");
	m_parts[2] = loader.LoadMesh(L"resources/meshes/mesh3.txt");
	m_parts[3] = loader.LoadMesh(L"resources/meshes/mesh4.txt");
	m_parts[4] = loader.LoadMesh(L"resources/meshes/mesh5.txt");
	m_parts[5] = loader.LoadMesh(L"resources/meshes/mesh6.txt");
	m_ground = loader.GetQuad(1.0f);
	m_ground.setWorldMatrix(XMMatrixTranslation(0,-1,0));
	m_ground = loader.GetQuad(1.0f);
	m_light = loader.GetSphere(100,100,0.5);
	m_light.setWorldMatrix(XMMatrixTranslation(lightPosition.x, lightPosition.y, lightPosition.z));
	m_ground.setWorldMatrix(XMMatrixTranslation(0, -1.01f, 0));
	m_plate = loader.GetQuad(.1f);
	m_plate.setWorldMatrix(XMMatrixRotationZ(XM_PIDIV2 + DISK_ROTATION) * XMMatrixTranslation(DISK_POSITION-0.01f, 0.0f, 0.0f));
	m_disk = loader.GetDisc(50, 0.02f);
	m_lidCyl1 = loader.GetDisc(50, 0.5f);
	m_lidCyl1.setWorldMatrix(XMMatrixRotationZ(XM_PIDIV2) * XMMatrixTranslation(-lightPosition.x-0.75f, -0.5f, 2.0f));
	m_lidCyl2 = loader.GetDisc(50, 0.5f);
	m_lidCyl2.setWorldMatrix(XMMatrixRotationZ(XM_PIDIV2) * XMMatrixTranslation(-lightPosition.x + 0.75f, -0.5f, 2.0f));
	m_disk.setWorldMatrix(XMMatrixRotationZ(XM_PIDIV2 + DISK_ROTATION) * XMMatrixTranslation(DISK_POSITION, 0.0f, 0.0f));
	m_cylinder = loader.GetCylinder(50, 50, 0.5f, 1.5f);
	m_cylinder.setWorldMatrix(XMMatrixRotationZ(XM_PIDIV2)* XMMatrixTranslation(-lightPosition.x, -0.5f, 2.0f));
}

// to jeszcze nie uzywane, pozniej sie moze przydac
void Robot::InitializeRenderStates()
{
	auto rsDesc = m_device.DefaultRasterizerDesc();
	rsDesc.CullMode = D3D11_CULL_BACK;
	m_rsCullNone = m_device.CreateRasterizerState(rsDesc);

	/*auto bsDesc = m_device.DefaultBlendDesc();
	bsDesc.RenderTarget[0].BlendEnable = true;
	bsDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bsDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bsDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	m_bsAlpha = m_device.CreateBlendState(bsDesc);

	auto dssDesc = m_device.DefaultDepthStencilDesc();
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	m_dssNoWrite = m_device.CreateDepthStencilState(dssDesc);*/
}

bool Robot::LoadContent()
{
	InitializeConstantBuffers();
	InitializeCamera();
	InitializeRenderStates();
	CreateScene();
	m_phongEffect = make_shared<PhongEffect>(m_device, m_layout);
	m_phongEffect->SetProjMtxBuffer(m_projCB);
	m_phongEffect->SetViewMtxBuffer(m_viewCB);
	m_phongEffect->SetWorldMtxBuffer(m_worldCB);
	m_lightPosCB->Update(m_context, lightPosition);
	m_phongEffect->SetLightPosBuffer(m_lightPosCB);
	m_phongEffect->SetSurfaceColorBuffer(m_surfaceColorCB);
	m_phongEffect->SetAmbientBuffer(m_lightAmbient);
	disk_normal = XMFLOAT3(0, 1, 0);
	XMVECTOR v = XMLoadFloat3(&disk_normal);
	XMVECTOR v1 = XMVector3TransformCoord(v, XMMatrixRotationZ(-XM_PI / 4.0f)* XMMatrixTranslation(-1.50f, 0, 0));
	XMStoreFloat3(&disk_normal, v1);
	disk_normal.x = -disk_normal.x;
	return true;
}

void Robot::UnloadContent()
{

}

void Robot::UpdateCamera()
{
	XMMATRIX viewMtx[2];
	m_camera.GetViewMatrix(viewMtx[0]);
	XMVECTOR det;
	viewMtx[1] = XMMatrixInverse(&det, viewMtx[0]);
	m_viewCB->Update(m_context, viewMtx);
}


void gk2::Robot::UpdateDiskPosition(float dt)
{
	disk_angle = XMScalarModAngle(disk_angle + 3 * dt);
	XMFLOAT3 p = XMFLOAT3(0, DISK_R * sin(disk_angle), DISK_R * cos(disk_angle));
	XMVECTOR v = XMLoadFloat3(&p);
	XMVECTOR vp = XMVector3TransformCoord(v, XMMatrixRotationZ(DISK_ROTATION));
	XMStoreFloat3(&p, vp);
	m_disk.setWorldMatrix(XMMatrixRotationZ(XM_PIDIV2 + DISK_ROTATION) * XMMatrixTranslation(DISK_POSITION + p.x, p.y, p.z));
}

void gk2::Robot::inverse_kinematics(XMFLOAT3 pos, XMFLOAT3 normal, float &a1, float &a2, float &a3, float &a4, float &a5)
{
	float l1 = .91f, l2 = .81f, l3 = .33f, dy = .27f, dz = .26f;

	float normalL = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
	normal.x /= normalL;
	normal.y /= normalL;
	normal.z /= normalL;

	XMFLOAT3 pos1 = XMFLOAT3(pos.x + normal.x * l3, pos.y + normal.y * l3, pos.z + normal.z * l3);

	float e = sqrtf(pos1.z * pos1.z + pos1.x * pos1.x - dz * dz);
	a1 = atan2(pos1.z, -pos1.x) + atan2(dz, e);

	XMFLOAT3 pos2(e, pos1.y - dy, .0f);
	a3 = -acosf(min(1.0f, (pos2.x*pos2.x + pos2.y*pos2.y - l1*l1 - l2*l2) / (2.0f*l1*l2)));

	float k = l1 + l2 * cosf(a3), l = l2 * sinf(a3);
	a2 = -atan2(pos2.y, sqrtf(pos2.x*pos2.x + pos2.z*pos2.z)) - atan2(l, k);

	XMFLOAT3 normal1;
	XMVECTOR normal1vec = XMLoadFloat3(&normal);
	XMVECTOR normal1vecRot = XMVector3Transform(normal1vec, XMMatrixRotationY(-a1));
	XMStoreFloat3(&normal1, normal1vecRot);
	normal1vec = XMLoadFloat3(&normal1);
	normal1vecRot = XMVector3Transform(normal1vec, XMMatrixRotationZ(-(a2 + a3)));
	XMStoreFloat3(&normal1, normal1vecRot);

	a5 = acosf(normal1.x);
	a4 = atan2(normal1.z, normal1.y);
}

void Robot::Update(float dt)
{
	static MouseState prevState;
	MouseState currentState;
	if (m_mouse->GetState(currentState))
	{
		auto change = true;
		if (prevState.isButtonDown(0))
		{
			auto d = currentState.getMousePositionChange();
			m_camera.Rotate(d.y/300.f, d.x/300.f);
		}
		else if (prevState.isButtonDown(1))
		{
			auto d = currentState.getMousePositionChange();
			m_camera.Move(XMFLOAT3(d.x/100.0f,d.y/100.0f,0.0f));
		}
		else if (prevState.isButtonDown(2))
		{
			auto d = currentState.getMousePositionChange();
			m_camera.Move(XMFLOAT3(0, 0, d.y / 10.0f));
		}
		else
			change = false;
		prevState = currentState;
		if (change)
			UpdateCamera();
	}
	UpdateDiskPosition(dt);
	UpdateRobot();
}

void Robot::UpdateRobot()
{
	float a1, a2, a3, a4, a5;
	float l1 = .91f, l2 = .81f, l3 = .33f, dy = .27f, dz = -.26f;
	XMMATRIX m = XMMatrixTranslation(-1.50f, 0, 0);
	XMFLOAT3 pos = XMFLOAT3(0, 0, 0);
	XMVECTOR v = XMLoadFloat3(&pos);
	XMVECTOR v1 = XMVector3TransformCoord(v, m);
	XMStoreFloat3(&pos, v1);

	XMFLOAT3 p = XMFLOAT3(0, DISK_R * sin(disk_angle), -DISK_R * cos(disk_angle));
	v = XMLoadFloat3(&p);
	v1 = XMVector3TransformCoord(v, XMMatrixRotationZ(DISK_ROTATION));
	XMStoreFloat3(&p, v1);
	v1 = XMVector3TransformCoord(XMLoadFloat3(&pos), XMMatrixTranslation(p.x, p.y, p.z));
	XMStoreFloat3(&pos, v1);
	
	inverse_kinematics(pos, disk_normal, a1, a2, a3, a4, a5);

	m_parts[0].setWorldMatrix(XMMatrixScaling(1, 1, -1));
	m_parts[1].setWorldMatrix(XMMatrixRotationY(a1)*m_parts[0].getWorldMatrix());
	m_parts[2].setWorldMatrix(XMMatrixTranslation(0, -dy, 0)* XMMatrixRotationZ(a2)*XMMatrixTranslation(0, +dy, 0)*m_parts[1].getWorldMatrix());
	m_parts[3].setWorldMatrix(XMMatrixTranslation(l1, -dy, 0)*XMMatrixRotationZ(a3)*XMMatrixTranslation(-l1, dy, 0)*m_parts[2].getWorldMatrix());
	m_parts[4].setWorldMatrix(XMMatrixTranslation(0, -dy, -dz)*XMMatrixRotationX(a4)*XMMatrixTranslation(0, dy, dz)*m_parts[3].getWorldMatrix());
	m_parts[5].setWorldMatrix(XMMatrixTranslation(l1 + l2, -dy, 0)*XMMatrixRotationZ(a5)*XMMatrixTranslation(-(l1 + l2), dy, 0) *m_parts[4].getWorldMatrix());
}

void Robot::DrawScene()
{
	m_context->RSSetState(m_rsCullNone.get());
	m_surfaceColorCB->Update(m_context, XMFLOAT4(1.0f, 0.4f, 0.0f, 1.0f));
	m_lightAmbient->Update(m_context, XMFLOAT4(1, 1, 1, 1));
	m_worldCB->Update(m_context, m_light.getWorldMatrix());
	m_light.Render(m_context);
	m_lightAmbient->Update(m_context, XMFLOAT4(0.2, 0.2, 0.2, 1));
	m_surfaceColorCB->Update(m_context, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	for (int i = 0; i < 6; i++)
	{
		m_worldCB->Update(m_context, m_parts[i].getWorldMatrix());
		m_parts[i].Render(m_context);
	}
	m_surfaceColorCB->Update(m_context, XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	m_worldCB->Update(m_context, m_ground.getWorldMatrix());
	m_ground.Render(m_context);
	m_surfaceColorCB->Update(m_context, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_worldCB->Update(m_context, m_plate.getWorldMatrix());
	m_plate.Render(m_context);
	m_surfaceColorCB->Update(m_context, XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));
	m_worldCB->Update(m_context, m_disk.getWorldMatrix());
	m_disk.Render(m_context);
	m_worldCB->Update(m_context, m_cylinder.getWorldMatrix());
	m_cylinder.Render(m_context);
	m_worldCB->Update(m_context, m_lidCyl1.getWorldMatrix());
	m_lidCyl1.Render(m_context);
	m_worldCB->Update(m_context, m_lidCyl2.getWorldMatrix());
	m_lidCyl2.Render(m_context);
	m_context->RSSetState(nullptr);
}

void Robot::Render()
{
	if (m_context == nullptr) return;
	UpdateCamera();
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_context->ClearRenderTargetView(m_backBuffer.get(), clearColor);
	m_context->ClearDepthStencilView(m_depthStencilView.get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_projCB->Update(m_context, m_projMtx);
	m_phongEffect->Begin(m_context);
	DrawScene();
	m_phongEffect->End();
	m_swapChain->Present(0, 0);
}

void Robot::DrawMirroredWorld()
{
}