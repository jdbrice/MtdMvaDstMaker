#ifndef MIXED_EVENT_SKIMMER_H
#define MIXED_EVENT_SKIMMER_H

#include "TreeAnalyzer.h"
#include "HistoBins.h"


class MixedEventSkimmer : public TreeAnalyzer
{
protected:
	
	BranchReader<FemtoEvent> _rEvent;
	TClonesArrayReader<FemtoTrack> _rTracks;
	TClonesArrayReader<FemtoMtdPidTraits> _rMtdPid;

	size_t bufferSize;
	map< int, vector<FemtoTrackContainer> > buffer;
	HistoBins h_vtxZ, h_gRefMult;

public:
	MixedEventSkimmer() {}
	~MixedEventSkimmer() {}
	

	virtual void initialize(){
		TreeAnalyzer::initialize();

		_rEvent.setup( chain, "Event" );
		_rTracks.setup( chain, "Tracks" );
		_rMtdPid.setup( chain, "MtdPidTraits" );

		bufferSize = 10;

	}

protected:




	virtual void analyzeEvent(){

	}
};



#endif