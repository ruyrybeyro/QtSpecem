/* 
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

#include "QtSpecem.h"


#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>  
#include <QtWidgets> 

extern "C" void execute();
extern "C" void do_reset();
extern "C" void do_fullreset();
extern "C" void do_nmi_int();
extern "C" void do_warmreset();

extern "C" unsigned char keybd_buff[8];
extern "C" unsigned char joystick;
extern "C" int open_sna(const char * file_name);
extern "C" void save_sna(const char * file_name);

#define BORDER_HORIZONTAL 32
#define BORDER_VERTICAL 16

#define BORDER_THRESHOLD_LOW 4096              /* Border scan start in T-states */
#define BORDER_THRESHOLD_HIGH 4096+(65535)     /* Border scan end in T-states */


static QImage background(256+(2*BORDER_HORIZONTAL), 192+(2*BORDER_VERTICAL), QImage::Format_Indexed8);

/* This stores one color for a specific border line */
static uint8_t border_colors[192+(2*BORDER_VERTICAL)];
static uint8_t border_color; // Last border color seen
static uint16_t border_ptr; // Current border line being updated */

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
    background.setPixel(x + BORDER_HORIZONTAL, y+BORDER_VERTICAL, colour);
}

void complete_border()
{
    while (border_ptr<(BORDER_VERTICAL*2)+192) {
        border_colors[border_ptr++] = border_color;
    }

}

extern "C" void border_updated(uint8_t color, unsigned long ticks)
{
    if (ticks<BORDER_THRESHOLD_LOW || ticks>BORDER_THRESHOLD_HIGH) {
        // Even if we do not draw anything, we need to save the color here.
        border_color = color;
        return;
    }
    // Compute border ptr
    unsigned new_border_ptr = ticks - BORDER_THRESHOLD_LOW;

    // Total number of screen lines including border
    const unsigned lines = sizeof(border_colors)/sizeof(border_colors[0]);

    const unsigned height = 192+(BORDER_VERTICAL*2);
    const unsigned delta = BORDER_THRESHOLD_HIGH - BORDER_THRESHOLD_LOW;

    // Remap the range LOW-HIGH into the image vertical area

    new_border_ptr *= height;
    new_border_ptr /= delta;

    // Do not let overflow.
    if (new_border_ptr>lines) {
        new_border_ptr = lines; // This is still 1-past the array size. No problem here.
    }

    while (border_ptr < new_border_ptr) {
        border_colors[border_ptr] = border_color; // Old color
        border_ptr++;
    }
    // If we already drawn the last line, skip, otherwise place the color there.
    if (border_ptr<lines) {
        border_colors[border_ptr++] = color; // new color
    }

    // Save color.
    border_color = color;
}

void DrawnWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    //menu.addAction(cutAct);
    //menu.addAction(copyAct);
    //menu.addAction(pasteAct);
    menu.exec(event->globalPos());
}

void DrawnWindow::newFile()
{
}

void DrawnWindow::open()
{
   QString fileName = QFileDialog::getOpenFileName(this,
    tr("Open Image"), "/home/jana", tr("Spectrum Files (*.z80 *.sna *.tap)"));
   QByteArray ba = fileName.toLocal8Bit(); 
   if (ba.data() != NULL)
      open_sna(ba.data());
}

void DrawnWindow::save()
{
}


void DrawnWindow::reset()
{
    do_reset();
}

void DrawnWindow::fullreset()
{
    do_fullreset();
}

void DrawnWindow::nmi()
{
    do_nmi_int();
}

void DrawnWindow::warmreset()
{
    do_warmreset();
}

void DrawnWindow::about()
{
    //infoLabel->setText(tr("<b>Help|About</b>"));
    QMessageBox::about(this, tr("About Menu"),
            tr("QtSpecem\n" 
               "https://github.com/ruyrybeyro/QtSpecem\n"
               __DATE__));
}

