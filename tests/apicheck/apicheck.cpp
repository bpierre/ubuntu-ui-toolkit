/*
 * Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
 * Copyright (C) 2015 Christian Dywan
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

#include <QtQml/qqmlengine.h>
#include <QtQml/private/qqmlmetatype_p.h>
#include <QtQml/private/qqmlopenmetaobject_p.h>
#include <QtQuick/private/qquickevents_p_p.h>
#include <QtQuick/private/qquickpincharea_p.h>

#include <QtGui/QGuiApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QSet>
#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>
#include <QtCore/private/qobject_p.h>
#include <QtCore/private/qmetaobject_p.h>
#include <QtQml/private/qqmldirparser_p.h>
#include <QJSEngine>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QLoggingCategory>

#include <iostream>
#include <algorithm>

#ifdef Q_OS_UNIX
#include <signal.h>
#endif

QString pluginImportPath;
bool verbose = false;
bool creatable = true;

QString currentProperty;
QString inObjectInstantiation;

void collectReachableMetaObjects(const QMetaObject *meta, QSet<const QMetaObject *> *metas, bool extended = false)
{
    if (! meta || metas->contains(meta))
        return;

    // dynamic meta objects can break things badly
    // but extended types are usually fine
    const QMetaObjectPrivate *mop = reinterpret_cast<const QMetaObjectPrivate *>(meta->d.data);
    if (extended || !(mop->flags & DynamicMetaObject))
        metas->insert(meta);

    collectReachableMetaObjects(meta->superClass(), metas);
}

void collectReachableMetaObjects(QObject *object, QSet<const QMetaObject *> *metas)
{
    if (! object)
        return;

    const QMetaObject *meta = object->metaObject();
    if (verbose)
        std::cerr << "Processing object" << qPrintable( meta->className() ) << std::endl;
    collectReachableMetaObjects(meta, metas);

    for (int index = 0; index < meta->propertyCount(); ++index) {
        QMetaProperty prop = meta->property(index);
        if (QQmlMetaType::isQObject(prop.userType())) {
            if (verbose)
                std::cerr << "  Processing property" << qPrintable( prop.name() ) << std::endl;
            currentProperty = QString("%1::%2").arg(meta->className(), prop.name());

            // if the property was not initialized during construction,
            // accessing a member of oo is going to cause a segmentation fault
            QObject *oo = QQmlMetaType::toQObject(prop.read(object));
            if (oo && !metas->contains(oo->metaObject()))
                collectReachableMetaObjects(oo, metas);
            currentProperty.clear();
        }
    }
}

void collectReachableMetaObjects(const QQmlType *ty, QSet<const QMetaObject *> *metas)
{
    collectReachableMetaObjects(ty->metaObject(), metas, ty->isExtendedType());
    if (ty->attachedPropertiesType())
        collectReachableMetaObjects(ty->attachedPropertiesType(), metas);
}

/* We want to add the MetaObject for 'Qt' to the list, this is a
   simple way to access it.
*/
class FriendlyQObject: public QObject
{
public:
    static const QMetaObject *qtMeta() { return &staticQtMetaObject; }
};

/* When we dump a QMetaObject, we want to list all the types it is exported as.
   To do this, we need to find the QQmlTypes associated with this
   QMetaObject.
*/
static QHash<QByteArray, QSet<const QQmlType *> > qmlTypesByCppName;

// No different versioning possible for a composite type.
static QMap<QString, const QQmlType * > qmlTypesByCompositeName;

static QHash<QByteArray, QByteArray> cppToId;

/* Takes a C++ type name, such as Qt::LayoutDirection or QString and
   maps it to how it should appear in the description file.

   These names need to be unique globally, so we don't change the C++ symbol's
   name much. It is mostly used to for explicit translations such as
   QString->string and translations for extended QML objects.
*/
QByteArray convertToId(const QString &cppName)
{
    QString qmlType(cppName);
    if (qmlType.contains("::")) {
        QStringList parts(qmlType.split("::"));
        return qPrintable(convertToId(parts[0]) + "." + convertToId(parts[1]));
    }

    QList<const QQmlType*>types(qmlTypesByCppName[qPrintable(cppName)].toList());
    std::sort(types.begin(), types.end());
    // Strip internal _QMLTYPE_xy suffix
    qmlType = qmlType.split("_")[0];
    if (!types.isEmpty())
        qmlType = QString(types[0]->qmlTypeName()).split("/")[1].toUtf8();
    else
        qmlType = cppToId.value(qPrintable(qmlType), qPrintable(cppName));
    // Strip internal _QMLTYPE_xy suffix
    qmlType = qmlType.split("_")[0];
    return qPrintable(qmlType.replace("QTestRootObject", "QtObject"));
}

QByteArray convertToId(const QMetaObject *mo)
{
    QByteArray className(mo->className());
    if (!className.isEmpty())
        return convertToId(className);

    // likely a metaobject generated for an extended qml object
    if (mo->superClass()) {
        className = convertToId(mo->superClass());
        className.append("_extended");
        return className;
    }

    static QHash<const QMetaObject *, QByteArray> generatedNames;
    className = generatedNames.value(mo);
    if (!className.isEmpty())
        return className;

    std::cerr << "Found a QMetaObject without a className, generating a random name" << std::endl;
    className = QByteArray("error-unknown-name-");
    className.append(QByteArray::number(generatedNames.size()));
    generatedNames.insert(mo, className);
    return className;
}

