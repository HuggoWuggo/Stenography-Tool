#include <QApplication>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QPushButton>

#include "terminal.h"
#include "Menu.h"

bool isHumanReadable(const QString& text, const double threshold = 0.9) {
    int printableCount = 0;
    const int totalCount = text.length();

    if (totalCount == 0) return false;

    for (const QChar& ch : text) {
        // Allow normal letters, digits, punctuation, and whitespace
        if (const ushort u = ch.unicode(); ch.isLetterOrNumber() || ch.isSpace() || ch.isPunct() || u >= 32 && u <= 126)
            printableCount++;
            // You can be more strict and reject characters above 127 if needed:
        else if (u >= 32 && u <= 126) {
            printableCount++;
        }
        // Allow emojis and CJK if needed
        else if (u >= 0x1F600 && u <= 0x1F64F) { // emoji range
            printableCount++;
        }
    }

    const double ratio = static_cast<double>(printableCount) / totalCount;
    return ratio >= threshold;
}


QTextEditStream* customStream = nullptr; // global pointer
std::streambuf* oldCout = nullptr;

void setupCoutRedirection(QTextEdit* textEdit) {
    static QTextEditStream stream(textEdit);
    customStream = &stream;
    oldCout = std::cout.rdbuf();
    std::cout.rdbuf(customStream);
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    auto window = QWidget();
    window.setWindowTitle("Stenography Tool");

    if (QFile file("../style.css"); file.open(QFile::ReadOnly | QFile::Text)) {
        const QString styleSheet = file.readAll();
        a.setStyleSheet(styleSheet);
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("FAILED TO LOAD STYLESHEET!");
        msgBox.exec();
        exit(1);
    }

    std::vector<Action> f_actions;

    const Action open("Open");
    const Action save("Save");
    f_actions.push_back(open);
    f_actions.push_back(save);

    auto title = QLabel();
    title.setText("Stenography Tool");
    title.setObjectName("title");

    auto textEdit = QTextEdit();
    textEdit.setFixedWidth(340);
    textEdit.setFixedHeight(500);
    textEdit.setPlaceholderText("TEXT");

    auto compileBtn = QPushButton();
    compileBtn.setIcon(QIcon("../res/compile.png"));
    compileBtn.setObjectName("compileBtn");
    compileBtn.setFixedSize(50, 50);
    compileBtn.setToolTip("Compile");

    auto decompileBtn = QPushButton();
    decompileBtn.setIcon(QIcon("../res/decompile.png"));
    decompileBtn.setObjectName("compileBtn");
    decompileBtn.setFixedSize(50, 50);
    decompileBtn.setToolTip("Decompile");

    auto fileEdit = QLabel();

    Menu menu(&fileEdit, &textEdit);
    menu.setFixedSize(780,30);
    menu.setStyle();

    QObject::connect(&compileBtn, &QPushButton::clicked, [&menu, &textEdit, &fileEdit]() {
        if (!fileEdit.pixmap()) {
            QMessageBox::critical(nullptr, "Error!", "No image loaded to encode message into!", QMessageBox::Ok);
            std::cout << "[ERROR] No image loaded to encode message into!" << std::endl;
            return;
        }

        QImage image = fileEdit.pixmap().toImage();

        if (textEdit.toPlainText().isEmpty())
        {
            QMessageBox::critical(nullptr, "Error!", "No message to encode!", QMessageBox::Ok);
            std::cout << "[ERROR] No message to encode!" << std::endl;
            return;
        }

        if (const QString message = textEdit.toPlainText(); Menu::encodeLSBIntoImage(image, message)) {
            fileEdit.setPixmap(QPixmap::fromImage(image));
            QMessageBox::information(nullptr, "Success!", "Message successfully encoded into image!", QMessageBox::Ok);
            std::cout << "[SUCCESS] Successfully Encoded Message: " << message.toStdString() << std::endl;
            textEdit.clear();
        } else {
            QMessageBox::critical(nullptr, "Error!", "Failed to encode message - image too small for message length", QMessageBox::Ok);
            std::cout << "[ERROR] Failed to encode file: " << message.toStdString() <<  ". image too small" << std::endl;
            return;
        }
    });

    QObject::connect(&decompileBtn, &QPushButton::clicked, [&menu, &textEdit, &fileEdit]() {
        if (!fileEdit.pixmap()) {
            QMessageBox::critical(nullptr, "Error!", "No Image imported!", QMessageBox::Ok);
            std::cout << "[ERROR] No Image imported!" << std::endl;
            return;
        }

        const QImage image = fileEdit.pixmap().toImage();

        if (const QString decodedMessage = Menu::decodeLSBFromImage(image, 1000); !decodedMessage.isEmpty()) {
            if (isHumanReadable(decodedMessage))
            {
                QMessageBox::information(nullptr, "Success!", "Message: " + decodedMessage, QMessageBox::Ok);
                std::cout << "[SUCCESS] Successfully Decoded Message: " << decodedMessage.toStdString() << std::endl;
            } else
            {
                switch (const int ret = QMessageBox::critical(nullptr, "Error!", "Message is not human readable!\n\n If this is intentional click Yes", QMessageBox::Yes, QMessageBox::No))
                {
                    case QMessageBox::Yes:
                        QMessageBox::information(nullptr, "Success!", "Message: " + decodedMessage, QMessageBox::Ok);
                        std::cout << "[SUCCESS] Successfully Decoded Message: " << decodedMessage.toStdString() << std::endl;
                        break;
                    case QMessageBox::No:
                        std::cout << "[ERROR] Message is not human readable!" << std::endl;
                        break;
                default:
                        std::cout << "[WARNING] How tf did you get here?!" << std::endl;
                }
            }
        } else {
            QMessageBox::critical(nullptr, "Error!", "No Message Found", QMessageBox::Ok);
            std::cout << "[ERROR] No Message Found!" << std::endl;
            return;
        }
    });

    auto output = QTextEdit();
    output.setFixedWidth(775);
    output.setReadOnly(true);

    auto verticalLayoutBtn = QVBoxLayout();
    verticalLayoutBtn.addWidget(&compileBtn, 0, Qt::AlignCenter);
    verticalLayoutBtn.addWidget(&decompileBtn, 0, Qt::AlignCenter);

    auto horizontalLayout_1 = QHBoxLayout();
    horizontalLayout_1.addWidget(&textEdit, 0, Qt::AlignTop | Qt::AlignLeft);
    horizontalLayout_1.addLayout(&verticalLayoutBtn);
    horizontalLayout_1.addWidget(&fileEdit, 0, Qt::AlignTop | Qt::AlignRight);

    auto mainLayout = QVBoxLayout();
    mainLayout.addWidget(&menu);
    mainLayout.addWidget(&title, 0, Qt::AlignCenter|Qt::AlignTop);
    mainLayout.addLayout(&horizontalLayout_1, 0);
    mainLayout.addWidget(&output);

    window.setLayout(&mainLayout);
    window.resize(800, 700);

    window.show();

    setupCoutRedirection(&output);

    return QApplication::exec();
}