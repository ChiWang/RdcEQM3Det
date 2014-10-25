/*
 *  $Id: TriggerMonitor.C, 2014-10-24 18:57:58 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 22/10/2014
 *
 *    Usage:
 *        $root -l TriggerMonitor.C
 *  root[0]LoadInputFile("aInputFile")
 *  root[1]TriggerStatus()
*/

TFile *f = 0;
TTree *RdcTree = 0;

void TriggerMonitor(){
  gSystem->Load("$DMPSWSYS/lib/libDmpEvent.so");
  gSystem->Load("$DMPSWWORK/lib/libDmpEventRaw.so");
}

void LoadInputFile(std::string file){
  f = new TFile(file.c_str());
  f->cd("Event");   RdcTree = Rdc;
}

void TriggerStatus(){
  TCanvas *c1 = new TCanvas("Trigger0~4","Trigger0~4");
  c1->Divide(5,3);
  c1->cd(1);    RdcTree->Draw("EventHeader.ChoosedTriggerType(0)");
  c1->cd(2);    RdcTree->Draw("EventHeader.ChoosedTriggerType(1)");
  c1->cd(3);    RdcTree->Draw("EventHeader.ChoosedTriggerType(2)");
  c1->cd(4);    RdcTree->Draw("EventHeader.ChoosedTriggerType(3)");
  c1->cd(5);    RdcTree->Draw("EventHeader.ChoosedTriggerType(4)");

  c1->cd(6);    RdcTree->Draw("EventHeader.GeneratedTrigger(0):EventHeader.ChoosedTriggerType(0)");
  c1->cd(7);    RdcTree->Draw("EventHeader.GeneratedTrigger(1):EventHeader.ChoosedTriggerType(1)");
  c1->cd(8);    RdcTree->Draw("EventHeader.GeneratedTrigger(2):EventHeader.ChoosedTriggerType(2)");
  c1->cd(9);    RdcTree->Draw("EventHeader.GeneratedTrigger(3):EventHeader.ChoosedTriggerType(3)");
  c1->cd(10);   RdcTree->Draw("EventHeader.GeneratedTrigger(4):EventHeader.ChoosedTriggerType(4)");

  c1->cd(11);   RdcTree->Draw("EventHeader.EnabledTrigger(0):EventHeader.ChoosedTriggerType(0)");
  c1->cd(12);   RdcTree->Draw("EventHeader.EnabledTrigger(1):EventHeader.ChoosedTriggerType(1)");
  c1->cd(13);   RdcTree->Draw("EventHeader.EnabledTrigger(2):EventHeader.ChoosedTriggerType(2)");
  c1->cd(14);   RdcTree->Draw("EventHeader.EnabledTrigger(3):EventHeader.ChoosedTriggerType(3)");
  c1->cd(15);   RdcTree->Draw("EventHeader.EnabledTrigger(4):EventHeader.ChoosedTriggerType(4)");

  TCanvas *c2 = new TCanvas("External_Period","External_Period");
  c2->Divide(2,2);
  c2->cd(1);    RdcTree->Draw("EventHeader.InjectedExternalTrigger()");
  c2->cd(2);    RdcTree->Draw("EventHeader.GeneratedPeriodTrigger()");
  
  c2->cd(3);    RdcTree->Draw("EventHeader.InjectedExternalTrigger():EventHeader.EnabledExternalTrigger()");
  c2->cd(4);    RdcTree->Draw("EventHeader.GeneratedPeriodTrigger():EventHeader.EnabledPeriodTrigger()");
}

