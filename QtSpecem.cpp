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

#define BORDER_HORIZONTAL 32
#define BORDER_VERTICAL 16

static QImage background(256+(2*BORDER_HORIZONTAL), 192+(2*BORDER_VERTICAL), QImage::Format_Indexed8);

static uint8_t border_colors[192+(2*BORDER_VERTICAL)];

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
    background.setPixel(x + BORDER_HORIZONTAL, y+BORDER_VERTICAL, colour);
}

#define BORDER_THRESHOLD_LOW 4096

static uint8_t border_color;
static uint16_t border_ptr;

void complete_border()
{
    while (border_ptr<(BORDER_VERTICAL*2)+192) {
        border_colors[border_ptr++] = border_color;
    }

}

#define BORDER_THRESHOLD_LOW 4096
#define BORDER_THRESHOLD_HIGH 4096+(65535)

extern "C" void border_updated(uint8_t color, unsigned long ticks)
{
//    printf("Border updated %02x ticks %ld\n", color, ticks);
    border_color = color;

    if (ticks<BORDER_THRESHOLD_LOW || ticks>BORDER_THRESHOLD_HIGH) {
        return;
    }
    // Compute border ptr
    unsigned new_border_ptr = ticks - BORDER_THRESHOLD_LOW;
    const unsigned height = 192+(BORDER_VERTICAL*2);
    const unsigned delta = BORDER_THRESHOLD_HIGH - BORDER_THRESHOLD_LOW;

    new_border_ptr *= height;
    new_border_ptr /= delta;

   // printf("Border updated %02x ticks %ld border_ptr %d\n", color, ticks, new_border_ptr);

    while (border_ptr < new_border_ptr) {
        border_colors[border_ptr] = border_color;
        border_ptr++;
    }

    /*Border updated 05 ticks 84
Border updated 02 ticks 2252
Border updated 05 ticks 4420
Border updated 02 ticks 6588
Border updated 05 ticks 8756
Border updated 02 ticks 10924
Border updated 05 ticks 13092
Border updated 02 ticks 15260
Border updated 05 ticks 17428
Border updated 02 ticks 19596
Border updated 05 ticks 21764
Border updated 02 ticks 23932
Border updated 05 ticks 26100
Border updated 02 ticks 28268
Border updated 05 ticks 30436
Border updated 02 ticks 32604
Border updated 05 ticks 34772
Border updated 02 ticks 36940
Border updated 05 ticks 39108
Border updated 02 ticks 41276
Border updated 05 ticks 43444
Border updated 02 ticks 45612
Border updated 05 ticks 47780
Border updated 02 ticks 49948
Border updated 05 ticks 52116
Border updated 02 ticks 54284
Border updated 05 ticks 56452
Border updated 02 ticks 58620
Border updated 05 ticks 60788
Border updated 02 ticks 62956
Border updated 05 ticks 65124
Border updated 02 ticks 67292
Border updated 05 ticks 69460 */

}

DrawnWindow::DrawnWindow(QWidget *parent) : QMainWindow(parent) {

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
       
               case Qt::Key_Space:      
                     keybd_buff[7] &= 0xFE; break;
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

