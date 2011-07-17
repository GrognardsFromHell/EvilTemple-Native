#ifndef GROUNDDISC_H
#define GROUNDDISC_H

#include <Ogre.h>

class GroundDiscFactory : public Ogre::ManualObjectFactory
{
public:
    static std::string FACTORY_TYPE_NAME;

    const std::string& getType(void) const;
    void destroyInstance( Ogre::MovableObject* obj);

protected:
    Ogre::MovableObject *createInstanceImpl(const std::string &name,
                                            const Ogre::NameValuePairList *params);
};

class GroundDisc : public Ogre::ManualObject
{
public:
    GroundDisc(const std::string &name, const std::string &material);

    const std::string &getMovableType(void) const;

    void setMaterial(const std::string &material);
private:
    ManualObjectSection *mSection;
};

#endif // GROUNDDISC_H
