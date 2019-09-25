/* 
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

#include "QtSpecem.h"


#include <QMessageBox>

extern "C" void execute();

extern "C" unsigned char keybd_buff[8];
extern "C" unsigned char joystick;
extern "C" int open_sna(const char * file_name);
extern "C" void save_sna(const char * file_name);

QImage background(256, 192, QImage::Format_Indexed8);
//unsigned char * img = background.bits();

/* RGB 'Spectrum' colors */
static unsigned char rgbvals[16][3]={
            /* Normal colours */
          { 0x00, 0x00, 0x00}, { 0x00, 0x00, 0xcf},
          { 0xcf, 0x00, 0x00}, { 0xcf, 0x00, 0xcf},
          { 0x00, 0xcf, 0x00}, { 0x00, 0xcf, 0xcf},
          { 0xcf, 0xcf, 0x00}, { 0xcf, 0xcf, 0xcf},

          /* Brigth colours */
          { 0x00, 0x00, 0x00}, { 0x00, 0x00, 0xff},
          { 0xff, 0x00, 0x00}, { 0xff, 0x00, 0xff},
          { 0x00, 0xff, 0x00}, { 0x00, 0xff, 0xff},
          { 0xff, 0xff, 0x00}, { 0xff, 0xff, 0xff}
};

extern "C" void init_pallete(void) {
    int i;
    QRgb value;
    
    for (i = 0 ; i < 16 ; i++) {
        value = qRgb(rgbvals[i][0], rgbvals[i][1], rgbvals[i][2]);
        background.setColor(i, value);
    }
}

extern "C" void pixel_host(unsigned short x, unsigned short  y, unsigned char colour) {
	background.setPixel(x, y, colour);
        //img[y*192+x] = colour;
}

DrawnWindow::DrawnWindow(QWidget *parent) : QMainWindow(parent) {

	setGeometry(0,0, 256*2,192*2);
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(repaint()));
	//timer->start();
        //event->installEventFilter(this); //
	timer->start(20); //
    setAcceptDrops(true);
}

void DrawnWindow::paintEvent(QPaintEvent *) {
	QPainter paint(this);
	// paint.drawImage(0, 0, background.scaled(height(),width(),Qt::KeepAspectRatio));
    
	    paint.drawImage(0, 0, background.scaled(size()));
        execute(); 
}

int lParam;

