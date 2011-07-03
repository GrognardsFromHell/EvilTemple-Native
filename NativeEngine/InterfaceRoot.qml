import QtQuick 1.1

/*
  This is the root interface item. All other interface items are anchored below this item.
  It is always resized to fill the game window in its entirety.
  */
Rectangle {
    id: root
    color: 'green'
    width: 100
    height: 100

    signal createObjectSuccess(int requestTicket, variant createdObject)

    signal createObjectError(int requestTicket, string error)

    property int latestRequestTicket : 0

    function createObject(url)
    {
        // URL should have a scheme if possible

        var component = Qt.createComponent(url);

        var ticket = ++latestRequestTicket;

        console.log("Creating object from " + url + ". Ticket: " + ticket);

        var statusChanged = function(status) {
            if (component.status == Component.Error) {
                console.log("Error loading " + url + ": " + component.errorString());
                createObjectError(ticket, component.errorString());
            } else if (component.status == Component.Ready) {
                var obj = component.createObject(root);

                if (obj === null) {
                    createObjectError(ticket, "Unable to create object from " + url);
                } else {
                    createObjectSuccess(ticket, obj);
                }
            }
        };

        component.statusChanged.connect(statusChanged);

        return ticket;
    }
}
