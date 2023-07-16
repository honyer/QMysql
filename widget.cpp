#include "widget.h"
#include "./ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("QMsql");

    loadAbout();
    getLineEditText();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pbtnQtPath_clicked()
{
    QFileDialog fileDlg;
    fileDlg.setWindowTitle("选择Qt文件夹");
    fileDlg.setDirectory(QDir::homePath());
    fileDlg.setFileMode(QFileDialog::Directory);

    // 判断默认的qt目录存在则设置为默认对话框的打开目录
    QDir defaut(qt_path);
    if (defaut.exists()) {
        fileDlg.setDirectory(qt_path);
    }

    //选择目录后判断Qt\Tools目录下的cmake、ninja、mingw
    if (fileDlg.exec()) {
        QString path = fileDlg.selectedFiles().at(0);
        path = path.replace("/", "\\");
        ui->letQtPath->setText(path);

        QDir dir(path + "\\Tools");
        if(!dir.exists()){
            return;
        }

        dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
        QStringList strlist = dir.entryList();

        QRegularExpression reCmake("^CMake", QRegularExpression::CaseInsensitiveOption);
        QRegularExpression reNinja("^Ninja", QRegularExpression::CaseInsensitiveOption);
        QRegularExpression reMingw("^mingw", QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match;
        QStringList mingwlist;
        foreach (QString list, strlist) {
            match = reCmake.match(list);
            if (match.hasMatch()) {
                ui->letQtCmake->setText(path + "\\Tools\\" + list);
                continue;
            }

            match = reNinja.match(list);
            if (match.hasMatch()) {
                ui->letNinja->setText(path + "\\Tools\\" + list);
                continue;
            }

            match = reMingw.match(list);
            if (match.hasMatch()) {
                mingwlist.append(list);
            }
        }
        // 多个mingw版本排序选最大
        mingwlist.sort();
        ui->letMingW->setText(path + "\\Tools\\" + mingwlist.at(0));
    }

    getQtVersion();
}

void Widget::on_pbtnMysqlPath_clicked()
{
    QFileDialog fileDlg;
    fileDlg.setWindowTitle("选择Mysql文件夹");
    fileDlg.setDirectory(QDir::homePath());

    //默认目录是否存在
    QDir defaut(ui->letMysqlPath->text());
    if (defaut.exists()) {
        fileDlg.setDirectory(ui->letMysqlPath->text());
    }

    fileDlg.setFileMode(QFileDialog::Directory);
    if (fileDlg.exec()) {
        QString path = fileDlg.selectedFiles().at(0);
        path = path.replace("/", "\\");

        ui->letMysqlPath->setText(path);
    }
}
void Widget::on_pbtnVisualStudio_clicked()
{
    QFileDialog fileDlg;
    fileDlg.setWindowTitle("选择Visual Studio文件夹");
    fileDlg.setDirectory(QDir::homePath());

    //默认目录是否存在
    QDir defaut(ui->letVisualStudio->text());
    if (defaut.exists()) {
        fileDlg.setDirectory(ui->letVisualStudio->text());
    }

    fileDlg.setFileMode(QFileDialog::Directory);
    if (fileDlg.exec()) {
        QString path = fileDlg.selectedFiles().at(0);
        path = path.replace("/", "\\");

        ui->letVisualStudio->setText(path);
    }
}

void Widget::on_pbtnMingW_clicked()
{
    QFileDialog fileDlg;
    fileDlg.setWindowTitle("选择MingW文件夹");
    fileDlg.setDirectory(QDir::homePath());

    //默认目录是否存在
    QDir defaut(ui->letQtPath->text() + "\\Tools");
    if (defaut.exists()) {
        fileDlg.setDirectory(ui->letQtPath->text() + "\\Tools");
    }

    fileDlg.setFileMode(QFileDialog::Directory);
    if (fileDlg.exec()) {
        QString path = fileDlg.selectedFiles().at(0);
        path = path.replace("/", "\\");

        ui->letMingW->setText(path);
    }
}

void Widget::on_cbxQtBuilder_currentIndexChanged(int index)
{
    builderChanged();
}

void Widget::on_cbxQtVersion_currentIndexChanged(int index)
{
    QString version = ui->cbxQtVersion->itemText(index);

    getQtBuilder(version);
}

void Widget::on_pbtnSaveText_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this, "保存文件", "", "Batch (*.bat)");
    if (!filename.isEmpty()) {
        QFile file(filename);
        if (file.open(QIODevice::WriteOnly)) {
            QString data = ui->textPreView->toPlainText();
            file.write(data.toUtf8());
            file.close();
        }
    }
}

