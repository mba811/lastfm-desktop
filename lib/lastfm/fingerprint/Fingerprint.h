/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
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

#ifndef LASTFM_FINGERPRINT_H
#define LASTFM_FINGERPRINT_H

#include <lastfm/FingerprintId>
#include <lastfm/Track>


namespace lastfm
{
    class LASTFM_FINGERPRINT_DLLEXPORT Fingerprint
    {
        lastfm::Track m_track;
        QByteArray m_data;
        int m_id;
        int m_duration;

    protected:
        bool m_complete;

    public:
        /** represents a partial fingerprint of 20 seconds of music, this is 
          * considered 99.9999...9999% unique and so we use it for most stuff as 
          * it is much quicker than a complete fingerprint, still though, you
          * should do the generate step in a thread. */
        Fingerprint( const lastfm::Track& );
    
        /** if the id isNull(), then you'll need to do generate, submit and decode */
        FingerprintId id() const { return m_id; }

        /** The actual data that is the fingerprint, this is about 70kB or so,
          * there isn't anything in it until you call generate.
          * HEY! The QByteArray returned is not valid if Fingerprint object is 
          * deleted or falls off the stack. Seriously! Even if you copy it! Although
          * you can of course *deep-copy* the data. */
        QByteArray data() const { return m_data; }

        enum Error
        {
            ReadError = 0,

            /** failed to extract samplerate, bitrate, channels, duration etc */
            HeadersError,

            DecodeError,
        
            /** there is a minimum track duration for fingerprinting */
            TrackTooShortError,
        
            /** sorry, liblastfm sucks, report bug with log! */
            InternalError            
        };

        /** This is CPU intensive, do it in a thread in your GUI application */
        void generate() throw( Error );

        /** Submits the fingerprint data to Last.fm in order to get a FingerprintId
          * back. You need to wait for the QNetworkReply to finish before you can
          * pass it to decode clearly. */
        QNetworkReply* submit() const;

        /** Pass a finished reply from submit(), if the response is sound, id()
          * will be valid. Otherwise query QNetworkReply for an error, if there is
          * no error, then the reply was malformed, try submit() again. 
          * The return value of id() will be updated by this function, if possible.
          */
        void decode( QNetworkReply*, bool* lastfm_needs_a_complete_fingerprint = 0 );
    };


    class LASTFM_FINGERPRINT_DLLEXPORT CompleteFingerprint : public Fingerprint
    {
    public:
        CompleteFingerprint( const lastfm::Track& t ) : Fingerprint( t )
        {
            m_complete = true;
        }
    };
}

#endif
