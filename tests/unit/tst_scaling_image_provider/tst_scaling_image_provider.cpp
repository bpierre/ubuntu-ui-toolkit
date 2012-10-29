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
 * Author: Florian Boucault <florian.boucault@canonical.com>
 */

#include <QtTest/QtTest>
#include "ucscalingimageprovider.h"

class tst_UCScalingImageProvider: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void scaleToHalfSize() {
        UCScalingImageProvider provider;
        QImage result;
        QSize returnedSize;
        QImage expected;

        result = provider.requestImage("0.5/" + QDir::currentPath() + QDir::separator() + "input.png", &returnedSize, QSize());
        expected = QImage("scaled_half.png");

        QCOMPARE(result, expected);
        QCOMPARE(returnedSize, expected.size());
    }

    void qrcSupport() {
        UCScalingImageProvider provider;
        QImage result;
        QSize returnedSize;
        QImage expected;

        result = provider.requestImage("0.5/:/test/prefix/input.png", &returnedSize, QSize());
        expected = QImage("scaled_half.png");

        QCOMPARE(result, expected);
        QCOMPARE(returnedSize, expected.size());
    }
};

QTEST_MAIN(tst_UCScalingImageProvider)

#include "tst_scaling_image_provider.moc"