void DrawnWindow::keyPressEvent(QKeyEvent *event)
{

    switch (event->key() )
    {
       case Qt::Key_1:      keybd_buff[3] |= ~0xFE; break;
               case Qt::Key_2:      keybd_buff[3] |= ~0xFD; break;
               case Qt::Key_3:      keybd_buff[3] |= ~0xFB; break;
               case Qt::Key_4:      keybd_buff[3] |= ~0xF7; break;
               case Qt::Key_5:      keybd_buff[3] |= ~0xEF; break;
               case Qt::Key_Q:      keybd_buff[2] |= ~0xFE; break;
               case Qt::Key_W:      keybd_buff[2] |= ~0xFD; break;
               case Qt::Key_E:      keybd_buff[2] |= ~0xFB; break;
              case Qt::Key_R:      keybd_buff[2] |= ~0xF7; break;
              case Qt::Key_T:      keybd_buff[2] |= ~0xEF; break;
              case Qt::Key_A:      keybd_buff[1] |= ~0xFE; break;
              case Qt::Key_S:      keybd_buff[1] |= ~0xFD; break;
               case Qt::Key_D:      keybd_buff[1] |= ~0xFB; break;
              case Qt::Key_F:      keybd_buff[1] |= ~0xF7; break;
              case Qt::Key_G:      keybd_buff[1] |= ~0xEF; break;
              case Qt::Key_CapsLock:
                          keybd_buff[0] |= ~0xFE; /* CAPS SHIFT */
                          break;
              case Qt::Key_Control:
              case Qt::Key_Shift:
                        keybd_buff[7] |= ~0xFD; /* SYMBOL SHIFT */
                      break;
              case Qt::Key_Z:  keybd_buff[0] |= ~0xFD; break;
               case Qt::Key_X:  keybd_buff[0] |= ~0xFB; break;
              case Qt::Key_C:  keybd_buff[0] |= ~0xF7; break;
              case Qt::Key_division:
                               keybd_buff[7] |= ~0xFD; break;
              case Qt::Key_V:  keybd_buff[0] |= ~0xEF; break;
              case Qt::Key_0:  keybd_buff[4] |= ~0xFE; break;
              case Qt::Key_9:  keybd_buff[4] |= ~0xFD; break;
              case Qt::Key_8:  keybd_buff[4] |= ~0xFB; break;
              case Qt::Key_7:  keybd_buff[4] |= ~0xF7; break;
              case Qt::Key_6:  keybd_buff[4] |= ~0xEF; break;
               case Qt::Key_P:  keybd_buff[5] |= ~0xFE; break;
               case Qt::Key_O:  keybd_buff[5] |= ~0xFD; break;
              case Qt::Key_I:  keybd_buff[5] |= ~0xFB; break;
              case Qt::Key_U:  keybd_buff[5] |= ~0xF7; break;
              case Qt::Key_Y:  keybd_buff[5] |= ~0xEF; break;
               case Qt::Key_Return: keybd_buff[6] |= ~0xFE; break;
               case Qt::Key_L:  keybd_buff[6] |= ~0xFD; break;
              //case VK_ADD:
              //         keybd_buff[7] |= ~0xFD; break;
               case Qt::Key_K:  keybd_buff[6] |= ~0xFB; break;
              //case VK_SUBTRACT:
              //       keybd_buff[7] |= ~0xFD; break;
              case Qt::Key_J:  keybd_buff[6] |= ~0xF7; break;
              case Qt::Key_H:  keybd_buff[6] |= ~0xEF; break;
      
              case Qt::Key_Escape:
                     keybd_buff[0] |= ~0xFE; /* CAPS SHIFT */
      
              case Qt::Key_Space:      keybd_buff[7] |= ~0xFE; break;
               case Qt::Key_M:  keybd_buff[7] |= ~0xFB; break;
               case Qt::Key_N:  keybd_buff[7] |= ~0xF7; break;
              //case VK_MULTIPLY:
              //       keybd_buff[7] |= ~0xFD;
              case Qt::Key_B:  keybd_buff[7] |= ~0xEF; break;
      
      
               /* Special keys */
               case Qt::Key_Tab: keybd_buff[0] |= ~0xFE;
                   keybd_buff[7] |= ~0xFD;
                   break;
      
              case Qt::Key_Backspace: keybd_buff[0] |= ~0xFE; /* CAPS SHIFT */
                    keybd_buff[4] |= ~0xFE;
                    break;
      
              /* kempston joystick */
              case Qt::Key_Left:  joystick |= 2; break;
              case Qt::Key_Right: joystick |= 1; break;
              case Qt::Key_Up:    joystick |= 8; break;
              case Qt::Key_Down:  joystick |= 4; break;
              case Qt::Key_Alt: joystick |= 16; break;
              /* Sinclair joystick */
              //case VK_NUMPAD5:
              //case VK_NUMPAD0: keybd_buff[0] |= ~0xFE;
              //         keybd_buff[4] |= ~0xFE; /* 0 - fire  */
              //          break;
              // case VK_NUMPAD4: keybd_buff[0] |= ~0xFE;
              //          keybd_buff[3] |= ~0xEF; /* 5 - left  */
              //          break;
              //case VK_NUMPAD6: keybd_buff[0] |= ~0xFE;
              //         keybd_buff[4] |= ~0xFB; /* 8 - right */
              //         break;
              //case VK_NUMPAD8: keybd_buff[0] |= ~0xFE;
              //          keybd_buff[4] |= ~0xF7; /* 7 - up    */
              //         break;
              //case VK_NUMPAD2: keybd_buff[0] |= ~0xFE;
              //         keybd_buff[4] |= ~0xEF; /* 6 - down  */
              //         break;
    }
}
 
