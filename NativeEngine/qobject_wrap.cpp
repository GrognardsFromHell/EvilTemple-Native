
/*
    Contains a C based generic wrapping layer for QObjects.
 */

#include <QtCore/QtGlobal>
#include <QtCore/QDebug>
#include <QtCore/QVariant>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QStringList>
#include <QtCore/QMap>

#include "connectionmanager.h"
#include "nativeengine.h"

#include "wraputils.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*
  Generic QObject wrapping functionality that allows dynamic methods to be called and
  properties to be read and written.
 */

WRAPPER_EXPORT QVariant* WRAPPER_STDCALL Variant_FromString(const wchar_t *string)
{
    Q_ASSERT(string);
    return new QVariant(QString::fromUtf16(string));
}

WRAPPER_EXPORT QVariant* WRAPPER_STDCALL Variant_FromReal(double number)
{
    return new QVariant(number);
}

WRAPPER_EXPORT QVariant* WRAPPER_STDCALL Variant_FromInt(int number)
{
    return new QVariant(number);
}

WRAPPER_EXPORT QVariant* WRAPPER_STDCALL Variant_FromBool(bool value)
{
    return new QVariant(value);
}

WRAPPER_EXPORT QVariant* WRAPPER_STDCALL Variant_FromMap(QVariantMap *value)
{
    Q_ASSERT(value);
    return new QVariant(*value);
}

WRAPPER_EXPORT QVariant* WRAPPER_STDCALL Variant_FromList(QVariantList *value)
{
    Q_ASSERT(value);
    return new QVariant(*value);
}

WRAPPER_EXPORT bool WRAPPER_STDCALL Variant_ToBool(QVariant *v)
{
    Q_ASSERT(v);
    return v->toBool();
}

WRAPPER_EXPORT int WRAPPER_STDCALL Variant_ToInt(QVariant *v)
{
    Q_ASSERT(v);
    return v->toInt();
}

WRAPPER_EXPORT double WRAPPER_STDCALL Variant_ToReal(QVariant *v)
{
    Q_ASSERT(v);
    return v->toReal();
}

WRAPPER_EXPORT QString* WRAPPER_STDCALL Variant_ToString(QVariant *v)
{
    Q_ASSERT(v);
    return new QString(v->toString());
}

WRAPPER_EXPORT QVariantList* WRAPPER_STDCALL Variant_ToList(QVariant *v)
{
    return new QVariantList(v->toList());
}

WRAPPER_EXPORT QVariantMap* WRAPPER_STDCALL Variant_ToMap(QVariant *v)
{
    return new QVariantMap(v->toMap());
}

WRAPPER_EXPORT QVariant* WRAPPER_STDCALL Variant_NewNull()
{
    return new QVariant();
}

WRAPPER_EXPORT bool WRAPPER_STDCALL Variant_IsNull(QVariant *v)
{
    Q_ASSERT(v);
    return v->isNull();
}

WRAPPER_EXPORT const char* WRAPPER_STDCALL Variant_GetType(QVariant *v)
{
    Q_ASSERT(v);
    return v->typeName();
}

WRAPPER_EXPORT QObject* WRAPPER_STDCALL Variant_GetQObjectStar(QVariant *v)
{
    Q_ASSERT(v);
    return v->value<QObject*>();
}

WRAPPER_EXPORT void WRAPPER_STDCALL Variant_Free(QVariant *v)
{
    delete v;
}

/********************************************************************************
 * Variant List Functions
 ********************************************************************************/

WRAPPER_EXPORT QVariantList* WRAPPER_STDCALL VariantList_New(int capacity)
{
    auto result = new QVariantList();
    result->reserve(capacity);
    return result;
}

WRAPPER_EXPORT QVariant* WRAPPER_STDCALL VariantList_Get(QVariantList *v, int i)
{
    Q_ASSERT(v);
    return new QVariant(v->at(i));
}

WRAPPER_EXPORT void WRAPPER_STDCALL VariantList_Add(QVariantList *list, QVariant *v)
{
    Q_ASSERT(list);
    list->append(*v);
}

WRAPPER_EXPORT int WRAPPER_STDCALL VariantList_Length(QVariantList *list)
{
    Q_ASSERT(list);
    return list->length();
}

WRAPPER_EXPORT void WRAPPER_STDCALL VariantList_Free(QVariantList *list)
{
    Q_ASSERT(list);
    delete list;
}

