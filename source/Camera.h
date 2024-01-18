#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include "Timer.h"
#include "pch.h"

namespace dae
{
	class Camera final
	{
	public:
		Camera() = default;
		Camera(float _aspectRatio, float _fovAngle = 90.f, const Vector3& _origin = { 0.f,0.f,0.f });

		void CalculateViewMatrix();
		void CalculateProjectionMatrix();

		void Update(const Timer* pTimer);
		Matrix GetViewMatrix() const { return m_ViewMatrix; }
		Matrix GetProjectionMatrix() const { return m_ProjectionMatrix; }

	private:
		Vector3 m_Origin{};
		float m_FovAngle{ 90.f };
		float m_Fov;

		Vector3 m_Forward{ Vector3::UnitZ };
		Vector3 m_Up{ Vector3::UnitY };
		Vector3 m_Right{ Vector3::UnitX };

		float m_TotalPitch;
		float m_TotalYaw;

		Matrix m_InvViewMatrix{};
		Matrix m_ViewMatrix{};
		Matrix m_ProjectionMatrix{};

		float m_AspectRatio;

		float m_NearPlane;
		float m_FarPlane;
	};
}