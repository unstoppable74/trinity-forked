////////////////////////////////////////////////////////////
//
//    Created:   August 2014
//    Copyright: CCP 2014
//
#pragma once
#ifndef EveSOFDNA_H
#define EveSOFDNA_H

#include "EveSOFDataMgr.h"

// forwards
BLUE_DECLARE( EveSOFDNA );
enum TriBatchType;

// --------------------------------------------------------------------------------
// Description:
//   This class is for encapsulating the DNA of an SOF ship
// SeeAlso:
//   EveSOF
// --------------------------------------------------------------------------------
BLUE_CLASS( EveSOFDNA ) :
	public IRoot
{
public:
	EXPOSE_TO_BLUE();

	EveSOFDNA( IRoot* lockobj = NULL );
	~EveSOFDNA();

	// initialize this dna
	void Setup( const char* dnaString, EveSOFDataMgrPtr dataMgr );

	// is it correctly initialized?
	bool isValid() const;

	// get standard data
	const char* GetHullName() const;
	const char* GetFactionName() const;
	const char* GetRaceName() const;

	// get racial data
	const EveSOFDataMgr::RaceBoosterData* GetRaceBoosterData() const;

	// get hull data
	const Vector4* GetHullBoundingSphere() const;
	bool IsHullAnimated() const;
	const EveSOFDataMgr::HullBoosterData* GetHullBoosterData() const;
	const char* GetHullGeometryResPath() const;
	const char* GetModelRotationCurvePath() const;
	const char* GetModelTranslationCurvePath() const;
	const std::vector<EveSOFDataMgr::HullAreas>* GetHullMeshAreas( TriBatchType type ) const;
	const std::vector<EveSOFDataMgr::HullChild>& GetHullChildren() const;
	const std::vector<EveSOFDataMgr::HullAnimation>& GetHullAnimations() const;
	const std::vector<EveSOFDataMgr::HullDecalData>& GetHullDecals() const;
	const std::vector<EveSOFDataMgr::HullPlaneSetData>& GetHullPlaneSets() const;
	const std::vector<EveSOFDataMgr::HullSpotlightSetData>& GetHullSpotlightSets() const;
	const std::vector<EveSOFDataMgr::HullSpriteSetData>& GetHullSpriteSets() const;

	// get faction data
	const char* GetFactionResPathInsert() const;
	const Vector4* GetFactionMeshAreaParameters( TriBatchType type, const char* areaDesignation, const char* parameterName ) const;
	const EveSOFDataMgr::FactionDecalData* GetFactionDecalData( int groupIndex ) const;
	const EveSOFDataMgr::FactionPlaneSetColorData* GetFactionPlaneSetData( int groupIndex ) const;
	const EveSOFDataMgr::FactionSpotlightSetColorData* GetFactionSpotlightSetData( int groupIndex ) const;
	const EveSOFDataMgr::FactionSpriteSetColorData* GetFactionSpriteSetData( int groupIndex ) const;


private:
	// the dna as a string
	std::string m_dna;

	// a temporary pointer to the BIG data
	EveSOFDataMgrPtr m_dataMgr;
	// pointers to the specific data inside the BIG data
	const EveSOFDataMgr::HullData* m_hullData;
	const EveSOFDataMgr::FactionData* m_factionData;
	const EveSOFDataMgr::RaceData* m_raceData;

	// decoded data
	std::string m_hullName;
	std::string m_factionName;
	std::string m_raceName;
};

TYPEDEF_BLUECLASS( EveSOFDNA );

#endif // EveSOFDNA_H