void DrawnWindow::createActions()
{
    newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &DrawnWindow::newFile);

    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &DrawnWindow::open);

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &DrawnWindow::save);

    resetAct = new QAction(tr("&Reset"), this);
    //resetAct->setShortcuts(QKeySequence::New);
    resetAct->setStatusTip(tr("Reset Spectrum"));
    connect(resetAct, &QAction::triggered, this, &DrawnWindow::reset);

    fullresetAct = new QAction(tr("&Full Reset"), this);
    //fullresetAct->setShortcuts(QKeySequence::New);
    fullresetAct->setStatusTip(tr("Power cyle Spectrum"));
    connect(fullresetAct, &QAction::triggered, this, &DrawnWindow::fullreset);

    nmiAct = new QAction(tr("&NMI"), this);
    //nmiAct->setShortcuts(QKeySequence::Open);
    nmiAct->setStatusTip(tr("NMI"));
    connect(nmiAct, &QAction::triggered, this, &DrawnWindow::nmi);

    warmresetAct = new QAction(tr("&Warm Reset"), this);
    //resetAct->setShortcuts(QKeySequence::New);
    warmresetAct->setStatusTip(tr("Warm Reset Spectrum"));
    connect(warmresetAct, &QAction::triggered, this, &DrawnWindow::warmreset);


    // About is bellow the app name
    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show About box"));
    connect(aboutAct, &QAction::triggered, this, &DrawnWindow::about);
}

void DrawnWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    //fileMenu->addSeparator();


    miscMenu = menuBar()->addMenu(tr("&Misc"));
    miscMenu->addAction(resetAct);
    miscMenu->addAction(fullresetAct);
    miscMenu->addAction(nmiAct);
    miscMenu->addAction(warmresetAct);
    //miscMenu->addSeparator();
    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
}

DrawnWindow::DrawnWindow(QWidget *parent) : QMainWindow(parent) {

    createActions();
    createMenus();

    setGeometry(0,0, ((256+(BORDER_HORIZONTAL*2))*2),
                (192+(BORDER_VERTICAL*2))*2);

    memset(border_colors,0x7, sizeof(border_colors));
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(repaint()));
    timer->start(20); //
    setAcceptDrops(true);
    border_ptr = 0;
}

void DrawnWindow::drawBorder()
{
    // Top border
    int i;
    uchar *ptr;

    for (i=0; i<BORDER_VERTICAL; i++) {
        uint8_t color = border_colors[i];
        ptr = background.scanLine(i);
        int x;
        for (x=0;x<256+(BORDER_HORIZONTAL*2);x++) {
           *ptr++ = color;
        }
    }
    for(i=0; i<192; i++) {
        ptr = background.scanLine(i+BORDER_VERTICAL);
        uint8_t color = border_colors[i+BORDER_VERTICAL];
        int x;
        for (x=0; x<BORDER_HORIZONTAL;x++) {
           *ptr++ = color;
        }
        ptr += 256;
        for (x=0; x<BORDER_HORIZONTAL;x++) {
           *ptr++ = color;
        }
    }

    for (i=0; i<BORDER_VERTICAL; i++) {
        ptr = background.scanLine(i + BORDER_VERTICAL + 192);
        uint8_t color = border_colors[i+BORDER_VERTICAL+192];

        int x;
        for (x=0;x<256+(BORDER_HORIZONTAL*2);x++) {
           *ptr++ = color;
        }
    }
}

void DrawnWindow::paintEvent(QPaintEvent *) {
    QPainter paint(this);
    drawBorder();
    paint.drawImage(0, 0, background.scaled(size()));
    execute();
    complete_border();

    // Reset border ptr
    border_ptr = 0;
}

int lParam;

