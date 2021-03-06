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

#include "XmlHistogram.h"
#include "XmlFunction.h"

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

	XmlHistogram bg_deltaTOF;
	TH2 * hsigdtof;
	XmlHistogram sig_deltaTOF;


	bool decayInsideTPC = false;
	bool decayOutsideTPC = false;
	

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
		_wTrackHeap.createBranch( wTree, "TrackHeap" );

		bg_deltaTOF.load( config, nodePath + ".DeltaTOF.XmlHistogram[1]" );
		sig_deltaTOF.load( config, nodePath + ".DeltaTOF.XmlHistogram[0]" );

		book->cd();
		sig_deltaTOF.getTH1()->Write();
		hsigdtof = ((TH2*)sig_deltaTOF.getTH1().get());
		bg_deltaTOF.getTH1()->Write();

		if ( nullptr != chain ){
			LOG_F( INFO, "nTrees=%d", chain->GetNtrees() );
		}

		decayInsideTPC = config.getBool( nodePath + ".input:decayInsideTPC", false );
		decayOutsideTPC = config.getBool( nodePath + ".input:decayOutsideTPC", true );

	
	}

protected:

	string plcName( int gid ){
		if ( 5 == gid )
			return "mu+";
		if ( 6 == gid )
			return "mu-";
		if ( 8 == gid )
			return "pi+";
		if ( 9 == gid )
			return "pi-";
		return "";
	}

	int plcCharge( int gid ){
		if ( 5 == gid )
			return 1;
		if ( 6 == gid )
			return -1;
		if ( 8 == gid )
			return 1;
		if ( 9 == gid )
			return -1;
		return 0;
	}

	bool isSignal( FemtoMcTrack * mcTrack ){
		if ( nullptr == mcTrack )
			return false;
		if ( 5 == mcTrack->mGeantPID || 6 == mcTrack->mGeantPID ){

			if ( mcTrack->mParentIndex < 0 )
				return true;
			else 
				return false;
		}
		return false;
	}

	bool isDecayMuon( FemtoMcTrack * mcTrack ){
		if (nullptr == mcTrack) 
			return false;
		if ( 5 == mcTrack->mGeantPID || 6 == mcTrack->mGeantPID ){
			if ( mcTrack->mParentIndex < 0 ) // signal muon (primary)
				return false;
			
			auto parent = _rMcTracks.get( mcTrack->mParentIndex );
			if (8 == parent->mGeantPID || 9 == parent->mGeantPID || 11 == parent->mGeantPID || 12 == parent->mGeantPID )
				return true;
			return false;
			LOG_F( 1, "Decay mu[%s -> %s]", plcName(parent->mGeantPID).c_str(), plcName(mcTrack->mGeantPID).c_str() );
		}

		return false;
	}

	bool isDecayMuonInsideTPC( FemtoMcTrack *mcTrack ){
		return isDecayMuon( mcTrack );
	}

	bool isDecayMuonOutsideTPC( FemtoMtdPidTraits *mtdPid, FemtoMcTrack *mcTrack ){
		if (nullptr == mtdPid) 
			return false;
		if ( mtdPid->mIdTruth < 0 )
			return false;
		auto mtdMcTrack = _rMcTracks.get( mtdPid->mIdTruth );

		if ( mcTrack->mParentIndex >= 0 )
			return false;

		return isDecayMuon( mtdMcTrack );
	}

	double nSigmaDeltaY( float pt, float dy ){
		double sigy = -17.6867 + 18.4528*exp(0.637142/pt);
		return dy / sigy;
	}

	double nSigmaDeltaZ( float pt, float dz ){
		double sigz = -32.6793 + 32.6034 * exp( 0.444217 / pt );
		return dz / sigz;
	}

	double nSigmaDeltaTOF( float pt, float dtof ){
		double sigdtof = 0.100915 + 0.00911555 * exp( 3.47368 / pt );
		return dtof / sigdtof;
	}

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

			if ( nullptr == mtdPid || nullptr == mcTrack )
				continue;

			bool inTPC = isDecayMuonInsideTPC(mcTrack);
			if ( inTPC != decayInsideTPC ){
				continue;
			}
			
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
			trackHeap.MtdPidTraits_mNSigDeltaY = nSigmaDeltaY( track->mPt, mtdPid->mDeltaY );
			trackHeap.MtdPidTraits_mDeltaZ     = mtdPid->mDeltaZ;
			trackHeap.MtdPidTraits_mNSigDeltaZ = nSigmaDeltaZ( track->mPt, mtdPid->mDeltaZ );
			trackHeap.MtdPidTraits_mMatchFlag  = mtdPid->mMatchFlag;
			trackHeap.MtdPidTraits_mMtdHitChan = mtdPid->mMtdHitChan;
			
			trackHeap.MtdPidTraits_mCell       = mtdPid->cell();
			trackHeap.MtdPidTraits_mBL         = mtdPid->backleg();
			trackHeap.MtdPidTraits_mModule     = mtdPid->module();

			double deltaTof = -999;
			if (isSignal( mcTrack  )){
				
				float pt = track->mPt;
				if ( pt > 14 ) pt = 14;
				int ptbin = hsigdtof->GetXaxis()->FindBin( pt );
				TH1 * hptslice = hsigdtof->ProjectionY( "tmp", ptbin, ptbin );
				
				// sample the dtof distribution
				deltaTof = hptslice->GetRandom();
				delete hptslice;

			} else {
				deltaTof = bg_deltaTOF.getTH1()->GetRandom();
			}

			trackHeap.MtdPidTraits_mDeltaTOF     = deltaTof;
			trackHeap.MtdPidTraits_mNSigDeltaTOF = nSigmaDeltaTOF( track->mPt, deltaTof );

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