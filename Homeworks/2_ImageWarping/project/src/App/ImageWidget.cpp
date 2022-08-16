#include "ImageWidget.h"
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>

using std::cout;
using std::endl;

ImageWidget::ImageWidget(void)
{
	ptr_image_ = new QImage();
	ptr_image_backup_ = new QImage();
	is_choose = false;
	is_draw = false;
}


ImageWidget::~ImageWidget(void)
{
	delete ptr_image_;
	delete ptr_image_backup_;
	start_list.clear();
	end_list.clear();
	
}

void ImageWidget::paintEvent(QPaintEvent *paintevent)
{
	QPainter painter;
	painter.begin(this);

	// Draw background
	painter.setBrush(Qt::lightGray);
	QRect back_rect(0, 0, width(), height());
	painter.drawRect(back_rect);

	// Draw image
	QRect rect = QRect( (width()-ptr_image_->width())/2, (height()-ptr_image_->height())/2, ptr_image_->width(), ptr_image_->height());
	painter.drawImage(rect, *ptr_image_); 

	// Draw points and lines
	for (int i = 0; i < start_list.size(); i++)
	{
		painter.setPen(QPen(Qt::blue, 10));
		painter.drawPoint(start_list[i]);
		painter.setPen(QPen(Qt::green, 10));
		painter.drawPoint(end_list[i]);
		painter.setPen(QPen(Qt::red, 3));
		painter.drawLine(start_list[i], end_list[i]);
	}
	if (is_draw)
	{
		painter.setPen(QPen(Qt::red, 3));
		painter.drawLine(start, end);
		painter.setPen(QPen(Qt::blue, 10));
		painter.drawPoint(start);
		painter.setPen(QPen(Qt::green, 10));
		painter.drawPoint(end);
	}

	painter.end();
}

void ImageWidget::Open()
{
	// Open file
	QString fileName = QFileDialog::getOpenFileName(this, tr("Read Image"), ".", tr("Images(*.bmp *.png *.jpg)"));

	// Load file
	if (!fileName.isEmpty())
	{
		ptr_image_->load(fileName);
		*(ptr_image_backup_) = *(ptr_image_);
	}

	//ptr_image_->invertPixels(QImage::InvertRgb);
	//*(ptr_image_) = ptr_image_->mirrored(true, true);
	//*(ptr_image_) = ptr_image_->rgbSwapped();
	cout<<"image size: "<<ptr_image_->width()<<' '<<ptr_image_->height()<<endl;
	update();
}

void ImageWidget::Save()
{
	SaveAs();
}

void ImageWidget::SaveAs()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
	if (filename.isNull())
	{
		return;
	}	

	ptr_image_->save(filename);
}

void ImageWidget::Invert()
{
	for (int i=0; i<ptr_image_->width(); i++)
	{
		for (int j=0; j<ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			ptr_image_->setPixel(i, j, qRgb(255-qRed(color), 255-qGreen(color), 255-qBlue(color)) );
		}
	}

	// equivalent member function of class QImage
	// ptr_image_->invertPixels(QImage::InvertRgb);
	update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	QImage image_tmp(*(ptr_image_));
	int width = ptr_image_->width();
	int height = ptr_image_->height();

	if (ishorizontal)
	{
		if (isvertical)
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(width-1-i, height-1-j));
				}
			}
		} 
		else			//仅水平翻转			
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(width-1-i, j));
				}
			}
		}
		
	}
	else
	{
		if (isvertical)		//仅垂直翻转
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(i, height-1-j));
				}
			}
		}
	}

	// equivalent member function of class QImage
	//*(ptr_image_) = ptr_image_->mirrored(true, true);
	update();
}

void ImageWidget::TurnGray()
{
	for (int i=0; i<ptr_image_->width(); i++)
	{
		for (int j=0; j<ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			int gray_value = (qRed(color)+qGreen(color)+qBlue(color))/3;
			ptr_image_->setPixel(i, j, qRgb(gray_value, gray_value, gray_value) );
		}
	}

	update();
}

void ImageWidget::Restore()
{
	*(ptr_image_) = *(ptr_image_backup_);
	update();
}

void ImageWidget::ChoosePoints()
{
	is_choose = !is_choose;
}

void ImageWidget::mousePressEvent(QMouseEvent* mouseevent)
{
	if (is_choose && Qt::LeftButton == mouseevent->button())
	{
		is_draw = true;
		start = mouseevent->pos();
		end = mouseevent->pos();
		update();
	}
}

void ImageWidget::mouseMoveEvent(QMouseEvent* mouseevent)
{
	if (is_draw)
	{
		end = mouseevent->pos();
		update();
	}
}

void ImageWidget::mouseReleaseEvent(QMouseEvent* mouseevent)
{
	if (is_draw)
	{
		start_list.push_back(start);
		end_list.push_back(end);
		is_draw = false;
		update();
	}
}

void ImageWidget::IDW()
{
	delete warping;
	warping = new IDWWarping((width() - ptr_image_->width()) / 2, (height() - ptr_image_->height()) / 2);
	warping->initAnchor(start_list,end_list);
	warping->imageWarping(*(ptr_image_));
	start_list.clear();
	end_list.clear();
	update();
}

void ImageWidget::RBF()
{
	delete warping;
	warping = new RBFWarping((width() - ptr_image_->width()) / 2, (height() - ptr_image_->height()) / 2);
	warping->initAnchor(start_list, end_list);
	warping->imageWarping(*(ptr_image_));
	start_list.clear();
	end_list.clear();
	update();
}