// Collect all metaobjects for types registered with qmlRegisterType() without parameters
void collectReachableMetaObjectsWithoutQmlName( QSet<const QMetaObject *>& metas ) {
    Q_FOREACH (const QQmlType *ty, QQmlMetaType::qmlAllTypes()) {
        if ( ! metas.contains(ty->metaObject()) ) {
            if (!ty->isComposite()) {
                collectReachableMetaObjects(ty, &metas);
            } else {
                qmlTypesByCompositeName[ty->elementName()] = ty;
            }
       }
    }
}

QSet<const QMetaObject *> collectReachableMetaObjects(QQmlEngine *engine,
                                                      QSet<const QMetaObject *> &noncreatables,
                                                      QSet<const QMetaObject *> &singletons,
                                                      const QList<QQmlType *> &skip = QList<QQmlType *>())
{
    QSet<const QMetaObject *> metas;
    metas.insert(FriendlyQObject::qtMeta());

    QHash<QByteArray, QSet<QByteArray> > extensions;
    Q_FOREACH (const QQmlType *ty, QQmlMetaType::qmlTypes()) {
        if (!ty->isCreatable())
            noncreatables.insert(ty->metaObject());
        if (ty->isSingleton())
            singletons.insert(ty->metaObject());
        if (!ty->isComposite()) {
            qmlTypesByCppName[ty->metaObject()->className()].insert(ty);
            if (ty->isExtendedType())
                extensions[ty->typeName()].insert(ty->metaObject()->className());
            collectReachableMetaObjects(ty, &metas);
        } else {
            qmlTypesByCompositeName[ty->elementName()] = ty;
        }
    }

    // Adjust exports of the base object if there are extensions.
    // For each export of a base object there can be a single extension object overriding it.
    // Example: QDeclarativeGraphicsWidget overrides the QtQuick/QGraphicsWidget export
    //          of QGraphicsWidget.
    Q_FOREACH (const QByteArray &baseCpp, extensions.keys()) {
        QSet<const QQmlType *> baseExports = qmlTypesByCppName.value(baseCpp);

        const QSet<QByteArray> extensionCppNames = extensions.value(baseCpp);
        Q_FOREACH (const QByteArray &extensionCppName, extensionCppNames) {
            const QSet<const QQmlType *> extensionExports = qmlTypesByCppName.value(extensionCppName);

            // remove extension exports from base imports
            // unfortunately the QQmlType pointers don't match, so can't use QSet::subtract
            QSet<const QQmlType *> newBaseExports;
            Q_FOREACH (const QQmlType *baseExport, baseExports) {
                bool match = false;
                Q_FOREACH (const QQmlType *extensionExport, extensionExports) {
                    if (baseExport->qmlTypeName() == extensionExport->qmlTypeName()
                            && baseExport->majorVersion() == extensionExport->majorVersion()
                            && baseExport->minorVersion() == extensionExport->minorVersion()) {
                        match = true;
                        break;
                    }
                }
                if (!match)
                    newBaseExports.insert(baseExport);
            }
            baseExports = newBaseExports;
        }
        qmlTypesByCppName[baseCpp] = baseExports;
    }

    if (creatable) {
        // find even more QMetaObjects by instantiating QML types and running
        // over the instances
        Q_FOREACH (QQmlType *ty, QQmlMetaType::qmlTypes()) {
            if (skip.contains(ty))
                continue;
            if (ty->isExtendedType())
                continue;
            if (!ty->isCreatable())
                continue;
            if (ty->typeName() == "QQmlComponent")
                continue;

            QString tyName = ty->qmlTypeName();
            tyName = tyName.mid(tyName.lastIndexOf(QLatin1Char('/')) + 1);
            if (tyName.isEmpty())
                continue;

            inObjectInstantiation = tyName;
            QObject *object = 0;

            if (ty->isSingleton()) {
                QQmlType::SingletonInstanceInfo *siinfo = ty->singletonInstanceInfo();
                if (!siinfo) {
                    std::cerr << "Internal error, " << qPrintable(tyName)
                              << "(" << qPrintable( QString::fromUtf8(ty->typeName()) ) << ")"
                              << " is singleton, but has no singletonInstanceInfo" << std::endl;
                    continue;
                }
                if (siinfo->qobjectCallback) {
                    if (verbose)
                        std::cerr << "Trying to get singleton for " << qPrintable(tyName)
                                  << " (" << qPrintable( siinfo->typeName )  << ")" << std::endl;
                    siinfo->init(engine);
                    collectReachableMetaObjects(object, &metas);
                    object = siinfo->qobjectApi(engine);
                } else {
                    inObjectInstantiation.clear();
                    continue; // we don't handle QJSValue singleton types.
                }
            } else {
                if (verbose)
                    std::cerr << "Trying to create object " << qPrintable( tyName )
                              << " (" << qPrintable( QString::fromUtf8(ty->typeName()) )  << ")" << std::endl;
                object = ty->create();
            }

            inObjectInstantiation.clear();

            if (object) {
                if (verbose)
                    std::cerr << "Got " << qPrintable( tyName )
                              << " (" << qPrintable( QString::fromUtf8(ty->typeName()) ) << ")" << std::endl;
                collectReachableMetaObjects(object, &metas);
                object->deleteLater();
            } else {
                std::cerr << "Could not create" << qPrintable(tyName) << std::endl;
            }
        }
    }

    collectReachableMetaObjectsWithoutQmlName(metas);

    return metas;
}

