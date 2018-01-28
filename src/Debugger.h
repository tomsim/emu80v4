﻿/*
 *  Emu80 v. 4.x
 *  © Viktor Pykhonin <pyk@mail.ru>, 2017-2018
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DEBUGWINDOW_H
#define DEBUGWINDOW_H

#include <list>

#include "PalKeys.h"
#include "EmuObjects.h"
#include "CpuHook.h"
#include "EmuWindow.h"

class Cpu8080Compatible;
class CpuZ80;


class CodeBreakpoint : public CpuHook
{
    public:
        CodeBreakpoint(uint16_t addr) : CpuHook(addr) {};
        virtual ~CodeBreakpoint();
        bool hookProc() override;

        void setSkipCount(int skips) {m_skipCount = skips;};

    private:
        int m_skipCount = 0;
};


struct DbgRegion {
    int left;
    int top;
    int width;
    int height;
};


struct DbgLayout {
    int cols = 80;
    int rows = 40;
    DbgRegion code;
    DbgRegion regs;
    DbgRegion flags;
    DbgRegion stack;
    DbgRegion bpts;
    DbgRegion regMemHex;
    DbgRegion regMemSmb;
    DbgRegion dump;
    DbgRegion menu;
    DbgRegion aux;
};


struct DbgWSymbol {
    int chr;
    int fgColor;
    int bgColor;
};

enum BreakpointType {
    BT_EXEC,
    BT_WRITE,
    BT_READ,
    BT_ACSESS
};

struct BreakpointInfo {
    uint16_t addr;
    BreakpointType type;
    CodeBreakpoint* codeBp = nullptr;
};

class DebugWindow : private EmuWindow
{
    public:
        DebugWindow(Cpu* cpu);
        ~DebugWindow();

        void processKey(PalKeyCode keyCode, bool isPressed) override;
        void closeRequest() override;

        void setCaption(std::string caption) {EmuWindow::setCaption(caption);};

        void startDebug();
        void draw();

        EmuWindowType getWindowType() override {return EWT_DEBUG;};

    private:
        struct Cpu8080State {
            uint8_t a;
            uint8_t f;
            uint8_t b;
            uint8_t c;
            uint8_t d;
            uint8_t e;
            uint8_t h;
            uint8_t l;
            uint16_t af;
            uint16_t bc;
            uint16_t de;
            uint16_t hl;
            uint16_t sp;
            uint16_t pc;
            int fl_c;
            int fl_z;
            int fl_p;
            int fl_m;
            int fl_ac;
            uint16_t stack0;
            uint16_t stack2;
            uint16_t stack4;
            uint16_t stack6;
            uint16_t stack8;
            uint16_t stackA;
            uint16_t stackC;
            uint16_t stackE;
            uint16_t word_mem_hl;
            uint16_t word_mem_sp;
            uint8_t mem_hl[29];
            uint8_t mem_bc[29];
            uint8_t mem_de[29];
            uint8_t mem_sp[29];
            uint8_t mem_mem_hl[29];
            uint8_t mem_mem_sp[29];

            // Z80 Specific

            uint8_t a2;
            uint8_t f2;
            uint8_t b2;
            uint8_t c2;
            uint8_t d2;
            uint8_t e2;
            uint8_t h2;
            uint8_t l2;

            uint16_t af2;
            uint16_t bc2;
            uint16_t de2;
            uint16_t hl2;

            uint16_t ix;
            uint16_t iy;

            int fl_n;

            uint8_t r;
            uint8_t im;
            int iff;

            uint8_t mem_ix[29];
            uint8_t mem_iy[29];
        };

        enum ActiveMode {
            AM_NONE,    // пежим не задан, используется в m_inputFromMode
            AM_CODE,    // секция кода
            AM_DUMP,    // секция дампа
            AM_REGS,    // секция регистров
            AM_BPOINTS, // секция точек останова
            AM_INPUT    // режим ввода числа
        };

        Cpu8080Compatible* m_cpu;
        CpuZ80* m_z80cpu;
        AddressableDevice* m_as;

        Cpu8080State m_states[2];
        int m_stateNum = 0;
        bool m_z80Mode = false;
        bool m_z80Mnemonics = false;

        uint32_t m_palette[16] = {
            0x000000, 0x000080, 0x008000, 0x008080,
            0x800000, 0x800080, 0x800000, 0xC0C0C0,
            0x808080, 0xFF0000, 0x00FF00, 0x00FFFF,
            0xFF0000, 0xFF00FF, 0xFFFF00, 0xFFFFFF
        };

        //const int m_cols = 80;
        //const int m_rows = 40;
        const int m_chrW = 8;
        const int m_chrH = 12;

        DbgLayout m_compactLayout;
        DbgLayout m_stdLayout;
        DbgLayout* m_curLayout;
        bool m_compactMode = false;

        EmuPixelData m_pixelData;
        uint32_t* m_pixels = nullptr;

        uint8_t* m_font = nullptr;

        DbgWSymbol m_screen[96][48];

        int m_curX = 0;
        int m_curY = 0;
        int m_curFgColor = 15;
        int m_curBgColor = 0;

        void setLayout(bool isCompact);

        void setPos(int x, int y) {m_curX = x; m_curY = y;};
        void setColors(int fgColor, int bgColor) {m_curFgColor = fgColor; m_curBgColor = bgColor;};
        void putString(int x, int y, std::string s, int fgColor, int bgColor);
        void putString(int x, int y, std::string s);
        void putString(std::string s, int fgColor, int bgColor);
        void putString(std::string s);
        void putChars(unsigned chr, int num, bool isVertical = false);
        void putChars(int x, int y, unsigned chr, int num, bool isVertical = false);
        void highlight(int x, int y, int color, int num, bool isVertical = false);
        void clearBlock(int x1, int y1, int x2, int y2, int bgColor);

        inline uint8_t memByte(uint16_t addr) {return m_cpu->getAddrSpace()->readByte(addr);};
        inline uint16_t memWord(uint16_t addr) {return memByte(addr) + ((uint16_t)memByte(addr + 1) << 8);};
        inline void writeByte(uint16_t addr, uint8_t value) {m_cpu->getAddrSpace()->writeByte(addr, value);};
        std::string getInstructionMnemonic(uint16_t addr);
        int getInstructionLength(uint16_t addr);
        bool getInstructionOverFlag(uint16_t addr);
        void fillCpuStatus();

        void drawDbgFrame();
        void drawHexByte(int x, int y, uint8_t val, bool changed);
        void drawHexWord(int x, int y, uint16_t val, bool changed);
        void drawInt(int x, int y, int val, bool changed);
        void drawSymSeq(int x, int y, uint8_t* seq, uint8_t* old_seq, int len);
        void drawHexSeq(int x, int y, uint8_t* seq, uint8_t* old_seq, int len);
        void drawHintBar();
        void displayCpuStatus();

        // cursor related variables
        bool m_cursorVisible;                   // признак видимости курсора
        int m_cursorXPos;                       // текущий столбец курсора
        int m_cursorYPos;                       // текущая строка курсора
        int m_cursorCounter = 0;                // счетчик отрисовки курсора


        ActiveMode m_mode = AM_CODE;

        void checkForInput();                  // проверка и обработка возможного завершенного ввода

        // debugger operations
        void step();
        void over();
        void skip();
        void here();
        void run();
        void breakpoint();

        bool m_isRunning = true;
        CodeBreakpoint* m_tempBp = nullptr;
        std::list<BreakpointInfo> m_bpList;
        void checkForCurBreakpoint();           // проверяет перед выполнением, не установлена ли точка останова на текущий PC

        // code section fields and methods
        const int m_codeDefPcLine = 7;          // номер строки листинга по умолчанию для отображения PC
        const int m_codeMaxPcLine = 16;         // макс. номер строки листинга для отображения PC
        int m_codeCurPcLine;                    // текущий номер строки с текущим PC
        int m_codeHighlightedLine;              // текущая подсвечиваемая строка
        uint16_t m_codeLayout[28];              // распределение адресов по строкам листинга
        void codeInit();                        // инициализация секции кода
        void codeGotoPc();                      // переход на текущий PC в листинге
        void codeFillLayout(int lineNum);       // подготовка соответствия строк листинга и PC для отрисовки
        void codeDraw();                        // отрисовка кода
        void codeKbdProc(PalKeyCode keyCode);   // клавиатурный обработчик секции кода
        void codeGotoAddr(uint16_t addr);       // переход на указанный адрес
        void codeProcessInput();                // обработка завершения ввода

        // dump section fields and methods
        uint16_t m_dumpCurStartAddr;            // текущий начальный адрес дампа
        uint16_t m_dumpCurAddr;                 // текущий адрес в дампе
        bool m_dumpInputAddr;                   // true, если вводится адрес и false, если значение ячейки
        void dumpInit();                        // инициализация секции дампа
        void dumpDraw();                        // отрисовка дампа
        void dumpKbdProc(PalKeyCode keyCode);   // клавиатурный обработчик секции дампа
        void dumpProcessInput();                // обработка завершения ввода

        // number input fields and methods
        ActiveMode m_inputFromMode;             // предыдущая активная секция
        int m_inputXPos;                        // позиция X поля ввода
        int m_inputYPos;                        // позиция y поля ввода
        unsigned m_inputNDigits;                     // количество цифр в поле ввода
        unsigned m_inputCurPos;                      // текущая позиция курсора в поле ввода
        unsigned m_inputReturnValue;            // возвращаемое значение из поля ввода
        std::string m_inputCurValue;                 // текущее значение в поле ввода
        void inputInit();                       // инициализация полей отрисовки поля ввода
        // начало ввода
        void inputStart(ActiveMode fromMode, int x, int y, int nDigits, bool useInitialNumber, unsigned initialNumber = 0);
        void inputDraw();                       // отрисовка поля ввода
        void inputKbdProc(PalKeyCode keyCode);  // клавиатурный обработчик поля ввода
        void inputDigit(char digit);            // ввод цифры

        // regs section fields and methods
        int m_regsCurReg;                       // номер текущего регистра
        void regsInit();                        // инициализация секции регистров
        void regsDraw();                        // отрисовка секции регистров
        uint16_t regsGetCurRegValue();          // возвращает значение текущего регистра
        void regsSetCurRegValue(uint16_t value);// устанавливает значение текущего регистра
        void regsKbdProc(PalKeyCode keyCode);   // клавиатурный обработчик секции регистров
        void regsProcessInput();                // обработка завершения ввода

        // vreakpoints section fields and methods
        unsigned m_curBpoint = 0;                    // номер текущей точки останова
        void bpointsDraw();
        void bpointsKbdProc(PalKeyCode keyCode);// клавиатурный обработчик секции точек останова
};


#endif // DEBUGWINDOW_H
