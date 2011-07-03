
#include <QtCore/QSet>
#include <QtCore/QDebug>
#include <QtCore/QMetaMethod>
#include <QtCore/QMetaObject>
#include <QtCore/QInternal>

#include "connectionmanager.h"

static const uint qt_meta_data_ConnectionManager[] = {

    // content:
    1,       // revision
    0,       // classname
    0,    0, // classinfo
    1,   10, // methods
    0,    0, // properties
    0,    0, // enums/sets

    // slots: signature, parameters, type, tag, flags
    35,   34,   34,   34, 0x0a,

    0        // eod
};

static const char qt_meta_stringdata_ConnectionManager[] = {
    "ConnectionManager\0\0execute()\0"
};

const QMetaObject ConnectionManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ConnectionManager,
    qt_meta_data_ConnectionManager, 0 }
};

static ConnectionManager* activeConnectionManager = nullptr;

bool ConnectionManager::globalSignalDisconnected(void **args)
{
    // This callback must not be registered while there is no active manager
    Q_ASSERT(activeConnectionManager);

    if (activeConnectionManager->mInDisconnect)
        return false; // We've initiated the disconnect

    auto sender = reinterpret_cast<QObject*>(args[0]);
    auto signal = reinterpret_cast<const char*>(args[1]);
    auto receiver = reinterpret_cast<QObject*>(args[2]);
    auto method = reinterpret_cast<const char*>(args[3]);

    if (receiver && receiver != activeConnectionManager || method)
        return false; // Disconnect doesn't affect us

    int signalIndex = -1;
    if (signal) {
        if (signal[0] == '2')
            signal++; // Skip what SIGNAL() prepends

        signalIndex = sender->metaObject()->indexOfSignal(signal);

        if (signalIndex == -1)
        {
            qWarning("Attempting to disconnect unknown signal: %s", signal);
            return false;
        }
    }

    if (signalIndex == activeConnectionManager->mDestroyedSignal) {
        qWarning("Destroyed signal is disconnected explicitly. This may cause crashes.");
        return false;
    }

    auto disconnectCallback = activeConnectionManager->mDisconnectCallback;

    // Check all connections from the sender
    bool disconnectDestroyed = true;
    auto it = activeConnectionManager->mConnections.begin();
    while (it != activeConnectionManager->mConnections.end()) {
        if (it->sender != sender) {
            it++;
            continue;
        }

        if (signalIndex == -1 || it->signalIndex == signalIndex) {
            if (disconnectCallback)
                disconnectCallback(it.key());

            it = activeConnectionManager->mConnections.erase(it);
        } else {
            disconnectDestroyed = false;
            it++;
        }
    }

    if (disconnectDestroyed) {
        activeConnectionManager->mInDisconnect = true;
        sender->disconnect(SIGNAL(destroyed(QObject*)), activeConnectionManager);
        activeConnectionManager->mInDisconnect = false;
    }

    return false;
}

ConnectionManager::ConnectionManager()
    : mSlotCounter(0),
      mDestroyedSlot(reserveSlot()),
      mDestroyedSignal(staticMetaObject.indexOfSignal("destroyed(QObject*)")),
      mInDisconnect(false),
      mDisconnectCallback(nullptr),
      mCallSlotCallback(nullptr)
{
    Q_ASSERT(mDestroyedSignal != -1);

    if (activeConnectionManager)
        qCritical("There may only be a single connection manager in a process.");
    activeConnectionManager = this;
    QInternal::registerCallback(QInternal::DisconnectCallback, &ConnectionManager::globalSignalDisconnected);
}

ConnectionManager::~ConnectionManager()
{
    activeConnectionManager = nullptr;
    QInternal::unregisterCallback(QInternal::DisconnectCallback, &ConnectionManager::globalSignalDisconnected);

    // Disconnect from all signals
    auto it = mConnections.constBegin();

    QSet<QObject*> disconnectedFrom;

    for ( ; it != mConnections.constEnd(); ++it) {
        QObject *sender = it->sender;
        if (!disconnectedFrom.contains(sender)) {
            sender->disconnect(this);
            disconnectedFrom.insert(sender);
        }
    }
}

const QMetaObject *ConnectionManager::metaObject() const
{
    return &staticMetaObject;
}

inline int ConnectionManager::reserveSlot()
{
    mSlotCounter++;

    if (mSlotCounter == 0)
        qWarning("Slot index overflow in ConnectionManager!");

    return mSlotCounter;
}

