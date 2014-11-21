/*
 * Copyright 2014 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dbuspropertywatcher_p.h"
#include <QtDBus/QDBusReply>
#include <unistd.h>
#include <sys/types.h>

/*
 * The class watches a dbus service on a given connection, path and interface for
 * different property changes. The properties parameter specifies the properties
 * to be watched. Multiple properties can be requested to be watched on the same
 * path and interface at the same time. If the property list is empty, the watcher
 * will report all the property changes on the path + interface.
 *
 * The property changes are reported thru the propertyChanged() signal.
 *
 * Example of use:
 * 1) watch all Accounts property changes
 * DBusPropertyWatcher watch(DBusConnection::systemBus(), "com.freedesktop.Accounts", "/com/freedesktop/Accounts",
 *                           "com.freedesktop.Accounts");
 *
 * 2) watch one property change
 * DBusPropertyWatcher watch(DBusConnection::systemBus(), "com.freedesktop.Accounts", "/com/freedesktop/Accounts",
 *                           "com.freedesktop.Accounts", QStringList("OtherVibrate"));
 *
 * 3) watch selected properties
 * DBusPropertyWatcher watch(DBusConnection::systemBus(), "com.freedesktop.Accounts", "/com/freedesktop/Accounts",
 *                           "com.freedesktop.Accounts",
 *                          (QStringList() << "IncomingCallVibrate" << "OtherVibrate"));
 */
DBusPropertyWatcher::DBusPropertyWatcher(const QDBusConnection &connection, const QString &service, const QString &path, const QString &iface, const QStringList &properties = QStringList(), QObject *parent)
    : QObject(parent)
    , connection(connection)
    , watcher(service, connection, QDBusServiceWatcher::WatchForOwnerChange)
    , iface(service, path, iface, connection)
    , service(service)
    , watchedProperties(properties)
{
    QObject::connect(&watcher, &QDBusServiceWatcher::serviceOwnerChanged,
                     this, &DBusPropertyWatcher::onOwnerChanged);
    setupInterface();
}

/*
 * Reads the values of the properties to be watched. The method will work only
 * if there are specific properties to watch for. serviceInterface is the DBus
 * interface the properties are retrieved from, which may be different from the
 * watcher interface. If null string is given, the sync will use the watched
 * interface.
 */
void DBusPropertyWatcher::syncProperties(const QString &serviceInterface)
{
    if (watchedProperties.isEmpty()) {
        return;
    }
    Q_FOREACH(const QString &property, watchedProperties) {
        QVariant value = readProperty(serviceInterface.isEmpty() ? iface.interface() : serviceInterface, property);
        Q_EMIT propertyChanged(property, value);
    }
}

/*
 * Read the property value synchronously.
 */
QVariant DBusPropertyWatcher::readProperty(const QString &interface, const QString &property)
{
    if (objectPath.isEmpty()) {
        return QVariant();
    }
    QDBusInterface readIFace(iface.interface(), objectPath, "org.freedesktop.DBus.Properties", connection);
    if (!readIFace.isValid()) {
        // invalid interface
        return QVariant();
    }
    QDBusReply<QDBusVariant> reply = readIFace.call("Get", interface, property);
    return reply.isValid() ? reply.value().variant() : QVariant();
}

/*
 * Write a property value synchronously. The interface represents the service interface writing teh properties.
 */
bool DBusPropertyWatcher::writeProperty(const QString &interface, const QString &property, const QVariant &value)
{
    if (objectPath.isEmpty()) {
        return false;
    }
    QDBusInterface writeIFace(iface.interface(), objectPath, "org.freedesktop.DBus.Properties", connection);
    if (!writeIFace.isValid()) {
        // invalid interface
        return false;
    }
    QDBusMessage msg = writeIFace.call("Set", interface, property, QVariant::fromValue(QDBusVariant(value)));
    return msg.type() == QDBusMessage::ReplyMessage;
}

/*
 * Setup interface connections on ownership change
 */
void DBusPropertyWatcher::onOwnerChanged(const QString &serviceName, const QString &oldOwner, const QString &newOwner)
{
    Q_UNUSED(oldOwner);
    Q_UNUSED(newOwner);
    if (serviceName != service) {
        return;
    }
    setupInterface();
}

/*
 * Check the properties invalidated, and report the ones watched
 */
void DBusPropertyWatcher::onPropertiesChanged(const QString &interface, const QVariantMap &properties, const QStringList &invalidated)
{
    Q_UNUSED(properties);
    bool reportAll = watchedProperties.isEmpty();
    Q_FOREACH(const QString &property, invalidated) {
        if (reportAll || watchedProperties.contains(property)) {
            QVariant value = readProperty(interface, property);
            Q_EMIT propertyChanged(property, value);
        }
    }
}

/*
 * Connect dbus signal identified by (service, path, iface, name) quaduple to a slot to receive
 * property changes.
 */
void DBusPropertyWatcher::setupInterface()
{
    QDBusReply<QDBusObjectPath> qObjectPath = iface.call(
                "FindUserById", qlonglong(getuid()));

    if (qObjectPath.isValid()) {
        objectPath = qObjectPath.value().path();
        iface.connection().connect(
            service,
            objectPath,
            "org.freedesktop.DBus.Properties",
            "PropertiesChanged",
            this,
            SLOT(onPropertiesChanged(QString,QVariantMap,QStringList)));
    }
}
