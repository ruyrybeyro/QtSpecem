#include "AboutDialog.h"
#include "Version.h"
#include <QVBoxLayout>
#include <QTabWidget>
#include <QTextBrowser>
#include <QDialogButtonBox>
#include <QApplication>
#include <QStyle>
#include <QLabel>
#include <QPixmap>
#include <QDesktopServices>
#include <QDateTime>
#include <QPushButton>
#include <QSysInfo>
#include <QLibraryInfo>
#include <QCoreApplication>
#include <SDL.h>
#include <QFile>
#include <QTextStream>

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle(tr("About QtSpecem"));
    setMinimumSize(650, 550);
    
    // Apply stylesheet for a nicer appearance
    setStyleSheet(
        "QDialog {"
        "  background-color: #2D2D30;"
        "  color: #F0F0F0;"
        "}"
        "QTabWidget::pane {"
        "  border: 1px solid #3F3F46;"
        "  background-color: #252526;"
        "  border-radius: 3px;"
        "}"
        "QTabBar::tab {"
        "  background-color: #2D2D30;"
        "  color: #F0F0F0;"
        "  min-width: 80px;"
        "  padding: 8px 12px;"
        "  border-top-left-radius: 3px;"
        "  border-top-right-radius: 3px;"
        "}"
        "QTabBar::tab:selected {"
        "  background-color: #007ACC;"
        "}"
        "QTabBar::tab:hover:!selected {"
        "  background-color: #3E3E42;"
        "}"
        "QPushButton {"
        "  background-color: #007ACC;"
        "  color: white;"
        "  border: none;"
        "  padding: 8px 16px;"
        "  border-radius: 3px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #1C97EA;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #0062A3;"
        "}"
    );
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Header with logo and title
    QHBoxLayout *headerLayout = new QHBoxLayout();
    
    QLabel *logoLabel = new QLabel();
    QPixmap logo(":/icons/application-icon.png");
    
    // Try different resource paths if the first one fails
    if (logo.isNull()) {
        logo = QPixmap(":/icon.png");
    }
    
    // If that still fails, try getting the application icon
    if (logo.isNull()) {
        logo = QApplication::windowIcon().pixmap(64, 64);
    }
    
    // Last resort fallback to system icons
    if (logo.isNull()) {
        logo = QApplication::style()->standardIcon(QStyle::SP_ComputerIcon).pixmap(64, 64);
    }
    
    logoLabel->setPixmap(logo.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    headerLayout->addWidget(logoLabel);
    
    QLabel *titleLabel = new QLabel(QString("<h1 style='margin-bottom:0px; color:#E0E0E0;'>QtSpecem %1</h1>"
                                     "<p style='margin-top:0px; color:#B0B0B0;'>ZX Spectrum Emulator</p>").arg(QTSPECEM_VERSION));
    headerLayout->addWidget(titleLabel, 1);
    
    mainLayout->addLayout(headerLayout);
    
    // Add a separator line
    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setStyleSheet("background-color: #3F3F46;");
    mainLayout->addWidget(separator);
    mainLayout->addSpacing(10);
    
    // Tab widget for different sections
    QTabWidget *tabWidget = new QTabWidget(this);
    
    // Collect system information for technical tab
    
    // Get Qt version
    QString qtVersion = qVersion();
    
    // Get SDL runtime version
    SDL_version sdlVersionRuntime;
    SDL_GetVersion(&sdlVersionRuntime);
    QString sdlVersionRuntimeStr = QString("%1.%2.%3")
                              .arg(sdlVersionRuntime.major)
                              .arg(sdlVersionRuntime.minor)
                              .arg(sdlVersionRuntime.patch);
    
    // Get SDL compiled version
    SDL_version sdlVersionCompiled;
    SDL_VERSION(&sdlVersionCompiled);
    QString sdlVersionCompiledStr = QString("%1.%2.%3")
                                 .arg(sdlVersionCompiled.major)
                                 .arg(sdlVersionCompiled.minor)
                                 .arg(sdlVersionCompiled.patch);
    
    // Compiler info
    #if defined(__GNUC__)
        QString compiler = QString("GCC %1.%2.%3").arg(__GNUC__).arg(__GNUC_MINOR__).arg(__GNUC_PATCHLEVEL__);
        
        #if defined(__MINGW32__) || defined(__MINGW64__)
            compiler += " (MinGW)";
        #endif
        
        #if defined(__clang__)
            compiler += QString(" with Clang %1.%2.%3").arg(__clang_major__).arg(__clang_minor__).arg(__clang_patchlevel__);
        #endif
    #elif defined(_MSC_VER)
        QString compiler = "MSVC ";
        
        // Extract major and minor version from _MSC_VER
        // Format is: NNNN (e.g., 1928 for VS 2019 16.8)
        int mscVer = _MSC_VER;
        
        // Map MSVC version numbers to Visual Studio versions
        if (mscVer >= 1930) compiler += "17.0+ (VS 2022+)";
        else if (mscVer >= 1920) compiler += "16.0-16.11 (VS 2019)";
        else if (mscVer >= 1910) compiler += "15.0-15.9 (VS 2017)";
        else if (mscVer >= 1900) compiler += "14.0 (VS 2015)";
        else if (mscVer >= 1800) compiler += "12.0 (VS 2013)";
        else if (mscVer >= 1700) compiler += "11.0 (VS 2012)";
        else if (mscVer >= 1600) compiler += "10.0 (VS 2010)";
        else compiler += QString::number(mscVer);
    #elif defined(__clang__)
        QString compiler = QString("Clang %1.%2.%3").arg(__clang_major__).arg(__clang_minor__).arg(__clang_patchlevel__);
    #else
        QString compiler = "Unknown Compiler";
    #endif
    
    // Date of build
    QString buildDate = __DATE__;
    QString buildTime = __TIME__;
    
    // Get detailed OS information
    QString osInfo = QSysInfo::prettyProductName();
    
    // If prettyProductName() returns something generic or unclear, enhance it
    if (osInfo.isEmpty() || osInfo.toLower() == "unknown" || osInfo.toLower() == "release") {
        #if defined(Q_OS_WIN)
            osInfo = QString("Windows %1").arg(QSysInfo::productVersion());
        #elif defined(Q_OS_MACOS)
            osInfo = QString("macOS %1").arg(QSysInfo::productVersion());
        #elif defined(Q_OS_LINUX)
            // Try to get more specific Linux distribution info
            QFile osRelease("/etc/os-release");
            if (osRelease.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&osRelease);
                QString line;
                while (!in.atEnd()) {
                    line = in.readLine();
                    if (line.startsWith("PRETTY_NAME=")) {
                        osInfo = line.mid(13).remove('"');
                        break;
                    }
                }
                osRelease.close();
            }
            
            if (osInfo.isEmpty() || osInfo.toLower() == "unknown" || osInfo.toLower() == "release") {
                osInfo = "Linux";
            }
        #else
            osInfo = QSysInfo::productType() + " " + QSysInfo::productVersion();
        #endif
    }
    
    // Create text browser for each tab with custom colors
    const QString cssStyle = 
        "QTextBrowser {"
        "  background-color: #252526;"
        "  color: #F0F0F0;"
        "  border: none;"
        "}"
        "a {"
        "  color: #3794FF;"
        "  text-decoration: none;"
        "}"
        "a:hover {"
        "  text-decoration: underline;"
        "}";
    
    // About tab
    QTextBrowser *aboutTextBrowser = new QTextBrowser();
    aboutTextBrowser->setStyleSheet(cssStyle);
    aboutTextBrowser->setOpenExternalLinks(true);
    aboutTextBrowser->setHtml(tr(
        "<div style='margin:15px; font-family:Arial;'>"
        "<h2 style='color:#3794FF;'>About QtSpecem</h2>"
        "<p>QtSpecem is a modern ZX Spectrum emulator that brings the classic 8-bit computer "
        "experience to today's systems with accurate emulation and enhanced features.</p>"
        "<p>Copyright &copy; 1991-2025 Rui Fernando Ferreira Ribeiro</p>"
        "<p>Visit <a href=\"https://github.com/ruyrybeyro/QtSpecem\">QtSpecem on GitHub</a> "
        "for the latest updates and information.</p>"
        "</div>"
    ));
    tabWidget->addTab(aboutTextBrowser, tr("About"));
    
    // Technical information tab
    QTextBrowser *techTextBrowser = new QTextBrowser();
    techTextBrowser->setStyleSheet(cssStyle);
    techTextBrowser->setHtml(tr(
        "<div style='margin:15px; font-family:Arial;'>"
        "<h2 style='color:#3794FF;'>Technical Information</h2>"
        "<table width='100%' cellspacing='8' style='font-size:10pt; color:#F0F0F0;'>"
        "<tr>"
        "  <td width='150'><b>Version:</b></td>"
        "  <td>%1</td>"
        "</tr>"
        "<tr>"
        "  <td><b>Build Date:</b></td>"
        "  <td>%2 %3</td>"
        "</tr>"
        "<tr>"
        "  <td><b>Qt Version:</b></td>"
        "  <td>%4 (built with Qt %5)</td>"
        "</tr>"
        "<tr>"
        "  <td><b>SDL Version:</b></td>"
        "  <td>%6 (built with SDL %7)</td>"
        "</tr>"
        "<tr>"
        "  <td><b>Compiler:</b></td>"
        "  <td>%8</td>"
        "</tr>"
        "<tr>"
        "  <td><b>Build Type:</b></td>"
        "  <td>%9</td>"
        "</tr>"
        "<tr>"
        "  <td><b>Operating System:</b></td>"
        "  <td>%10</td>"
        "</tr>"
        "<tr>"
        "  <td><b>Architecture:</b></td>"
        "  <td>%11</td>"
        "</tr>"
        "<tr>"
        "  <td><b>CPU:</b></td>"
        "  <td>%12</td>"
        "</tr>"
        "</table>"
        "</div>"
    )
    .arg(QTSPECEM_VERSION)
    .arg(buildDate)
    .arg(buildTime)
    .arg(qtVersion)
    .arg(QT_VERSION_STR)
    .arg(sdlVersionRuntimeStr)
    .arg(sdlVersionCompiledStr)
    .arg(compiler)
    .arg(
        #ifdef QT_DEBUG
            "Debug"
        #else
            "Release"
        #endif
    )
    .arg(osInfo)
    .arg(QSysInfo::currentCpuArchitecture())
    .arg(QSysInfo::buildCpuArchitecture())
    );
    tabWidget->addTab(techTextBrowser, tr("Technical"));
    
    // Authors tab
    QTextBrowser *authorsTextBrowser = new QTextBrowser();
    authorsTextBrowser->setStyleSheet(cssStyle);
    authorsTextBrowser->setHtml(tr(
        "<div style='margin:15px; font-family:Arial;'>"
        "<h2 style='color:#3794FF;'>Authors</h2>"
        "<p><b>Original Author:</b><br>"
        "Rui Fernando Ferreira Ribeiro</p>"
        "<p><b>Contributors:</b><br>"
        "<ul>"
        "<li>Please check the GitHub repository for a complete list of contributors.</li>"
        "</ul></p>"
        "</div>"
    ));
    tabWidget->addTab(authorsTextBrowser, tr("Authors"));
    
    // Technologies tab with SDL2 acknowledgment
    QTextBrowser *techsTextBrowser = new QTextBrowser();
    techsTextBrowser->setStyleSheet(cssStyle);
    techsTextBrowser->setHtml(tr(
        "<div style='margin:15px; font-family:Arial;'>"
        "<h2 style='color:#3794FF;'>Technologies</h2>"
        "<p>QtSpecem is built using:</p>"
        "<ul>"
        "<li><b>Qt Framework</b> - Cross-platform application framework</li>"
        "<li><b>SDL2 (Simple DirectMedia Layer)</b> - Cross-platform sound, gamepad/joystick support</li>"
        "<li><b>EmZ80/WSpecem Z80 CPU (Rui Ribeiro)</b> - Z80 emulation</li>"
        "</ul>"
        "<h3 style='color:#3794FF;'>SDL2 Acknowledgment</h3>"
        "<p>SDL2 is provided under the zlib license.<br>"
        "Copyright &copy; 1997-2023 Sam Lantinga &lt;slouken@libsdl.org&gt;</p>"
        "<p>SDL2 is a cross-platform development library designed to provide low-level access "
        "to audio, keyboard, mouse, joystick, and graphics hardware via OpenGL and Direct3D. "
        "It is used by video playback software, emulators, and popular games including Valve's "
        "award-winning catalog and many Humble Bundle games.</p>"
        "<p>Project Website: <a href=\"https://www.libsdl.org/\">https://www.libsdl.org/</a></p>"
        "</div>"
    ));
    tabWidget->addTab(techsTextBrowser, tr("Technologies"));
    
    // License tab
    QTextBrowser *licenseTextBrowser = new QTextBrowser();
    licenseTextBrowser->setStyleSheet(cssStyle);
    licenseTextBrowser->setHtml(tr(
        "<div style='margin:15px; font-family:Arial;'>"
        "<h2 style='color:#3794FF;'>License</h2>"
        "<p>This program is free software: you can redistribute it and/or modify "
        "it under the terms of the GNU General Public License as published by "
        "the Free Software Foundation, either version 3 of the License, or "
        "(at your option) any later version.</p>"
        "<p>This program is distributed in the hope that it will be useful, "
        "but WITHOUT ANY WARRANTY; without even the implied warranty of "
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
        "GNU General Public License for more details.</p>"
        "<p>You should have received a copy of the GNU General Public License "
        "along with this program. If not, see <a href=\"https://www.gnu.org/licenses/\">https://www.gnu.org/licenses/</a>.</p>"
        "</div>"
    ));
    tabWidget->addTab(licenseTextBrowser, tr("License"));
    
    mainLayout->addWidget(tabWidget);
    mainLayout->addSpacing(10);
    
    // Buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox();
    
    // Add Website button
    QPushButton *websiteButton = new QPushButton(tr("Visit Website"));
    buttonBox->addButton(websiteButton, QDialogButtonBox::ActionRole);
    connect(websiteButton, &QPushButton::clicked, this, []{
        QDesktopServices::openUrl(QUrl("https://github.com/ruyrybeyro/QtSpecem"));
    });
    
    // Add OK button
    QPushButton *okButton = new QPushButton(tr("OK"));
    buttonBox->addButton(okButton, QDialogButtonBox::AcceptRole);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    
    mainLayout->addWidget(buttonBox, 0, Qt::AlignRight);
}

