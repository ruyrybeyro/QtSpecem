#ifndef __RAND___H_
#define __RAND___H_

#include<QApplication>
#include<QMainWindow>
#include<QWidget>
#include<QPainter>
#include<QImage>
#include<QTimer>
#include<QtGui>

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QLabel;
class QMenu;
QT_END_NAMESPACE

class DrawnWindow: public QMainWindow {
Q_OBJECT
public:
	DrawnWindow(QWidget *parent = 0);
	void paintEvent(QPaintEvent *) override;
	void KeyPress(QMainWindow *parent = 0);
protected:
    void drawBorder();
    void keyPressEvent(QKeyEvent *) override;
    void keyReleaseEvent(QKeyEvent *) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
     	//void newFile();

    	void open();
    	void save();
	void reset();
	void fullreset();
        void nmi();
	void warmreset();
        void fullscreen();
	void about();

private:
	void createActions();
    	void createMenus();

	QTimer  * timer;

        QMenu   * fileMenu;
        QMenu   * miscMenu;
        QMenu   * windowMenu;
        QMenu   * helpMenu;
   	// QAction * newAct;
    	QAction * openAct;
    	QAction * saveAct;
        QAction * resetAct;
        QAction * fullresetAct;
        QAction * nmiAct;
        QAction * warmresetAct;
        QAction * fullscreenAct;
        QAction * aboutAct;
        QLabel  * infoLabel;
};

#endif

