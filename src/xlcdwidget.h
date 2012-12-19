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
#ifndef xlcd_widget_h
#define xlcd_widget_h

#include <QWidget>

#define W           212
#define H           64

class xlcdWidget : public QWidget {
  public:
    xlcdWidget(QWidget * parent = 0):
      QWidget(parent),
      lcd_buf(NULL),
      lightEnable(false)
    {
      memset(previous_buf, 0, W*H/8);
    }

    void setData(uint8_t *buf)
    {
      lcd_buf = buf;
    }
    void setRgb(uint8_t R, uint8_t G, uint8_t B) {
        r=R;
        g=G;
        b=B;
    }
    void makeScreenshot(const QString & fileName)
    {
      QPixmap buffer(2*W, 2*H);
      QPainter p(&buffer);
      doPaint(p);
      buffer.toImage().save(fileName);
    }

    void onLcdChanged(bool light)
    {
      if (light != lightEnable || memcmp(previous_buf, lcd_buf, W*H/8)) {
        lightEnable = light;
        memcpy(previous_buf, lcd_buf, W*H/8);
        update();
      }
    }

    virtual void mousePressEvent(QMouseEvent * event)
    {
      setFocus();
      QWidget::mousePressEvent(event);
    }

  protected:

    uint8_t *lcd_buf;

    bool lightEnable;
    uint8_t r,g,b;
    
    uint8_t previous_buf[W*H/8];

    inline void doPaint(QPainter & p)
    {
      QRgb rgb;
      if (lightEnable)
        rgb = qRgb(r, g, b);
      else
        rgb = qRgb(161, 161, 161);
      p.setBackground(QBrush(rgb));
      p.eraseRect(0, 0, 2*W, 2*H);

      if (lcd_buf) {
        rgb = qRgb(0, 0, 0);

        p.setPen(rgb);
        p.setBrush(QBrush(rgb));

        for (int x=0; x<W; x++) {
          for (int y=0; y<H; y++) {
            int idx = x+(y/8)*W;
            if (lcd_buf[idx] & (1<<(y%8)))
              p.drawRect(2*x, 2*y, 1, 1);
          }
        }
      }
    }

    void paintEvent(QPaintEvent*)
    {
      QPainter p(this);
      doPaint(p);
    }

};

#endif
