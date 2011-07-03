
#include "LegacyFormats/clippinggeometryobject.h"
#include "LegacyFormats/camera.h"

#include <QMatrix4x4>

namespace Troika
{

    ClippingGeometryObject::ClippingGeometryObject(QObject *parent) :
        GeometryMeshObject(parent)
    {
    }


    ClippingGeometryObject::~ClippingGeometryObject()
    {
    }

    void ClippingGeometryObject::updateWorldMatrix()
    {       
        _worldMatrix.setToIdentity();

        if (!_position.isNull())
            _worldMatrix.translate(_position);

        if (!_rotation.isIdentity())
            _worldMatrix.rotate(_rotation);

        _worldMatrix *= Camera::baseViewMatrix().inverted();

        if (_scale != IdentityScale) {
            _worldMatrix.scale(_scale);
        }

        _worldMatrix *= Camera::baseViewMatrix();

        _worldMatrix.scale(1, 1, -1); // Convert model-space from DirectX to OpenGL

        _worldMatrixValid = true;
    }

}
