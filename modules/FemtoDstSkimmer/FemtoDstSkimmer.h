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

#include "MvaDstFormat/TrackHeap.h"

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
	
	BranchWriter<TrackHeap> _wTrackHeap;
	

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
		wTree = new TTree( "MvaDst", "MvaDst" );
		_wTrackHeap.createBranch( wTree, "" );
	
	}

protected:

	virtual void analyzeEvent(){
		_event = _rEvent.get();

		// _wEvent.set( _event );
		TrackHeap trackHeap;
		// _wTracks.reset();
		// _wTrackHeap.reset();
		size_t nTracks = _rTracks.N();
		for (size_t i = 0; i < nTracks; i++ ){
			FemtoTrack * track = _rTracks.get(i);


			trackHeap.reset();

			FemtoMcTrack * mcTrack = nullptr;
			FemtoMtdPidTraits *mtdPid = nullptr;
			if ( track->mMtdPidTraitsIndex >= 0) 
				mtdPid = _rMtdPid.get( track->mMtdPidTraitsIndex );
			if ( track->mMcIndex >= 0 )
				mcTrack = _rMcTracks.get( track->mMcIndex );

			if ( nullptr == mtdPid || nullptr == mcTrack || mcTrack->mParentIndex >= 0 )
				continue;
			
			trackHeap.Tracks_mPt               = track->mPt;
			trackHeap.Tracks_mEta              = track->mEta;
			trackHeap.Tracks_mPhi              = track->mPhi;
			trackHeap.Tracks_mDedx             = track->dEdx();;
			trackHeap.Tracks_mCharge           = track->mNHitsFit / abs(track->mNHitsFit);;
			trackHeap.Tracks_mNHitsFit         = abs(track->mNHitsFit);
			trackHeap.Tracks_mNHitsMax         = track->mNHitsMax;
			trackHeap.Tracks_mNHitsDedx        = track->mNHitsDedx;
			trackHeap.Tracks_mNSigmaPion       = track->nSigmaPion();;
			trackHeap.Tracks_mDCA              = track->gDCA();;


			trackHeap.McTracks_mPt             = mcTrack->mPt;
			trackHeap.McTracks_mEta            = mcTrack->mEta;
			trackHeap.McTracks_mPhi            = mcTrack->mPhi;
			trackHeap.McTracks_mGeantPID       = mcTrack->mGeantPID;
			trackHeap.McTracks_mCharge         = mcTrack->mCharge;
			trackHeap.McTracks_mNHits          = mcTrack->mNHits;

			trackHeap.MtdPidTraits_mDeltaY     = mtdPid->mDeltaY;
			trackHeap.MtdPidTraits_mDeltaZ     = mtdPid->mDeltaZ;
			trackHeap.MtdPidTraits_mMatchFlag  = mtdPid->mMatchFlag;
			trackHeap.MtdPidTraits_mMtdHitChan = mtdPid->mMtdHitChan;
			// flip the cell for the ast 2 modules so that the phi direction is always the same
			trackHeap.MtdPidTraits_mCell       = mtdPid->cell();
			if ( mtdPid->module() >= 3 )
				trackHeap.MtdPidTraits_mCell       = 12 - mtdPid->cell();
				
			trackHeap.MtdPidTraits_mBL         = mtdPid->backleg();
			trackHeap.MtdPidTraits_mModule     = mtdPid->module();


			_wTrackHeap.set( trackHeap );
			wTree->Fill();
		} // nTracks


		
	}


	virtual void postMake() {
		TreeAnalyzer::postMake();

		wTree->Write();
	}
	
};

#endif