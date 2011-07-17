#ifndef SELECTIONDATA_H
#define SELECTIONDATA_H

struct SelectionData {
    long id;
    float radius;
    float height;

    SelectionData(long _id, float _radius, float _height) : id(_id), radius(_radius), height(_height) {}
};

inline std::ostream &operator <<(std::ostream &o, const SelectionData &data)
{
    o << data.id << data.height << data.radius;
    return o;
}

#endif // SELECTIONDATA_H
