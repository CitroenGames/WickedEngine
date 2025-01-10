#include "Player.h"
#include <algorithm>
#include <numbers>

constexpr float PI = 3.14159265358979323846f;

void Player::Init()
{
    wi::scene::Scene& scene = wi::scene::GetScene();
    mEntity = wi::ecs::CreateEntity();
    scene.transforms.Create(mEntity);

    wi::scene::TransformComponent* transform = scene.transforms.GetComponent(mEntity);
    transform->Translate(Vector3{0.f, 10.f, 0.f});
    transform->UpdateTransform();

    mCameraTransform.ClearTransform();
    mCameraTransform.Translate(transform->GetPosition());
    mCameraTransform.RotateRollPitchYaw(Vector3(mCameraPitch, mCameraYaw, /*roll*/ 0));
    mCameraTransform.UpdateTransform();
    wi::scene::GetCamera().TransformCamera(mCameraTransform);
    original_mouse_pos = wi::input::GetPointer();

    CharacterBody::Init(mEntity, HEIGHT);
}

void Player::Deinit()
{
    wi::scene::Scene& scene = wi::scene::GetScene();
    scene.transforms.Remove(mEntity);

    scene.Entity_Remove(mEntity);
}

void Player::SetCapturePointer(bool inCapture)
{
    pointer_captured = inCapture;
    if (pointer_captured) original_mouse_pos = wi::input::GetPointer();
    wi::input::HidePointer(inCapture);
}

void Player::Update(float inDT)
{
    if (wi::input::Press((wi::input::KEYBOARD_BUTTON_ESCAPE))) SetCapturePointer(!pointer_captured);
    if (pointer_captured) wi::input::SetPointer(original_mouse_pos);

    if (wi::input::Press((wi::input::KEYBOARD_BUTTON_F1))) mIsFlying = !mIsFlying;

    Vector3 inputMovement = FPSCameraUpdate(inDT);
    if (!mIsFlying) {
        SetVelocity(inputMovement + mCurrentVelocity);
        CharacterBody::MoveAndSlide(inDT);
        if (IsOnFloor()) mCurrentVelocity = Vector3{0, 0, 0};
        else mCurrentVelocity += mGravity * inDT;
    }

    wi::scene::TransformComponent* transform = wi::scene::GetScene().transforms.GetComponent(mEntity);
    mCameraTransform = *transform;
    mCameraTransform.RotateRollPitchYaw(Vector3(mCameraPitch, 0, /*roll*/ 0));
    mCameraTransform.UpdateTransform();
    wi::scene::GetCamera().TransformCamera(mCameraTransform);
    wi::scene::GetCamera().SetDirty();
}

DirectX::SimpleMath::Vector3 Player::FPSCameraUpdate(float inDT)
{
    wi::backlog::post("Player::FPSCameraUpdate: " + std::to_string(inDT));
    Vector3 desiredMovement{0, 0, 0};

    wi::scene::Scene& scene = wi::scene::GetScene();
    wi::scene::TransformComponent* transform = scene.transforms.GetComponent(mEntity);

    float xDif = 0, yDif = 0;

    if (pointer_captured) {
        xDif = wi::input::GetMouseState().delta_position.x;
        yDif = wi::input::GetMouseState().delta_position.y;
        xDif = 0.1f * xDif * (1.0f / 60.0f);
        yDif = 0.1f * yDif * (1.0f / 60.0f);
    }

    const Vector4 leftStick = wi::input::GetAnalog(wi::input::GAMEPAD_ANALOG_THUMBSTICK_L, 0);
    const Vector4 rightStick = wi::input::GetAnalog(wi::input::GAMEPAD_ANALOG_THUMBSTICK_R, 0);
    const Vector4 rightTrigger = wi::input::GetAnalog(wi::input::GAMEPAD_ANALOG_TRIGGER_R, 0);

    const float jostickrotspeed = 0.005f;
    xDif += rightStick.x * jostickrotspeed;
    yDif += rightStick.y * jostickrotspeed;

    // FPS Camera
    // if dt > 100 millisec, don't allow the camera to jump too far...
    const float clampedDT = std::min(inDT, 0.1f);

    const float speed = ((wi::input::Down(wi::input::KEYBOARD_BUTTON_LSHIFT) ? 10.0f : 1.0f) + rightTrigger.x * 10.0f)
      * (mIsFlying ? FLYSPEED : SPEED);
    Vector3 move = cam_move;
    Vector3 moveNew{0, 0, 0};

    if (wi::input::Down((wi::input::BUTTON)'A') || wi::input::Down(wi::input::GAMEPAD_BUTTON_LEFT)) {
        moveNew += Vector3(-1, 0, 0);
    }
    if (wi::input::Down((wi::input::BUTTON)'D') || wi::input::Down(wi::input::GAMEPAD_BUTTON_RIGHT)) {
        moveNew += Vector3(1, 0, 0);
    }
    if (wi::input::Down((wi::input::BUTTON)'W') || wi::input::Down(wi::input::GAMEPAD_BUTTON_UP)) {
        moveNew += Vector3(0, 0, 1);
    }
    if (wi::input::Down((wi::input::BUTTON)'S') || wi::input::Down(wi::input::GAMEPAD_BUTTON_DOWN)) {
        moveNew += Vector3(0, 0, -1);
    }
    if (wi::input::Down((wi::input::BUTTON)'E') || wi::input::Down(wi::input::GAMEPAD_BUTTON_2)) {
        moveNew += Vector3(0, 1, 0);
    }
    if (wi::input::Down((wi::input::BUTTON)'Q') || wi::input::Down(wi::input::GAMEPAD_BUTTON_1)) {
        moveNew += Vector3(0, -1, 0);
    }
    moveNew.Normalize();
    moveNew += Vector3{leftStick.x, 0, leftStick.y};
    moveNew *= speed * clampedDT;

    move = Vector3::Lerp(move, moveNew, std::clamp(0.18f * clampedDT * 60.0f, 0.0f, 1.0f)); // smooth the movement a bit
    float moveLength = move.Length();

    if (moveLength < 0.0001f) {
        move = Vector3{0, 0, 0};
    }

    if (std::abs(xDif) + std::abs(yDif) > 0 || moveLength > 0.0001f) {
        mCameraPitch = mCameraPitch = std::clamp(mCameraPitch + yDif, -PI * 0.49f, PI * 0.49f);
        mCameraYaw += xDif;

        Matrix camRot = Matrix::CreateFromQuaternion((mIsFlying ? &mCameraTransform : transform)->rotation_local);
        Vector3 move_rot = Vector3::TransformNormal(move, camRot);
        if (mIsFlying) {
            transform->Translate(move_rot);
        } else {
            desiredMovement = move_rot / inDT;
        }

        // pitch - yaw - roll
        transform->RotateRollPitchYaw(Vector3(0, xDif, 0));
    }

    transform->UpdateTransform();
    cam_move = move;
    return desiredMovement;
}
