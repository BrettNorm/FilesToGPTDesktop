#include "fileitemwidget.h"

FileItemWidget::FileItemWidget(const QString &filePath, QWidget *parent)
    : QWidget(parent), filePath(filePath) {

    checkBox = new QCheckBox(this);
    checkBox->setChecked(true);

    filePathLabel = new QLabel(filePath, this);
    filePathLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    removeButton = new QPushButton("Remove", this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(checkBox);
    layout->addWidget(filePathLabel);
    layout->addWidget(removeButton);
    layout->setContentsMargins(0, 0, 0, 0);

    connect(checkBox, &QCheckBox::checkStateChanged, this, &FileItemWidget::onCheckboxStateChanged);
    connect(removeButton, &QPushButton::clicked, this, &FileItemWidget::onRemoveButtonClicked);
}

bool FileItemWidget::isChecked() const {
    return checkBox->isChecked();
}

QString FileItemWidget::getFilePath() const {
    return filePath;
}

void FileItemWidget::onCheckboxStateChanged(int state) {
    Q_UNUSED(state);
    emit checkboxStateChanged();
}

void FileItemWidget::onRemoveButtonClicked() {
    emit removeRequested(filePath);
}
