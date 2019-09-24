#ifndef __RAND___H_
#define __RAND___H_

#include<QApplication>
#include<QMainWindow>
#include<QWidget>
#include<QPainter>
#include<QImage>
#include<QTimer>
#include <QtGui>

class DrawnWindow: public QMainWindow {
Q_OBJECT
public:
	DrawnWindow(QWidget *parent = 0);
	void paintEvent(QPaintEvent *) override;
	void KeyPress(QMainWindow *parent = 0);
protected:
    void keyPressEvent(QKeyEvent *) override;
    void keyReleaseEvent(QKeyEvent *) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *e) override;

private:
	QTimer *timer;
};

#endif

