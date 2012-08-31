#ifndef TESTCASEEXECUTIONDIALOG_H
#define TESTCASEEXECUTIONDIALOG_H

#include <QDialog>
#include <QtDeclarative>

namespace Ui {
    class TestCaseExecutionDialog;
}

class TestCaseExecutionDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TestCaseExecutionDialog(QString testcase, QString testdata, QUrl qmlFile, QWidget *parent = 0);
    ~TestCaseExecutionDialog();

protected slots:
    void on_pushButton_clicked();
    void on_pushButtonQuit_clicked();
    
private:
    QDeclarativeView* m_declarativeView;
    Ui::TestCaseExecutionDialog *m_ui;
};

#endif // TESTCASEEXECUTIONDIALOG_H
