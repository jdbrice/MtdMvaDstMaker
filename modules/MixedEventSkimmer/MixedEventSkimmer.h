#ifndef MIXED_EVENT_SKIMMER_H
#define MIXED_EVENT_SKIMMER_H

#include "TreeAnalyzer.h"

#include "HistoBins.h"

#include "FemtoDstFormat/FemtoTrackContainer.h"

#include "vendor/loguru.h"

#include <map>
#include <tuple>

// ROOT
#include "TRandom3.h"
#include "TNamed.h"

class MixedEventSkimmer : public TreeAnalyzer
{
protected:
	
	BranchReader<FemtoEvent> _rEvent;
	TClonesArrayReader<FemtoTrack> _rTracks;
	TClonesArrayReader<FemtoMtdPidTraits> _rMtdPid;

	size_t bufferSize;
	map< tuple<char, size_t, size_t>, vector<FemtoTrackContainer> > buffer;
	HistoBins h_vtxZ, h_gRefMult;

	vector<FemtoTrackContainer> nullBuffer;

	TRandom3 rander;

	bool rejectSingleTrack = false;


public:
	virtual const char* classname() const { return "MixedEventSkimmer"; }
	MixedEventSkimmer() {}
	~MixedEventSkimmer() {}
	

	virtual void initialize(){
		TreeAnalyzer::initialize();

		_rEvent.setup( chain, "Event" );
		_rTracks.setup( chain, "Tracks" );
		_rMtdPid.setup( chain, "MtdPidTraits" );

		bufferSize = config.getInt( "bufferSize", 10 );

		h_vtxZ.load( config, nodePath + ".Mix.vtxZ" );
		h_gRefMult.load( config, nodePath + ".Mix.gRefMult" );

		LOG_F(1, "h_vtxZ: %s", h_vtxZ.toString().c_str() );
		LOG_F(1, "h_gRefMult: %s", h_gRefMult.toString().c_str() );

		rander.SetSeed(0);

	}

protected:


	vector<FemtoTrackContainer> &currentBuffer( int charge ){
		FemtoEvent *event = _rEvent.get();

		if ( nullptr == event ) return nullBuffer;
		if ( charge != -1 && charge != 1 ) return nullBuffer;

		size_t iVtx = h_vtxZ.findBin( event->mPrimaryVertex_mX3 );
		size_t iGRM = h_gRefMult.findBin( event->mGRefMult );

		// creates if DNE
		return buffer[ std::make_tuple( charge, iVtx, iGRM ) ];

	}

	void addToBuffer( vector<FemtoTrackContainer> &vtc, FemtoTrackContainer tc ){
		if ( vtc.size() < bufferSize )
			vtc.push_back( tc );
		else {
			// replace in proper size of buffer
			size_t index = rander.Integer( bufferSize );
			vtc[ index ] = tc;
		}
	}


	virtual void analyzePairs( FemtoTrackContainer &tc, vector<FemtoTrackContainer> &buff ){

	}


	virtual void analyzeEvent(){
		FemtoEvent *event = _rEvent.get();
		if ( nullptr == event ) return;

		vector<FemtoTrackContainer> &pos = currentBuffer( 1 );
		vector<FemtoTrackContainer> &neg = currentBuffer( -1 );

		FemtoTrackContainer tc;
		tc._runId = event->mRunId;
		tc._eventId = event->mEventId;

		size_t nTracks = _rTracks.N();
		if ( rejectSingleTrack && nTracks < 2 ) return;
		for (size_t i = 0; i < nTracks; i++ ){
			tc.assemble( i, _rTracks, _rMtdPid );

			if ( tc._track->charge() == 1 ){
				analyzePairs( tc, neg ); // ULS pairs
				analyzePairs( tc, pos ); // LS pairs
				addToBuffer( pos, tc );
			}
			else if ( tc._track->charge() == -1 ){
				analyzePairs( tc, pos ); // ULS pairs
				analyzePairs( tc, neg ); // LS pairs
				addToBuffer( neg, tc );
			}
		}


		// analyzeBuffers( pos, neg );
	}

	virtual void postEventLoop(){
		LOG_F( 0, "# of Buffers: %lu", buffer.size() );

		LOG_F( 0, "Entries in buffer[vtx, grm ]" );
		for ( auto kv : buffer ){
			LOG_F( 0, "Entries in buffer[ %d, %lu, %lu ] = %lu", std::get<0>(kv.first), std::get<1>(kv.first), std::get<2>(kv.first), kv.second.size() );
			// LOG_F( 0, "RunId=%lu, EvnetId=%lu", kv.second[0]._runId, kv.second[0]._eventId );
		}

		TNamed config_str( "config", config.toXml() );
		config_str.Write();

		TNamed config_dump( "plain_config", config.dump() );
		config_dump.Write();
	}
};



#endif