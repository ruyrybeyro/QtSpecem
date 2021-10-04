/* 
 *
 * Copyright 1991-2019 Rui Fernando Ferreira Ribeiro.
 *
 */

#include "QtSpecem.h"

extern "C" void init_emul();
extern "C" void init_pallete();
extern "C" void open_sna(const char *);
extern "C" void writebyte(unsigned short, unsigned char);
extern "C" void patch_rom(int);
extern "C" void Close_Z80Emu();
extern unsigned char * mem;

int main(int argc, char **argv) {
	QApplication app(argc, argv);
	DrawnWindow draw;
	DrawnWindow *keyPress = new DrawnWindow();
        const char * p;
        QByteArray data;
        int i;

        
    QFile file(":/rom/spectrum.rom");        

    init_pallete();
    
    init_emul();

    atexit(Close_Z80Emu);

    //draw.show();

   if(file.open(QIODevice::ReadOnly)){
      data=file.readAll();
      file.close();
      p=data;
      for (i=0; i < 16384 ; i++)
         *(mem+i) = *(p++);
   }

   if ( argc > 1 )
   {
      open_sna(argv[1]);
   }

   // keyPress->setWindowState(keyPress->windowState() | Qt::WindowFullScreen);
   keyPress->show();

   return app.exec();
}

