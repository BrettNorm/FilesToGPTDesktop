#ifndef FILEITEMWIDGET_H
#define FILEITEMWIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

class FileItemWidget : public QWidget {
    Q_OBJECT  // Important for signal and slot mechanisms

public:
    explicit FileItemWidget(const QString &filePath, QWidget *parent = nullptr);

    bool isChecked() const;
    QString getFilePath() const;

signals:
    void checkboxStateChanged();
    void removeRequested(const QString &filePath);

private slots:
    void onCheckboxStateChanged(int state);
    void onRemoveButtonClicked();

private:
    QCheckBox *checkBox;
    QLabel *filePathLabel;
    QPushButton *removeButton;
    QString filePath;
};

#endif // FILEITEMWIDGET_H
