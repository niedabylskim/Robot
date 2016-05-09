#ifndef __GK2_ROBOT_H_
#define __GK2_ROBOT_H_

#include "gk2_applicationBase.h"
#include "gk2_meshLoader.h"
#include "gk2_camera.h"
#include "gk2_phongEffect.h"
#include "gk2_lightShadowEffect.h"
#include "gk2_constantBuffer.h"
#include "gk2_particles.h"

namespace gk2
{
	class Robot : public ApplicationBase
	{
	public:
		explicit Robot(HINSTANCE hInstance);
		virtual ~Robot();

		static void* operator new(size_t size);
		static void operator delete(void* ptr);

	protected:
		bool LoadContent() override;
		void UnloadContent() override;

		void Update(float dt) override;
		void Render() override;

	private:
		static const float TABLE_H;
		static const float TABLE_TOP_H;
		static const float TABLE_R;
		static const float DISK_R;
		static const DirectX::XMFLOAT4 TABLE_POS;
		static const unsigned int BS_MASK;
		/*static const unsigned int VB_STRIDE;
		static const unsigned int VB_OFFSET;*/
		Mesh m_parts[6];
		Mesh m_ground;
		Mesh m_plateRight;
		Mesh m_plateLeft;
		Mesh m_disk;
		Mesh m_light;
		Mesh m_cylinder;
		Mesh m_lidCyl1;
		Mesh m_lidCyl2;
		DirectX::XMFLOAT3 disk_normal;
		static float disk_angle;
		static const float DISK_POSITION;
		static const float DISK_ROTATION;
		static const DirectX::XMFLOAT4 lightPosition;
		DirectX::XMMATRIX m_projMtx;

		Camera m_camera;

		std::shared_ptr<CBMatrix> m_worldCB;
		std::shared_ptr<CBMatrix> m_viewCB;
		std::shared_ptr<CBMatrix> m_projCB;
		std::shared_ptr<ConstantBuffer<DirectX::XMFLOAT4>> m_lightPosCB;
		std::shared_ptr<ConstantBuffer<DirectX::XMFLOAT4>> m_surfaceColorCB;
		std::shared_ptr<ConstantBuffer<DirectX::XMFLOAT4>> m_cameraPosCB;
		std::shared_ptr<ConstantBuffer<DirectX::XMFLOAT4>> m_lightAmbient;
		std::shared_ptr<ID3D11DepthStencilState> m_dssWrite;
		std::shared_ptr<ID3D11DepthStencilState> m_dssTest;
		std::shared_ptr<ID3D11RasterizerState> m_rsCounterClockwise;
		std::shared_ptr<PhongEffect> m_phongEffect;
		std::shared_ptr<LightShadowEffect> m_lightShadowEffect;
		std::shared_ptr<ParticleSystem> m_particles;
		std::shared_ptr<ID3D11InputLayout> m_layout;

		std::shared_ptr<ID3D11RasterizerState> m_rsCullNone;
		std::shared_ptr<ID3D11BlendState> m_bsAlpha;
		std::shared_ptr<ID3D11DepthStencilState> m_dssNoWrite;

		void InitializeConstantBuffers();
		void InitializeCamera();
		void InitializeRenderStates();
		void CreateScene();
		void UpdateCamera();
		void UpdateCamera(const XMMATRIX& view) const;
		void UpdateDiskPosition(float dt);
		void inverse_kinematics(XMFLOAT3 pos, XMFLOAT3 normal, float &a1, float &a2, float &a3, float &a4, float &a5);
		void UpdateRobot();
		void DrawMirroredWorld();
		void DrawPlateRight();
		void DrawPlateLeft();
		void DrawGround();
		void DrawScene();
		void DrawDisk();
		void DrawCylinder();
		void DrawRobot();
		void SetLight();
		XMFLOAT3 GetDiscPos();
	};
}

#endif __GK2_ROOM_H_