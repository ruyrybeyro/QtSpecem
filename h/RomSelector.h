#ifndef ROMSELECTOR_H
#define ROMSELECTOR_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QStringList>
#include <QMap>

class RomSelector : public QDialog
{
    Q_OBJECT

public:
    explicit RomSelector(QWidget *parent = nullptr);
    ~RomSelector();

    QString getSelectedRomPath() const;

private slots:
    void onRomSelectionChanged();
    void onLoadButtonClicked();

private:
    void initializeRomList();

    QListWidget *romListWidget;
    QPushButton *loadButton;
    QPushButton *cancelButton;
    QLabel *descriptionLabel;

    // Maps ROM names to their file paths
    QMap<QString, QString> romPaths;
    
    // Maps ROM names to their descriptions
    QMap<QString, QString> romDescriptions;
    
    // Store the selected ROM path
    QString selectedRomPath;
};

#endif // ROMSELECTOR_H