/********************************************************************************
 * Variant Map Functions
 ********************************************************************************/

WRAPPER_EXPORT QVariantMap* WRAPPER_STDCALL VariantMap_New()
{
    return new QVariantMap;
}

WRAPPER_EXPORT void WRAPPER_STDCALL VariantMap_Add(QVariantMap *map, const wchar_t *key, QVariant *value)
{
    Q_ASSERT(map);
    Q_ASSERT(key);
    Q_ASSERT(value);

    QString keyStr = QString::fromWCharArray(key);
    map->insert(keyStr, *value);
}

WRAPPER_EXPORT QVariant* WRAPPER_STDCALL VariantMap_Get(QVariantMap *map, const wchar_t *key)
{
    Q_ASSERT(map);
    Q_ASSERT(key);

    QString keyStr = QString::fromWCharArray(key);
    return new QVariant(map->value(keyStr));
}

WRAPPER_EXPORT int WRAPPER_STDCALL VariantMap_Size(QVariantMap *map)
{
    Q_ASSERT(map);
    return map->size();
}


WRAPPER_EXPORT QList<QString>* WRAPPER_STDCALL VariantMap_Keys(QVariantMap *map)
{
    Q_ASSERT(map);
    return new QList<QString>(map->keys());
}

WRAPPER_EXPORT void WRAPPER_STDCALL VariantMap_Free(QVariantMap *map)
{
    Q_ASSERT(map);
    delete map;
}

/********************************************************************************
 * QStringList Functions
 ********************************************************************************/

WRAPPER_EXPORT QString* WRAPPER_STDCALL StringList_Get(QList<QString> *list, int i)
{
    Q_ASSERT(list);
    return new QString(list->at(i));
}

WRAPPER_EXPORT int WRAPPER_STDCALL StringList_Length(QList<QString> *list)
{
    Q_ASSERT(list);
    return list->length();
}

WRAPPER_EXPORT void WRAPPER_STDCALL StringList_Free(QList<QString> *list)
{
    Q_ASSERT(list);
    delete list;
}

/********************************************************************************
 * QString Functions
 ********************************************************************************/

WRAPPER_EXPORT const wchar_t* WRAPPER_STDCALL QString_Utf16(QString *s)
{
    Q_ASSERT(s);
    return s->utf16();
}

WRAPPER_EXPORT void WRAPPER_STDCALL QString_Free(QString *s)
{
    Q_ASSERT(s);
    delete s;
}

/********************************************************************************
 * QObject Functions
 ********************************************************************************/

WRAPPER_EXPORT bool WRAPPER_STDCALL QObject_SetProperty(QObject *obj, const char *name, QVariant *value)
{
    Q_ASSERT(obj);
    Q_ASSERT(name);
    Q_ASSERT(value);

    return obj->setProperty(name, *value);
}

WRAPPER_EXPORT QVariant *WRAPPER_STDCALL QObject_GetProperty(QObject *obj, const char *name)
{
    Q_ASSERT(obj);
    Q_ASSERT(name);

    QVariant result = obj->property(name);
    if (!result.isValid())
        return nullptr;
    else
        return new QVariant(result);
}

