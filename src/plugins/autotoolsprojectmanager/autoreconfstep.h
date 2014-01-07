/**************************************************************************
**
** Copyright (C) 2014 Openismus GmbH.
** Authors: Peter Penz (ppenz@openismus.com)
**          Patricia Santana Cruz (patriciasantanacruz@gmail.com)
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

#ifndef AUTORECONFSTEP_H
#define AUTORECONFSTEP_H

#include <projectexplorer/abstractprocessstep.h>

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

namespace AutotoolsProjectManager {
namespace Internal {

class AutotoolsProject;
class AutoreconfStep;

////////////////////////////////
// AutoreconfStepFactory class
////////////////////////////////
/**
 * @brief Implementation of the ProjectExplorer::IBuildStepFactory interface.
 *
 * The factory is used to create instances of AutoreconfStep.
 */
class AutoreconfStepFactory : public ProjectExplorer::IBuildStepFactory
{
    Q_OBJECT

public:
    AutoreconfStepFactory(QObject *parent = 0);

    QList<Core::Id> availableCreationIds(ProjectExplorer::BuildStepList *bc) const;
    QString displayNameForId(const Core::Id id) const;

    bool canCreate(ProjectExplorer::BuildStepList *parent, const Core::Id id) const;
    ProjectExplorer::BuildStep *create(ProjectExplorer::BuildStepList *parent, const Core::Id id);
    bool canClone(ProjectExplorer::BuildStepList *parent, ProjectExplorer::BuildStep *source) const;
    ProjectExplorer::BuildStep *clone(ProjectExplorer::BuildStepList *parent, ProjectExplorer::BuildStep *source);
    bool canRestore(ProjectExplorer::BuildStepList *parent, const QVariantMap &map) const;
    ProjectExplorer::BuildStep *restore(ProjectExplorer::BuildStepList *parent, const QVariantMap &map);

    bool canHandle(ProjectExplorer::BuildStepList *parent) const;
};

/////////////////////////
// AutoreconfStep class
/////////////////////////
/**
 * @brief Implementation of the ProjectExplorer::AbstractProcessStep interface.
 *
 * A autoreconf step can be configured by selecting the "Projects" button
 * of Qt Creator (in the left hand side menu) and under "Build Settings".
 *
 * It is possible for the user to specify custom arguments. The corresponding
 * configuration widget is created by AutoreconfStep::createConfigWidget and is
 * represented by an instance of the class AutoreconfStepConfigWidget.
 */

class AutoreconfStep : public ProjectExplorer::AbstractProcessStep
{
    Q_OBJECT
    friend class AutoreconfStepFactory;
    friend class AutoreconfStepConfigWidget;

public:
    AutoreconfStep(ProjectExplorer::BuildStepList *bsl);

    bool init();
    void run(QFutureInterface<bool> &interface);
    ProjectExplorer::BuildStepConfigWidget *createConfigWidget();
    bool immutable() const;
    QString additionalArguments() const;
    QVariantMap toMap() const;

public slots:
    void setAdditionalArguments(const QString &list);

signals:
    void additionalArgumentsChanged(const QString &);

protected:
    AutoreconfStep(ProjectExplorer::BuildStepList *bsl, AutoreconfStep *bs);
    AutoreconfStep(ProjectExplorer::BuildStepList *bsl, const Core::Id id);

    bool fromMap(const QVariantMap &map);

private:
    void ctor();

    QString m_additionalArguments;
    bool m_runAutoreconf;
};

//////////////////////////////////////
// AutoreconfStepConfigWidget class
//////////////////////////////////////
/**
 * @brief Implementation of the ProjectExplorer::BuildStepConfigWidget interface.
 *
 * Allows to configure a autoreconf step in the GUI..
 */
class AutoreconfStepConfigWidget : public ProjectExplorer::BuildStepConfigWidget
{
    Q_OBJECT

public:
    AutoreconfStepConfigWidget(AutoreconfStep *autoreconfStep);

    QString displayName() const;
    QString summaryText() const;

private slots:
    void updateDetails();

private:
    AutoreconfStep *m_autoreconfStep;
    QString m_summaryText;
    QLineEdit *m_additionalArguments;
};

} // namespace Internal
} // namespace AutotoolsProjectManager

#endif // AUTORECONFSTEP_H