class KnownAttributes {
    QHash<QByteArray, int> m_properties;
    QHash<QByteArray, QHash<int, int> > m_methods;
public:
    bool knownMethod(const QByteArray &name, int nArgs, int revision)
    {
        if (m_methods.contains(name)) {
            QHash<int, int> overloads = m_methods.value(name);
            if (overloads.contains(nArgs) && overloads.value(nArgs) <= revision)
                return true;
        }
        m_methods[name][nArgs] = revision;
        return false;
    }

    bool knownProperty(const QByteArray &name, int revision)
    {
        if (m_properties.contains(name) && m_properties.value(name) <= revision)
            return true;
        m_properties[name] = revision;
        return false;
    }
};

class Dumper
{
    QJsonObject* json;
    QString relocatableModuleUri;
    QString importVersion;

public:
    Dumper(QJsonObject* json): json(json) {}

    void setRelocatableModuleUri(const QString &uri)
    {
        relocatableModuleUri = uri;
    }

    void setImportVersion(const QString &version)
    {
        importVersion = version;
    }

    const QString getExportString(QString qmlTyName, int majorVersion, int minorVersion)
    {
        if (qmlTyName.startsWith(relocatableModuleUri + QLatin1Char('/'))) {
            qmlTyName.remove(0, relocatableModuleUri.size() + 1);
        }
        if (qmlTyName.startsWith("./")) {
            qmlTyName.remove(0, 2);
        }
        if (qmlTyName.startsWith("/")) {
            qmlTyName.remove(0, 1);
        }
        // Work-around for version -1, -1
        if (majorVersion == -1)
            return QString("%1 %2").arg(qmlTyName).arg(importVersion);
        return QString("%1 %2.%3").arg(qmlTyName).arg(majorVersion).arg(minorVersion);
    }

    void writeMetaContent(QJsonObject* object, const QMetaObject *meta, KnownAttributes *knownAttributes = 0)
    {
        QSet<QString> implicitSignals;
        for (int index = meta->propertyOffset(); index < meta->propertyCount(); ++index) {
            const QMetaProperty &property = meta->property(index);
            dump(object, property, knownAttributes);
            if (knownAttributes)
                knownAttributes->knownMethod(QByteArray(property.name()).append("Changed"),
                                             0, property.revision());
            implicitSignals.insert(QString("%1Changed").arg(QString::fromUtf8(property.name())));
        }

        QJsonArray methods;
        if (meta == &QObject::staticMetaObject) {
            // for QObject, hide deleteLater() and onDestroyed
            for (int index = meta->methodOffset(); index < meta->methodCount(); ++index) {
                QMetaMethod method = meta->method(index);
                QByteArray signature = method.methodSignature();
                if (signature == QByteArrayLiteral("destroyed(QObject*)")
                        || signature == QByteArrayLiteral("destroyed()")
                        || signature == QByteArrayLiteral("deleteLater()"))
                    continue;
                dump(&methods, method, implicitSignals, knownAttributes);
            }

            // and add toString(), destroy() and destroy(int)
            if (!knownAttributes || !knownAttributes->knownMethod(QByteArray("toString"), 0, 0)) {
                QJsonObject method;
                method["type"] = "function";
                method["name"] = "toString";
                methods.append(method);
            }
            if (!knownAttributes || !knownAttributes->knownMethod(QByteArray("destroy"), 0, 0)) {
                QJsonObject method;
                method["type"] = "function";
                method["name"] = "destroy";
                methods.append(method);
            }
            if (!knownAttributes || !knownAttributes->knownMethod(QByteArray("destroy"), 1, 0)) {
                QJsonObject method;
                method["type"] = "function";
                method["name"] = "destroy";
                QJsonArray parameters;
                QJsonObject parameter;
                parameter["type"] = "int";
                parameter["name"] = "delay";
                method.insert("parameters", parameters);
                methods.append(method);
            }
        } else {
            for (int index = meta->methodOffset(); index < meta->methodCount(); ++index) {
                // Omit "Changed" methods of properties
                QByteArray methName(meta->method(index).name());
                if (!methName.isEmpty() && methName.endsWith("Changed"))
                    continue;
                dump(&methods, meta->method(index), implicitSignals, knownAttributes);
            }
        }
        if (!methods.empty())
            object->insert("methods", methods);
    }

    void dumpQMLComponent(QObject* qtobject, const QString& qmlTyName, const QString& exportString, bool isSingleton, const QStringList& internalTypes)
    {
        const QMetaObject *mainMeta = qtobject->metaObject();
        QJsonObject object;
        QStringList exportStrings(QStringList() << exportString);
        // Merge objects to get all exported versions of the same type
        if (json->contains(relocatableModuleUri + qmlTyName)) {
            object = json->value(relocatableModuleUri + qmlTyName).toObject();
            QJsonArray exports(object["exports"].toArray());
            Q_FOREACH(QJsonValue expor, exports) {
                exportStrings.append(QString("%1").arg(expor.toString()));
            }
        }
        exportStrings.removeDuplicates();
        exportStrings.sort();
        object.insert("exports", QJsonArray::fromStringList(exportStrings));
        object.insert("prototype", mainMeta->superClass()->className());
        object.insert("exportMetaObjectRevisions", QJsonArray::fromStringList((QStringList() << exportString.split(".")[1])));
        object.insert("isComposite", true);
        if (isSingleton) {
            object.insert("isCreatable", false);
            object.insert("isSingleton", true);
        }

        for (int index = mainMeta->classInfoCount() - 1 ; index >= 0 ; --index) {
            QMetaClassInfo classInfo = mainMeta->classInfo(index);
            if (QLatin1String(classInfo.name()) == QLatin1String("DefaultProperty")) {
                object.insert("defaultProperty", classInfo.value());
                break;
            }
        }

        KnownAttributes knownAttributes;
        // Strip internal _QMLTYPE_xy suffix
        QString prototype(QString(mainMeta->superClass()->className()).split("_")[0]);
        // Merge internal base class
        if (internalTypes.contains(prototype))
            writeMetaContent(&object, mainMeta->superClass(), &knownAttributes);
        writeMetaContent(&object, mainMeta, &knownAttributes);
        json->insert(relocatableModuleUri + qmlTyName, object);
    }

