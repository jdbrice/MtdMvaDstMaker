#ifndef FEMTO_DST_SKIMMER_H
#define FEMTO_DST_SKIMMER_H

#include "TreeAnalyzer.h"

// FemtoDstFormat
#include "FemtoDstFormat/BranchReader.h"
#include "FemtoDstFormat/TClonesArrayReader.h"
#include "FemtoDstFormat/FemtoEvent.h"
#include "FemtoDstFormat/FemtoTrack.h"
#include "FemtoDstFormat/FemtoMtdPidTraits.h"
#include "FemtoDstFormat/FemtoTrackProxy.h"

// Analyzers
#include "FemtoDstSkimmer/PairHistogramMaker.h"
#include "FemtoDstSkimmer/TrackHistogramMaker.h"
#include "FemtoDstSkimmer/MtdHistogramMaker.h"

#include "vendor/loguru.h"


class FemtoDstSkimmer : public TreeAnalyzer
{
protected:
	FemtoEvent *_event;
	FemtoTrackProxy _proxy;
	FemtoTrackProxy _proxy2;

	BranchReader<FemtoEvent> _rEvent;
	TClonesArrayReader<FemtoTrack> _rTracks;
	TClonesArrayReader<FemtoMtdPidTraits> _rMtdPid;

	//Analyzers
	PairHistogramMaker phm;
	TrackHistogramMaker thm;
	MtdHistogramMaker mhm;

	bool trackQA, pairQA, mtdQA;

public:
	virtual const char* classname() const {return "FemtoDstSkimmer";}
	FemtoDstSkimmer() {}
	~FemtoDstSkimmer() {}

	virtual void initialize(){
		TreeAnalyzer::initialize();

		_rEvent.setup( chain, "Event" );
		_rTracks.setup( chain, "Tracks" );
		_rMtdPid.setup( chain, "MtdPidTraits" );

		trackQA = config.getBool( "TrackHistogramMaker:enable", true );
		pairQA  = config.getBool( "PairHistogramMaker:enable", true );
		mtdQA   = config.getBool( "MtdHistogramMaker:enable", true );

		if ( pairQA )  phm.setup( config, "PairHistogramMaker", book );
		if ( trackQA ) thm.setup( config, "TrackHistogramMaker", book );
		if ( mtdQA )   mhm.setup( config, "MtdHistogramMaker", book );

	}

protected:

	virtual void analyzeEvent(){
		if ( !trackQA && !pairQA ) return; 

		_event = _rEvent.get();
		if ( pairQA ) phm.setEvent( _event );

		size_t nTracks = _rTracks.N();
		for (size_t i = 0; i < nTracks; i++ ){
			_proxy.assemble( i, _rTracks, _rMtdPid );
			if ( trackQA ) thm.analyze( _proxy );
			if ( mtdQA )   mhm.analyze( _proxy );

			if ( pairQA ){
				for (size_t j = i; j < nTracks; j++ ){
					if ( i == j ) continue;
					_proxy2.assemble( j, _rTracks, _rMtdPid );
					phm.analyze( _proxy, _proxy2 );
				}
			}
		}


		if ( pairQA )  phm.fillAggregates();
		if ( trackQA ) thm.fillAggregates();
	}
	
};

#endif