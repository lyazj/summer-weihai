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
/// \file B1/src/RunAction.cc
/// \brief Implementation of the B1::RunAction class

#include "RunAction.hh"
#include "DetectorConstruction.hh"
#include "G4RunManager.hh"
#include <TFile.h>
#include <TTree.h>
#include <unistd.h>
#include <filesystem>

namespace fs = std::filesystem;

namespace B1
{

RunAction::RunAction()
{
  fFilePath = "output/" + std::to_string(getpid()) + ".root";
  fFile = NULL;
  fTree = NULL;
}

void RunAction::BeginOfRunAction(const G4Run *)
{
  auto d = (DetectorConstruction *)G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  fScoringVolume = d->GetScoringVolume();
  fNCellX = d->GetNCellX(), fNCellY = d->GetNCellY();
  fDetectorMinZ = d->GetDetectorMinZ();
  fDetectorX = d->GetDetectorX(), fDetectorY = d->GetDetectorY(), fDetectorZ = d->GetDetectorZ();
  fEdepMap.resize(d->GetNLayer());

  if(fFile == NULL) {
    auto dirpath = fs::path(fFilePath.c_str()).parent_path();
    if(!dirpath.empty()) fs::create_directory(dirpath);
    fFile = new TFile(fFilePath, "RECREATE");
    if(!fFile->IsOpen()) throw std::runtime_error("Cannot open file " + fFilePath);
    fTree = new TTree("tree", "tree");
    fTree->Branch("Pos", &fPos);
    fTree->Branch("Edep", &fEdep);
    fTree->Branch("Theta", &fTheta);
    fTree->Branch("Phi", &fPhi);
    fTree->Branch("Energy", &fEnergy);
    fTree->Branch("X", &fX);
    fTree->Branch("Y", &fY);
    fTree->Branch("Z", &fZ);
  }
}

void RunAction::EndOfRunAction(const G4Run *)
{
  fTree->AutoSave("SaveSelf, Overwrite");
}

void RunAction::AddStep(const G4Step *step)
{
  G4VPhysicalVolume *volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  if(volume->GetLogicalVolume() != fScoringVolume) return;

  G4ThreeVector r = (step->GetPreStepPoint()->GetPosition() + step->GetPostStepPoint()->GetPosition()) / 2.0;
  G4double x = r.x(), y = r.y(), z = r.z();
  G4int i = (z - fDetectorMinZ) / fDetectorZ;
  G4int j = (x + fDetectorX / 2) / fDetectorX * fNCellX;
  G4int k = (y + fDetectorY / 2) / fDetectorY * fNCellY;
  if(i < 0 || i >= (G4int)fEdepMap.size() || j < 0 || j >= fNCellX || k < 0 || k >= fNCellY) {
    throw std::runtime_error("invalid cell index: " + std::to_string(i) + "," + std::to_string(j) + "," + std::to_string(k));
  }

  fEdepMap[i][j * fNCellY + k] += step->GetTotalEnergyDeposit();
}

void RunAction::FillAndReset()
{
  for(size_t i = 0; i < fEdepMap.size(); ++i) {
    for(auto [pos, edep] : fEdepMap[i]) {
      fPos.push_back(i * fNCellX * fNCellY + pos);
      fEdep.push_back(edep);
    }
  }
  fTree->Fill();
  for(size_t i = 0; i < fEdepMap.size(); ++i) fEdepMap[i].clear();
  fPos.clear();
  fEdep.clear();
}

RunAction::~RunAction()
{
  if(!fFile) return;
  fFile->cd();
  fTree->Write(NULL, TObject::kOverwrite);
  fFile->Close();
}

}
