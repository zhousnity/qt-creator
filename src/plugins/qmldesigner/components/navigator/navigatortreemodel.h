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

#pragma once

#include <modelnode.h>
#include <nodemetainfo.h>

#include <QStandardItem>
#include <QStandardItemModel>
#include <QPointer>

namespace QmlDesigner {

class Model;
class AbstractView;
class ModelNode;

class NavigatorTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    explicit NavigatorTreeModel(QObject *parent = 0);
    ~NavigatorTreeModel();

    QVariant data(const QModelIndex &index, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    void setView(AbstractView *view);

    ModelNode modelNodeForIndex(const QModelIndex &index) const;
    bool hasModelNodeForIndex(const QModelIndex &index) const;

    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    bool dropMimeData(const QMimeData *data,
                      Qt::DropAction action,
                      int row,
                      int column,
                      const QModelIndex &parent) override;


    QModelIndex indexForModelNode(const ModelNode &node) const;

    QModelIndex createIndexFromModelNode(int row, int column, const ModelNode &modelNode) const;

    void setId(const QModelIndex &index, const QString &newId);
    void openContextMenu(const QPoint &position);

    Qt::DropActions supportedDropActions() const override;
    Qt::DropActions supportedDragActions() const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    void notifyDataChanged(const ModelNode &modelNode);
    void notifyModelNodesRemoved(const QList<ModelNode> &modelNodes);
    void notifyModelNodesInserted(const QList<ModelNode> &modelNodes);
    void notifyModelNodesMoved(const QList<ModelNode> &modelNodes);
    bool isNodeVisible(const ModelNode &modelNode) const;
    bool isNodeVisible(const QModelIndex &index) const;
    bool hasError(const QModelIndex &index) const;

private:
    void handleChangedExport(const ModelNode &modelNode, bool exportItem);
    void moveNodesInteractive(NodeAbstractProperty &parentProperty, const QList<ModelNode> &modelNodes, int targetIndex);
    void handleInternalDrop(const QMimeData *mimeData, int rowNumber, const QModelIndex &dropModelIndex);
    void handleItemLibraryItemDrop(const QMimeData *mimeData, int rowNumber, const QModelIndex &dropModelIndex);
    void handleItemLibraryImageDrop(const QMimeData *mimeData, int rowNumber, const QModelIndex &dropModelIndex);
    QList<QPersistentModelIndex> nodesToPersistentIndex(const QList<ModelNode> &modelNodes);

    QPointer<AbstractView> m_view;
    mutable QHash<ModelNode, QModelIndex> m_nodeIndexHash;
};

} // namespace QmlDesigner