    void dump(const QMetaObject *meta, bool isUncreatable, bool isSingleton)
    {
        QStringList exportStrings;
        QSet<const QQmlType *> qmlTypes = qmlTypesByCppName.value(meta->className());
        if (!qmlTypes.isEmpty()) {
            bool foreignNamespace = false;
            QHash<QString, const QQmlType *> exports;

            Q_FOREACH (const QQmlType *qmlTy, qmlTypes) {
                const QString exportString = getExportString(qmlTy->qmlTypeName(), qmlTy->majorVersion(), qmlTy->minorVersion());
                if (exportString.contains("/"))
                    foreignNamespace = true;
                exports.insert( exportString, qmlTy);
            }

            // Ignore classes from different namespaces
            if (foreignNamespace)
                return;

            // ensure exports are sorted and don't change order when the plugin is dumped again
            exportStrings = exports.keys();
            std::sort(exportStrings.begin(), exportStrings.end());
        }

        QJsonObject object;

        for (int index = meta->classInfoCount() - 1 ; index >= 0 ; --index) {
            QMetaClassInfo classInfo = meta->classInfo(index);
            if (QLatin1String(classInfo.name()) == QLatin1String("DefaultProperty")) {
                object.insert("defaultProperty", classInfo.value());
                break;
            }
        }

        if (meta->superClass())
            object.insert("prototype", meta->superClass()->className());

        if (!qmlTypes.isEmpty()) {
            object.insert("exports", QJsonArray::fromStringList(exportStrings));

            if (isUncreatable)
                object.insert("isCreatable", false);
            if (isSingleton)
                object.insert("isSingleton", true);

            if (const QMetaObject *attachedType = (*qmlTypes.begin())->attachedPropertiesType()) {
                // Can happen when a type is registered that returns itself as attachedPropertiesType()
                // because there is no creatable type to attach to.
                if (attachedType != meta) {
                    object.insert("attachedType", attachedType->className());
                }
            }
        }

        for (int index = meta->enumeratorOffset(); index < meta->enumeratorCount(); ++index)
            dump(meta->enumerator(index));

        writeMetaContent(&object, meta);

        QString id(meta->className());
        // FIXME: Work-around to omit Qt types unintentionally included
        if (convertToId(meta).startsWith("Q")) {
            if (verbose)
                std::cout << "Omitting " << qPrintable(id) << std::endl;
            return;
        }

        json->insert(id, object);
    }

private:
    /* Removes pointer and list annotations from a type name, returning
       what was removed in isList and isPointer
    */
    static void removePointerAndList(QByteArray *typeName, bool *isList, bool *isPointer)
    {
        static QByteArray declListPrefix = "QQmlListProperty<";

        if (typeName->endsWith('*')) {
            *isPointer = true;
            typeName->truncate(typeName->length() - 1);
            removePointerAndList(typeName, isList, isPointer);
        } else if (typeName->startsWith(declListPrefix)) {
            *isList = true;
            typeName->truncate(typeName->length() - 1); // get rid of the suffix '>'
            *typeName = typeName->mid(declListPrefix.size());
            removePointerAndList(typeName, isList, isPointer);
        }
    }

    void writeTypeProperties(QJsonObject* object, QByteArray typeName, bool isWritable)
    {
        bool isList = false, isPointer = false;
        removePointerAndList(&typeName, &isList, &isPointer);

        object->insert("type", QString(typeName));

        if (isList)
            object->insert("isList", true);
        if (!isWritable)
            object->insert("isReadonly", true);
        if (isPointer)
            object->insert("isPointer", true);
    }

    void dump(QJsonObject* object, const QMetaProperty &prop, KnownAttributes *knownAttributes = 0)
    {
        int revision = prop.revision();
        QByteArray propName = prop.name();
        if (knownAttributes && knownAttributes->knownProperty(propName, revision))
            return;
        // Two leading underscores: internal API
        if (QString(propName).startsWith("__"))
            return;

        QJsonObject property;
        if (revision)
            property["revision"] = QString::number(revision);
        writeTypeProperties(&property, prop.typeName(), prop.isWritable());
        object->insert(prop.name(), property);
    }

