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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef SUFFIXTREE_P_H
#define SUFFIXTREE_P_H

#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QList>

// node of a selector
class SelectorNode {
    public:
    enum Relationship {Child, Descendant, Sibling};
    enum NodeSensitivity {
        Normal = 0,
        IgnoreRelationship = 0x01,
        IgnoreStyleId = 0x02,
        IgnoreDerivates = 0x04,
        IgnoreAll = IgnoreRelationship | IgnoreStyleId | IgnoreDerivates};
    SelectorNode();
    SelectorNode(const QString &selectorString, NodeSensitivity sensitivity = Normal);
    void setMultipleClasses(const QString &value);
    QString multipleClasses() const;
    QString toString(bool appendDerivates = false) const;
    bool operator==(const SelectorNode &other);
    QString className;
    QString styleClass;
    QString styleId;
    QString derives;
    Relationship relationship;
    int sensitivity;
};

// selector type
class Selector : public QList<SelectorNode> {
public:
    inline Selector() {}
    inline Selector(const Selector& s) : QList<SelectorNode>(s){}
    Selector(const QString &string, SelectorNode::NodeSensitivity sensitivity = SelectorNode::Normal);
    virtual ~Selector() {}
    QString toString(bool appendDerivates = true) const;
};
Q_DECLARE_TYPEINFO(Selector, Q_MOVABLE_TYPE);
uint qHash(const Selector &key);

// style rule tree
class QQmlComponent;
class StyleTreeNode {
public:
    StyleTreeNode(StyleTreeNode *parent = 0);
    StyleTreeNode(StyleTreeNode *parent, const SelectorNode &node, QQmlComponent *style, QQmlComponent *delegate);
    ~StyleTreeNode();
    void clear();
    void addStyleRule(const Selector &path, QQmlComponent *style, QQmlComponent *delegate);
    Selector path() const;
    StyleTreeNode *lookupStyleRule(const Selector &path, bool strict = false);
    StyleTreeNode *testNode(SelectorNode &nextNode, const Selector &sparePath, bool &strict);

public:
    StyleTreeNode *parent;
    SelectorNode styleNode;
    QQmlComponent *style;
    QQmlComponent *delegate;
    // the key is the next style node's "relationship class#name" combination
    QHash<QString, StyleTreeNode*> children;
};



#endif // SUFFIXTREE_P_H
