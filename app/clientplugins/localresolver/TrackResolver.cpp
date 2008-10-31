/**************************************************************************
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
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
***************************************************************************/

#include "TrackResolver.h"
#include "LocalContentScanner.h"
#include <QThreadPool>


TrackResolver::TrackResolver()
: m_queryPool(0)
, m_scanner(0)
{}

TrackResolver::~TrackResolver()
{
    delete m_scanner;
    delete m_queryPool;
}

void
TrackResolver::init()
{
    m_queryPool = new QThreadPool();
    m_scanner = new LocalContentScanner;
}

void 
TrackResolver::resolve(class ITrackResolveRequest *req)
{
    Q_ASSERT(m_queryPool && req);
    if (req) {
        if (m_queryPool)
	        m_queryPool->start(new RequestRunnable(req, m_dbPath));
        else
            req->finished();
    }
}

void 
TrackResolver::finished()
{
    delete this;
}

//////////////////////////////////////////////////////////////////////

    
TrackResolver::Response::Response(const LocalCollection::ResolveResult &r)
:   m_matchQuality( 1 ),
    m_artist( r.m_artist.toUtf8() ),
    m_album( r.m_album.toUtf8() ),
    m_title( r.m_title.toUtf8() ),
    m_duration( r.m_duration ),
    m_kbps( r.m_kbps )
{
    QString url(r.m_sourcename + r.m_path + r.m_filename);
    m_url = url/*.replace('\\', "/")*/.toUtf8();
}

float
TrackResolver::Response::matchQuality() const
{
    return m_matchQuality;
}

const char *
TrackResolver::Response::url() const
{
    return m_url.constData();
}

const char *
TrackResolver::Response::artist() const
{
    return m_artist.constData();
}

const char *
TrackResolver::Response::album() const
{
    return m_album.constData();
}

const char *
TrackResolver::Response::title() const
{
    return m_title.constData();
}

const char *
TrackResolver::Response::filetype() const
{
    return "audio/mpeg";    // yes this is mp3, todo: the others
}

unsigned 
TrackResolver::Response::duration() const
{
    return m_duration;
}

unsigned 
TrackResolver::Response::kbps() const
{
    return m_kbps;
}


void 
TrackResolver::Response::finished()
{
    delete this;
}


//////////////////////////////////////////////////////////////////////


TrackResolver::RequestRunnable::RequestRunnable(ITrackResolveRequest *r, const QString &dbPath)
:   m_req(r),
    m_dbPath(dbPath)
{}

void
TrackResolver::RequestRunnable::run()
{
	try 
	{
        QList<LocalCollection::ResolveResult> results = 
            LocalCollection::instance().resolve(
                QString::fromUtf8(m_req->artist()),
                QString::fromUtf8(m_req->album()),
                QString::fromUtf8(m_req->title()));

        foreach (const LocalCollection::ResolveResult &r, results) {
            bool bLooksReadable;
            {
                QFile test(r.m_sourcename + r.m_path + r.m_filename);
                bLooksReadable = test.open(QIODevice::ReadOnly);
            }
            if (bLooksReadable)
                m_req->result(new Response(r));
        }
	} 
	catch(...)
	{
        // todo
	}
	m_req->finished();
}