int ConnectionManager::connect(QObject *sender, const char *signalName, int paramCount, Qt::ConnectionType type)
{
    Q_ASSERT(sender);
    Q_ASSERT(signalName);
    Q_ASSERT(paramCount >= 0);

    // Find matching signal in sender
    for (int i = 0; i < sender->metaObject()->methodCount(); ++i)
    {
        const QMetaMethod &method = sender->metaObject()->method(i);

        if (method.methodType() != QMetaMethod::Signal)
            continue;

        const char *startOfParams = strchr(method.signature(), '(');
        int lengthOfName;

        if (!startOfParams)
            lengthOfName = strlen(method.signature());
        else
            lengthOfName = startOfParams - method.signature();

        if (memcmp(method.signature(), signalName, lengthOfName))
            continue;

        if (method.parameterTypes().size() != paramCount)
            continue;

        return connect(sender, i, type);
    }

    qWarning() << "No matching signal found: " << signalName << " (Params: " << paramCount << ")";
    return -1;
}

int ConnectionManager::connect(QObject *sender, int signalIndex, Qt::ConnectionType connectionType)
{
    Q_ASSERT(sender);
    Q_ASSERT(signalIndex != -1);

    Connection newConnection;
    newConnection.signalIndex = signalIndex;
    newConnection.sender = sender;

    int slotIndex = reserveSlot();
    int result = slotIndex; // This is what will be returned to the callback

    Q_ASSERT(!mConnections.contains(slotIndex));

    mConnections.insert(slotIndex, newConnection);

    slotIndex += this->metaObject()->methodOffset();

    bool success = sender->metaObject()->connect(sender, signalIndex, this, slotIndex, connectionType);

    if (success) {
        // Connect to destroyed signal
        int slotIndex = mDestroyedSlot;
        slotIndex += this->metaObject()->methodOffset();
        sender->metaObject()->connect(sender, mDestroyedSignal, this, slotIndex);
    } else {
        // Free the slot we just reserved (NOTE: NOT thread safe)
        mSlotCounter--;
        mConnections.remove(result);
        return -1;
    }

    return result;
}

bool ConnectionManager::disconnect(int slotId)
{
    Connections::iterator it = mConnections.find(slotId);

    if (it == mConnections.end())
        return false;

    if (mDisconnectCallback)
        mDisconnectCallback(it.key());

    Connection connection = *it;
    mConnections.erase(it);

    int slotIndex = metaObject()->methodOffset() + slotId;
    QObject *sender = connection.sender;
    int signalIndex = connection.signalIndex;

    mInDisconnect = true;
    sender->metaObject()->disconnect(sender, signalIndex, this, slotIndex);
    mInDisconnect = false;

    // Disconnect from destroyed signal (IF THIS IS THE ONLY SENDER)
    bool destroyedStillRequired = false;
    it = mConnections.begin();
    while (it != mConnections.end()) {
        if ((it++)->sender == sender) {
            destroyedStillRequired = true;
            break;
        }
    }

    if (!destroyedStillRequired) {
        mInDisconnect = true;
        slotIndex = this->metaObject()->methodOffset() + mDestroyedSlot;
        sender->metaObject()->disconnect(sender, mDestroyedSignal, this, slotIndex);
        mInDisconnect = false;
    }

    return true;
}

void *ConnectionManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ConnectionManager))
        return static_cast<void*>(const_cast<ConnectionManager*>(this));
    return QObject::qt_metacast(_clname);
}

int ConnectionManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        execute(_id, _a);
        _id -= mSlotCounter;
    }
    return _id;
}

void ConnectionManager::execute(int slotId, void **argv)
{
    if (mDestroyedSlot == slotId) {
        QObject *whichOne = reinterpret_cast<QObject*>(argv[0]);
        if (!whichOne)
            whichOne = sender();

        // Remove the entire connection state
        Connections::iterator it = mConnections.begin();

        while (it != mConnections.end()) {
            if (it->sender == whichOne) {
                if (mDisconnectCallback)
                    mDisconnectCallback(it.key());
                it = mConnections.erase(it);
            } else {
                it++;
            }
        }

        return;
    }

    Connections::const_iterator it = mConnections.find(slotId);

    Q_ASSERT(it != mConnections.end());

    int signalIndex = it->signalIndex;
    QMetaMethod signalMethod = sender()->metaObject()->method(signalIndex);

    int paramCount = signalMethod.parameterTypes().size();

    QList<QByteArray> parameterTypes = signalMethod.parameterTypes();
    QScopedArrayPointer<const char*> paramTypes(new const char*[paramCount]);
    for (int i = 0; i < parameterTypes.size(); ++i)
        paramTypes[i] = parameterTypes[i].constData();

    argv++; // Skip the return argument pointer at position 0

    if (mCallSlotCallback)
        mCallSlotCallback(slotId, paramCount, paramTypes.data(), argv);
}
