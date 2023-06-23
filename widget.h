#pragma once

#include <QDebug>
#include <QFileDialog>
#include <QProcess>
#include <QWidget>
#include <algorithm>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    bool generateScriptText();
    void getQtVersion();
    void getQtBuilder(QString &version);
    void builderChanged();
    void getLineEditText();
    void checkDirExists();
    void loadAbout();

private slots:
    void on_pbtnQtPath_clicked();

    void on_pbtnMysqlPath_clicked();

    void on_pbtnVisualStudio_clicked();

    void on_pbtnMingW_clicked();

    void on_cbxQtVersion_currentIndexChanged(int index);

    void on_cbxQtBuilder_currentIndexChanged(int index);

    void on_pbtnGenerateScript_clicked();

    void on_pbtnSaveText_clicked();

private:
    Ui::Widget *ui;
    QString qt_path;
    QString cmake_path;
    QString ninja_path;
    QString version;
    QString builder;
    QString mysql_path;
    QString visualstudio_path;
    QString mingw_path;
};
