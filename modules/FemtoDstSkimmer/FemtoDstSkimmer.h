#ifndef FEMTO_DST_SKIMMER_H
#define FEMTO_DST_SKIMMER_H

#include "TreeAnalyzer.h"

// FemtoDstFormat
#include "FemtoDstFormat/BranchReader.h"
#include "FemtoDstFormat/TClonesArrayReader.h"

#include "FemtoDstFormat/BranchWriter.h"
#include "FemtoDstFormat/TClonesArrayWriter.h"

#include "FemtoDstFormat/FemtoEvent.h"
#include "FemtoDstFormat/FemtoTrack.h"
#include "FemtoDstFormat/FemtoMcTrack.h"
#include "FemtoDstFormat/FemtoMcVertex.h"
#include "FemtoDstFormat/FemtoMtdPidTraits.h"
#include "FemtoDstFormat/FemtoTrackProxy.h"

#include "vendor/loguru.h"


class FemtoDstSkimmer : public TreeAnalyzer
{
protected:
	FemtoEvent *_event;
	FemtoTrackProxy _proxy;
	

	BranchReader<FemtoEvent> _rEvent;
	TClonesArrayReader<FemtoTrack> _rTracks;
	TClonesArrayReader<FemtoTrackHelix> _rHelices;
	TClonesArrayReader<FemtoMcTrack> _rMcTracks;
	TClonesArrayReader<FemtoMcVertex> _rMcVertices;
	TClonesArrayReader<FemtoMtdPidTraits> _rMtdPid;

	// Writing
	TTree * wTree = nullptr;
	
	BranchWriter<FemtoEvent> _wEvent;
	TClonesArrayWriter<FemtoTrack> _wTracks;
	TClonesArrayWriter<FemtoMtdPidTraits> _wMtdPid;
	TClonesArrayWriter<FemtoMcTrack> _wMcTracks;
	TClonesArrayWriter<FemtoMcVertex> _wMcVertices;
	TClonesArrayWriter<FemtoTrackHelix> _wHelices;

public:
	virtual const char* classname() const {return "FemtoDstSkimmer";}
	FemtoDstSkimmer() {}
	~FemtoDstSkimmer() {}

	virtual void initialize(){
		TreeAnalyzer::initialize();

		_rEvent.setup( chain, "Event" );
		_rTracks.setup( chain, "Tracks" );
		_rMcTracks.setup( chain, "McTracks" );
		_rMcVertices.setup( chain, "McVertices" );
		_rMtdPid.setup( chain, "MtdPidTraits" );


		book->cd();
		wTree = new TTree( "FemtoDst", "FemtoDst" );
		_wEvent.createBranch( wTree, "Event" );
		_wTracks.createBranch( wTree, "Tracks" );
		_wMcTracks.createBranch( wTree, "McTracks" );
		_wMcVertices.createBranch( wTree, "McVertices" );
		_wMtdPid.createBranch( wTree, "MtdPidTraits" );


	}

protected:

	virtual void analyzeEvent(){
		_event = _rEvent.get();

		_wEvent.set( _event );
		
		_wTracks.reset();
		_wMtdPid.reset();
		size_t nTracks = _rTracks.N();
		for (size_t i = 0; i < nTracks; i++ ){
			FemtoTrack * track = _rTracks.get(i);

			FemtoMtdPidTraits *mtdPid = nullptr;
			if ( track->mMtdPidTraitsIndex >= 0) 
				mtdPid = _rMtdPid.get( track->mMtdPidTraitsIndex );
			
		
			if ( nullptr != mtdPid ){
				_wTracks.add( track );
				_wMtdPid.add( mtdPid );
			}
		} // nTracks


		_wMcTracks.reset();
		size_t nMcTracks = _rMcTracks.N();
		for (size_t i = 0; i < nMcTracks; i++ ){
			FemtoMcTrack * mcTrack = _rMcTracks.get( i );
			_wMcTracks.add( mcTrack );
		}

		_wMcVertices.reset();
		size_t nMcVertices = _rMcVertices.N();
		for (size_t i = 0; i < nMcVertices; i++ ){
			FemtoMcVertex * mcVertex = _rMcVertices.get( i );
			_wMcVertices.add( mcVertex );
		}

		wTree->Fill();
	}


	virtual void postMake() {
		TreeAnalyzer::postMake();

		wTree->Write();
	}
	
};

#endif