//
// Created by hugol on 6/06/2025.
//

#include "terminal.h"

QTextEditStream::QTextEditStream(QTextEdit* textEdit) : textEdit(textEdit) {
    setp(buffer, buffer + sizeof(buffer) - 1);
}

std::streambuf::int_type QTextEditStream::overflow(const int_type v)
{
    if (v == '\n') {
        flushBuffer();
    } else {
        *pptr() = v;
        pbump(1);
    }
    return v;
}

int QTextEditStream::sync()
{
    flushBuffer();
    return 0;
}

void QTextEditStream::flushBuffer()
{
    if (pbase() != pptr())
    {
        const QString str = QString::fromLocal8Bit(pbase(), static_cast<int>(pptr() - pbase()));

        // Trim and color-check
        QString htmlStr;

        if (str.contains("[ERROR]")) {
            htmlStr = QString("<span style='color:red;'>%1</span>").arg(str.toHtmlEscaped());
        } else if (str.contains("[WARNING]")) {
            htmlStr = QString("<span style='color:orange;'>%1</span>").arg(str.toHtmlEscaped());
        } else if (str.contains("[SUCCESS]")) {
            htmlStr = QString("<span style='color:green;'>%1</span>").arg(str.toHtmlEscaped());
        } else {
            htmlStr = QString("<span>%1</span>").arg(str.toHtmlEscaped());
        }

        textEdit->moveCursor(QTextCursor::End);
        textEdit->insertHtml(htmlStr);
        textEdit->insertPlainText("\n");  // keeps formatting clean

        pbump(static_cast<int>(pbase() - pptr()));  // reset buffer
    }
}
