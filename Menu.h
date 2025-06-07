//
// Created by hugol on 23/05/2025.
//

#ifndef MENU_H
#define MENU_H

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include <QPainter>
#include <QColor>
#include <QStyleOption>
#include <QMenu>
#include <QToolButton>
#include <QFileDialog>
#include <QLabel>
#include <QTextEdit>

class Action final
{
public:
    explicit Action(const std::string& name);
    std::string name;
    std::string description;
};

class Tab
{
public:
    explicit Tab(const std::string& name, const std::vector<Action>& actions);

    std::string name;
    std::vector<Action> actions;
private:
};

class Menu final : public QWidget {
public:
    Menu(QLabel* fileArea, QTextEdit* customText);
    ~Menu() override;

    void setStyle();
    void setupTabs();

    void addTab(const std::string& name, const std::vector<Action>& actions);

    static bool encodeLSBIntoImage(QImage& image, const QString& message);
    static QString decodeLSBFromImage(const QImage& image, int maxLength = 1000);
private:
    std::vector<Tab> tabs;
    std::vector<QToolButton*> buttons;

    QLabel* fileArea;

private slots:
void openFileDialog();

void saveFileDialog();

protected:
    void paintEvent(QPaintEvent *event) override;
};



#endif //MENU_H
