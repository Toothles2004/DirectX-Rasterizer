#pragma once
#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include "Timer.h"

namespace dae
{
	struct Camera
	{
	public:
		Camera() = default;

		Camera(const Vector3& origin, float aspectRatio, float fovAngle = 90.f);

		void Initialize(float aspectRatio, float fovAngle = 90.f, Vector3 origin = { 0.f,0.f,0.f });

		void CalculateViewMatrix();
		void CalculateProjectionMatrix();
		void Update(Timer* pTimer);

	private:
		Vector3 m_Origin{};
		float m_FovAngle;
		float m_Fov;
		float m_AspectRatio;

		float m_TotalPitch;
		float m_TotalYaw;

		float m_NearPlane;
		float m_FarPlane;

		Vector3 m_Forward{};
		Vector3 m_Up{};
		Vector3 m_Right{};

		Matrix m_InvViewMatrix{};
		Matrix m_ViewMatrix{};
		Matrix m_ProjectionMatrix{};
	};
}

