#include "pch.h"
#include "Camera.h"

dae::Camera::Camera(float _aspectRatio, float _fovAngle, const Vector3& _origin) :
	m_Origin{ _origin },
	m_FovAngle{ _fovAngle },
	m_Fov{ tanf((m_FovAngle * TO_RADIANS) / 2.f) },
	m_TotalPitch{},
	m_TotalYaw{},
	m_AspectRatio{ _aspectRatio },
	m_NearPlane{ 0.1f },
	m_FarPlane{ 1000.f },
	m_MoveSpeed{ 25.f },
	m_RotSpeed{ 150.f * TO_RADIANS }
{
}

void dae::Camera::CalculateViewMatrix()
{
	//ONB => m_InvViewMatrix
			//Inverse(ONB) => ViewMatrix
	m_Right = Vector3::Cross(Vector3::UnitY, m_Forward);
	m_Up = Vector3::Cross(m_Forward, m_Right);

	//ViewMatrix => Matrix::CreateLookAtLH(...) [not implemented yet]
	//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatlh
	m_ViewMatrix = Matrix::CreateLookAtLH(m_Origin, m_Forward, m_Up);
	m_InvViewMatrix = Matrix::Inverse(m_ViewMatrix);
}

void dae::Camera::CalculateProjectionMatrix()
{
	//ProjectionMatrix => Matrix::CreatePerspectiveFovLH(...) [not implemented yet]
	//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectivefovlh
	m_ProjectionMatrix = Matrix::CreatePerspectiveFovLH(m_Fov, m_AspectRatio, m_FarPlane, m_NearPlane);
}

void dae::Camera::Update(const dae::Timer* pTimer)
{
	//Camera Update Logic

	const float deltaTime = pTimer->GetElapsed();
	const float initFov = m_Fov;

	//Keyboard Input
	const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

	//Mouse Input
	int mouseX{}, mouseY{};
	const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

	const bool keysUsed =
		(
			pKeyboardState[SDL_SCANCODE_UP] |
			pKeyboardState[SDL_SCANCODE_DOWN] |
			pKeyboardState[SDL_SCANCODE_W] |
			pKeyboardState[SDL_SCANCODE_S] |
			pKeyboardState[SDL_SCANCODE_D] |
			pKeyboardState[SDL_SCANCODE_A] |
			pKeyboardState[SDL_SCANCODE_RIGHT] |
			pKeyboardState[SDL_SCANCODE_LEFT]
			);

	if (keysUsed || mouseState & SDL_BUTTON_RMASK || mouseState & SDL_BUTTON_LMASK)
	{

		const bool keyUp = pKeyboardState[SDL_SCANCODE_UP] + pKeyboardState[SDL_SCANCODE_W];
		const bool keyDown = pKeyboardState[SDL_SCANCODE_DOWN] + pKeyboardState[SDL_SCANCODE_S];
		const bool keyLeft = pKeyboardState[SDL_SCANCODE_LEFT] + pKeyboardState[SDL_SCANCODE_A];
		const bool keyRight = pKeyboardState[SDL_SCANCODE_RIGHT] + pKeyboardState[SDL_SCANCODE_D];

		const bool buttonLeft = static_cast<bool>(mouseState & SDL_BUTTON_LMASK);
		const bool buttonRight = static_cast<bool>(mouseState & SDL_BUTTON_RMASK);


		//CALCULATE CAMERA MOVEMENT:
		m_Origin +=
			m_MoveSpeed *                           //Multiply the movement vectors with the speed
			deltaTime *
			(
				//Calculate m_Forward
				(
					(buttonLeft * mouseY *				//MBL: move m_Forward using only mouseY
						(buttonRight - 1)) +				//(Not when MBR is pressed)									
					(keyUp)-							//[UP] & [W] : move m_Forward
					(keyDown)
					) *										//[DOWN] & [S] : move backwards
				m_Forward +

				//Calculate m_Right
				((keyRight)-							//[RIGHT] & [D] : move m_Right
					(keyLeft)) *							//[LEFT] & [A] : move left
				m_Right +

				//Calculate m_Up
				(buttonLeft & buttonRight) *			//MBL & MRB : move m_Up using mouse y
				-mouseY *
				m_Up
				);

		//CALCULATE CAMERA ROTATION:
		m_TotalYaw += m_RotSpeed * deltaTime * (mouseX * (buttonLeft ^ buttonRight));				//MBL xor MBR:  move camera yaw using mouseX
		m_TotalPitch += m_RotSpeed * deltaTime * (-mouseY) * (buttonRight);							//MBR : move camera pitch using mouseY

		m_Forward =
			Matrix::CreateRotation(
				m_TotalPitch,
				m_TotalYaw,
				0).TransformVector(Vector3::UnitZ);	// Matrix magic

		m_Forward.Normalize();
	}

	//checks for angle change
	if (std::abs(initFov - m_FovAngle) >= 0.00001f)
	{
		m_Fov = std::atanf(m_FovAngle);
	}

	//Update Matrices
	CalculateViewMatrix();
	if (abs(initFov - m_Fov) > 0.0001f)
	{
		CalculateProjectionMatrix(); //Try to optimize this - should only be called once or when m_Fov/m_AspectRatio changes
	}
}
