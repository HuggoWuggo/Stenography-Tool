//
// Created by hugol on 23/05/2025.
//

#include "Menu.h"

#include <QWidget>
#include <QPen>
#include <QPushButton>
#include <QFile>
#include <QStyle>
#include <QImageReader>
#include <QPixmap>

void Menu::setupTabs()
{
    const std::vector<Action> f_actions = { Action("Open"), Action("Export") };

    this->addTab("FILE", f_actions);

    int index = 0;
    for (auto& tab : this->tabs)
    {
        auto* tabButton = new QToolButton(this);
        tabButton->setText(QString::fromStdString(tab.name));
        tabButton->setObjectName("TabButton");
        tabButton->setGeometry(index * 80, 0, 80, 30);
        tabButton->setPopupMode(QToolButton::InstantPopup);

        auto* m = new QMenu(tabButton);

        for (auto& action : tab.actions)
        {
            auto a = m->addAction(QString::fromStdString(action.name));
            if (action.name == "Open")
                connect(a, &QAction::triggered, this, &Menu::openFileDialog);
            else if (action.name == "Export")
                connect(a, &QAction::triggered, this, &Menu::saveFileDialog);
        }

        tabButton->setMenu(m);
        tabButton->show();
        buttons.push_back(tabButton);
        index++;
    }
}

void Menu::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QStyleOption opt;
    opt.initFrom(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    QPen pen(QColor(144, 238, 144));
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawRect(QRect(0, 0, width() - 1, 30));

    const QFont font("Arial", 10, QFont::Bold);
    painter.setFont(font);
}


Menu::Menu(QLabel* fileArea, QTextEdit* customText)
{
    this->setObjectName("Menu");
    this->setupTabs();
    this->setAttribute(Qt::WA_StyledBackground);
    this->setStyle();

    this->fileArea = fileArea;

    QImage testImage(100, 100, QImage::Format_RGB32);
    testImage.fill(Qt::white);  // Fill with white color
    QString testMessage = "Hell World!";

    /*if (encodeLSBIntoImage(testImage, testMessage)) {
        QString decoded = decodeLSBFromImage(testImage, 100);
        qDebug() << "Original:" << testMessage;
        qDebug() << "Decoded:" << decoded;
    }*/
}

Menu::~Menu()
{
    for (const auto btn : buttons)
    {
        delete btn;
    }
}

void Menu::setStyle()
{
    if (QFile file("../menu.css"); file.open(QFile::ReadOnly | QFile::Text)) {
        const QString styleSheet = file.readAll();
        this->setStyleSheet(styleSheet);
    }
}

void Menu::addTab(const std::string& name, const std::vector<Action>& actions)
{
    const auto tab = Tab(name, actions);

    tabs.push_back(tab);
}

void Menu::openFileDialog()
{
    if (const QString fileName = QFileDialog::getOpenFileName(this, "Open a file", QString(), "Pictures (*.png *.jpg)"); !fileName.isEmpty()) {
        std::cout << "Opened File: " << fileName.toStdString() << std::endl;

        if (fileName.endsWith(".jpg"))
        {
            std::cout << "[WARNING] Opening as JPEG image, which is much more likely to fail than .png" << std::endl;
        }

        if (const QImage image(fileName); !image.isNull()) {
            const QPixmap pixmap = QPixmap::fromImage(image);

            constexpr QSize maxSize(350, 700);
            const QPixmap scaledPixmap = pixmap.scaled(maxSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            this->fileArea->setPixmap(scaledPixmap);
        } else {
            std::cout << "[ERROR] Failed to load image!" << std::endl;
        }
    } else {
        std::cout << "[ERROR] No file selected" << std::endl;
    }
}

void Menu::saveFileDialog() {
    const QString fileName = QFileDialog::getSaveFileName(
        this,
        "Save the image",
        QString(),
        "PNG Images (*.png);;JPEG Images (*.jpg)"
    );

    if (!fileName.isEmpty()) {
        if (!fileArea || !fileArea->pixmap()) {
            std::cout << "[ERROR] No image to save" << std::endl;
            return;
        }

        if (fileName.endsWith(".jpg"))
        {
            std::cout << "[WARNING] Saving as JPEG image, which is much more risky than PNG" << std::endl;
        }

        // Save as PNG (default) regardless of file extension
        if (const QImage image = fileArea->pixmap().toImage(); !image.save(fileName, "PNG")) {
            std::cout << "[ERROR] Failed to save image!" << std::endl;
        } else {
            std::cout << "[SUCCESS] Image saved with secret message!" << std::endl;
        }
    }
}


bool Menu::encodeLSBIntoImage(QImage& image, const QString& message) {
    QByteArray msgBytes = message.toUtf8();
    msgBytes.append('\0'); // Add null terminator to mark the end

    int bitIndex = 0;
    for (char byte : msgBytes) {
        for (int b = 7; b >= 0; --b) {
            if (bitIndex >= image.width() * image.height())
                return false; // Not enough space

            int x = bitIndex % image.width();
            int y = bitIndex / image.width();

            QRgb pixel = image.pixel(x, y);
            int red = qRed(pixel) & ~1; // Clear LSB
            red |= ((byte >> b) & 1);   // Set LSB from message bit

            image.setPixel(x, y, qRgb(red, qGreen(pixel), qBlue(pixel)));
            ++bitIndex;
        }
    }

    return true;
}

QString Menu::decodeLSBFromImage(const QImage& image, const int maxLength) {
    QString hiddenText;
    char currentChar = 0;
    int bitCount = 0;

    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            const QRgb pixel = image.pixel(x, y);
            const int redLSB = qRed(pixel) & 1;

            currentChar = (currentChar << 1) | redLSB;
            ++bitCount;

            if (bitCount == 8) {
                if (currentChar == '\0')
                    return hiddenText;

                hiddenText.append(currentChar);
                currentChar = 0;
                bitCount = 0;

                if (hiddenText.length() >= maxLength)
                    return hiddenText;
            }
        }
    }

    return hiddenText;
}

Action::Action(const std::string& name)
{
    this->name = name;
}

Tab::Tab(const std::string& name, const std::vector<Action>& actions)
{
    this->name = name;
    this->actions = actions;
}