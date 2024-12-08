#include "CharacterBody.h"

void CharacterBody::Init(wi::ecs::Entity inEntity, float inHeight)
{
    mHeight = inHeight;
    SetEntity(inEntity);
    wi::scene::Scene& scene = wi::scene::GetScene();

    mColliderEntity = wi::ecs::CreateEntity();
    scene.transforms.Create(mColliderEntity);
    wi::scene::ColliderComponent& collider = scene.colliders.Create(mColliderEntity);
    collider.SetCPUEnabled(false);
    collider.SetGPUEnabled(true);
    collider.shape = wi::scene::ColliderComponent::Shape::Capsule;
    collider.radius = 0.8;
    collider.offset = Vector3(0, 0, 0);
    collider.tail = Vector3(0, mHeight, 0);
}

void CharacterBody::Deinit()
{
    wi::scene::Scene& scene = wi::scene::GetScene();
    scene.transforms.Remove(mColliderEntity);
    scene.colliders.Remove(mColliderEntity);

    scene.Entity_Remove(mColliderEntity);
}

bool CharacterBody::MoveAndSlide(float inDT)
{
    bool hasCollided = false;

    wi::scene::Scene& scene = wi::scene::GetScene();
    wi::scene::TransformComponent* transform = scene.transforms.GetComponent(mEntity);
    Vector3 position = transform->GetPosition();

    wi::primitive::Capsule capsule{position, position + Vector3(0, mHeight, 0), CAPSULE_RADIUS};
    const Vector3 originalCapsulePos = capsule.base;
    Vector3 capsulePos = originalCapsulePos;
    float capsuleHeight = (capsule.tip - capsule.base).Length();
    Vector3 platformVelocityAccumulation;
    uint8_t platformVelocityCount = 0;

    mCollisionUpdateDeltaTarget += inDT;

    // perform fixed timestep logic:
    if (mCollisionUpdateDeltaTarget >= FIXED_COLLISION_DT) {
        mIsOnFloor = false;
    }

    while (mCollisionUpdateDeltaTarget >= FIXED_COLLISION_DT) {
        mCollisionUpdateDeltaTarget -= FIXED_COLLISION_DT;
        // HERE VELOCITY IS APPLIED!
        capsulePos += mVelocity * FIXED_COLLISION_DT;
        capsule = wi::primitive::Capsule(capsulePos, capsulePos + Vector3(capsuleHeight), capsuleHeight);

        const uint32_t filterMask = wi::enums::FILTER_NAVIGATION_MESH | wi::enums::FILTER_COLLIDER;
        const uint32_t layerMask = ~FIRST_PERSON_LAYER;
        uint lod = 0;
        wi::scene::Scene::CapsuleIntersectionResult collision = scene.Intersects(capsule, filterMask, layerMask, lod);

        if (collision.entity != wi::ecs::INVALID_ENTITY) {
            hasCollided = true;
            // collision.entity;
            // collision.position;
            // collision.normal;
            // collision.velocity;
            // collision.orientation;
            float groundSlope = Vector3(collision.normal).Dot(mUp);
            if (groundSlope > SLOPE_THRESHOLD) {
                mIsOnFloor = true;
                capsulePos += Vector3(0, collision.depth, 0);
                platformVelocityAccumulation += collision.velocity;
                platformVelocityCount += 1;

                // TODO verify this mess!
                //  mVelocity.y = 0;
                Vector3 normalUp = mUp; // could use mGravity here
                normalUp.Normalize();
                float gravityComponent = mVelocity.Dot(normalUp);
                Vector3 vGravityComponent = normalUp * gravityComponent;
                mVelocity -= vGravityComponent;
            } else {
                capsulePos += (collision.normal * collision.depth);
            }
        }

        // TODO gravity and force stuff
        // Vector3 impulse = mGravity * FIXED_COLLISION_DT;
        ////force += mForce;
        // float forceLenght = force.Lenght();
        // if (forceLenght > 30)
        //{
        //	forceLenght = 30;
        //	force.Normalize();
        //	force *= forceLenght;
        // }
    }

    // apply moving platform velocity
    if (platformVelocityCount > 0) {
        capsulePos += (platformVelocityAccumulation * (1.0 / platformVelocityCount));
    }

    if (mIsOnFloor) {
        // force *= 0.85; //TODO ground friction
    }

    position = capsulePos;

    // apply to transform
    {
        // wi::scene::TransformComponent &entityTransform = scene.transforms.GetComponent(mEntity);
        // entityTransform.ClearTransform();
        // entityTransform.Rotate(Vector3(mRotationVertical, mRotationHorizontal));
        // entityTransform.Translate(mPosition + Vector3(0, mHeight, 0));
        // entityTransform.UpdateTransform();

        // camera.TransformCamera(entityTransform);
        // camera.UpdateCamera();

        // wi::scene::ColliderComponent &collirder = scene.colliders.GetComponent(mColliderEntity);
        // collider.tail = Vector3(0, mHeight, 0);
        // wi::scene::colliderTransform &colliderTransform = scene.transforms.GetComponent(mColliderEntity);
        // colliderTransform.ClearTransform;
        // colliderTransform.Translate(mPosition);
    }
    // else
    {
        // wi::scene::TransformComponent* transform = scene.transforms.GetComponent(mEntity);
        Vector3 move = position - transform->GetPosition(); // new - original
        transform->Translate(move);

        // // else
        // //Matrix camRot = Matrix::CreateFromQuaternion(mTransform.rotation_local);
        // //Vector3 move_rot = Vector3::TransformNormal(move, camRot);
        // //mTransform.RotateRollPitchYaw(Vector3(yDif, xDif, 0));
        // //mTransform.Translate(move_rot);

        transform->UpdateTransform();

        wi::scene::ColliderComponent* collider = scene.colliders.GetComponent(mColliderEntity);
        collider->tail = Vector3(0, mHeight, 0);
        wi::scene::TransformComponent* colliderTransform = scene.transforms.GetComponent(mColliderEntity);
        colliderTransform->ClearTransform();
        colliderTransform->Translate(position);
    }

    return false;
}
