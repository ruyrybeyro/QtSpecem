#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QString>
#include <QStandardPaths>
#include <QDir>

class Settings {
public:
    static Settings& instance() {
        static Settings settings;
        return settings;
    }

    QString getLastDirectory() const {
        return settings.value("lastDirectory", QDir::homePath()).toString();
    }

    void setLastDirectory(const QString& dir) {
        settings.setValue("lastDirectory", dir);
        settings.sync();
    }

    bool getFullscreenOnStartup() const {
        return settings.value("fullscreenOnStartup", false).toBool();
    }

    void setFullscreenOnStartup(bool fullscreen) {
        settings.setValue("fullscreenOnStartup", fullscreen);
        settings.sync();
    }

    QString getSavePath() const {
        QString path = settings.value("savePath", 
            QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/saves").toString();
        
        // Ensure directory exists
        QDir dir(path);
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        
        return path;
    }

    void setSavePath(const QString& path) {
        settings.setValue("savePath", path);
        settings.sync();
    }
    
    int getScaleFactor() const {
        return settings.value("scaleFactor", 2).toInt();
    }
    
    void setScaleFactor(int factor) {
        settings.setValue("scaleFactor", factor);
        settings.sync();
    }
    
    bool getSmoothing() const {
        return settings.value("smoothScaling", false).toBool();
    }
    
    void setSmoothing(bool smooth) {
        settings.setValue("smoothScaling", smooth);
        settings.sync();
    }
    
    bool getAudioEnabled() const {
        return settings.value("audioEnabled", true).toBool();
    }
    
    void setAudioEnabled(bool enabled) {
        settings.setValue("audioEnabled", enabled);
        settings.sync();
    }
    
    int getAudioVolume() const {
        return settings.value("audioVolume", 75).toInt();
    }
    
    void setAudioVolume(int volume) {
        settings.setValue("audioVolume", volume);
        settings.sync();
    }
    
    QString getJoystickType() const {
        return settings.value("joystickType", "Kempston").toString();
    }
    
    void setJoystickType(const QString& type) {
        settings.setValue("joystickType", type);
        settings.sync();
    }

private:
    Settings() : settings(QSettings::IniFormat, QSettings::UserScope, 
                          "QtSpecem", "QtSpecem") {
    }

    // Delete copy and move constructors and assign operators
    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;
    Settings(Settings&&) = delete;
    Settings& operator=(Settings&&) = delete;

    QSettings settings;
};

#endif // SETTINGS_H

