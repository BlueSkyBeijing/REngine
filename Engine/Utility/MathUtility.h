#pragma once
#include "Prerequisite.h"

void ConstructMatrixLookAtLH(FMatrix4x4& viewMatrix, FVector3& pos, FVector3& target, FVector3& up);

void ConstructMatrixLookRight(FMatrix4x4& viewMatrix, FVector3& pos, FVector3& look, FVector3& right);

void ConstructMatrixPerspectiveFovXLH(FMatrix4x4& projectionMatrix, float fovX, float aspectRatio, float nearPlane, float farPlane);

void ConstructMatrixPerspectiveFovYLH(FMatrix4x4& projectionMatrix, float fovY, float aspectRatio, float nearPlane, float farPlane);

FMatrix4x4 ConstructAffineMatrix(float rotX, float rotY, float rotZ, FVector3 trans);

void ConstructMatrixOrthoOffCenterLH(
    FMatrix4x4& matrix,
    float left,
    float right,
    float bottom,
    float top,
    float znearPlane,
    float zfarPlane);
