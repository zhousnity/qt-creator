/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#ifndef CPPLOCATORFILTER_H
#define CPPLOCATORFILTER_H

#include "cpplocatordata.h"
#include "searchsymbols.h"

#include <locator/ilocatorfilter.h>

namespace CppTools {
namespace Internal {

class CppModelManager;

class CppLocatorFilter : public Locator::ILocatorFilter
{
    Q_OBJECT

public:
    CppLocatorFilter(CppLocatorData *locatorData);
    ~CppLocatorFilter();

    QList<Locator::FilterEntry> matchesFor(QFutureInterface<Locator::FilterEntry> &future, const QString &entry);
    void accept(Locator::FilterEntry selection) const;
    void refresh(QFutureInterface<void> &future);

private:
    virtual QList<QList<ModelItemInfo> > itemsToMatchUserInputAgainst() const;
    virtual Locator::FilterEntry filterEntryFromModelItemInfo(const ModelItemInfo &info);

protected:
    CppLocatorData *m_data;
};

} // namespace Internal
} // namespace CppTools

#endif // CPPLOCATORFILTER_H
