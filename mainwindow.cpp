#include "mainwindow.h"
#include <QApplication>
#include <QTextStream>
#include <QClipboard>
#include <QGuiApplication>
#include <QMessageBox>
#include <QGroupBox>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    // Enable drag-and-drop
    setAcceptDrops(true);

    // Create central widget
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Create layouts
    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // Initialize default ignore lists
    defaultIgnoreSuffixes = { ".log", ".obj", ".o", ".exe", ".dll", ".so", ".dylib", ".class", ".pyc", ".pyo", ".pyd", ".db", ".sqlite", ".jpg", ".jpeg", ".png", ".gif", ".bmp", ".ico", ".zip", ".tar", ".gz", ".rar", ".7z", ".pdf" };
    defaultIgnoreFolders = { ".git", ".svn", ".hg", ".idea", ".vscode", "node_modules", "build", "dist", "__pycache__", ".DS_Store" };

    // Copy defaults to current ignore lists
    ignoreSuffixes = defaultIgnoreSuffixes;
    ignoreFolders = defaultIgnoreFolders;

    // Create UI components
    QLabel *titleLabel = new QLabel("Files to GPT", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");

    addFoldersButton = new QPushButton("Add Folders", this);
    taskDescriptionEdit = new QLineEdit(this);
    taskDescriptionEdit->setPlaceholderText("Enter task description");

    outputTextEdit = new QTextEdit(this);
    outputTextEdit->setReadOnly(true);

    darkModeCheckBox = new QCheckBox("Dark Mode", this);

    // Create labels and text edits for ignore lists
    QLabel *ignoreSuffixesLabel = new QLabel("Ignore Suffixes (comma-separated):", this);
    ignoreSuffixesEdit = new QLineEdit(this);
    ignoreSuffixesEdit->setText(ignoreSuffixes.join(", "));

    QLabel *ignoreFoldersLabel = new QLabel("Ignore Folders (comma-separated):", this);
    ignoreFoldersEdit = new QLineEdit(this);
    ignoreFoldersEdit->setText(ignoreFolders.join(", "));

    // Create reset buttons
    QPushButton *resetSuffixesButton = new QPushButton("Reset Suffixes to Default", this);
    QPushButton *resetFoldersButton = new QPushButton("Reset Folders to Default", this);

    // Create action buttons
    copyToClipboardButton = new QPushButton("Copy to Clipboard", this);
    saveAsTextButton = new QPushButton("Save as .txt", this);
    clearFilesButton = new QPushButton("Clear Files", this);

    // Create file list widget
    fileListWidget = new QListWidget(this);
    fileListWidget->setSelectionMode(QAbstractItemView::NoSelection);

    // Token count label
    tokenCountLabel = new QLabel("Estimated Tokens: 0", this);
    tokenCountLabel->setAlignment(Qt::AlignRight);

    // Connect buttons to slots
    connect(resetSuffixesButton, &QPushButton::clicked, this, &MainWindow::resetSuffixes);
    connect(resetFoldersButton, &QPushButton::clicked, this, &MainWindow::resetFolders);
    connect(copyToClipboardButton, &QPushButton::clicked, this, &MainWindow::copyToClipboard);
    connect(saveAsTextButton, &QPushButton::clicked, this, &MainWindow::saveAsTextFile);
    connect(clearFilesButton, &QPushButton::clicked, this, &MainWindow::clearFiles);

    // Connect signals and slots
    connect(addFoldersButton, &QPushButton::clicked, this, &MainWindow::addFolders);
    connect(darkModeCheckBox, &QCheckBox::checkStateChanged, this, &MainWindow::toggleDarkMode);
    connect(taskDescriptionEdit, &QLineEdit::textChanged, this, &MainWindow::updateOutput);

    // Organize layouts
    // Create a group box for ignore suffixes
    QGroupBox *ignoreSuffixesGroup = new QGroupBox("Ignore Suffixes");
    QVBoxLayout *ignoreSuffixesLayout = new QVBoxLayout();
    ignoreSuffixesLayout->addWidget(ignoreSuffixesEdit);
    ignoreSuffixesLayout->addWidget(resetSuffixesButton);
    ignoreSuffixesLayout->setContentsMargins(10, 20, 10, 10);
    ignoreSuffixesGroup->setLayout(ignoreSuffixesLayout);

    // Create a group box for ignore folders
    QGroupBox *ignoreFoldersGroup = new QGroupBox("Ignore Folders");
    QVBoxLayout *ignoreFoldersLayout = new QVBoxLayout();
    ignoreFoldersLayout->addWidget(ignoreFoldersEdit);
    ignoreFoldersLayout->addWidget(resetFoldersButton);
    ignoreFoldersLayout->setContentsMargins(10, 20, 10, 10);
    ignoreFoldersGroup->setLayout(ignoreFoldersLayout);

    // Action buttons layout
    QHBoxLayout *actionButtonsLayout = new QHBoxLayout();
    actionButtonsLayout->setAlignment(Qt::AlignLeft);
    actionButtonsLayout->setSpacing(10);
    actionButtonsLayout->addWidget(copyToClipboardButton);
    actionButtonsLayout->addWidget(saveAsTextButton);
    actionButtonsLayout->addWidget(clearFilesButton);

    // Create group boxes for sections
    // Task Description Group
    QGroupBox *taskDescriptionGroup = new QGroupBox("Task Description", this);
    QVBoxLayout *taskDescriptionLayout = new QVBoxLayout();
    taskDescriptionLayout->addWidget(taskDescriptionEdit);
    taskDescriptionLayout->setContentsMargins(10, 20, 10, 10);
    taskDescriptionGroup->setLayout(taskDescriptionLayout);

    // File List Group
    QGroupBox *fileListGroup = new QGroupBox("Files to Include", this);
    QVBoxLayout *fileListLayout = new QVBoxLayout();
    fileListLayout->addWidget(fileListWidget);
    fileListLayout->setContentsMargins(10, 20, 10, 10);
    fileListGroup->setLayout(fileListLayout);

    // Final Prompt Group
    QGroupBox *finalPromptGroup = new QGroupBox("Final Prompt", this);
    QVBoxLayout *finalPromptLayout = new QVBoxLayout();
    finalPromptLayout->addWidget(outputTextEdit);
    finalPromptLayout->setContentsMargins(10, 20, 10, 10);
    finalPromptGroup->setLayout(finalPromptLayout);

    // Add components to main layout
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(addFoldersButton);
    mainLayout->addWidget(taskDescriptionGroup);
    mainLayout->addWidget(ignoreSuffixesGroup);
    mainLayout->addWidget(ignoreFoldersGroup);
    mainLayout->addWidget(fileListGroup);
    mainLayout->addWidget(finalPromptGroup);
    mainLayout->addWidget(tokenCountLabel);
    mainLayout->addLayout(actionButtonsLayout);
    mainLayout->addWidget(darkModeCheckBox);
}

