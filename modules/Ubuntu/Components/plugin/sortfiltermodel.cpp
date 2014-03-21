/*
 * Copyright (C) 2012-2014 Canonical, Ltd.
 *
 * Authors:
 *   Michal Hruby <michal.hruby@canonical.com>
 *   Christian Dywan <christian.dywan@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "sortfiltermodel.h"

/*!
 * \qmltype SortFilterModel
 * \instantiates QSortFilterProxyModel
 * \inqmlmodule Ubuntu.Components 0.1
 * \ingroup ubuntu
 * \brief SortFilterModel lorem ipsum qml sit amet.
 *
 * Carpe noctem.
 *
 * Example usage:
 * \qml
 * import QtQuick 2.0
 * import Ubuntu.Components 0.1
 * import Ubuntu.Components.ListItems 0.1
 *
 * ListModel {
 *     ListElement { title: “Esign”; producer: “Chris Larkee” }
 *     ListElement { title: “Elephants Dream”; producer “Blender” }
 *     ListElement { title: “Big Buck Bunny”; producer “Blender” }
 * }
 *
 * SortFilterModel {
 *     model: movies
 *     sort.property: "title"
 *     sort.order: Qt.DescendingOrder
 *
 *     filter.property: "producer
 *     filter.pattern: /blender/
 * }
 *
 * ListView {
 *     model: sortedMovies
 *     width: units.gu(80)
 *     height: units.gu(100)
 *
 *     delegate: Subtitled {
 *         text: title
 *         subText: producer
 *     }
 *
 *     section.delegate: sectionDelegate
 *     section.property: "title"
 *     section.criteria: ViewSection.FirstCharacter
 * }
 *
 * Label {
 *     text: {
 *         var firstRow = sortedMovies.findFirst("title", /Elephant/))
 *         var sourceRow = sortedMovies.mapToSource(firstRow)
 *         var foundTitle = movies.get(sourceRow).title
 *         i18n.tr("First matching movie: %1").arg(foundTitle)
 *     }
 * }
 * \endqml
 *
 * Pay attention to the differences between the original model and the result:
 * - Big Buck Bunny will be the first row, because it's sorted by title
 * - Esign won't be visible, because it's from the wrong producer
 * - The label displays a title extracted via Javascript
 */


QSortFilterProxyModelQML::QSortFilterProxyModelQML(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    // This is virtually always what you want in QML
    setDynamicSortFilter(true);
    connect(this, SIGNAL(modelReset()), SIGNAL(countChanged()));
    connect(this, SIGNAL(rowsInserted(QModelIndex,int,int)), SIGNAL(countChanged()));
    connect(this, SIGNAL(rowsRemoved(QModelIndex,int,int)), SIGNAL(countChanged()));
    connect(&m_sortBehavior, &SortBehavior::propertyChanged, this, &QSortFilterProxyModelQML::sortChanged);
    connect(&m_sortBehavior, &SortBehavior::orderChanged, this, &QSortFilterProxyModelQML::sortChanged);
    connect(&m_filterBehavior, &FilterBehavior::propertyChanged, this, &QSortFilterProxyModelQML::filterChanged);
    connect(&m_filterBehavior, &FilterBehavior::patternChanged, this, &QSortFilterProxyModelQML::filterChanged);
}

int
QSortFilterProxyModelQML::roleByName(const QString& roleName) const
{
    const QHash<int, QByteArray> roles = roleNames();
    for(int role = 0; role < roles.count(); role++)
        if (roles[role] == roleName)
            return role;
    return 0;
}

SortBehavior*
QSortFilterProxyModelQML::sortBehavior()
{
    return &m_sortBehavior;
}

FilterBehavior*
QSortFilterProxyModelQML::filterBehavior()
{
    return &m_filterBehavior;
}

QString
SortBehavior::property() const
{
    return m_property;
}

Qt::SortOrder
SortBehavior::order() const
{
    return m_order;
}

void
SortBehavior::setProperty(const QString& property)
{
    m_property = property;
    Q_EMIT propertyChanged();
}

void
SortBehavior::setOrder(Qt::SortOrder order)
{
    m_order = order;
    Q_EMIT orderChanged();
}

void
QSortFilterProxyModelQML::sortChanged()
{
    setSortRole(roleByName(m_sortBehavior.property()));
    sort(sortColumn() != -1 ? sortColumn() : 0, m_sortBehavior.order());
}

QString
FilterBehavior::property() const
{
    return m_property;
}

void
FilterBehavior::setProperty(const QString& property)
{
    m_property = property;
    Q_EMIT propertyChanged();
}

QRegExp
FilterBehavior::pattern() const
{
    return m_pattern;
}

void
FilterBehavior::setPattern(QRegExp pattern)
{
    m_pattern = pattern;
    Q_EMIT patternChanged();
}

void
QSortFilterProxyModelQML::filterChanged()
{
    setFilterRole(roleByName(m_filterBehavior.property()));
    setFilterRegExp(m_filterBehavior.pattern());
}

QHash<int, QByteArray> QSortFilterProxyModelQML::roleNames() const
{
    return sourceModel() ? sourceModel()->roleNames() : QHash<int, QByteArray>();
}

void
QSortFilterProxyModelQML::setModel(QAbstractItemModel *itemModel)
{
    if (itemModel == NULL) {
        return;
    }

    if (itemModel != sourceModel()) {
        if (sourceModel() != NULL) {
            sourceModel()->disconnect(this);
        }

        setSourceModel(itemModel);
        Q_EMIT modelChanged();
    }
}

QVariantMap
QSortFilterProxyModelQML::get(int row)
{
    QVariantMap res;
    const QHash<int, QByteArray> roles = roleNames();
    for(int role = 0; role < roles.count(); role++)
        res.insert (roles[role], data(row, role));
    return res;
}

QVariant
QSortFilterProxyModelQML::data(int row, int role)
{
    if (sourceModel() == NULL) {
        return QVariant();
    }

    return index(row, 0).data(role);
}

int
QSortFilterProxyModelQML::count()
{
    return rowCount();
}

bool
QSortFilterProxyModelQML::filterAcceptsRow(int sourceRow,
                                           const QModelIndex &sourceParent) const
{
    if (filterRegExp().isEmpty()) {
        return true;
    }

    bool result = QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
    return result;
}

int
QSortFilterProxyModelQML::findFirst(const QString& roleName, const QVariant& value) const
{
    int role(roleByName(roleName));
    QModelIndexList matches = match(index(0, 0), role, value, 1, Qt::MatchExactly);
    if (!matches.isEmpty()) {
        return matches.first().row();
    } else {
        return -1;
    }
}

int
QSortFilterProxyModelQML::mapFromSource(int row)
{
    if (sourceModel() != NULL) {
        return QSortFilterProxyModel::mapFromSource(sourceModel()->index(row, 0)).row();
    } else {
        return -1;
    }
}

int
QSortFilterProxyModelQML::mapToSource(int row)
{
    if (sourceModel() != NULL) {
        return QSortFilterProxyModel::mapToSource(index(row, 0)).row();
    } else {
        return -1;
    }
}

#include "moc_sortfiltermodel.cpp"

