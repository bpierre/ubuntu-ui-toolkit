/*
 * Copyright 2016 Canonical Ltd.
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
 * Author: Loïc Molinari <loic.molinari@canonical.com>
 */

#ifndef UCSTROKESHAPE_H
#define UCSTROKESHAPE_H

#include <QtQuick/QQuickItem>
#include <QtQuick/QSGMaterial>
#include <QtQuick/QSGNode>

class UCStrokeShapeMaterial : public QSGMaterial
{
public:
    UCStrokeShapeMaterial();
    virtual QSGMaterialType* type() const;
    virtual QSGMaterialShader* createShader() const;
    virtual int compare(const QSGMaterial* other) const;

    quint32 textureId() const { return m_textureId; }

private:
    quint32 m_textureId;
};

class UCStrokeShapeNode : public QSGGeometryNode
{
public:
  struct Vertex { float x, y, s1, t1, s2, t2; quint32 color; };

    static const unsigned short* indices();
    static const QSGGeometry::AttributeSet& attributeSet();

    UCStrokeShapeNode();
    void updateGeometry(const QSizeF& itemSize, float strokeSize, float radiusSize, QRgb color);

private:
    UCStrokeShapeMaterial m_material;
    QSGGeometry m_geometry;
};

class UCStrokeShape : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(qreal size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    UCStrokeShape(QQuickItem* parent = 0);

    qreal size() const { return m_size; }
    void setSize(qreal size);
    qreal radius() const { return m_radius; }
    void setRadius(qreal radius);
    QColor color() const {
      return QColor(qRed(m_color), qGreen(m_color), qBlue(m_color), qAlpha(m_color)); }
    void setColor(const QColor& color);

Q_SIGNALS:
    void sizeChanged();
    void radiusChanged();
    void colorChanged();

private:
    virtual QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data);

    QRgb m_color;
    float m_size;
    float m_radius;

    Q_DISABLE_COPY(UCStrokeShape)
};

QML_DECLARE_TYPE(UCStrokeShape)

#endif  // UCSTROKESHAPE_H