MainWindow::~MainWindow() {
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event) {
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;

    foreach (QUrl url, urls) {
        QString filePath = url.toLocalFile();
        QFileInfo fileInfo(filePath);

        if (fileInfo.exists()) {
            if (fileInfo.isDir()) {
                processDirectory(QDir(filePath));
            } else if (fileInfo.isFile()) {
                processFile(filePath);
            }
        }
    }

    updateOutput();
    event->acceptProposedAction();
}

void MainWindow::addFolders() {
    // Update ignore lists from user input
    QStringList suffixes = ignoreSuffixesEdit->text().split(',', Qt::SkipEmptyParts);
    for (int i = 0; i < suffixes.size(); ++i) {
        suffixes[i] = suffixes[i].trimmed().toLower();
    }
    ignoreSuffixes = suffixes;

    QStringList folders = ignoreFoldersEdit->text().split(',', Qt::SkipEmptyParts);
    for (int i = 0; i < folders.size(); ++i) {
        folders[i] = folders[i].trimmed().toLower();
    }
    ignoreFolders = folders;

    // Open a directory selection dialog
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Select Folder"), QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dirPath.isEmpty()) {
        QFileInfo fileInfo(dirPath);

        if (fileInfo.exists() && fileInfo.isDir()) {
            processDirectory(QDir(dirPath));
            updateOutput();
        }
    }
}

void MainWindow::processDirectory(const QDir &directory) {
    QFileInfoList entries = directory.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    foreach (QFileInfo entry, entries) {
        if (entry.isDir()) {
            QString folderName = entry.fileName().toLower();
            if (ignoreFolders.contains(folderName)) {
                continue;
            }
            processDirectory(QDir(entry.absoluteFilePath()));
        } else if (entry.isFile()) {
            processFile(entry.absoluteFilePath());
        }
    }
}

void MainWindow::processFile(const QString &filePath) {
    if (isTextFile(filePath)) {
        if (!fileList.contains(filePath)) {
            fileList.append(filePath);

            // Read file content
            QFile file(filePath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                QString content = in.readAll();
                fileContentsMap[filePath] = content;
                file.close();

                // Add to file list display
                addFileToListWidget(filePath);
            }
        }
    }
}

void MainWindow::addFileToListWidget(const QString &filePath) {
    QListWidgetItem *item = new QListWidgetItem(fileListWidget);

    FileItemWidget *fileItemWidget = new FileItemWidget(filePath, this);
    item->setSizeHint(fileItemWidget->sizeHint());

    fileListWidget->addItem(item);
    fileListWidget->setItemWidget(item, fileItemWidget);

    // Store the association
    itemWidgetMap[item] = fileItemWidget;

    // Connect signals
    connect(fileItemWidget, &FileItemWidget::checkboxStateChanged, this, &MainWindow::updateOutput);
    connect(fileItemWidget, &FileItemWidget::removeRequested, this, &MainWindow::removeFile);
}

