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
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();
  auto Cu = nist->FindOrBuildMaterial("G4_Cu");
  auto W = nist->FindOrBuildMaterial("G4_W");
  auto Si = nist->FindOrBuildMaterial("G4_Si");

  auto Galactic = nist->FindOrBuildMaterial("G4_Galactic");


  auto worldS = new G4Box("World", 1 * m, 1 * m, 1 * m);
  auto worldLV = new G4LogicalVolume(worldS, Galactic, "World");
  auto worldPV = new G4PVPlacement(nullptr,
    G4ThreeVector(),
    worldLV,
    "World",
    nullptr,
    false,
    0,
    true);

  // Absorber
  auto absorberS = new G4Box("Absorber", 50.5 * cm, 50.5 * cm, 3.5 * mm);
  auto absorberLV = new G4LogicalVolume(absorberS, W, "Absorber");  
  for (int i = 0; i < 26; i++){
/*     for (int j = 0; j < 50; j++){
      for (int k = 0; k < 50; k++){ // width: j; height: k. */
        new G4PVPlacement(nullptr,
          //G4ThreeVector((-49.55 + 1.01 * j) * mm, (-49.55 + 1.01 * k) * mm, (1.75 + i * 5.5) * mm),
          G4ThreeVector(0, 0, (1.75 + i * 5.5) * mm),
          absorberLV,
          "Absorber",
          worldLV,
          false,
          i,
          true);
  }

  // Sen
  auto senS = new G4Box("Sens", 50.5 * cm, 50.5 * cm, 0.3 * mm);
  auto senLV = new G4LogicalVolume(senS, Si, "Sen");
  for (int i = 0; i < 26; i++){
/*     for(int j = 0; j < 50; j++){
      for (int k = 0; k < 50; k++){ */
        new G4PVPlacement(nullptr,
          //G4ThreeVector((-49.55 + 1.01 * j) * mm, (-49.55 + 1.01 * k) * mm, (3.65 + i * 5.5) * mm),
          G4ThreeVector(0, 0, (3.65 + i * 5.5) * mm),
          senLV,
          "Sen",
          worldLV,
          false,
          i,
          true);
  }

  // Gap
  auto gapS = new G4Box("Gap", 50.5 * cm, 50.5 * cm, 1.7 * mm);
  auto gapLV = new G4LogicalVolume(gapS, Cu, "Gap");
  for (int i = 0; i < 26; i++){
/*     for(int j = 0; j < 50; j++){
      for (int k = 0; k < 50; k++){ */
        new G4PVPlacement(nullptr,
          //G4ThreeVector((-49.55 + 1.01 * j) * mm, (-49.55 + 1.01 * k) * mm, (4.65 + i * 5.5) * mm),
          G4ThreeVector(0, 0, (4.65 + i * 5.5) * mm),
          gapLV,
          "Gap",
          worldLV,
          false,
          i,
          true);
  }

  return worldPV;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
