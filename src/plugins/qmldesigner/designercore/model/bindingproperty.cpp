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

#include "bindingproperty.h"
#include "nodeproperty.h"
#include "internalproperty.h"
#include "invalidmodelnodeexception.h"
#include "invalidpropertyexception.h"
#include "invalidargumentexception.h"
#include "internalnode_p.h"
#include "model.h"
#include "model_p.h"
namespace QmlDesigner {

BindingProperty::BindingProperty()
{
}

BindingProperty::BindingProperty(const BindingProperty &property, AbstractView *view)
    : AbstractProperty(property.name(), property.internalNode(), property.model(), view)
{
}


BindingProperty::BindingProperty(const PropertyName &propertyName, const Internal::InternalNodePointer &internalNode, Model* model, AbstractView *view)
    : AbstractProperty(propertyName, internalNode, model, view)
{
}


void BindingProperty::setExpression(const QString &expression)
{
    Internal::WriteLocker locker(model());
    if (!isValid())
        throw InvalidModelNodeException(__LINE__, __FUNCTION__, __FILE__);

    if (isDynamic())
        qWarning() << "Calling BindingProperty::setExpression on dynamic property.";

    if (name() == "id") { // the ID for a node is independent of the state, so it has to be set with ModelNode::setId
        throw InvalidPropertyException(__LINE__, __FUNCTION__, __FILE__, name());
    }

    if (expression.isEmpty())
        throw InvalidArgumentException(__LINE__, __FUNCTION__, __FILE__, name());

    if (internalNode()->hasProperty(name())) { //check if oldValue != value
        Internal::InternalProperty::Pointer internalProperty = internalNode()->property(name());
        if (internalProperty->isBindingProperty()
            && internalProperty->toBindingProperty()->expression() == expression)

            return;
    }

    if (internalNode()->hasProperty(name()) && !internalNode()->property(name())->isBindingProperty())
        model()->d->removeProperty(internalNode()->property(name()));

    model()->d->setBindingProperty(internalNode(), name(), expression);
}

QString BindingProperty::expression() const
{
    if (internalNode()->hasProperty(name())
        && internalNode()->property(name())->isBindingProperty())
        return internalNode()->bindingProperty(name())->expression();

    return QString();
}

static ModelNode resolveBinding(const QString &binding, ModelNode currentNode, AbstractView* view)
{
    int i = 0;
    QString element = binding.split(QLatin1Char('.')).at(0);
    while (!element.isEmpty())
    {
        if (element == "parent") {
            if (currentNode.hasParentProperty())
                currentNode = currentNode.parentProperty().toNodeAbstractProperty().parentModelNode();
            else
                return ModelNode(); //binding not valid
        } else if (currentNode.hasProperty(element.toUtf8())) {
            if (currentNode.property(element.toUtf8()).isNodeProperty())
                currentNode = currentNode.nodeProperty(element.toUtf8()).modelNode();
            else if (view->hasId(element))
                currentNode = view->modelNodeForId(element); //id
            else
                return ModelNode(); //binding not valid

        } else {
            currentNode = view->modelNodeForId(element); //id
        }
        i++;
        if (i < binding.split(QLatin1Char('.')).count())
            element = binding.split(QLatin1Char('.')).at(i);
        else
            element.clear();
    }
    return currentNode;

}

ModelNode BindingProperty::resolveToModelNode() const
{
    if (!isValid())
        throw InvalidModelNodeException(__LINE__, __FUNCTION__, __FILE__);

    return resolveBinding(expression(), parentModelNode(), view());
}

static inline QStringList commaSeparatedSimplifiedStringList(const QString &string)
{
    QStringList stringList = string.split(QLatin1String(","));
    QStringList simpleList;
    foreach (const QString &simpleString, stringList)
        simpleList.append(simpleString.simplified());
    return simpleList;
}


AbstractProperty BindingProperty::resolveToProperty() const
{
    if (!isValid())
        throw InvalidModelNodeException(__LINE__, __FUNCTION__, __FILE__);

    QString binding = expression();
    ModelNode node = parentModelNode();
    QString element;
    if (binding.contains(QLatin1Char('.'))) {
        element = binding.split(QLatin1Char('.')).last();
        QString nodeBinding = binding;
        nodeBinding.chop(element.length());
        node = resolveBinding(nodeBinding, parentModelNode(), view());
    } else {
        element = binding;
    }

    if (node.isValid())
        return node.property(element.toUtf8());
    else
        return AbstractProperty();
}

bool BindingProperty::isList() const
{
    if (!isValid())
        throw InvalidModelNodeException(__LINE__, __FUNCTION__, __FILE__);

    return expression().startsWith('[') && expression().endsWith(']');
}

QList<ModelNode> BindingProperty::resolveToModelNodeList() const
{
    QList<ModelNode> returnList;
    if (!isValid())
        throw InvalidModelNodeException(__LINE__, __FUNCTION__, __FILE__);
    if (isList()) {
        QString string = expression();
        string.chop(1);
        string.remove(0, 1);
        QStringList simplifiedList = commaSeparatedSimplifiedStringList(string);
        foreach (const QString &nodeId, simplifiedList) {
            if (view()->hasId(nodeId))
                returnList.append(view()->modelNodeForId(nodeId));
        }
    }
    return returnList;
}

void BindingProperty::setDynamicTypeNameAndExpression(const TypeName &typeName, const QString &expression)
{
    Internal::WriteLocker locker(model());
    if (!isValid())
        throw InvalidModelNodeException(__LINE__, __FUNCTION__, __FILE__);

    if (name() == "id") { // the ID for a node is independent of the state, so it has to be set with ModelNode::setId
        throw InvalidPropertyException(__LINE__, __FUNCTION__, __FILE__, name());
    }

    if (expression.isEmpty())
        throw InvalidArgumentException(__LINE__, __FUNCTION__, __FILE__, name());

    if (typeName.isEmpty())
        throw InvalidArgumentException(__LINE__, __FUNCTION__, __FILE__, name());

    if (internalNode()->hasProperty(name())) { //check if oldValue != value
        Internal::InternalProperty::Pointer internalProperty = internalNode()->property(name());
        if (internalProperty->isBindingProperty()
            && internalProperty->toBindingProperty()->expression() == expression
            && internalProperty->toBindingProperty()->dynamicTypeName() == typeName)

            return;
    }

    if (internalNode()->hasProperty(name()) && !internalNode()->property(name())->isBindingProperty())
        model()->d->removeProperty(internalNode()->property(name()));

     model()->d->setDynamicBindingProperty(internalNode(), name(), typeName, expression);
}

} // namespace QmlDesigner