    void dump(QJsonArray* array, const QMetaMethod &meth, const QSet<QString> &implicitSignals,
              KnownAttributes *knownAttributes = 0)
    {
        if (meth.methodType() == QMetaMethod::Signal) {
            if (meth.access() != QMetaMethod::Public)
                return; // nothing to do.
        } else if (meth.access() != QMetaMethod::Public) {
            return; // nothing to do.
        }

        QByteArray name = meth.name();
        // Two leading underscores: internal API
        if (name.startsWith("__"))
            return;
        const QString typeName = meth.typeName();

        if (implicitSignals.contains(name)
                && !meth.revision()
                && meth.methodType() == QMetaMethod::Signal
                && meth.parameterNames().isEmpty()
                && typeName == QLatin1String("void")) {
            // don't mention implicit signals
            return;
        }

        int revision = meth.revision();
        if (knownAttributes && knownAttributes->knownMethod(name, meth.parameterNames().size(), revision))
            return;

        QJsonObject method;
        if (meth.methodType() == QMetaMethod::Signal)
            method["type"] = "signal";
        else
            method["type"] = "function";

        if (revision)
            method["revision"] = QString::number(revision);

        if (typeName != QLatin1String("void"))
            method["returns"] = typeName;

        QJsonArray parameters;
        for (int i = 0; i < meth.parameterTypes().size(); ++i) {
            QByteArray argName = meth.parameterNames().at(i);
            QJsonObject parameter;
            parameter["name"] = QString(argName);
            writeTypeProperties(&parameter, meth.parameterTypes().at(i), true);
            parameters.append(parameter);
        }
        if (!parameters.empty())
            method.insert("parameters", parameters);
        method.insert("name", QString(name));
        array->append(method);
    }

    void dump(const QMetaEnum &e)
    {
        // FIXME: Work-around to omit Qt types unintentionally included
        if (QString(e.scope()).startsWith("Q"))
            return;

        QJsonObject object;
        object["prototype"] = e.isFlag() ? "Flag" : "Enum";

        QList<QPair<QString, QString> > namesValues;
        for (int index = 0; index < e.keyCount(); ++index) {
            object[e.key(index)] = QString::number(e.value(index));
        }
        json->insert(QString("%1::%2").arg(e.scope()).arg(e.name()), object);
    }
};

enum ExitCode {
    EXIT_INVALIDARGUMENTS = 1,
    EXIT_SEGV = 2,
    EXIT_IMPORTERROR = 3
};

#ifdef Q_OS_UNIX
void sigSegvHandler(int) {
    fprintf(stderr, "Error: SEGV\n");
    if (!currentProperty.isEmpty())
        fprintf(stderr, "While processing the property '%s', which probably has uninitialized data.\n", currentProperty.toLatin1().constData());
    if (!inObjectInstantiation.isEmpty())
        fprintf(stderr, "While instantiating the object '%s'.\n", inObjectInstantiation.toLatin1().constData());
    exit(EXIT_SEGV);
}
#endif

void printUsage(const QString &appName)
{
    std::cerr << qPrintable(QString(
                                 "Usage: %1 [-v] [-noinstantiate] [-defaultplatform] [-[non]relocatable] [-qml] [-json] module.uri version [module/import/path]\n"
                                 "       %1 [-v] [-noinstantiate] [-qml] [-json] -path path/to/qmldir/directory [version]\n"
                                 "Example: %1 -json Qt.labs.folderlistmodel 2.0 /home/user/dev/qt-install/imports").arg(
                                 appName)) << std::endl;
}

inline std::wostream &operator<<(std::wostream &str, const QString &s)
{
    str << s.toStdWString();
    return str;
}

void printDebugMessage(QtMsgType, const QMessageLogContext &, const QString &msg)
{
    std::wcerr << msg << std::endl;
    // In case of QtFatalMsg the calling code will abort() when appropriate.
}

static QObject *s_testRootObject = 0;
static QObject *testRootObject(QQmlEngine *engine, QJSEngine *jsEngine)
{
    Q_UNUSED(jsEngine);
    if (!s_testRootObject) {
        s_testRootObject = new QObject(engine);
    }
    return s_testRootObject;
}

