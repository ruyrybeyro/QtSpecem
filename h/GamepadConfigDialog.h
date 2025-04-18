#ifndef GAMEPADCONFIGDIALOG_H
#define GAMEPADCONFIGDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QTimer>
#include <QListWidget>
#include "GamepadManager.h"

class GamepadConfigDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit GamepadConfigDialog(GamepadManager* gamepadManager, QWidget* parent = nullptr)
        : QDialog(parent), m_gamepadManager(gamepadManager)
    {
        setWindowTitle(tr("Gamepad Configuration"));
        
        // Create main layout
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        
        // Controller status
        QGroupBox* statusBox = new QGroupBox(tr("Controller Status"));
        QVBoxLayout* statusLayout = new QVBoxLayout(statusBox);
        
        m_statusLabel = new QLabel(tr("Checking controller status..."));
        statusLayout->addWidget(m_statusLabel);
        
        // Create refresh button
        QPushButton* refreshButton = new QPushButton(tr("Refresh Controller List"));
        connect(refreshButton, &QPushButton::clicked, this, &GamepadConfigDialog::refreshControllerList);
        statusLayout->addWidget(refreshButton);
        
        // Add connected controllers list
        m_controllerList = new QListWidget();
        statusLayout->addWidget(m_controllerList);
        
        // Joystick type selection
        QGroupBox* typeBox = new QGroupBox(tr("Joystick Type"));
        QVBoxLayout* typeLayout = new QVBoxLayout(typeBox);
        
        m_joystickTypeCombo = new QComboBox();
        
        // Add joystick types
        for (auto it = GamepadManager::JoystickTypeNames.begin(); 
             it != GamepadManager::JoystickTypeNames.end(); ++it) {
            m_joystickTypeCombo->addItem(it.value(), it.key());
        }
        
        // Set initial selection
        if (m_gamepadManager) {
            m_joystickTypeCombo->setCurrentIndex(m_gamepadManager->joystickType());
        }
        
        typeLayout->addWidget(m_joystickTypeCombo);
        
        // Add info about joystick types
        QLabel* infoLabel = new QLabel(tr(
            "<b>Joystick Types:</b><br>"
            "<b>Kempston</b>: The most common joystick interface for Spectrum games.<br>"
            "<b>Sinclair Interface 2 Port 1</b>: Uses numeric keys 6,7,8,9,0 for directions and fire.<br>"
            "<b>Sinclair Interface 2 Port 2</b>: Uses numeric keys 1,2,3,4,5 for directions and fire.<br>"
            "<b>Cursor Keys</b>: Uses Q,A,O,P,M for directions and fire."
        ));
        infoLabel->setWordWrap(true);
        typeLayout->addWidget(infoLabel);
        
        // Button mapping (read-only for now)
        QGroupBox* mappingBox = new QGroupBox(tr("Button Mapping"));
        QVBoxLayout* mappingLayout = new QVBoxLayout(mappingBox);
        
        // Create basic button info
        QLabel* buttonInfoLabel = new QLabel(tr(
            "Current Button Mapping:<br>"
            "D-Pad/Left Stick: Directions<br>"
            "A/Button 1: Fire<br>"
            "B/Button 2: Space<br>"
            "Start: Enter<br>"
            "Select: Caps Shift<br>"
            "L1: Symbol Shift<br>"
            "R1: P (Pause)"
        ));
        mappingLayout->addWidget(buttonInfoLabel);
        
        // Add groups to main layout
        mainLayout->addWidget(statusBox);
        mainLayout->addWidget(typeBox);
        mainLayout->addWidget(mappingBox);
        
        // Add info text
        QLabel* generalInfoLabel = new QLabel(tr("Connect a gamepad before starting the emulator for best results. "
                                       "Changes take effect immediately."));
        generalInfoLabel->setWordWrap(true);
        mainLayout->addWidget(generalInfoLabel);
        
        // Add button box
        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
        connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
        mainLayout->addWidget(buttonBox);
        
        // Connect signals
        connect(m_joystickTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &GamepadConfigDialog::onJoystickTypeChanged);
        
        // Set minimum size
        setMinimumWidth(450);
        setMinimumHeight(500);
        
        // Update controller status
        refreshControllerList();
        
        // Set up refresh timer
        m_refreshTimer = new QTimer(this);
        connect(m_refreshTimer, &QTimer::timeout, this, &GamepadConfigDialog::updateControllerStatus);
        m_refreshTimer->start(500);  // Update every half second
    }
    
    ~GamepadConfigDialog() {
        if (m_refreshTimer) {
            m_refreshTimer->stop();
        }
    }
    
private slots:
    void onJoystickTypeChanged(int index) {
        if (m_gamepadManager) {
            GamepadManager::JoystickType type = static_cast<GamepadManager::JoystickType>(index);
            m_gamepadManager->setJoystickType(type);
            updateControllerStatus();
        }
    }
    
    void updateControllerStatus() {
        if (m_gamepadManager) {
            bool connected = m_gamepadManager->isGamepadConnected();
            
            if (connected) {
                m_statusLabel->setText(tr("Controller connected"));
                m_statusLabel->setStyleSheet("color: green; font-weight: bold;");
            } else {
                m_statusLabel->setText(tr("No controller detected"));
                m_statusLabel->setStyleSheet("color: red;");
            }
        }
    }
    
    void refreshControllerList() {
        m_controllerList->clear();
        
        // Poll SDL to update the list
        SDL_JoystickUpdate();
        
        int joystickCount = SDL_NumJoysticks();
        if (joystickCount > 0) {
            for (int i = 0; i < joystickCount; i++) {
                SDL_Joystick* joy = SDL_JoystickOpen(i);
                if (joy) {
                    QString joystickInfo = QString("%1: %2 (%3 buttons, %4 axes, %5 hats)")
                        .arg(i)
                        .arg(SDL_JoystickName(joy))
                        .arg(SDL_JoystickNumButtons(joy))
                        .arg(SDL_JoystickNumAxes(joy))
                        .arg(SDL_JoystickNumHats(joy));
                    
                    m_controllerList->addItem(joystickInfo);
                    SDL_JoystickClose(joy);
                }
            }
        } else {
            m_controllerList->addItem(tr("No joysticks detected"));
        }
        
        updateControllerStatus();
    }
    
private:
    GamepadManager* m_gamepadManager;
    QLabel* m_statusLabel;
    QComboBox* m_joystickTypeCombo;
    QListWidget* m_controllerList;
    QTimer* m_refreshTimer;
};

#endif // GAMEPADCONFIGDIALOG_H
