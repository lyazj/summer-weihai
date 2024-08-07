//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B1/src/DetectorConstruction.cc
/// \brief Implementation of the B1::DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  G4double x = 50.5 * cm, y = x;
  G4double z_absorber = 3.5 * mm, z_sensitive = 0.3 * mm, z_gap = 1.7 * mm;
  G4double z_layer = z_absorber + z_sensitive + z_gap;
  G4int nlayer = GetNLayer();
  G4double distance = 2 * m;
  G4double z_detector = nlayer * z_layer;
  G4double z = z_detector + distance;
  G4Color c_absorber, c_sensitive, c_gap;
  G4VisAttributes v_absorber, v_sensitive, v_gap;

  // Materials.
  G4NistManager* nist = G4NistManager::Instance();
  auto Cu = nist->FindOrBuildMaterial("G4_Cu");
  auto W = nist->FindOrBuildMaterial("G4_W");
  auto Si = nist->FindOrBuildMaterial("G4_Si");
  auto Galactic = nist->FindOrBuildMaterial("G4_Galactic");

  // World.
  auto worldS = new G4Box("World", x * 0.6, y * 0.6, z * 0.6);
  auto worldLV = new G4LogicalVolume(worldS, Galactic, worldS->GetName());
  auto worldPV = new G4PVPlacement(nullptr, G4ThreeVector(), worldLV, worldLV->GetName(), nullptr, false, 0, true);

  // Absorber.
  auto absorberS = new G4Box("Absorber", x * 0.5, y * 0.5, z_absorber * 0.5);
  auto absorberLV = new G4LogicalVolume(absorberS, W, absorberS->GetName());
  G4Colour::GetColour("blue", c_absorber);
  c_absorber.SetAlpha(0.2);
  v_absorber.SetColour(c_absorber);
  v_absorber.SetForceSolid();
  absorberLV->SetVisAttributes(v_absorber);

  // Sensitive.
  auto sensitiveS = new G4Box("Sensitive", x * 0.5, y * 0.5, z_sensitive * 0.5);
  auto sensitiveLV = new G4LogicalVolume(sensitiveS, Si, sensitiveS->GetName());
  G4Colour::GetColour("red", c_sensitive);
  v_sensitive.SetColour(c_sensitive);
  v_sensitive.SetForceSolid();
  sensitiveLV->SetVisAttributes(v_sensitive);

  // Gap.
  auto gapS = new G4Box("Gap", x * 0.5, y * 0.5, z_gap * 0.5);
  auto gapLV = new G4LogicalVolume(gapS, Cu, gapS->GetName());
  G4Colour::GetColour("brown", c_gap);
  v_gap.SetColour(c_gap);
  c_gap.SetAlpha(0.5);
  v_gap.SetForceSolid();
  gapLV->SetVisAttributes(v_gap);

  // Layer.
  auto layerS = new G4Box("Layer", x * 0.5, y * 0.5, z_layer * 0.5);
  auto layerLV = new G4LogicalVolume(layerS, Galactic, layerS->GetName());

  // Layer content.
  {
    std::vector<G4LogicalVolume *> layerContent = {absorberLV, sensitiveLV, gapLV};
    G4double zp = -z_layer * 0.5;
    for(auto lv : layerContent) {
      G4double zt = ((G4Box *)lv->GetSolid())->GetZHalfLength();
      new G4PVPlacement(nullptr, {0, 0, zp + zt}, lv, lv->GetName(), layerLV, false, 0, true);
      zp += 2 * zt;
    }
  }

  // Layer placement.
  {
    G4double zp = z * 0.5 - z_detector;
    for(G4int i = 0; i < nlayer; ++i) {
      G4double zt = z_layer * 0.5;
      new G4PVPlacement(nullptr, {0, 0, zp + zt}, layerLV, layerLV->GetName(), worldLV, false, i, true);
      zp += 2 * zt;
    }
  }

  fScoringVolume = sensitiveLV;
  fSourceZ = -z * 0.5;
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
