#include "Transform.h"

const MATRIX Transform::GetRotMatrix() const
{
    MATRIX mRotZ = MGetRotZ(rotation.z);
    MATRIX mRotX = MGetRotX(rotation.x);
    MATRIX mRotY = MGetRotY(rotation.y);
    return mRotZ * mRotX * mRotY;
}

const MATRIX Transform::GetRotMatrixY() const
{
    return MGetRotY(rotation.y);
}

const MATRIX Transform::GetMatrix() const
{
    MATRIX mScale = MGetScale(scale);
    MATRIX mTrans = MGetTranslate(position);

    MATRIX m = mScale * GetRotMatrix() * mTrans;
    return m;
}

int Transform::SetMatrixModel(int _model) const
{   
    return MV1SetMatrix(_model, GetMatrix());
}
