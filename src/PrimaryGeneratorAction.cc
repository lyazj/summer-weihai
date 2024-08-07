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
/// \file B1/src/PrimaryGeneratorAction.cc
/// \brief Implementation of the B1::PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "G4RunManager.hh"
#include "DetectorConstruction.hh"
#include "G4ParticleGun.hh"
#include "Randomize.hh"
#include "G4SystemOfUnits.hh"

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction(RunAction* r)
{
  fRunAction = r;
  fParticleGun = new G4ParticleGun(1);

  // default particle kinematic
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
  fParticleGun->SetParticleDefinition(particle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  auto d = (DetectorConstruction *)G4RunManager::GetRunManager()->GetUserDetectorConstruction();

  // Energy.
  // The value is set elsewhere.
  fRunAction->SetEnergy(fParticleGun->GetParticleEnergy());

  // Momentum.
  G4double cos_theta_min = cos(2 * deg / rad);
  G4double cos_theta_max = cos(4 * deg / rad);
  G4double cos_theta = G4UniformRand() * (cos_theta_max - cos_theta_min) + cos_theta_min;
  G4double theta = acos(cos_theta);
  G4double phi = G4UniformRand() * 2 * M_PI - M_PI;
  fRunAction->SetAngle(theta, phi);
  fParticleGun->SetParticleMomentumDirection({sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta)});

  // Position.
  G4double x = d->GetDetectorX() * (G4UniformRand() - 0.5);
  G4double y = d->GetDetectorY() * (G4UniformRand() - 0.5);
  G4double z = d->GetSourceZ();
  fRunAction->SetPosition(x, y, z);
  fParticleGun->SetParticlePosition({x, y, z});
  fParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}


