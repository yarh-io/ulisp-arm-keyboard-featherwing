#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "tty.h"

CTTy::CTTy() {
}

void CTTy::begin(Adafruit_ILI9341 *tft) {
  m_tft = tft;
  reset();
}

void CTTy::reset(void) {
  m_curChar = '\0';
  m_textColor = ILI9341_GREEN;
  m_backColor = ILI9341_BLACK;
  m_x = 0;
  m_y = 0;
  m_maxX = 320;
  m_maxY = 240;
  m_charWidth = 6;
  m_charHeight = 8;
  m_cursorDrawn = false;
  m_tft->setRotation(1);
  m_tft->fillScreen(m_backColor);
  m_tft->setTextColor(m_textColor, m_backColor);
  m_tft->setTextSize(1);
  m_tft->setCursor(m_x, m_y);
}

void CTTy::scrollUp(uint16_t &x, uint16_t &y) {
}

void CTTy::calcNextCursorPos(uint16_t &x, uint16_t &y) {
  if (x < m_maxX - m_charWidth) {
    x += m_charWidth;
  }
  else {
    x = 0;
    if (y < m_maxY - m_charHeight) {
      y += m_charHeight;
    }
    else {
      scrollUp(x, y);
    }
  }
}

void CTTy::moveCursor(void) {
  calcNextCursorPos(m_x, m_y);
  m_tft->setCursor(m_x, m_y);
}

void CTTy::drawCursor(void) {
  m_tft->fillRect(m_x, m_y, m_charWidth, m_charHeight, m_textColor);
  m_cursorDrawn = true;
}

void CTTy::clearCursor(void) {
  if (m_cursorDrawn) {
    m_tft->fillRect(m_x, m_y, m_charWidth, m_charHeight, m_backColor);
  }
  m_cursorDrawn = false;
}

void CTTy::moveOneLeft(void) {
  if (m_x > m_charWidth) {
    m_x -= m_charWidth;
  }
  else {
    // move to prev line or scroll up
    m_x = 0; // temp
    m_y -= m_charHeight; // temp
  }
}

void CTTy::processChar() {
  switch (m_curChar) {
    case '\b':
      moveOneLeft();
      m_tft->fillRect(m_x, m_y, m_charWidth, m_charHeight, m_backColor);
      moveOneLeft();
      moveCursor();
      drawCursor();
      break;
    case '\r':
//      m_x = 0;
//      m_tft->setCursor(m_x, m_y);
//      drawCursor();
      break;
    case '\n':
      m_x = 0;
      if (m_y < m_maxY - m_charHeight) {
        m_y += m_charHeight;
      }
      else {
        scrollUp(m_x, m_y);
      }
      m_tft->setCursor(m_x, m_y);
      drawCursor();
      break;
    default:
      if(m_curChar < ' ') {
        char pHexStr[100];
        sprintf(pHexStr,"0X%X",m_curChar);
        m_tft->print(pHexStr);
      }
      else {
        m_tft->print(m_curChar);
        moveCursor();
        drawCursor();
      }
      break;
  }
}

void CTTy::write(char c) {
  m_curChar = c;
  clearCursor();
  processChar();
}