void MainWindow::removeFile(const QString &filePath) {
    // Remove from fileList
    fileList.removeAll(filePath);

    // Remove from fileContentsMap
    fileContentsMap.remove(filePath);

    // Find and remove the item from the list widget
    for (int i = 0; i < fileListWidget->count(); ++i) {
        QListWidgetItem *item = fileListWidget->item(i);
        FileItemWidget *fileItemWidget = itemWidgetMap[item];

        if (fileItemWidget->getFilePath() == filePath) {
            // Disconnect signals
            disconnect(fileItemWidget, nullptr, this, nullptr);

            // Remove from itemWidgetMap
            itemWidgetMap.remove(item);

            // Remove item from the list widget
            delete fileListWidget->takeItem(i);

            // Delete the widget
            fileItemWidget->deleteLater();

            break;
        }
    }

    // Update the output
    updateOutput();
}

bool MainWindow::isTextFile(const QString &filePath) {
    QString suffix = "." + QFileInfo(filePath).suffix().toLower();

    // Check if the file suffix is in the ignore list
    if (ignoreSuffixes.contains(suffix)) {
        return false;
    }

    // List of text-based file extensions
    QStringList textExtensions = {
        ".txt", ".cpp", ".h", ".hpp", ".py", ".java", ".js", ".html", ".css",
        ".json", ".md", ".xml", ".yaml", ".yml", ".ini", ".cfg", ".c", ".cs",
        ".php", ".rb", ".swift", ".go", ".rs", ".kt", ".sh", ".bat", ".tsx", ".ts"
    };

    return textExtensions.contains(suffix);
}

void MainWindow::toggleDarkMode(int state) {
    if (state == Qt::Checked) {
        // Apply dark theme styles
        qApp->setStyleSheet(
            "QMainWindow { background-color: #2e2e2e; color: #ffffff; }"
            "QGroupBox { color: #ffffff; border: 1px solid #555; }"
            "QLabel { color: #ffffff; }"
            "QPushButton { background-color: #3a3a3a; color: #ffffff; border: 1px solid #555; }"
            "QPushButton:hover { background-color: #4a4a4a; }"
            "QLineEdit, QTextEdit { background-color: #3a3a3a; color: #ffffff; border: 1px solid #555; }"
            "QCheckBox { color: #ffffff; }"
            "QListWidget { background-color: #3a3a3a; color: #ffffff; border: 1px solid #555; }"
        );
    } else {
        qApp->setStyleSheet(""); // Revert to system styles
    }
}

void MainWindow::resetSuffixes() {
    ignoreSuffixes = defaultIgnoreSuffixes;
    ignoreSuffixesEdit->setText(ignoreSuffixes.join(", "));
}

void MainWindow::resetFolders() {
    ignoreFolders = defaultIgnoreFolders;
    ignoreFoldersEdit->setText(ignoreFolders.join(", "));
}

void MainWindow::updateOutput() {
    // Get the current task description
    QString taskDescription = taskDescriptionEdit->text();

    // Combine task description and file contents
    QString finalOutput;

    if (!taskDescription.isEmpty()) {
        finalOutput += taskDescription + "\n\n";
    }

    // Append the contents of checked files
    for (int i = 0; i < fileListWidget->count(); ++i) {
        QListWidgetItem *item = fileListWidget->item(i);
        FileItemWidget *fileItemWidget = itemWidgetMap[item];
        QString filePath = fileItemWidget->getFilePath();

        if (fileItemWidget->isChecked()) {
            QString content = fileContentsMap.value(filePath);
            finalOutput += QString("=== FILE: %1 ===\n%2\n\n")
                               .arg(filePath)
                               .arg(content);
        }
    }

    // Update the output textbox
    outputTextEdit->setPlainText(finalOutput);

    // Calculate token count
    int tokenCount = estimateTokenCount(finalOutput);

    // Update token count display
    tokenCountLabel->setText(QString("Estimated Tokens: %1").arg(tokenCount));
}

int MainWindow::estimateTokenCount(const QString &text) {
    QProcess process;
    QStringList arguments;
    arguments << "token_counter.py";

    process.start("python3", arguments);
    if (!process.waitForStarted()) {
        // If the process didn't start, fallback to estimation
        return text.length() / 4;
    }

    process.write(text.toUtf8());
    process.closeWriteChannel();

    if (!process.waitForFinished()) {
        // If the process didn't finish, fallback to estimation
        return text.length() / 4;
    }

    QString output = process.readAllStandardOutput();
    bool ok;
    int tokenCount = output.trimmed().toInt(&ok);

    if (ok) {
        return tokenCount;
    } else {
        // Fallback in case of error
        return text.length() / 4;
    }
}

void MainWindow::copyToClipboard() {
    QString finalOutput = outputTextEdit->toPlainText();
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(finalOutput);

    QMessageBox::information(this, "Copied", "The final result has been copied to the clipboard.");
}

void MainWindow::saveAsTextFile() {
    QString finalOutput = outputTextEdit->toPlainText();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Text Files (*.txt)"));

    if (!fileName.isEmpty()) {
        QFile file(fileName);

        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << finalOutput;
            file.close();

            QMessageBox::information(this, "Saved", "The file has been saved successfully.");
        } else {
            QMessageBox::warning(this, "Error", "Could not save the file.");
        }
    }
}

void MainWindow::clearFiles() {
    fileList.clear();
    fileContentsMap.clear();

    // Clear item widgets
    itemWidgetMap.clear();
    fileListWidget->clear();

    // Update the output
    updateOutput();
}
