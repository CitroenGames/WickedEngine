#pragma once

#include "CharacterBody.h"
#include <WickedEngine.h>

#include <SimpleMath.h>
using namespace DirectX::SimpleMath;

class Player : public CharacterBody
{
    static const constexpr float HEIGHT = 1.6;
    static const constexpr float SPEED = 1.0;
    static const constexpr float FLYSPEED = 100.0;

    wi::ecs::Entity mEntity = wi::ecs::INVALID_ENTITY;
    Vector3 cam_move;
    Vector3 mGravity{0, -9.8, 0};
    Vector3 mCurrentVelocity{0, 0, 0};

    bool pointer_captured = false;
    bool mIsFlying = false;
    Vector4 original_mouse_pos{0, 0, 0, 0};
    float mCameraPitch = 0.0;
    float mCameraYaw = 0.0;
    wi::scene::TransformComponent mCameraTransform;

public:
    void Init();

    void Deinit();

    void SetCapturePointer(bool inCapture);

    void Update(float inDT);

    /**
     * Updates the camera rotation and sends the travel direction to the CharacterBody::SetVelocity()
     */
    Vector3 FPSCameraUpdate(float inDT);
};
