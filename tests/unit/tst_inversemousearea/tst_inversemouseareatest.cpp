/*
 * Copyright 2012 Canonical Ltd.
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
 *
 */

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>

#include "themeengine.h"
#include "themeengine_p.h"
#include "inversemouseareatype.h"
#include "ucstyle.h"

class tst_InverseMouseAreaTest : public QObject
{
    Q_OBJECT
    
public:
    tst_InverseMouseAreaTest() {}

private:
    QQuickView *quickView;
    QQmlEngine *quickEngine;
    QObjectCleanupHandler eventCleanup;

    InverseMouseAreaType *testArea(const QString &document, const QUrl &theme = QUrl())
    {
        // delete previous root
        QObject *rootObject = quickView->rootObject();
        if (rootObject)
            delete rootObject;
        QTest::waitForEvents();

        ThemeEngine::initializeEngine(quickEngine);
        if (theme.isValid()) {
            ThemeEngine::instance()->loadTheme(theme);
            if (!ThemeEngine::instance()->error().isEmpty()) {
                QWARN("Theme loading failed");
                return 0;
            }
        } else
            ThemeEngine::instance()->resetError();
        quickView->setSource(QUrl::fromLocalFile(document));
        QCoreApplication::processEvents();

        rootObject = quickView->rootObject();
        if (!rootObject)
            return 0;

        QList<QQuickItem*> items = rootObject->findChildren<QQuickItem*>();
        Q_FOREACH(QQuickItem *item, items) {
            InverseMouseAreaType *area = qobject_cast<InverseMouseAreaType*>(item);
            if (area)
                return area;
        }
        return 0;
    }

protected Q_SLOTS:
    void capturePressed(QQuickMouseEvent *event)
    {
        eventCleanup.add((QObject*)event);
    }

private Q_SLOTS:

    void initTestCase()
    {
        QString modules("../../../modules");
        QVERIFY(QDir(modules).exists());

        quickView = new QQuickView(0);
        quickEngine = quickView->engine();

        quickView->setGeometry(0,0, 240, 320);
        //add modules folder so we have access to the plugin from QML
        QStringList imports = quickEngine->importPathList();
        imports << QDir(modules).absolutePath();
        quickEngine->setImportPathList(imports);

        bool result = (ThemeEngine::initializeEngine(quickEngine) != 0);
        QVERIFY(result);
        // check if theme gets loaded
        QCOMPARE(ThemeEngine::instance()->error(), QString(""));
    }

    void cleanupTestCase()
    {
        delete quickView;
    }

    void testCase_DoNotPropagateEvents()
    {
        eventCleanup.clear();
        InverseMouseAreaType *area = testArea("InverseMouseAreaDoNotPropagateEvents.qml");
        QVERIFY(area);
        // connect pressed signal to capture mouse object
        QObject::connect(area, SIGNAL(pressed(QQuickMouseEvent*)), this, SLOT(capturePressed(QQuickMouseEvent*)));
        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(10, 10));
        QTest::waitForEvents();
        QVERIFY(eventCleanup.isEmpty());
    }


    void testCase_PropagateEvents()
    {
        eventCleanup.clear();
        InverseMouseAreaType *area = testArea("InverseMouseAreaPropagateEvents.qml");
        QVERIFY(area);
        // connect pressed signal to capture mouse object
        QObject::connect(area, SIGNAL(pressed(QQuickMouseEvent*)), this, SLOT(capturePressed(QQuickMouseEvent*)));
        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(10, 10));
        QTest::waitForEvents();
        QVERIFY(eventCleanup.isEmpty());
    }

};

QTEST_MAIN(tst_InverseMouseAreaTest)

#include "tst_inversemouseareatest.moc"
