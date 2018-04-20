// Copyright 2018 Patrick Flynn
//
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, 
//	this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this 
//	list of conditions and the following disclaimer in the documentation and/or 
//	other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may 
//	be used to endorse or promote products derived from this software 
//	without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#include <QTextDocument>
#include <QTextCursor>
#include <QTextCharFormat>

#include "finder.hh"
#include "tabpane.hh"

Finder::Finder()
    : find(new QToolButton),
      findNext(new QToolButton),
      entry(new QLineEdit)
{
    this->setMovable(false);
    this->setContextMenuPolicy(Qt::PreventContextMenu);

    find->setText("Find");
    findNext->setText("Find Next");

    connect(find,&QToolButton::clicked,this,&Finder::onFindClicked);
    connect(findNext,&QToolButton::clicked,this,&Finder::onFindNextClicked);

    this->addWidget(find);
    this->addWidget(findNext);
    this->addWidget(entry);
}

Finder::~Finder() {
    delete find;
    delete findNext;
    delete entry;
}

void Finder::clear() {
    if (lastEntry.isEmpty()) {
        return;
    }

    QTextDocument *doc = TabPane::currentWidget()->document();
    bool found = false;

    QTextCursor hCursor(doc);
    QTextCursor cursor(doc);

    cursor.beginEditBlock();

    QTextCharFormat firstFormat(hCursor.charFormat());
    QTextCharFormat hFormat = firstFormat;
    hFormat.setBackground(Qt::white);

    while (!hCursor.isNull() && !hCursor.atEnd()) {
        hCursor = doc->find(lastEntry,hCursor,QTextDocument::FindWholeWords);
        if (!hCursor.isNull()) {
            found = true;
            hCursor.movePosition(QTextCursor::WordRight,QTextCursor::KeepAnchor);
            hCursor.mergeCharFormat(hFormat);
        }
    }

    cursor.endEditBlock();

    TabPane::currentWidget()->setModified(false);
    index = -1;
}

void Finder::findText(bool next) {
    QString toSearch = entry->text();
    if (toSearch.isEmpty()) {
        return;
    }
    lastEntry = toSearch;

    QTextDocument *doc = TabPane::currentWidget()->document();
    bool found = false;
    int c = 0;

    QTextCursor hCursor(doc);
    QTextCursor cursor(doc);

    cursor.beginEditBlock();

    QTextCharFormat firstFormat(hCursor.charFormat());
    QTextCharFormat hFormat = firstFormat;
    hFormat.setBackground(Qt::yellow);

    while (!hCursor.isNull() && !hCursor.atEnd()) {
        hCursor = doc->find(toSearch,hCursor,QTextDocument::FindWholeWords);
        if (!hCursor.isNull()) {
            found = true;
            hCursor.movePosition(QTextCursor::WordRight,QTextCursor::KeepAnchor);
            hCursor.mergeCharFormat(hFormat);
            if (next) {
                if (index==c) {
                    hFormat.setBackground(Qt::gray);
                    hCursor.mergeCharFormat(hFormat);
                } else {
                    hFormat.setBackground(Qt::yellow);
                    hCursor.mergeCharFormat(hFormat);
                }
                c++;
            } else {
                count++;
            }
        }
    }

    cursor.endEditBlock();

    TabPane::currentWidget()->setModified(false);
}

void Finder::onFindClicked() {
    clear();
    findText(false);
}

void Finder::onFindNextClicked() {
    if (index==count) {
        index = -1;
    } else {
        index++;
    }
    findText(true);
}
