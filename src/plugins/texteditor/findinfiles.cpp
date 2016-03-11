/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "findinfiles.h"

#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/find/findplugin.h>
#include <coreplugin/icore.h>
#include <utils/filesearch.h>
#include <utils/fileutils.h>
#include <utils/historycompleter.h>
#include <utils/pathchooser.h>
#include <utils/qtcassert.h>

#include <QDebug>
#include <QSettings>
#include <QDir>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>
#include <QHBoxLayout>

using namespace Core;
using namespace TextEditor;
using namespace Utils;

static FindInFiles *m_instance = 0;
static const char HistoryKey[] = "FindInFiles.Directories.History";

FindInFiles::FindInFiles()
  : m_configWidget(0),
    m_directory(0)
{
    m_instance = this;
    connect(EditorManager::instance(), &EditorManager::findOnFileSystemRequest,
            this, &FindInFiles::findOnFileSystem);
}

FindInFiles::~FindInFiles()
{
}

bool FindInFiles::isValid() const
{
    return m_directory->isValid();
}

QString FindInFiles::id() const
{
    return QLatin1String("Files on Disk");
}

QString FindInFiles::displayName() const
{
    return tr("Files in File System");
}

FileIterator *FindInFiles::files(const QStringList &nameFilters,
                                 const QVariant &additionalParameters) const
{
    return new SubDirFileIterator(QStringList() << additionalParameters.toString(),
                                  nameFilters,
                                  EditorManager::defaultTextCodec());
}

QVariant FindInFiles::additionalParameters() const
{
    return qVariantFromValue(path().toString());
}

QString FindInFiles::label() const
{
    QString title = tr("Directory");
    if (FileFindExtension *ext = extension()) {
        if (ext->isEnabled())
            title = ext->title();
    }
    const QChar slash = QLatin1Char('/');
    const QStringList &nonEmptyComponents = path().toFileInfo().absoluteFilePath()
            .split(slash, QString::SkipEmptyParts);
    return tr("%1 \"%2\":")
            .arg(title)
            .arg(nonEmptyComponents.isEmpty() ? QString(slash) : nonEmptyComponents.last());
}

QString FindInFiles::toolTip() const
{
    //: %3 is filled by BaseFileFind::runNewSearch
    return tr("Path: %1\nFilter: %2\n%3")
            .arg(path().toUserOutput())
            .arg(fileNameFilters().join(QLatin1Char(',')));
}

QWidget *FindInFiles::createConfigWidget()
{
    if (!m_configWidget) {
        m_configWidget = new QWidget;
        QGridLayout * const gridLayout = new QGridLayout(m_configWidget);
        gridLayout->setMargin(0);
        m_configWidget->setLayout(gridLayout);

        int row = 0;
        if (FileFindExtension *ext = extension())
            gridLayout->addWidget(ext->widget(), row++, 1, 1, 2);

        QLabel *dirLabel = new QLabel(tr("Director&y:"));
        gridLayout->addWidget(dirLabel, row, 0, Qt::AlignRight);
        m_directory = new PathChooser;
        m_directory->setExpectedKind(PathChooser::ExistingDirectory);
        m_directory->setPromptDialogTitle(tr("Directory to Search"));
        connect(m_directory.data(), &PathChooser::pathChanged,
                this, &FindInFiles::pathChanged);
        connect(m_directory.data(), &PathChooser::validChanged,
                this, &FindInFiles::enabledChanged);
        m_directory->setHistoryCompleter(QLatin1String(HistoryKey),
                                         /*restoreLastItemFromHistory=*/ true);
        if (!HistoryCompleter::historyExistsFor(QLatin1String(HistoryKey))) {
            auto completer = static_cast<HistoryCompleter *>(m_directory->lineEdit()->completer());
            const QStringList legacyHistory = Core::ICore::settings()->value(
                        QLatin1String("Find/FindInFiles/directories")).toStringList();
            for (const QString &dir: legacyHistory)
                completer->addEntry(dir);
        }
        dirLabel->setBuddy(m_directory);
        gridLayout->addWidget(m_directory, row++, 1, 1, 2);

        QLabel * const filePatternLabel = new QLabel(tr("Fi&le pattern:"));
        filePatternLabel->setMinimumWidth(80);
        filePatternLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        filePatternLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        QWidget *patternWidget = createPatternWidget();
        filePatternLabel->setBuddy(patternWidget);
        gridLayout->addWidget(filePatternLabel, row, 0);
        gridLayout->addWidget(patternWidget, row++, 1, 1, 2);
        m_configWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    }
    return m_configWidget;
}

FileName FindInFiles::path() const
{
    return m_directory->fileName();
}

void FindInFiles::writeSettings(QSettings *settings)
{
    settings->beginGroup(QLatin1String("FindInFiles"));
    writeCommonSettings(settings);
    settings->endGroup();
}

void FindInFiles::readSettings(QSettings *settings)
{
    settings->beginGroup(QLatin1String("FindInFiles"));
    readCommonSettings(settings, QLatin1String("*.cpp,*.h"));
    settings->endGroup();
}

void FindInFiles::setDirectory(const FileName &directory)
{
    m_directory->setFileName(directory);
}

FileName FindInFiles::directory() const
{
    return m_directory->fileName();
}

void FindInFiles::findOnFileSystem(const QString &path)
{
    QTC_ASSERT(m_instance, return);
    const QFileInfo fi(path);
    const QString folder = fi.isDir() ? fi.absoluteFilePath() : fi.absolutePath();
    m_instance->setDirectory(FileName::fromString(folder));
    Find::openFindDialog(m_instance);
}

FindInFiles *FindInFiles::instance()
{
    return m_instance;
}