void DrawnWindow::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key() )
    {
               case Qt::Key_1: keybd_buff[3] &= 0xFE; break;
                case Qt::Key_2: keybd_buff[3] &= 0xFD; break;
               case Qt::Key_3: keybd_buff[3] &= 0xFB; break;
               case Qt::Key_4: keybd_buff[3] &= 0xF7; break;
               case Qt::Key_5: keybd_buff[3] &= 0xEF; break;
               case Qt::Key_Q: keybd_buff[2] &= 0xFE; break;
               case Qt::Key_W: keybd_buff[2] &= 0xFD; break;
               case Qt::Key_E: keybd_buff[2] &= 0xFB; break;
               case Qt::Key_R: keybd_buff[2] &= 0xF7; break;
               case Qt::Key_T: keybd_buff[2] &= 0xEF; break;
               case Qt::Key_A: keybd_buff[1] &= 0xFE; break;
               case Qt::Key_S: keybd_buff[1] &= 0xFD; break;
                case Qt::Key_D: keybd_buff[1] &= 0xFB; break;
                case Qt::Key_F: keybd_buff[1] &= 0xF7; break;
               case Qt::Key_G: keybd_buff[1] &= 0xEF; break;
               case Qt::Key_CapsLock:
                     keybd_buff[0] &= 0xFE; /* CAPS SHIFT */
                    break;
               case Qt::Key_Control:
               case Qt::Key_Shift:
                     keybd_buff[7] &= 0xFD; /* SYMBOL SHIFT */
                 break;
                case Qt::Key_Z: keybd_buff[0] &= 0xFD; break;
               case Qt::Key_X: keybd_buff[0] &= 0xFB; break;
               case Qt::Key_C: keybd_buff[0] &= 0xF7; break;
               case Qt::Key_division:
                               keybd_buff[7] &= 0xFD; break;
               case Qt::Key_V: keybd_buff[0] &= 0xEF; break;
               case Qt::Key_0: keybd_buff[4] &= 0xFE; break;
               case Qt::Key_9: keybd_buff[4] &= 0xFD; break;
               case Qt::Key_8: keybd_buff[4] &= 0xFB; break;
                case Qt::Key_7: keybd_buff[4] &= 0xF7; break;
               case Qt::Key_6: keybd_buff[4] &= 0xEF; break;
               case Qt::Key_P: keybd_buff[5] &= 0xFE; break;
                case Qt::Key_O: keybd_buff[5] &= 0xFD; break;
               case Qt::Key_I: keybd_buff[5] &= 0xFB; break;
               case Qt::Key_U: keybd_buff[5] &= 0xF7; break;
               case Qt::Key_Y: keybd_buff[5] &= 0xEF; break;
                case Qt::Key_Return: keybd_buff[6] &= 0xFE; break;
                case Qt::Key_L: keybd_buff[6] &= 0xFD; break;
                //case VK_ADD:
                //     keybd_buff[7] &= 0xFD;
                case Qt::Key_K: keybd_buff[6] &= 0xFB; break;
               //case VK_SUBTRACT:
               //        keybd_buff[7] &= 0xFD;
               case Qt::Key_J: keybd_buff[6] &= 0xF7; break;
               case Qt::Key_H: keybd_buff[6] &= 0xEF; break;
       
               case Qt::Key_Escape:
                     keybd_buff[0] &= 0xFE; /* CAPS SHIFT */
       
               case Qt::Key_Space:      keybd_buff[7] &= 0xFE; break;
                case Qt::Key_M: keybd_buff[7] &= 0xFB; break;
               case Qt::Key_N: keybd_buff[7] &= 0xF7; break;
                //case VK_MULTIPLY:
                //     keybd_buff[7] &= 0xFD;
               case Qt::Key_B: keybd_buff[7] &= 0xEF; break;
                case Qt::Key_Tab:
                 keybd_buff[0] &= 0xFE;
                 keybd_buff[7] &= 0xFD;
                 break;
       
                case Qt::Key_Backspace: keybd_buff[0] &= 0xFE; /* CAPS SHIFT */
                     keybd_buff[4] &= 0xFE;
                     break;
       
               /* kempston joystick */
               case Qt::Key_Left:  joystick &= ~2; break;
               case Qt::Key_Right: joystick &= ~1; break;
               case Qt::Key_Up:    joystick &= ~8; break;
               case Qt::Key_Down:  joystick &= ~4; break;
               case Qt::Key_Alt:   joystick &= ~16; break;
       
                /* Sinclair joystick */
               //case VK_NUMPAD5:
               //case VK_NUMPAD0: keybd_buff[0] &= 0xFE;
               //          keybd_buff[4] &= 0xFE; /* 0 - fire  */
               //         break;
               // case VK_NUMPAD4: keybd_buff[0] &= 0xFE;
               //          keybd_buff[3] &= 0xEF; /* 5 - left  */
               //          break;
               //case VK_NUMPAD6: keybd_buff[0] &= 0xFE;
               //          keybd_buff[4] &= 0xFB; /* 8 - right */
               //          break;
               //case VK_NUMPAD8: keybd_buff[0] &= 0xFE;
               //         keybd_buff[4] &= 0xF7; /* 7 - up    */
               //         break;
               //case VK_NUMPAD2: keybd_buff[0] &= 0xFE;
               //         keybd_buff[4] &= 0xEF; /* 6 - down  */
               //         break;
       
                //case VK_F1:
                //     PostMessage(hwnd, WM_COMMAND, IDM_HELP, 0L);
                //     break;
       
                /* shortcut key for saveas and load a' la Z80 */
       
               //case VK_F2:
               //      PostMessage(hwnd, WM_COMMAND, IDM_SAVEAS, 0L);
               //        break;
               case Qt::Key_F2:
                   {
                   static int n = 0;
                   char s[200];
                       
                   sprintf(s, "/tmp/w%04d.z80", n++);
                   save_sna(s);
                   }
                   break;
       
               //case VK_F3:
               //        PostMessage(hwnd, WM_COMMAND, IDM_OPEN, 0L);
               //      break;
       
               /* for PAUSE */
               //case VK_F4:
               //      PostMessage(hwnd, WM_COMMAND, IDM_PAUSE, 0L);
               //      break;
       
               /* for RESET */
               //case VK_F5:
               //      PostMessage(hwnd, WM_COMMAND, IDM_RESET, 0L);
               //      break;

    }
}

void DrawnWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void DrawnWindow::dragMoveEvent(QDragMoveEvent *e)
{
    e->acceptProposedAction();
}

void DrawnWindow::dropEvent(QDropEvent *e)
{
   // check for our needed mime type, here a file or a list of files
   if (e->mimeData()->hasUrls())
   {

    foreach (const QUrl &url, e->mimeData()->urls()) {
        QString fileName = url.toLocalFile();
        const char * filename = fileName.toUtf8().constData();
        open_sna(filename);
//QMessageBox msgBox;
//msgBox.setText(filename);
//msgBox.exec();
    }
   }

}

