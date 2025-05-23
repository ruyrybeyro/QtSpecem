#include "RomSelector.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QFile>
#include <QDebug>
#include <QSettings>

RomSelector::RomSelector(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("ROM Selection"));
    setMinimumSize(400, 300);

    // Create layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Add explanation label
    QLabel *explanationLabel = new QLabel(tr("Select a ROM to load:"), this);
    mainLayout->addWidget(explanationLabel);
    
    // Create ROM list widget
    romListWidget = new QListWidget(this);
    romListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    mainLayout->addWidget(romListWidget);
    
    // Add description label
    descriptionLabel = new QLabel(this);
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setMinimumHeight(60);
    descriptionLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    mainLayout->addWidget(descriptionLabel);
    
    // Add buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    loadButton = buttonBox->button(QDialogButtonBox::Ok);
    loadButton->setText(tr("Load"));
    loadButton->setEnabled(false);
    cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttonBox);
    
    // Connect signals and slots
    connect(romListWidget, &QListWidget::currentRowChanged, this, &RomSelector::onRomSelectionChanged);
    connect(loadButton, &QPushButton::clicked, this, &RomSelector::onLoadButtonClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    // Initialize ROM list
    initializeRomList();
    
    // Load last selected ROM from settings if available
    QSettings settings;
    QString lastRom = settings.value("lastSelectedRom", "ZX Spectrum").toString();
    for (int i = 0; i < romListWidget->count(); i++) {
        if (romListWidget->item(i)->text() == lastRom) {
            romListWidget->setCurrentRow(i);
            break;
        }
    }
    
    // Set the initial selection if not set by settings
    if (romListWidget->currentRow() < 0 && romListWidget->count() > 0) {
        romListWidget->setCurrentRow(0);
    }
}

RomSelector::~RomSelector()
{
}

void RomSelector::initializeRomList()
{
    // Define ROMs
    romPaths["ZX Spectrum"] = ":/rom/spectrum.rom";
    romPaths["TK90X v3 (English)"] = ":/rom/tk90X_v3en.rom";
    romPaths["Timex TC2048"] = ":/rom/tc2048.rom";
    
    // Define descriptions
    romDescriptions["ZX Spectrum"] = tr("Standard ZX Spectrum 48K ROM (Sinclair)");
    romDescriptions["TK90X v3 (English)"] = tr("Brazilian TK90X clone ROM v3 with English translation");
    
    // Check which ROMs exist
    QStringList availableRoms;
    for (auto it = romPaths.begin(); it != romPaths.end(); ++it) {
        if (QFile::exists(it.value())) {
            availableRoms.append(it.key());
        } 
#ifdef DEBUG
else {
            qDebug() << "ROM not found:" << it.value();
        }
#endif
    }
    
    // Add available ROMs to list widget
    romListWidget->clear();
    romListWidget->addItems(availableRoms);
}

void RomSelector::onRomSelectionChanged()
{
    if (romListWidget->currentRow() >= 0) {
        QString romName = romListWidget->currentItem()->text();
        
        // Update description
        if (romDescriptions.contains(romName)) {
            descriptionLabel->setText(romDescriptions[romName]);
        } else {
            descriptionLabel->setText("");
        }
        
        // Set selected ROM path
        if (romPaths.contains(romName)) {
            selectedRomPath = romPaths[romName];
            loadButton->setEnabled(true);
        } else {
            selectedRomPath.clear();
            loadButton->setEnabled(false);
        }
    } else {
        descriptionLabel->setText("");
        selectedRomPath.clear();
        loadButton->setEnabled(false);
    }
}

void RomSelector::onLoadButtonClicked()
{
    // Save last selection in settings
    if (romListWidget->currentRow() >= 0) {
        QString romName = romListWidget->currentItem()->text();
        QSettings settings;
        settings.setValue("lastSelectedRom", romName);
    }
    
    accept();
}

QString RomSelector::getSelectedRomPath() const
{
    return selectedRomPath;
}