int main(int argc, char *argv[])
{
    // The default message handler might not print to console on some systems. Enforce this.
    qInstallMessageHandler(printDebugMessage);
#ifdef Q_OS_UNIX
    // qmldump may crash, but we don't want any crash handlers to pop up
    // therefore we intercept the segfault and just exit() ourselves
    struct sigaction sigAction;

    sigemptyset(&sigAction.sa_mask);
    sigAction.sa_handler = &sigSegvHandler;
    sigAction.sa_flags   = 0;

    sigaction(SIGSEGV, &sigAction, 0);
#endif

    // don't require a window manager even though we're a QGuiApplication
    bool requireWindowManager = false;
    for (int index = 1; index < argc; ++index) {
        if (QString::fromLocal8Bit(argv[index]) == "--defaultplatform"
                || QString::fromLocal8Bit(argv[index]) == "-defaultplatform") {
            requireWindowManager = true;
            break;
        }
    }

    if (!requireWindowManager)
        qputenv("QT_QPA_PLATFORM", QByteArrayLiteral("minimal"));

    QGuiApplication app(argc, argv);
    const QStringList args = app.arguments();
    const QString appName = QFileInfo(app.applicationFilePath()).baseName();
    if (args.size() < 2) {
        printUsage(appName);
        return EXIT_INVALIDARGUMENTS;
    }

    QString pluginImportUri;
    QString pluginImportVersion;
    bool relocatable = true;
    bool output_json = false, output_qml = false;
    enum Action { Uri, Path };
    Action action = Uri;
    {
        QStringList positionalArgs;
        Q_FOREACH (const QString &arg, args) {
            if (!arg.startsWith(QLatin1Char('-'))) {
                positionalArgs.append(arg);
                continue;
            }

            if (arg == QLatin1String("--notrelocatable")
                    || arg == QLatin1String("-notrelocatable")
                    || arg == QLatin1String("--nonrelocatable")
                    || arg == QLatin1String("-nonrelocatable")) {
                relocatable = false;
            } else if (arg == QLatin1String("--relocatable")
                        || arg == QLatin1String("-relocatable")) {
                relocatable = true;
            } else if (arg == QLatin1String("--json")
                        || arg == QLatin1String("-json")) {
                output_json = true;
            } else if (arg == QLatin1String("--qml")
                        || arg == QLatin1String("-qml")) {
                output_qml = true;
            } else if (arg == QLatin1String("--noinstantiate")
                       || arg == QLatin1String("-noinstantiate")) {
                creatable = false;
            } else if (arg == QLatin1String("--path")
                       || arg == QLatin1String("-path")) {
                action = Path;
            } else if (arg == QLatin1String("-v")) {
                verbose = true;
            } else if (arg == QLatin1String("--defaultplatform")
                       || arg == QLatin1String("-defaultplatform")) {
                continue;
            } else {
                std::cerr << "Invalid argument: " << qPrintable(arg) << std::endl;
                return EXIT_INVALIDARGUMENTS;
            }
        }

        if (action == Uri) {
            if (positionalArgs.size() != 3 && positionalArgs.size() != 4) {
                std::cerr << "Incorrect number of positional arguments" << std::endl;
                return EXIT_INVALIDARGUMENTS;
            }
            pluginImportUri = positionalArgs[1];
            if (positionalArgs[2].toDouble() != 0.0) {
                pluginImportVersion = positionalArgs[2];
                if (positionalArgs.size() >= 4)
                    pluginImportPath = positionalArgs[3];
            } else
                pluginImportPath = positionalArgs[2];
        } else if (action == Path) {
            if (positionalArgs.size() != 2 && positionalArgs.size() != 3) {
                std::cerr << "Incorrect number of positional arguments" << std::endl;
                return EXIT_INVALIDARGUMENTS;
            }
            pluginImportPath = QDir::fromNativeSeparators(positionalArgs[1]);
            if (positionalArgs.size() == 3)
                pluginImportVersion = positionalArgs[2];
        }
    }

    // By default output JSON
    if (!output_json and !output_qml)
        output_json = true;

    if (!verbose)
        QLoggingCategory::setFilterRules(QStringLiteral("*=false"));

    // Allow import of Qt.Test
    qmlRegisterSingletonType<QObject>("Qt.test.qtestroot", 1, 0, "QTestRootObject", testRootObject);

    QQmlEngine engine;
    QObject::connect(&engine, SIGNAL(quit()), QCoreApplication::instance(), SLOT(quit()));
    if (!pluginImportPath.isEmpty()) {
        QDir cur = QDir::current();
        cur.cd(pluginImportPath);
        pluginImportPath = cur.canonicalPath();
        QDir::setCurrent(pluginImportPath);
        engine.addImportPath(pluginImportPath);
    }

    // load the QtQuick 2 plugin
    {
        QByteArray code("import QtQuick 2.0\nQtObject {}");
        QQmlComponent c(&engine);
        c.setData(code, QUrl::fromLocalFile(pluginImportPath + "/loadqtquick2.qml"));
        c.create();
        if (!c.errors().isEmpty()) {
            std::cerr << "Failed to load QtQuick2 built-in" << std::endl;
            Q_FOREACH (const QQmlError &error, c.errors())
                std::cerr << qPrintable( error.toString() ) << std::endl;
            return EXIT_IMPORTERROR;
        }
    }

    // find all QMetaObjects reachable from the builtin module
    QSet<const QMetaObject *> uncreatableMetas;
    QSet<const QMetaObject *> singletonMetas;
    QSet<const QMetaObject *> defaultReachable = collectReachableMetaObjects(&engine, uncreatableMetas, singletonMetas);
    QList<QQmlType *> defaultTypes = QQmlMetaType::qmlTypes();

    // add some otherwise unreachable QMetaObjects
    defaultReachable.insert(&QQuickMouseEvent::staticMetaObject);
    // QQuickKeyEvent, QQuickPinchEvent, QQuickDropEvent are not exported
    QSet<QByteArray> defaultReachableNames;

    // this will hold the meta objects we want to dump information of
    QSet<const QMetaObject *> metas;

    // find a valid QtQuick import
    QByteArray importCode;
    QQmlType *qtObjectType = QQmlMetaType::qmlType(&QObject::staticMetaObject);
    if (!qtObjectType) {
        std::cerr << "Could not find QtObject type" << std::endl;
        importCode = QByteArray("import QtQuick 2.0\n");
    } else {
        QString module = qtObjectType->qmlTypeName();
        module = module.mid(0, module.lastIndexOf(QLatin1Char('/')));
        importCode = QString("import %1 %2.%3\n").arg(module,
                                                      QString::number(qtObjectType->majorVersion()),
                                                      QString::number(qtObjectType->minorVersion())).toUtf8();
    }

    QStringList internalTypes;
    // create an object that will be the API description
    QJsonObject json;

    QStringList modules(pluginImportUri.split(" "));
    Q_FOREACH (const QString& plugin, modules) {
        pluginImportUri = plugin;
        pluginImportVersion = "";

    QString pluginModulePath(pluginImportPath + "/" + QString(pluginImportUri).replace(".", "/"));
    QFile f(pluginModulePath + "/qmldir");
    if (!f.open(QIODevice::ReadOnly)) {
        std::cerr << "Failed to read " << qPrintable(f.fileName()) << std::endl;
        return EXIT_IMPORTERROR;
    }
    QQmlDirParser p;
    p.parse(f.readAll());
    if (p.hasError()) {
        std::cerr << "Failed to read " << qPrintable(f.fileName()) << std::endl;
        Q_FOREACH (const QQmlError &error, p.errors("qmldir"))
            std::cerr << qPrintable( error.toString() ) << std::endl;
        return EXIT_IMPORTERROR;
    }
    if (pluginImportUri.isEmpty())
        pluginImportUri = p.typeNamespace();

    // find all QMetaObjects reachable when the specified module is imported
    QStringList importVersions;
    if (pluginImportVersion.isEmpty()) {
        // Collect all the available versioned imports
        int highestMajor = 0, highestMinor = 1;
        Q_FOREACH(QQmlDirParser::Component c, p.components()) {
            // pluginImportVersion can be empty, pick latest
            if (c.majorVersion > highestMajor) {
                highestMajor = c.majorVersion;
                highestMinor = c.minorVersion;
            } else if (c.majorVersion == highestMajor && c.minorVersion > highestMinor) {
                highestMinor = c.minorVersion;
            }

            // Work-around for version -1, -1
            if (c.majorVersion == -1)
                continue;
            importVersions << QString("%1.%2").arg(c.majorVersion).arg(c.minorVersion);
        }
        pluginImportVersion = QString("%1.%2").arg(highestMajor).arg(highestMinor);
    }
    importVersions << pluginImportVersion;
    importVersions.removeDuplicates();

    // Create a component with all QML types to add them to the type system
    QByteArray code = importCode;
    Q_FOREACH(const QString& version, importVersions) {
        QString pluginAlias(QString("%1.%2").arg(pluginImportUri).arg(version).replace(".", "_"));
        code += QString("import %1 %2 as %3\n").arg(pluginImportUri).arg(version).arg(pluginAlias);
    }
    code += "Item {\n";

    QStringList exportedTypes;
    QMap<QString, QString> exportedVersions;
    Q_FOREACH(QQmlDirParser::Component c, p.components()) {
        // Map filename-based PageHeadConfiguration11 to PageHeadConfiguration
        QString filename(QFileInfo(c.fileName).baseName());
        cppToId.insert(qPrintable(filename), qPrintable(c.typeName));

        if (c.internal) {
            internalTypes.append(c.typeName);
            continue;
        }
        exportedTypes.append(QFileInfo(c.fileName).fileName());
        QString version(QString("%1.%2").arg(c.majorVersion).arg(c.minorVersion));
        if (version > exportedVersions[c.typeName])
            exportedVersions.insert(c.typeName, version);
    }

    code += "}";
    if (verbose)
        std::cerr << "Importing QML components:" << std::endl << qPrintable(code) << std::endl;

    QQmlComponent c(&engine);
    c.setData(code, QUrl::fromLocalFile(pluginModulePath + "/qmldir"));
    std::cerr << "Creating QML component for " << qPrintable(pluginImportUri) << std::endl;
    c.create();
    if (!c.errors().isEmpty()) {
        Q_FOREACH (const QQmlError &error, c.errors()) {
            // Despite the error we get all type information we need from singletons
            if (error.description().contains(QRegExp("(Composite Singleton Type .+|Element) is not creatable")))
                continue;
            std::cerr << "Failed to load " << qPrintable(pluginImportUri) << std::endl;
            std::cerr << qPrintable(code) << std::endl;
            std::cerr << qPrintable( error.toString() ) << std::endl;
            return EXIT_IMPORTERROR;
        }
    }

    QSet<const QMetaObject *> candidates = collectReachableMetaObjects(&engine, uncreatableMetas, singletonMetas, defaultTypes);
    candidates.subtract(defaultReachable);

    // Also eliminate meta objects with the same classname.
    // This is required because extended objects seem not to share
    // a single meta object instance.
    Q_FOREACH (const QMetaObject *mo, defaultReachable)
        defaultReachableNames.insert(QByteArray(mo->className()));
    Q_FOREACH (const QMetaObject *mo, candidates) {
        if (!defaultReachableNames.contains(mo->className()))
            metas.insert(mo);
    }

    // setup static rewrites of type names
    cppToId.insert("QString", "string");
    cppToId.insert("QUrl", "url");
    cppToId.insert("QVariant", "var");
    cppToId.insert("QVector2D", "vector2d");
    cppToId.insert("QVector3D", "vector3d");
    cppToId.insert("QVector4D", "vector4d");
    cppToId.insert("QPoint", "Qt.point");
    cppToId.insert("QColor", "color");
    cppToId.insert("QQmlEasingValueType::Type", "Type");

    QMap<QString, QString> scripts;
    Q_FOREACH(QQmlDirParser::Script s, p.scripts()) {
        QFile sf(pluginModulePath + "/" + s.fileName);
        if (!sf.open(QIODevice::ReadOnly)) {
            std::cerr << "Failed to read " << qPrintable(sf.fileName()) << std::endl;
            return EXIT_IMPORTERROR;
        }
        scripts.insertMulti(s.nameSpace, QString("%1 %2.%3").arg(s.nameSpace).arg(s.majorVersion).arg(s.minorVersion));
    }

    Q_FOREACH(QString nameSpace, scripts.uniqueKeys()) {
        QJsonObject script;
        script["type"] = "script";
        QJsonArray exports(QJsonArray::fromStringList(scripts.values(nameSpace)));
        script["exports"] = exports;
        json[nameSpace] = script;
    }

    // put the metaobjects into a map so they are always dumped in the same order
    QMap<QString, const QMetaObject *> nameToMeta;
    Q_FOREACH (const QMetaObject *meta, metas)
        nameToMeta.insertMulti(convertToId(meta), meta);

    Dumper dumper(&json);
    if (relocatable)
        dumper.setRelocatableModuleUri(pluginImportUri);
    dumper.setImportVersion(pluginImportVersion);
    Q_FOREACH (const QMetaObject *meta, nameToMeta) {
        dumper.dump(meta, uncreatableMetas.contains(meta), singletonMetas.contains(meta));
    }
    Q_FOREACH(QQmlDirParser::Component c, p.components()) {
        if (c.internal)
            continue;
        QString version(QString("%1.%2").arg(c.majorVersion).arg(c.minorVersion));
        if (c.majorVersion == -1)
            version = pluginImportVersion;
        QQmlComponent e(&engine, pluginModulePath + "/" + c.fileName);
        QObject* qtobject(e.create());
        if (!qtobject) {
            std::cerr << "Failed to instantiate " << qPrintable(c.typeName) << " from " << qPrintable(pluginModulePath + "/" + c.fileName) << std::endl;
            Q_FOREACH (const QQmlError &error, e.errors())
                std::cerr << qPrintable(error.toString()) << std::endl;
            exit(1);
        }
        dumper.dumpQMLComponent(qtobject, c.typeName, QString("%1 %2").arg(c.typeName).arg(version), c.singleton, internalTypes);
    }

    }

    if (output_json) {
        // write JSON representation of the API
        QJsonDocument jsonDoc(json);
        std::cout << qPrintable(jsonDoc.toJson());
    }

    if (output_qml) {
        // write QML representation of the API
        QStringList keys(json.keys());

        // Sort by exports
        QMap<QString, QString>byExports;
        Q_FOREACH(const QString& key, keys) {
            QJsonValue value(json[key]);
            if (value.isObject()) {
                QJsonObject object(value.toObject());
                QJsonArray exports(object["exports"].toArray());
                QStringList exportsSl;
                Q_FOREACH(QJsonValue expor, exports) {
                    exportsSl.append(convertToId(expor.toString()));
                }
                // Reverse exports: latest to oldest
                for(int k = 0; k < (exportsSl.size() / 2); k++)
                    exportsSl.swap(k, exportsSl.size() - (1 + k));
                QString sortedExports;
                if (!exportsSl.isEmpty())
                    sortedExports = exportsSl.join(" ");
                else
                    sortedExports = convertToId(key);
                // Exports may not be unique across namespaces ie. Header
                byExports.insertMulti(sortedExports, key);
            }
        }

        QMap<QString, QString>::const_iterator i = byExports.constBegin();
        while (i != byExports.constEnd()) {
            QString exports(i.key());
            QString key(i.value());
            QJsonValue value(json[key]);
            if (value.isObject()) {
                QJsonObject object(value.toObject());
                QString signature(exports);
                if (object.contains("prototype"))
                    signature += " " + convertToId(object["prototype"].toString());
                if (object.contains("isSingleton"))
                    signature += " singleton";
                signature += "\n";
                Q_FOREACH(const QString& fieldName, object.keys()) {
                    if (fieldName == "exports" || fieldName == "prototype" || fieldName == "type")
                        continue;
                    if (fieldName == "methods") {
                        QJsonArray values(object[fieldName].toArray());
                        Q_FOREACH(const QJsonValue& value, values) {
                            QJsonObject valu(value.toObject());
                            signature += "    ";
                            signature += valu["type"].toString() + " ";
                            if (valu.contains("returns"))
                                signature += convertToId(valu["returns"].toString()) + " ";
                            QStringList args;
                            if (valu.contains("parameters")) {
                                Q_FOREACH(const QJsonValue& parameterValue, valu["parameters"].toArray()) {
                                    QJsonObject parameter(parameterValue.toObject());
                                    args.append(convertToId(parameter["type"].toString()) + " " + parameter["name"].toString());
                                }
                            }
                            signature += valu["name"].toString() + "(" + args.join(", ") + ")\n";
                        }
                        continue;
                    }
                    QJsonObject field(object[fieldName].toObject());
                    if (!field.contains("type") && object["prototype"] != "Enum" && object["prototype"] != "Flag")
                        continue;
                    signature += "    ";
                    if (object["prototype"] != "Enum" && object["prototype"] != "Flag") {
                        if (object["defaultProperty"] == fieldName)
                            signature += "default ";
                        if (field.contains("isReadonly"))
                            signature += "readonly ";
                        signature += "property ";
                    }
                    if (field.contains("type"))
                        signature += QString(convertToId(field["type"].toString())) + " ";
                    signature += fieldName;
                    signature += "\n";
                }
                std::cout << qPrintable(signature);
            }
            i++;
        }
    }

    // workaround to avoid crashes on exit
    QTimer timer;
    timer.setSingleShot(true);
    timer.setInterval(0);
    QObject::connect(&timer, SIGNAL(timeout()), &app, SLOT(quit()));
    timer.start();

    return app.exec();
}