WRAPPER_EXPORT bool WRAPPER_STDCALL QObject_CallMetaMethod(QObject *obj, const char *methodName, QVariant *args[], int argCount, QVariant **result, QString **errorMessage)
{
    Q_ASSERT(obj);
    Q_ASSERT(methodName);
    Q_ASSERT(argCount >= 0);

    QVariant returnArg;
    bool success;

    *errorMessage = nullptr;

    switch (argCount) {
    case 0:
        success = QMetaObject::invokeMethod(obj, methodName, Q_RETURN_ARG(QVariant, returnArg));
    case 1:
        success = QMetaObject::invokeMethod(obj, methodName, Q_RETURN_ARG(QVariant, returnArg), Q_ARG(QVariant, *args[0]));
        break;
    case 2:
        success = QMetaObject::invokeMethod(obj, methodName, Q_RETURN_ARG(QVariant, returnArg),
                                            Q_ARG(QVariant, *args[0]), Q_ARG(QVariant, *args[1]));
        break;
    case 3:
        success = QMetaObject::invokeMethod(obj, methodName, Q_RETURN_ARG(QVariant, returnArg),
                                            Q_ARG(QVariant, *args[0]), Q_ARG(QVariant, *args[1]), Q_ARG(QVariant, *args[2]));
        break;
    case 4:
        success = QMetaObject::invokeMethod(obj, methodName, Q_RETURN_ARG(QVariant, returnArg),
                                            Q_ARG(QVariant, *args[0]), Q_ARG(QVariant, *args[1]), Q_ARG(QVariant, *args[2]),
                                            Q_ARG(QVariant, *args[3]));
        break;
    case 5:
        success = QMetaObject::invokeMethod(obj, methodName, Q_RETURN_ARG(QVariant, returnArg),
                                            Q_ARG(QVariant, *args[0]), Q_ARG(QVariant, *args[1]), Q_ARG(QVariant, *args[2]),
                                            Q_ARG(QVariant, *args[3]), Q_ARG(QVariant, *args[4]));
        break;
    case 6:
        success = QMetaObject::invokeMethod(obj, methodName, Q_RETURN_ARG(QVariant, returnArg),
                                            Q_ARG(QVariant, *args[0]), Q_ARG(QVariant, *args[1]), Q_ARG(QVariant, *args[2]),
                                            Q_ARG(QVariant, *args[3]), Q_ARG(QVariant, *args[4]), Q_ARG(QVariant, *args[5]));
        break;
    case 7:
        success = QMetaObject::invokeMethod(obj, methodName, Q_RETURN_ARG(QVariant, returnArg),
                                            Q_ARG(QVariant, *args[0]), Q_ARG(QVariant, *args[1]), Q_ARG(QVariant, *args[2]),
                                            Q_ARG(QVariant, *args[3]), Q_ARG(QVariant, *args[4]), Q_ARG(QVariant, *args[5]),
                                            Q_ARG(QVariant, *args[6]));
        break;
    case 8:
        success = QMetaObject::invokeMethod(obj, methodName, Q_RETURN_ARG(QVariant, returnArg),
                                            Q_ARG(QVariant, *args[0]), Q_ARG(QVariant, *args[1]), Q_ARG(QVariant, *args[2]),
                                            Q_ARG(QVariant, *args[3]), Q_ARG(QVariant, *args[4]), Q_ARG(QVariant, *args[5]),
                                            Q_ARG(QVariant, *args[6]), Q_ARG(QVariant, *args[7]));
        break;
    case 9:
        success = QMetaObject::invokeMethod(obj, methodName, Q_RETURN_ARG(QVariant, returnArg),
                                            Q_ARG(QVariant, *args[0]), Q_ARG(QVariant, *args[1]), Q_ARG(QVariant, *args[2]),
                                            Q_ARG(QVariant, *args[3]), Q_ARG(QVariant, *args[4]), Q_ARG(QVariant, *args[5]),
                                            Q_ARG(QVariant, *args[6]), Q_ARG(QVariant, *args[7]), Q_ARG(QVariant, *args[8]));
        break;
    case 10:
        success = QMetaObject::invokeMethod(obj, methodName, Q_RETURN_ARG(QVariant, returnArg),
                                            Q_ARG(QVariant, *args[0]), Q_ARG(QVariant, *args[1]), Q_ARG(QVariant, *args[2]),
                                            Q_ARG(QVariant, *args[3]), Q_ARG(QVariant, *args[4]), Q_ARG(QVariant, *args[5]),
                                            Q_ARG(QVariant, *args[6]), Q_ARG(QVariant, *args[7]), Q_ARG(QVariant, *args[8]),
                                            Q_ARG(QVariant, *args[9]));
        break;
    default:
        *errorMessage = new QString("Only up to 10 arguments are supported. Given: " + argCount);
        return false;
    }

    if (!success || returnArg.isNull())
        *result = nullptr;
    else {
        /*
          This very odd way of reporting errors seems to be QML specific.
          We try to detect this behaviour anyway.
         */
        if (returnArg.type() == QVariant::Map) {
            QVariantMap resultMap = returnArg.toMap();
            if (resultMap.size() == 7
                    && resultMap.contains("expressionBeginOffset")
                    && resultMap.contains("expressionCaretOffset")
                    && resultMap.contains("expressionEndOffset")
                    && resultMap.contains("fileName")
                    && resultMap.contains("lineNumber")
                    && resultMap.contains("message")
                    && resultMap.contains("sourceId"))
            {
                *errorMessage = new QString;
                QTextStream out(*errorMessage);
                out << "Error when invoking method " << obj->metaObject()->className() << "::" << methodName << ":" << endl;
                out << resultMap["fileName"].toString() << ":" << resultMap["lineNumber"].toString() << ": " << resultMap["message"].toString() << endl;
                return false;
            }
        }

        *result = new QVariant(returnArg);
    }

    return success;
}

