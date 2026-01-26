// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
#include "tools.h"
#include <QCoreApplication>
#include <QLoggingCategory>
#include "settingsmanager.h"
#include "toolsproperties.h"

Q_LOGGING_CATEGORY(toolsLog, "tools.qtagopengps")

Tools *Tools::s_instance = nullptr;
QMutex Tools::s_mutex;
bool Tools::s_cpp_created = false;

Tools::Tools(QObject *parent)
    : QObject{parent}
    , m_toolsWithSectionsModel(new ToolsWithSectionsModel(this))
    , m_toolsProperties(new ToolsProperties(this))
{
    //put create tools from settings.  CTool and FormGPS_position will fill in
    //specific values for leftPosition, rightPosition, easting, northing, etc.
    //eventually this will be done differently.

    generateToolFromSettings();
    connect(SettingsManager::instance(), &SettingsManager::tool_isSectionsNotZonesChanged,
            this, &Tools::generateToolFromSettings);
    connect(SettingsManager::instance(), &SettingsManager::vehicle_numSectionsChanged,
            this, &Tools::generateToolFromSettings);
    connect(SettingsManager::instance(), &SettingsManager::tool_zonesChanged,
            this, &Tools::generateToolFromSettings);
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

    //pass on the signals from this tool
    connect(tool, &Tool::sectionButtonStateChanged,
            [this](int sectionButtonNo, SectionButtonsModel::State new_state) {
                emit sectionButtonStateChanged(m_toolsList.count()-1, sectionButtonNo, new_state);
    });

    if (tool->sectionButtonsModel()->count()) {
        m_toolsWithSectionsModel->addToolIndex(m_toolsList.count()-1);
    }
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
    m_toolsWithSectionsModel->removeToolIndex(index);

    emit toolsListChanged();

    qDebug(toolsLog) << "Tool removed at index" << index << ". Remaining tools:" << m_toolsList.count();
}

void Tools::clearTools()
{
    m_toolsList.clear();
    m_toolsWithSectionsModel->clear();

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

void Tools::setSectionButtonState(int toolIndex, int sectionButtonNo, SectionButtonsModel::State new_state)
{
    if ( toolIndex < m_toolsList.count()) {
        m_toolsList[toolIndex].value<Tool *>()->setSectionButtonState(sectionButtonNo, new_state);
    }
}

void Tools::setAllSectionButtonsToState(int toolIndex, SectionButtonsModel::State new_state)
{
    if ( toolIndex < m_toolsList.count()) {
        m_toolsList[toolIndex].value<Tool *>()->setAllSectionButtonsToState(new_state);
    }
}

void Tools::generateToolFromSettings() {
    int numSections;
    m_toolsList.clear();

    if (SettingsManager::instance()->tool_isTBT()) {
        //create a tool to represent the cart, but will
        //have no sections associated with it.
        auto *newTool = new Tool(this);

        newTool->set_isTBTTank(true);
        newTool->set_trailing(true);
        newTool->set_hitchLength(SettingsManager::instance()->vehicle_tankTrailingHitchLength());

        //nothing added to toolsWithSectionsModel

        addTool(newTool);

    } else {
        if (SettingsManager::instance()->tool_isSectionsNotZones()) {
            numSections = SettingsManager::instance()->vehicle_numSections();
        } else {
            QVector<int> zoneRanges;
            zoneRanges = SettingsManager::instance()->tool_zones();
            if (zoneRanges.size() > 0) {
                numSections = zoneRanges[0];
            } else {
                qWarning() << "Zones used, not sections, but the number of zones is zero!";
                numSections = 0;
            }
        }

        auto *newTool = new Tool(this);
        if (SettingsManager::instance()->tool_isToolTrailing()) {
            newTool->set_trailing(true);
            newTool->set_hitchLength(SettingsManager::instance()->tool_toolTrailingHitchLength());
        } else {
            newTool->set_trailing(false);
            newTool->set_hitchLength(0);
        }

        //Set up the QML buttons
        for (int i=0; i  < numSections; i++) {
            newTool->sectionButtonsModel()->addSectionState( {i, SectionButtonsModel::Off} );
        }
        addTool(newTool);
    }
}
