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

#ifndef UBUNTU_COMPONENTS_I18N_H
#define UBUNTU_COMPONENTS_I18N_H

#include <QObject>

class QQmlContext;

class UbuntuI18n : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString domain READ domain WRITE setDomain)

public:
    static UbuntuI18n& instance() {
        static UbuntuI18n instance;
        return instance;
    }

    explicit UbuntuI18n(QObject* parent = 0);

    static void qmlRegisterTypes(const char* uri);

    /**
     * Add an object named "gettext" to context.
     *
     * One can then get translations with gettext.gettext("english text")
     */
    void qmlInit(QQmlContext* context);

    /**
     * @brief Translate the given string using gettext.
     * @param text The text to translate.
     */
    Q_INVOKABLE QString tr(const QString& text);

    /**
     * @brief Translate the given string using gettext. Should be called like this:
     *          tr("%n file", "%n files", count)
     * @param singular The singular version of the text to translate.
     * @param plural The plural version of the text to translate.
     * @param n Number of items.
     */
    Q_INVOKABLE QString ntr(const QString& singular, const QString& plural, int n);

    /**
     * @brief Translate the given string using gettext.
     * @param domain The domain to use for the translation.
     * @param text The text to translate.
     */
    Q_INVOKABLE QString dtr(const QString& domain, const QString& text);

    /**
     * @brief Translate the given string using gettext. Should be called like this:
     *          tr(domain, "%n file", "%n files", count)
     * @param domain The domain to use for the translation.
     * @param singular The singular version of the text to translate.
     * @param plural The plural version of the text to translate.
     * @param n Number of items.
     */
    Q_INVOKABLE QString dntr(const QString& domain, const QString& singular, const QString& plural, int n);


    // getter
    QString domain();

    // setter
    void setDomain(QString domain);

Q_SIGNALS:

    void domainChanged();

private:
    QString _domain;
};

class ContextPropertyChangeListener : public QObject
{
    Q_OBJECT
public:
    explicit ContextPropertyChangeListener(QQmlContext* context, QString contextProperty);
    Q_SLOT void updateContextProperty();
    QQmlContext* m_context;
    QString m_contextProperty;
};

#endif // UBUNTU_COMPONENTS_I18N_H
