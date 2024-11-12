#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QListWidget>
#include <QMap>
#include "fileitemwidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void addFolders();
    void toggleDarkMode(int state);
    void resetSuffixes();
    void resetFolders();
    void updateOutput();
    void copyToClipboard();
    void saveAsTextFile();
    void clearFiles();
    void removeFile(const QString &filePath);

private:
    void processDirectory(const QDir &directory);
    void processFile(const QString &filePath);
    bool isTextFile(const QString &filePath);
    void addFileToListWidget(const QString &filePath);
    int estimateTokenCount(const QString &text);

    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QPushButton *addFoldersButton;
    QPushButton *clearFilesButton;
    QTextEdit *outputTextEdit;
    QLineEdit *taskDescriptionEdit;
    QCheckBox *darkModeCheckBox;

    // Ignore lists
    QStringList defaultIgnoreSuffixes;
    QStringList defaultIgnoreFolders;
    QStringList ignoreSuffixes;
    QStringList ignoreFolders;

    QLineEdit *ignoreSuffixesEdit;
    QLineEdit *ignoreFoldersEdit;

    // Action buttons
    QPushButton *copyToClipboardButton;
    QPushButton *saveAsTextButton;

    // File list
    QStringList fileList;
    QListWidget *fileListWidget;

    // Map to store file contents
    QMap<QString, QString> fileContentsMap;

    // Map to associate list items with widgets
    QMap<QListWidgetItem*, FileItemWidget*> itemWidgetMap;

    // Token count label
    QLabel *tokenCountLabel;
};

#endif // MAINWINDOW_H
