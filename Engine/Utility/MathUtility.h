#pragma once
#include "Prerequisite.h"

void ConstructMatrixLookAtLH(FMatrix4x4& viewMatrix, FVector3& pos, FVector3& target, FVector3& up);

void ConstructMatrixLookRight(FMatrix4x4& viewMatrix, FVector3& pos, FVector3& look, FVector3& right);

void ConstructMatrixPerspectiveFovLH(FMatrix4x4& projectionMatrix, float fovY, float aspect, float nearPlane, float farPlane);


//Eigen::Matrix<float, 4, 4, Eigen::ColMajor> ConstructAffineMatrix(float a, float b, float c, FVector3 trans);

//Eigen::Affine3f create_rotation_matrix(float ax, float ay, float az);
