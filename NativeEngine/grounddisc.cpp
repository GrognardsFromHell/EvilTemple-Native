#include "grounddisc.h"

std::string GroundDiscFactory::FACTORY_TYPE_NAME = "GroundDisc";

const std::string& GroundDiscFactory::getType(void) const
{
    return FACTORY_TYPE_NAME;
}

Ogre::MovableObject *GroundDiscFactory::createInstanceImpl(const std::string &name,
                                                           const Ogre::NameValuePairList *params)
{
    if (params->find("material") == params->end())
        throw std::exception("Material parameter is missing for creation of ground disc.");

    std::string material = params->at("material");

    return new GroundDisc(name, material);
}

void GroundDiscFactory::destroyInstance(Ogre::MovableObject* obj)
{
    delete obj;
}

GroundDisc::GroundDisc(const std::string &name, const std::string &material) : Ogre::ManualObject(name)
{
    estimateVertexCount(9); // 4 Corners + 4 midpoints + center
    estimateIndexCount(6 * 4); // Quad has 6 indices, we have 4 quads.

    begin(material);

    position(0, 0, 0); /* 0: center */
    textureCoord(1, 1);

    position(-1, 0, 1); /* 1: N */
    textureCoord(0, 0);

    position(0, 0, 1); /* 2: NW */
    textureCoord(0, 1);

    position(1, 0, 1); /* 3: W */
    textureCoord(0, 0);

    position(1, 0, 0); /* 4: SW */
    textureCoord(1, 0);

    position(1, 0, -1); /* 5: S */
    textureCoord(0, 0);

    position(0, 0, -1); /* 6: SE */
    textureCoord(0, 1);

    position(-1, 0, -1); /* 7: E */
    textureCoord(0, 0);

    position(-1, 0, 0); /* 8: NE */
    textureCoord(1, 0);

    quad(0, 8, 1, 2);
    quad(0, 2, 3, 4);
    quad(0, 4, 5, 6);
    quad(0, 6, 7, 8);

    mSection = end();
}

void GroundDisc::setMaterial(const std::string &material)
{
    mSection->setMaterialName(material);
}

const std::string &GroundDisc::getMovableType(void) const
{
    return GroundDiscFactory::FACTORY_TYPE_NAME;
}
