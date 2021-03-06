#include "PrecompiledHeader.h"

#include "MathUtility.h"

void ConstructMatrixLookAtLH(FMatrix4x4& viewMatrix, FVector3& pos, FVector3& target, FVector3& up)
{
    //from:https://docs.microsoft.com/en-us/previous-versions/windows/desktop/bb281710(v=vs.85)
    //    zaxis = normal(cameraTarget - cameraPosition)
    //    xaxis = normal(cross(cameraUpVector, zaxis))
    //    yaxis = cross(zaxis, xaxis)

    //    xaxis.x           yaxis.x           zaxis.x          0
    //    xaxis.y           yaxis.y           zaxis.y          0
    //    xaxis.z           yaxis.z           zaxis.z          0
    //    - dot(xaxis, cameraPosition) - dot(yaxis, cameraPosition) - dot(zaxis, cameraPosition)  1
    const FVector3 zaxis = (target - pos).normalized();
    const FVector3 xaxis = up.cross(zaxis).normalized();
    const FVector3 yaxis = zaxis.cross(xaxis);

    viewMatrix.col(0) = FVector4(xaxis.x(), xaxis.y(), xaxis.z(), -xaxis.dot(pos));
    viewMatrix.col(1) = FVector4(yaxis.x(), yaxis.y(), yaxis.z(), -yaxis.dot(pos));
    viewMatrix.col(2) = FVector4(zaxis.x(), zaxis.y(), zaxis.z(), -zaxis.dot(pos));
    viewMatrix.col(3) = FVector4(0.0f, 0.0f, 0.0f, 1.0f);
}

void ConstructMatrixLookRight(FMatrix4x4& viewMatrix, FVector3& pos, FVector3& look, FVector3& right)
{
    const FVector3 zaxis = look;
    const FVector3 xaxis = right;
    const FVector3 yaxis = zaxis.cross(xaxis);

    viewMatrix.col(0) = FVector4(xaxis.x(), xaxis.y(), xaxis.z(), -xaxis.dot(pos));
    viewMatrix.col(1) = FVector4(yaxis.x(), yaxis.y(), yaxis.z(), -yaxis.dot(pos));
    viewMatrix.col(2) = FVector4(zaxis.x(), zaxis.y(), zaxis.z(), -zaxis.dot(pos));
    viewMatrix.col(3) = FVector4(0.0f, 0.0f, 0.0f, 1.0f);
}

void ConstructMatrixPerspectiveFovYLH(FMatrix4x4& projectionMatrix, float fovY, float aspectRatio, float nearPlane, float farPlane)
{
    //from:https://docs.microsoft.com/en-us/previous-versions/windows/desktop/bb281727(v=vs.85)
    //    w       0       0                                             0
    //    0       h       0                                             0
    //    0       0       zfarPlane / (zfarPlane - znearPlane)          1
    //    0       0 - znearPlane * zfarPlane / (zfarPlane - znearPlane)  0
    // h = cot(fieldOfViewY/2)
    // h = w / aspectRatio
    // cot(x) = 1 / tan(x)

    const float halfFovY = fovY * 0.5f;
    const float h = 1.0f / tan(halfFovY);
    const float w = h * aspectRatio;
    const float range = farPlane - nearPlane;

    projectionMatrix.setConstant(0.0f);
    projectionMatrix(0, 0) = w;
    projectionMatrix(1, 1) = h;
    projectionMatrix(2, 2) = farPlane / range;
    projectionMatrix(2, 3) = 1.0f;
    projectionMatrix(3, 2) = -nearPlane * farPlane / range;
    projectionMatrix(3, 3) = 0.0f;
}

void ConstructMatrixPerspectiveFovXLH(FMatrix4x4& projectionMatrix, float fovX, float aspectRatio, float nearPlane, float farPlane)
{
    const float halfFovX = fovX * 0.5f;
    const float w = 1.0f / tan(halfFovX);
    const float h = w * aspectRatio;
    const float range = farPlane - nearPlane;

    projectionMatrix.setConstant(0.0f);
    projectionMatrix(0, 0) = w;
    projectionMatrix(1, 1) = h;
    projectionMatrix(2, 2) = farPlane / range;
    projectionMatrix(2, 3) = 1.0f;
    projectionMatrix(3, 2) = -nearPlane * farPlane / range;
    projectionMatrix(3, 3) = 0;
}


FMatrix4x4 ConstructAffineMatrix(float rotX, float rotY, float rotZ, FVector3 trans)
{
    //from:https://stackoverflow.com/questions/25504397/eigen-combine-rotation-and-translation-into-one-matrix
    Eigen::Transform<float, 3, Eigen::Affine> transform;
    transform = Eigen::AngleAxis<float>(rotZ, FVector3::UnitZ());
    transform.prerotate(Eigen::AngleAxis<float>(rotY, FVector3::UnitY()));
    transform.prerotate(Eigen::AngleAxis<float>(rotX, FVector3::UnitX()));
    transform.pretranslate(trans);

    return transform.matrix();
}

void ConstructMatrixOrthoOffCenterLH(
    FMatrix4x4& matrix,
    float left,
    float right,
    float bottom,
    float top,
    float znearPlane,
    float zfarPlane)
{
    //from:https://docs.microsoft.com/en-us/previous-versions/windows/desktop/bb281724(v=vs.85)
    //    2 / (right - l)           0                         0                                  0
    //    0                     2 / (top - bottom)            0                                  0
    //    0                     0                         1 / (zfarPlane - znearPlane)           0
    //    (l + right) / (l - right)  (top + bottom) / (bottom - top)  znearPlane / (znearPlane - zfarPlane)  1
    matrix.setIdentity();

    matrix(0, 0) = 2.0f / (right - left);
    matrix(1, 1) = 2.0f / (top - bottom);
    matrix(2, 2) = 1.0f / (zfarPlane - znearPlane);
    matrix(3, 0) = (left + right) / (left - right);
    matrix(3, 1) = (top + bottom) / (bottom - top);
    matrix(3, 2) = znearPlane / (znearPlane - zfarPlane);
}