void Widget::getQtVersion()
{
    // 获取Qt目录过滤获得当前目录下的文件夹名strlist
    QString path = ui->letQtPath->text();
    QDir dir(path);
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    QStringList strlist = dir.entryList();

    // 创建一个versionlist存储遍历Qt目录下的所有版本
    QStringList versionlist;
    QRegularExpression re("\\d+\\.\\d+\\.\\d+");
    QRegularExpressionMatch match;
    foreach (QString list, strlist) {
        match = re.match(list);
        if (match.hasMatch()) {
            versionlist.append(list);
        }
    }
    // 版本号排序后反序，高版本优先
    versionlist.sort();
    std::reverse(versionlist.begin(), versionlist.end());

    // 清空cbxQtVersion，将版本号装载到下拉组件
    ui->cbxQtVersion->clear();
    ui->cbxQtVersion->addItems(versionlist);
    ui->cbxQtVersion->setCurrentIndex(0);
}

void Widget::getQtBuilder(QString &version)
{
    QString path = ui->letQtPath->text() + "\\" + version;
    QDir dir(path);

    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    QStringList strlist = dir.entryList();

    //正则表达式以mingw和msvc开头
    QRegularExpression re("^(mingw|msvc)");
    ui->cbxQtBuilder->clear();
    foreach (QString list, strlist) {
        QRegularExpressionMatch match = re.match(list);
        if (match.hasMatch()) {
            ui->cbxQtBuilder->addItem(list);
        }
    }

    ui->cbxQtBuilder->setCurrentIndex(0);
}

void Widget::builderChanged()
{
    QString type = ui->cbxQtBuilder->currentText();
    if (type.startsWith("msvc")) {
        ui->letVisualStudio->setEnabled(true);
        ui->pbtnVisualStudio->setEnabled(true);
        ui->letNinja->setEnabled(true);

        ui->letMingW->setEnabled(false);
        ui->pbtnMingW->setEnabled(false);
    } else {
        ui->letMingW->setEnabled(true);
        ui->pbtnMingW->setEnabled(true);

        ui->letVisualStudio->setEnabled(false);
        ui->pbtnVisualStudio->setEnabled(false);
        ui->letNinja->setEnabled(false);
    }
}

