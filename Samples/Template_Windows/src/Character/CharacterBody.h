#pragma once

#include <WickedEngine.h>
#include "SimpleMath.h"
using namespace DirectX::SimpleMath;

class CharacterBody
{
    // used to filter out weapon from intersection checks
    static const uint32_t FIRST_PERSON_LAYER = 1 << 0;
    // How much slopeiness will cause the character to slide down instead of standing on it
    static const constexpr float SLOPE_THRESHOLD = 0.2;
    //
    static const constexpr float CAPSULE_RADIUS = 0.4;
    // Collision detection and resolution timestep
    static const constexpr float FIXED_COLLISION_DT = 1.0 / 120.0; // 120FPS

    //--------------------------------------
    // Input values
    float mHeight = 1.5;
    Vector3 mVelocity{0, 0, 0};
    Vector3 mUp{0, 1, 0};
    wi::ecs::Entity mEntity = wi::ecs::INVALID_ENTITY;

    // Internal memory
    wi::ecs::Entity mColliderEntity = wi::ecs::INVALID_ENTITY;
    float mCollisionUpdateDeltaTarget = 0;

    // Output values
    bool mIsOnFloor = false;
    bool mIsOnWall = false;

public:
    void Init(wi::ecs::Entity inEntity, float inHeight);

    inline void Deinit();

    inline const Vector3& GetVelocity() const { return mVelocity; }
    inline wi::ecs::Entity GetEntity() const { return mEntity; }

    inline void SetVelocity(const Vector3& inVelocity) { mVelocity = inVelocity; }
    inline void SetEntity(wi::ecs::Entity inEntity) { mEntity = inEntity; }
    inline void SetUp(const Vector3& inUp) { mUp = inUp; }

    inline bool IsOnFloor() const { return mIsOnFloor; }
    inline bool IsOnWall() const { return mIsOnWall; }

    /**
     * FROM GODOT
     *
     * Moves the body based on velocity. If the body collides with another, it will slide along the other body rather
     * than stop immediately. If the other body is a CharacterBody3D or RigidBody3D, it will also be affected by the
     * motion of the other body. You can use this to make moving and rotating platforms, or to make nodes push other
     * nodes
     *
     * Modifies velocity if a slide collision occurred. To get the latest collision call get_last_slide_collision(),
     * for more detailed information about collisions that occurred, use get_slide_collision().
     *
     * TODO
     * When the body touches a moving platform, the platform's velocity is automatically added to the body motion. If a
     * collision occurs due to the platform's motion, it will always be first in the slide collisions.
     *
     * Returns true if the body collided, otherwise, returns false.
     */
    bool MoveAndSlide(float inDT);
};
