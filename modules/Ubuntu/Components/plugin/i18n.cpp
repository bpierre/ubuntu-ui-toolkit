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
 */

#include "i18n.h"

// libc
#include <libintl.h>
//#include "/usr/share/gettext/gettext.h"
//#include <gettext-po.h>
#include <QDir>
#include <QtCore>
#include <QtQml/QQmlContext>

//#include <stdio.h>

UbuntuI18n::UbuntuI18n(QObject* parent) : QObject(parent)
{
}

QString UbuntuI18n::domain() {
    return _domain;
}

void UbuntuI18n::setDomain(QString domain) {
    _domain = domain;
    textdomain(domain.toUtf8().constData());
    Q_EMIT domainChanged();
}

QString UbuntuI18n::tr(const QString& text, const QString& domain)
{
    if (domain.isNull()) {
        return QString::fromUtf8(dgettext(NULL, text.toUtf8().constData()));
    } else {
        return QString::fromUtf8(dgettext(domain.toUtf8().constData(), text.toUtf8().constData()));
    }
}

QString UbuntuI18n::tr(const QString& singular, const QString& plural, int n, const QString& domain)
{
    if (domain.isNull()) {
        return QString::fromUtf8(dngettext(NULL, singular.toUtf8().constData(), plural.toUtf8().constData(), n));
    } else {
        return QString::fromUtf8(dngettext(domain.toUtf8().constData(), singular.toUtf8().constData(), plural.toUtf8().constData(), n));
    }
}

ContextPropertyChangeListener::ContextPropertyChangeListener(QQmlContext *context,QString contextProperty) :
    m_context(context),
    m_contextProperty(contextProperty)
{
}

void ContextPropertyChangeListener::updateContextProperty()
{
    QVariant value = m_context->contextProperty(m_contextProperty);
    m_context->setContextProperty(m_contextProperty, value);
}
