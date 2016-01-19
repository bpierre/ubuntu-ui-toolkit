#ifndef UCMAINVIEWBASE_H
#define UCMAINVIEWBASE_H

#include "ucpagetreenode.h"

class UCMainViewBasePrivate;
class UCActionManager;
class UCAction;

class UCMainViewBase : public UCPageTreeNode
{
    Q_OBJECT
    Q_PROPERTY(QString applicationName READ applicationName WRITE setApplicationName NOTIFY applicationNameChanged)
    Q_PROPERTY(bool anchorToKeyboard READ anchorToKeyboard WRITE setAnchorToKeyboard NOTIFY anchorToKeyboardChanged)
    Q_PROPERTY(QColor headerColor READ headerColor WRITE setHeaderColor NOTIFY headerColorChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(QColor footerColor READ footerColor WRITE setFooterColor NOTIFY footerColorChanged)
    Q_PROPERTY(QQmlListProperty<UCAction> actions READ actions)
    Q_PROPERTY(UCActionManager* actionManager READ actionManager)

public:
    UCMainViewBase(QQuickItem *parent = nullptr);


    QString applicationName() const;
    void setApplicationName(QString applicationName);

    bool anchorToKeyboard() const;
    void setAnchorToKeyboard(bool anchorToKeyboard);

    QColor headerColor() const;
    void setHeaderColor(QColor headerColor);

    QColor backgroundColor() const;
    void setBackgroundColor(QColor backgroundColor);

    QColor footerColor() const;
    void setFooterColor(QColor footerColor);

    QQmlListProperty<UCAction> actions() const;

    UCActionManager *actionManager() const;

Q_SIGNALS:
    void applicationNameChanged(QString applicationName);
    void anchorToKeyboardChanged(bool anchorToKeyboard);
    void headerColorChanged(QColor headerColor);
    void backgroundColorChanged(QColor backgroundColor);
    void footerColorChanged(QColor footerColor);

protected:
    UCMainViewBase(UCMainViewBasePrivate &dd, QQuickItem *parent);

private:
    Q_DECLARE_PRIVATE(UCMainViewBase)
};

#endif // UCMAINVIEWBASE_H