bool Widget::generateScriptText()
{
    if (qt_path.length() < 1 || cmake_path.length() < 1 || version.length() < 0
        || builder.length() < 1 || mysql_path.length() < 1) {
        return false;
    }

    // C:\Qt\6.4.0\mingw_64
    QString qvb = qt_path + "\\" + version + "\\" + builder;

    // . -DMySQL_INCLUDE_DIR="C:\mysql_8.0.29\include" -DMySQL_LIBRARY="C:\mysql_8.0.29\lib\libmysql.lib" -DCMAKE_INSTALL_PREFIX="C:\Qt\6.4.0\mingw_64"
    QString buildpama = " . -DMySQL_INCLUDE_DIR=\"" + mysql_path + "\\include\" -DMySQL_LIBRARY=\""
                        + mysql_path + "\\lib\\libmysql.lib\"  -DCMAKE_INSTALL_PREFIX=\"" + qvb
                        + "\"";

    // C:\Qt\6.5.1\Src\qtbase\src\plugins\sqldrivers
    QString srcpath = qt_path + "\\" + version + "\\Src\\qtbase\\src\\plugins\\sqldrivers\n";

    //拼接脚本内容
    QString script = "set PATH=%PATH%;" + cmake_path + "\\bin;";
    if (builder.startsWith("msvc")) {
        script += ninja_path + "\n";
        script += "cd " + srcpath;
        script += "call \"" + visualstudio_path + "\\VC\\Auxiliary\\Build\\vcvars64.bat\"\n";
        script += "call " + qvb + "\\bin\\qt-cmake.bat -G \"Ninja\"" + buildpama + "\n";
        script += "ninja\nninja install\n";
        script += "call " + qvb + "\\bin\\qt-cmake.bat -G \"Ninja\"" + buildpama
                  + "  -DCMAKE_BUILD_TYPE=Release\n";
        script += "ninja\nninja install\n";
    } else {
        script += mingw_path + "\\bin\n";
        script += "cd " + srcpath;
        script += "call " + qvb + "\\bin\\qt-cmake.bat -G \"MinGW Makefiles\"" + buildpama
                  + " -DCMAKE_C_COMPILER=\"gcc.exe\" -DCMAKE_CXX_COMPILER=\"g++.exe\"\n";
        script += "mingw32-make\nmingw32-make install\n";
    }

    // 脚本增加复制命令
    script += "xcopy plugins\\sqldrivers\\* " + qvb + "\\plugins\\sqldrivers /Y\n";
    script += "copy \"" + mysql_path + "\\lib\\libmysql.dll\" " + qvb + "\\bin\n";
    script += "pause\n";
    ui->textPreView->clear();
    ui->textPreView->insertPlainText(script);

    return true;
}

void Widget::on_pbtnGenerateScript_clicked()
{
    ui->lableNotify->setText("");

    getLineEditText(); //刷新变量值
    if (!version.startsWith("6")) {
        ui->lableNotify->setStyleSheet("color: red;");
        ui->lableNotify->setText("脚本不支持Qt6以下的版本");
        return;
    }
    if (generateScriptText()) {
        ui->lableNotify->setStyleSheet("color: green;");
        ui->lableNotify->setText("生成成功！");
        ui->pbtnSaveText->setEnabled(true);
        return;
    }

    ui->lableNotify->setStyleSheet("color: red;");
    ui->lableNotify->setText("请检查各项路径");
    ui->pbtnSaveText->setEnabled(false);
    loadAbout();
}

void Widget::getLineEditText()
{
    qt_path = ui->letQtPath->text();
    cmake_path = ui->letQtCmake->text();
    ninja_path = ui->letNinja->text();
    version = ui->cbxQtVersion->currentText();
    builder = ui->cbxQtBuilder->currentText();
    mysql_path = ui->letMysqlPath->text();
    visualstudio_path = ui->letVisualStudio->text();
    mingw_path = ui->letMingW->text();
}

void Widget::checkDirExists()
{
    QDir path;
    //    QStringList allpath = qt_path << cmake_path << mysql_path;
    //TODO
    //检测所有文件夹路劲是否存在
}

void Widget::loadAbout()
{
    QString about
        = "About：\nQt公司不再提供Mysql的二进制插件，开发者必须根据自己的Qt版本编译器版本以及Mysql"
          "版本来构建编译。\n\n本工具是根据 https://github.com/thecodemonkey86/qt_mysql_driver "
          "编写的可视化界面，生成bat脚本。\n\n仅在windows平台qt6以上版本测试实现。\n\n编译前备份压"
          "缩src，编译时会修改一些配置文件，编译另一个版本将源码还原，否则可能编译不通过。\n编译后"
          "将dll文件复制到编译器目录下plugins\\sqldrivers，并将mysql目录下的libmysql."
          "dll复制到编译器目录下";

    ui->textPreView->clear();
    ui->textPreView->insertPlainText(about);
}
