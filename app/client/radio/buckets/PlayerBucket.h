/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/
 
#ifndef PLAYER_BUCKET_H
#define PLAYER_BUCKET_H

#include <QDebug>
#include <QListWidget>
#include <QDropEvent>
#include <QInputDialog>

#include <QDomElement>
#include <QDomNodeList>

class PlayerBucket : public QListWidget
{
	Q_OBJECT
public:
	PlayerBucket( QWidget* w );
	struct {
		class QListWidget* previewList;
	} ui;
	
	QRect visualRect ( const QModelIndex & index ) const;
	QModelIndex indexAt( const QPoint& point ) const;
	
protected:
	
	void resizeEvent ( QResizeEvent* event );	
	void dropEvent( QDropEvent* event);	
	void dragEnterEvent ( QDragEnterEvent * event );
	
	void paintEvent( QPaintEvent* );
	
private:
	static const QString k_dropText;
	bool m_showDropText;
	QMap< QModelIndex, QRect > m_itemRects;
	class QNetworkAccessManager* m_networkManager;
	
private slots:
	void playlistFetched();

};

#include <QAbstractItemDelegate>
#include <QPainter>
class PlayerBucketDelegate : public QAbstractItemDelegate
{
	Q_OBJECT
public:
	PlayerBucketDelegate( QObject* parent = 0 ):QAbstractItemDelegate( parent ){};
	
	void paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
	{
		if( option.state == QStyle::State_Active )
			painter->fillRect( option.rect, QColor( 0x2e, 0x2e, 0x7e, 0x77) );
		else
			painter->fillRect( option.rect, QColor( 0x2e, 0x2e, 0x2e, 0x77) );
		
		QIcon icon = index.data( Qt::DecorationRole ).value<QIcon>();
		icon.paint( painter, option.rect );
		
		painter->setPen( Qt::white );
		QRect textRect = option.rect.adjusted( 5, 60, -5, -5 );

		QString text = index.data( Qt::DisplayRole ).toString();
		painter->drawText( textRect, Qt::AlignCenter , text);
	}
	
	QSize sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const
	{
		return option.rect.size();
	}
};

#endif //PLAYER_BUCKET_H