/*  
 * Copyright (C) 1991-2019 Rui Fernando Ferreira Ribeiro
 *  
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, either version 3 of the License, or  
 * (at your option) any later version.  
 */

#include "QtSpecem.h"
#include <algorithm>  // For std::copy

extern "C" void init_emul();
extern "C" void init_pallete();
extern "C" void open_sna(const char *);
extern "C" void writebyte(unsigned short, unsigned char);
extern "C" void patch_rom(int);
extern "C" void Close_Z80Emu();
extern unsigned char *mem;

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    DrawnWindow draw;
    auto keyPress = new DrawnWindow();

    QCoreApplication::setApplicationName("QtSpecem");
    QCoreApplication::setApplicationVersion("0.0.1");

    QFile file(":/rom/spectrum.rom");

    init_pallete();
    init_emul();
    atexit(Close_Z80Emu);

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();

        if (data.size() >= 16384) {
            std::copy(data.begin(), data.begin() + 16384, mem);
        }
    }

    if (argc > 1) {
        open_sna(argv[1]);
    }

    keyPress->show();
    int result = app.exec();

    delete keyPress;  // Avoid memory leak
    return result;
}

