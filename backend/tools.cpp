// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
#include "tools.h"
#include <QCoreApplication>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(toolsLog, "tools.qtagopengps")

Tools *Tools::s_instance = nullptr;
QMutex Tools::s_mutex;
bool Tools::s_cpp_created = false;

Tools::Tools(QObject *parent)
    : QObject{parent}
    , m_toolsSectionsModel(new ToolsSectionsModel(this))
{
    //put in a default tool to keep QML happy
    addTool(new Tool(this));
}

Tools *Tools::instance() {
    QMutexLocker locker(&s_mutex);
    if (!s_instance) {
        s_instance = new Tools();
        qDebug(toolsLog) << "Tools singleton created by C++ code.";
        s_cpp_created = true;
        // Ensure cleanup on app exit
        QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
                         s_instance, []() {
                             delete s_instance;
                             s_instance = nullptr;
                         });
    }
    return s_instance;
}

Tools *Tools::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine) {
    Q_UNUSED(jsEngine)

    QMutexLocker locker(&s_mutex);

    if (!s_instance) {
        s_instance = new Tools();
        qDebug(toolsLog) << "Tools singleton created by QML engine.";
    } else if (s_cpp_created) {
        qmlEngine->setObjectOwnership(s_instance, QQmlEngine::CppOwnership);
    }

    return s_instance;
}

void Tools::addTool(Tool *tool)
{
    if (!tool) {
        qWarning(toolsLog) << "Cannot add null tool";
        return;
    }

    // Add to the tools list
    m_toolsList.append(QVariant::fromValue(tool));

    // Add the tool's sections model to the ToolsSectionsModel
    m_toolsSectionsModel->addSectionsModel(tool->sections());

    emit toolsListChanged();

    qDebug(toolsLog) << "Tool added. Total tools:" << m_toolsList.count();
}

void Tools::removeTool(int index)
{
    if (index < 0 || index >= m_toolsList.count()) {
        qWarning(toolsLog) << "Invalid tool index:" << index;
        return;
    }

    m_toolsList.removeAt(index);
    m_toolsSectionsModel->removeRowAt(index);

    emit toolsListChanged();

    qDebug(toolsLog) << "Tool removed at index" << index << ". Remaining tools:" << m_toolsList.count();
}

void Tools::clearTools()
{
    m_toolsList.clear();
    m_toolsSectionsModel->clear();

    emit toolsListChanged();

    qDebug(toolsLog) << "All tools cleared";
}

Tool* Tools::toolAt(int index) const
{
    if (index < 0 || index >= m_toolsList.count()) {
        qWarning(toolsLog) << "Invalid tool index:" << index;
        return nullptr;
    }

    return m_toolsList.at(index).value<Tool*>();
}