void DrawnWindow::keyPressEvent(QKeyEvent *event)
{
    // qDebug() << QKeySequence(event->key()).toString();
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
              /*case Qt::"+":
                               keybd_buff[7] |= ~0xFD; break;
              */
              case Qt::Key_K:  keybd_buff[6] |= ~0xFB; break;
              /*case Qt::"-":
                               keybd_buff[7] |= ~0xFD; break;
              */
              case Qt::Key_J:  keybd_buff[6] |= ~0xF7; break;
              case Qt::Key_H:  keybd_buff[6] |= ~0xEF; break;
      
              case Qt::Key_Escape:
                     keybd_buff[0] |= ~0xFE; /* CAPS SHIFT */
      
              case Qt::Key_Space:      keybd_buff[7] |= ~0xFE; break;
              case Qt::Key_M:  keybd_buff[7] |= ~0xFB; break;
              case Qt::Key_N:  keybd_buff[7] |= ~0xF7; break;
              case Qt::Key_Asterisk:
                     keybd_buff[7] |= ~0xFD; break;
              case Qt::Key_B:  keybd_buff[7] |= ~0xEF; break;
      
      
               /* Special keys */
               case Qt::Key_Tab: keybd_buff[0] |= ~0xFE;
                   keybd_buff[7] |= ~0xFD;
                   break;
      
              case Qt::Key_Backspace: keybd_buff[0] |= ~0xFE; /* CAPS SHIFT */
                    keybd_buff[4] |= ~0xFE;
                    break;

              /* kempston joystick / CAPS + cursor 58760 */
              case Qt::Key_Left:  joystick |= 2; 
                                  keybd_buff[0] |= ~0xFE; keybd_buff[3] |= ~0xEF; 
                                  break;
              case Qt::Key_Right: joystick |= 1; 
                                  keybd_buff[0] |= ~0xFE; keybd_buff[4] |= ~0xFB; 
                                  break;
              case Qt::Key_Up:    joystick |= 8; 
                                  keybd_buff[0] |= ~0xFE; keybd_buff[4] |= ~0xF7; 
                                  break;
              case Qt::Key_Down:  joystick |= 4; 
                                  keybd_buff[0] |= ~0xFE; keybd_buff[4] |= ~0xEF; 
                                  break;
              case Qt::Key_Alt:   joystick |= 16; 
                                  keybd_buff[0] |= ~0xFE;keybd_buff[4] |= ~0xFE; 
                                  break;
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
               /*case Qt::"+":
                               keybd_buff[7] &= 0xFD;
                */
               case Qt::Key_K: keybd_buff[6] &= 0xFB; break;
               /*case Qt::"-":
                               keybd_buff[7] &= 0xFD;
                */
               case Qt::Key_J: keybd_buff[6] &= 0xF7; break;
               case Qt::Key_H: keybd_buff[6] &= 0xEF; break;
       
               case Qt::Key_Escape:
                     keybd_buff[0] &= 0xFE; /* CAPS SHIFT */
       
               case Qt::Key_Space:      
                     keybd_buff[7] &= 0xFE; break;
               case Qt::Key_M: keybd_buff[7] &= 0xFB; break;
               case Qt::Key_N: keybd_buff[7] &= 0xF7; break;
               case Qt::Key_Asterisk:
                 keybd_buff[7] &= 0xFD; break;
               case Qt::Key_B: keybd_buff[7] &= 0xEF; break;
               case Qt::Key_Tab:
                 keybd_buff[0] &= 0xFE;
                 keybd_buff[7] &= 0xFD;
                 break;
       
               case Qt::Key_Backspace: keybd_buff[0] &= 0xFE; /* CAPS SHIFT */
                     keybd_buff[4] &= 0xFE;
                     break;

	       /* Key_Comma Key_Colon Key_Semicolon Key_Slash Key_Acircumflex 	
                  Key_AsciiTilde Key_Backslash Key_Greater Key_Less
                  Key_Period Key_Underscore
                */
      
               /* kempston joystick / CAPS + cursor 58760 */
               case Qt::Key_Left:  joystick &= ~2; 
                                   keybd_buff[4] &= 0xFE; keybd_buff[3] &= 0xEF;break;
               case Qt::Key_Right: joystick &= ~1; 
                                   keybd_buff[4] &= 0xFE; keybd_buff[4] &= 0xFB;break;
               case Qt::Key_Up:    joystick &= ~8; 
                                   keybd_buff[4] &= 0xFE; keybd_buff[4] &= 0xF7;break;
               case Qt::Key_Down:  joystick &= ~4; 
                                   keybd_buff[4] &= 0xFE; keybd_buff[4] &= 0xEF;break;
               case Qt::Key_Alt:   joystick &= ~16;
                                   keybd_buff[4] &= 0xFE; keybd_buff[4] &= 0xFE;break;
       
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

               case Qt::Key_F5:
                   {
                   static int n = 0;
                   char s[200];

                   sprintf(s, "/tmp/w%04d.tap", n++);
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

