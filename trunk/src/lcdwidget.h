/*
 * Author - Bertrand Songis <bsongis@gmail.com>
 * 
 * Based on th9x -> http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef lcd_widget_h
#define lcd_widget_h

#include <QWidget>

#define W           128
#define H           64

class lcdWidget : public QWidget {
  public:
    lcdWidget(QWidget * parent = 0):
      QWidget(parent),
      lcd_buf(NULL)
    {
    }

    void setData(uint8_t *buf) {
      lcd_buf = buf;
    }

  protected:

    uint8_t *lcd_buf;

    void paintEvent(QPaintEvent*)
    {
      QPainter p(this);

      QRgb rgb = qRgb(150, 200, 152);
      p.setPen(rgb);
      p.setBrush(QBrush(rgb));
      p.drawRect(0, 0, W*2, H*2);

      if (lcd_buf) {
        rgb = qRgb(0, 0, 0);

        p.setPen(rgb);
        p.setBrush(QBrush(rgb));

        for (int x=0; x<W; x++) {
          for (int y=0; y<H; y++) {
            int idx = x+(y/8)*W;
            //TODO QRgb rgb = qRgb(200, 200, 200);
            if (lcd_buf[idx] & (1<<(y%8)))
              p.drawRect(2*x, 2*y, 1, 1);
          }
        }
      }
    }
};

#endif