WRAPPER_EXPORT const char* WRAPPER_STDCALL QObject_GetClassName(QObject *o)
{
    Q_ASSERT(o);
    return o->metaObject()->className();
}

/*
  There should be only a single instance of the connection manager and
  it is hosted here in the C interface
  */
static ConnectionManager *connectionManager = NULL;

static inline void initConnectionManager()
{
    if (!connectionManager)
        connectionManager = new ConnectionManager;
}

WRAPPER_EXPORT int WRAPPER_STDCALL ConnectionManager_Connect(QObject *sender, const char *signalName, int paramCount)
{
    initConnectionManager();
    return connectionManager->connect(sender, signalName, paramCount);
}

WRAPPER_EXPORT bool WRAPPER_STDCALL ConnectionManager_Disconnect(int slotId)
{
    initConnectionManager();
    return connectionManager->disconnect(slotId);
}

WRAPPER_EXPORT int WRAPPER_STDCALL ConnectionManager_GetConnectionCount()
{
    initConnectionManager();
    return connectionManager->connectionCount();
}

WRAPPER_EXPORT void WRAPPER_STDCALL ConnectionManager_SetCallSlotCallback(CallSlotFn callback)
{
    initConnectionManager();
    connectionManager->setCallSlotCallback(callback);
}

WRAPPER_EXPORT CallSlotFn WRAPPER_STDCALL ConnectionManager_GetCallSlotCallback()
{
    initConnectionManager();
    return connectionManager->callSlotCallback();
}

WRAPPER_EXPORT void WRAPPER_STDCALL ConnectionManager_SetDisconnectCallback(DisconnectCallback callback)
{
    initConnectionManager();
    connectionManager->setDisconnectCallback(callback);
}

WRAPPER_EXPORT DisconnectCallback WRAPPER_STDCALL ConnectionManager_GetDisconnectCallback()
{
    initConnectionManager();
    return connectionManager->disconnectCallback();
}

/* Custom wrapper functions for NativeEngine callbacks */
WRAPPER_EXPORT void WRAPPER_STDCALL NativeEngine_SetKeyPressCallback(NativeEngine *engine, KeyEventFn callback)
{
    Q_ASSERT(engine);
    engine->setKeyPressCallback(callback);
}

WRAPPER_EXPORT void WRAPPER_STDCALL NativeEngine_SetKeyReleaseCallback(NativeEngine *engine, KeyEventFn callback)
{
    Q_ASSERT(engine);
    engine->setKeyReleaseCallback(callback);
}

WRAPPER_EXPORT void WRAPPER_STDCALL NativeEngine_SetMouseDoubleClickCallback(NativeEngine *engine, MouseEventFn callback)
{
    Q_ASSERT(engine);
    engine->setMouseDoubleClickCallback(callback);
}

WRAPPER_EXPORT void WRAPPER_STDCALL NativeEngine_SetMousePressCallback(NativeEngine *engine, MouseEventFn callback)
{
    Q_ASSERT(engine);
    engine->setMousePressCallback(callback);
}

WRAPPER_EXPORT void WRAPPER_STDCALL NativeEngine_SetMouseReleaseCallback(NativeEngine *engine, MouseEventFn callback)
{
    Q_ASSERT(engine);
    engine->setMouseReleaseCallback(callback);
}

WRAPPER_EXPORT void WRAPPER_STDCALL NativeEngine_SetMouseMoveCallback(NativeEngine *engine, MouseEventFn callback)
{
    Q_ASSERT(engine);
    engine->setMouseMoveCallback(callback);
}

WRAPPER_EXPORT void WRAPPER_STDCALL NativeEngine_SetWheelCallback(NativeEngine *engine, WheelEventFn callback)
{
    Q_ASSERT(engine);
    engine->setWheelCallback(callback);
}

#if defined(__cplusplus)
}
#endif
