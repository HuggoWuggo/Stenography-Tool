//
// Created by hugol on 6/06/2025.
//

#ifndef TERMINAL_H
#define TERMINAL_H

#include <streambuf>
#include <ostream>
#include <QString>
#include <QTextEdit>

class QTextEditStream final : public std::basic_streambuf<char> {
public:
    explicit QTextEditStream(QTextEdit* textEdit);

protected:
    int_type overflow(int_type v) override;
    int sync() override;

private:
    void flushBuffer();

    QTextEdit* textEdit;
    char buffer[1024]{};
};




#endif //TERMINAL_H
