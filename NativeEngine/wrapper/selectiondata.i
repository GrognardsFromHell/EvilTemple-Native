
%extend MovableObject {

    void setSelectionData(qint64 id, float radius, float height)
    {
        ::SelectionData selectionData(id, radius, height);

        $self->setUserAny(Ogre::Any(selectionData));
    }

    bool hasSelectionData()
    {
        const Ogre::Any &userAny = $self->getUserAny();
        return (!userAny.isEmpty() && userAny.getType() == typeid(::SelectionData) );
    }

    void clearSelectionData()
    {
        $self->setUserAny(Ogre::Any());
    }

    qint64 getSelectionId() {
        const Ogre::Any &userAny = $self->getUserAny();
        if (userAny.isEmpty() || userAny.getType() != typeid(::SelectionData))
            throw new std::exception("This object doesn't have selection data.");
        return Ogre::any_cast<::SelectionData>($self->getUserAny()).id;
    }

    float getSelectionRadius() {
        const Ogre::Any &userAny = $self->getUserAny();
        if (userAny.isEmpty() || userAny.getType() != typeid(::SelectionData))
            throw new std::exception("This object doesn't have selection data.");
        return Ogre::any_cast<::SelectionData>($self->getUserAny()).radius;
    }

    float getSelectionHeight() {
        const Ogre::Any &userAny = $self->getUserAny();
        if (userAny.isEmpty() || userAny.getType() != typeid(::SelectionData))
            throw new std::exception("This object doesn't have selection data.");
        return Ogre::any_cast<::SelectionData>($self->getUserAny()).height;
    }

}
