#ifndef TRACK_HEAP_H
#define TRACK_HEAP_H

#include "TObject.h"

class TrackHeap : public TObject
{
public:
	virtual const char* classname() const { return "TrackHeap"; }
	TrackHeap(){
		reset();
	}
	~TrackHeap(){
	}

	void reset(){
		Tracks_mPt               = 0;
		Tracks_mEta              = 0;
		Tracks_mPhi              = 0;
		Tracks_mDedx             = 0;
		Tracks_mCharge           = 0;
		Tracks_mNHitsFit         = 0;
		Tracks_mNHitsMax         = 0;
		Tracks_mNHitsDedx        = 0;
		Tracks_mNSigmaPion       = -99;
		Tracks_mDCA              = 99;
		
		McTracks_mPt             = 0;
		McTracks_mEta            = 0;
		McTracks_mPhi            = 0;
		McTracks_mGeantPID       = 0;
		McTracks_mCharge         = 0;
		McTracks_mNHits          = 0;

		MtdPidTraits_mDeltaY     = 999;
		MtdPidTraits_mDeltaZ     = 999;
		MtdPidTraits_mMatchFlag  = 0;
		MtdPidTraits_mMtdHitChan = 0;
		MtdPidTraits_mCell       = 0;
		MtdPidTraits_mBL         = 0;
		MtdPidTraits_mModule     = 0;
	
	}

	void copy( TrackHeap * that ) {
		this->Tracks_mPt               = that->Tracks_mPt;
		this->Tracks_mEta              = that->Tracks_mEta;
		this->Tracks_mPhi              = that->Tracks_mPhi;
		this->Tracks_mDedx             = that->Tracks_mDedx;
		this->Tracks_mCharge           = that->Tracks_mCharge;
		this->Tracks_mNHitsFit         = that->Tracks_mNHitsFit;
		this->Tracks_mNHitsMax         = that->Tracks_mNHitsMax;
		this->Tracks_mNHitsDedx        = that->Tracks_mNHitsDedx;
		this->Tracks_mNSigmaPion       = that->Tracks_mNSigmaPion;
		this->Tracks_mDCA              = that->Tracks_mDCA;
		this->McTracks_mPt             = that->McTracks_mPt;
		this->McTracks_mEta            = that->McTracks_mEta;
		this->McTracks_mPhi            = that->McTracks_mPhi;
		this->McTracks_mGeantPID       = that->McTracks_mGeantPID;
		this->McTracks_mCharge         = that->McTracks_mCharge;
		this->McTracks_mNHits          = that->McTracks_mNHits;
		this->MtdPidTraits_mDeltaY     = that->MtdPidTraits_mDeltaY;
		this->MtdPidTraits_mDeltaZ     = that->MtdPidTraits_mDeltaZ;
		this->MtdPidTraits_mMatchFlag  = that->MtdPidTraits_mMatchFlag;
		this->MtdPidTraits_mMtdHitChan = that->MtdPidTraits_mMtdHitChan;
		this->MtdPidTraits_mCell       = that->MtdPidTraits_mCell;
		this->MtdPidTraits_mBL         = that->MtdPidTraits_mBL;
		this->MtdPidTraits_mModule     = that->MtdPidTraits_mModule;
	}

	Float_t 	Tracks_mPt;				    // primary track px
	Float_t 	Tracks_mEta;				// primary track py
	Float_t 	Tracks_mPhi;				// primary track pz
	Float_t 	Tracks_mDedx;				// dEdx
	Char_t      Tracks_mCharge;				// q
	Char_t 		Tracks_mNHitsFit;			// nHitsFit - TPC
	Char_t 		Tracks_mNHitsMax;			// nHitsMax - TPC
	UChar_t 	Tracks_mNHitsDedx;			// nHitsDedx - TPC
	Float_t 	Tracks_mNSigmaPion;		    // nsigmaPi
	Float_t 	Tracks_mDCA;				// global track gDCA
	

	// MC Track
	Float_t   McTracks_mPt;        // primary track px
	Float_t   McTracks_mEta;       // primary track py
	Float_t   McTracks_mPhi;       // primary track pz
	UShort_t  McTracks_mGeantPID; 
	Char_t    McTracks_mCharge; 
	Char_t    McTracks_mNHits;


	// MTD PidTratis
	Float_t   MtdPidTraits_mDeltaY;					// DeltaY between matched track-hit pair
	Float_t   MtdPidTraits_mDeltaZ;					// DeltaZ between matched track-hit pair
	Char_t    MtdPidTraits_mMatchFlag;				// Matching flag indicating multiple matches
	Short_t   MtdPidTraits_mMtdHitChan;				// (backleg-1) * 60 + (module-1) * 12 + cell
	Short_t   MtdPidTraits_mCell;
	Short_t   MtdPidTraits_mBL;
	Short_t   MtdPidTraits_mModule;
	ClassDef( TrackHeap, 1 )
};

#endif