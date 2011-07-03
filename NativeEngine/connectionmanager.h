#ifndef ENGINE_CONNECTIONMANAGER_H
#define ENGINE_CONNECTIONMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QHash>

typedef void (__stdcall *CallSlotFn)(int slotId, int paramCount, const char **paramTypes, void **paramValues);
typedef void (__stdcall *DisconnectCallback)(int slotId);

class ConnectionManager : public QObject {
public:
    static const QMetaObject staticMetaObject;
    virtual const QMetaObject *metaObject() const;
    virtual void *qt_metacast(const char *);
    virtual int qt_metacall(QMetaObject::Call, int, void **argv);

    ConnectionManager();
    ~ConnectionManager();

    /**
      Connect to a signal on a sender by using the signal's method name and parameter count.
      Returns the virtual slot id that was generated for this connection.
      */
    int connect(QObject *sender, const char *signalName, int paramCount, Qt::ConnectionType type = Qt::AutoConnection);

    /**
      Connect to a signal on a sender by using the signal's index and given connection type.
      Returns the virtual slot id that was generated for this connection.
      */
    int connect(QObject *sender, int signalIndex, Qt::ConnectionType type = Qt::AutoConnection);

    /**
      Disconnects the connection for the given slot identifier.
      */
    bool disconnect(int slotId);

    /**
      Returns the number of active connections maintained by this manager.
      */
    int connectionCount() const;

    /**
      Sets the callback that is called when a slot is triggered by a signal.
      May be null.
      */
    void setCallSlotCallback(CallSlotFn);

    /**
      Gets the current slot execution callback. May be null.
      */
    CallSlotFn callSlotCallback() const;

    /**
      Sets the callback that is called whenever a slot is disconnected,
      either explicitly through this interface or via other means.
      May be null.
      */
    void setDisconnectCallback(DisconnectCallback callback);

    /**
      Gets the current disconnect callback. May be null.
      */
    DisconnectCallback disconnectCallback() const;

private:
    struct Connection {
        QObject *sender;
        int signalIndex;
    };
    typedef QHash<int, Connection> Connections;

    int reserveSlot();

    int mSlotCounter; // Will wrap after roughly 2 billion connections
    CallSlotFn mCallSlotCallback;
    DisconnectCallback mDisconnectCallback;
    Connections mConnections;
    bool mInDisconnect;

    void execute(int slotIndex, void **argv);
    static bool globalSignalDisconnected(void **argsPtr);

    /*
      Used to connect to the destroy signal to clean up after
      destroyed signals
      */
    int mDestroyedSlot;
    int mDestroyedSignal;
};

inline void ConnectionManager::setCallSlotCallback(CallSlotFn callback)
{
    mCallSlotCallback = callback;
}

inline CallSlotFn ConnectionManager::callSlotCallback() const
{
    return mCallSlotCallback;
}

inline void ConnectionManager::setDisconnectCallback(DisconnectCallback callback)
{
    mDisconnectCallback = callback;
}

inline DisconnectCallback ConnectionManager::disconnectCallback() const
{
    return mDisconnectCallback;
}

inline int ConnectionManager::connectionCount() const
{
    return mConnections.size();
}

#endif // ENGINE_CONNECTIONMANAGER_H
